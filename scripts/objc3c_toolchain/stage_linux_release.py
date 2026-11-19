"""Assemble a draft release from two successful Linux SDK qualification runs."""

import argparse
from compression import zstd
import errno
import json
from pathlib import Path
import re
import shutil
import subprocess
import tarfile
import tempfile

from .package import SCHEMA, _digest, _file_hash, distribution_license
from . import release_install


HOSTS = {'linux-x86_64': 'x86_64-unknown-linux-gnu',
         'linux-aarch64': 'aarch64-unknown-linux-gnu'}
INSTALLERS = {'install-linux.sh': 'scripts/install_objc3c_linux.sh',
              'install-sdk.py': 'scripts/objc3c_toolchain/install_sdk.py'}
MANIFEST = 'objc3-sdk/objc3-sdk.json'
ARCHIVE_LICENSE = 'objc3-sdk/share/licenses/slopjective-c/LICENSE'
FOUNDATION = 'objc3-sdk/include/clang/Basic/ObjC3Foundation.h'
QUALIFICATION = '.github/workflows/platform-host-evidence.yml'


def validate_run(run: dict, *, repo: str, revision: str, run_id: int) -> None:
    if (run.get('id') != run_id or run.get('path') != QUALIFICATION or
            run.get('event') != 'workflow_dispatch' or run.get('head_sha') != revision or
            (run.get('head_repository') or {}).get('full_name') != repo or
            type(run.get('run_attempt')) is not int or run['run_attempt'] < 1 or
            run.get('status') != 'completed' or run.get('conclusion') != 'success'):
        raise ValueError(f'run {run_id} is not successful release qualification for {repo}@{revision}')


def clean_installs(root: Path, *, host: str, revision: str, run: int, attempt: int,
                   archive: dict) -> list[dict]:
    records = []
    for distribution in release_install.DISTRIBUTIONS:
        path = root / distribution.replace(':', '-') / 'result.json'
        row = json.loads(path.read_bytes())
        expected = {'schema': release_install.SCHEMA, 'status': 'PASS', 'host': host,
                    'revision': revision, 'run': run, 'attempt': attempt, 'distribution': distribution,
                    'archive_sha256': archive['sha256'], 'seal_sha256': archive['seal_sha256'],
                    'source_build_id': archive['source_build_id']}
        if (any(row.get(key) != value for key, value in expected.items()) or
                not release_install.IMAGE.fullmatch(row.get('image', '')) or
                not release_install.IMAGE_ID.fullmatch(row.get('image_id', ''))):
            raise ValueError(f'{host} {distribution} installation receipt differs from the qualified archive')
        records.append(row)
    return records


def read_release_archive(archive: Path, *, check_license: bool) -> tuple[dict, dict[str, bytes]]:
    selected = {MANIFEST: 32 * 1024 * 1024, FOUNDATION: 64 * 1024}
    selected.update({f'objc3-sdk/share/objc3c/{name}': 128 * 1024 for name in INSTALLERS})
    if check_license:
        selected[ARCHIVE_LICENSE] = 1024 * 1024
    members = {}
    with zstd.ZstdFile(archive, 'rb') as decoded:
        with tarfile.open(fileobj=decoded, mode='r|') as tar:
            for member in tar:
                if member.name not in selected:
                    continue
                if not member.isfile() or member.name in members or member.size > selected[member.name]:
                    raise ValueError(f'invalid release archive member {member.name}')
                with tar.extractfile(member) as stream:
                    members[member.name] = stream.read()
    if members.keys() != selected.keys():
        missing = ', '.join(sorted(selected.keys() - members.keys()))
        raise ValueError(f'release archive is missing required files: {missing}')
    return json.loads(members.pop(MANIFEST)), members


def admit_archive(download: Path, *, host: str, checkout: Path) -> tuple[Path, dict]:
    archives = list(download.rglob('*.tar.zst'))
    receipts = list(download.rglob('*.tar.json'))
    if len(archives) != 1 or receipts != [archives[0].with_suffix('.json')]:
        raise ValueError(f'{host} must provide one matched release archive and receipt')
    archive = archives[0]
    receipt = json.loads(receipts[0].read_bytes())
    archive_sha256 = _file_hash(archive)
    if (receipt.get('host') != host or receipt.get('target') != HOSTS[host] or
            receipt.get('archive_sha256') != archive_sha256 or
            receipt.get('archive_bytes') != archive.stat().st_size):
        raise ValueError(f'{host} archive receipt differs from the downloaded bytes or target')

    license_file = distribution_license(checkout)
    manifest, members = read_release_archive(archive, check_license=license_file is not None)
    seal = manifest.pop('seal_sha256', None)
    build_id = manifest.get('source_build_id')
    if (manifest.get('schema') != SCHEMA or manifest.get('target') != HOSTS[host] or
            seal != receipt.get('seal_sha256') or seal != _digest(manifest) or
            not build_id or build_id != receipt.get('source_build_id')):
        raise ValueError(f'{host} SDK seal or source build differs from the release receipt')
    for name, path in INSTALLERS.items():
        if members[f'objc3-sdk/share/objc3c/{name}'] != (checkout / path).read_bytes():
            raise ValueError(f'{host} {name} differs from the qualified revision')
    if license_file is not None and members[ARCHIVE_LICENSE] != license_file.read_bytes():
        raise ValueError(f'{host} license differs from the qualified revision')
    foundation = members[FOUNDATION]
    abi = re.search(rb'^#define CLANG_OBJC3_EXTENSION_ABI_REVISION ([1-9][0-9]*)$', foundation, re.M)
    upstream = re.search(rb'^#define CLANG_OBJC3_UPSTREAM_COMMIT "([a-f0-9]{40})"$', foundation, re.M)
    if not abi or not upstream or upstream[1].decode() != manifest['upstream_commit']:
        raise ValueError(f'{host} foundation header does not identify the qualified compiler')
    return archive, {
        'target': manifest['target'], 'sha256': archive_sha256, 'bytes': receipt['archive_bytes'],
        'seal_sha256': seal, 'source_build_id': build_id, 'linux_abi': manifest['linux_abi'],
        'extension_abi': int(abi[1]),
        'upstream_commit': manifest['upstream_commit'], 'patch_digest': manifest['patch_digest'],
    }


def stage(checkout: Path, *, repo: str, revision: str, tag: str,
          runs: dict[str, int], downloads: Path, output: Path) -> dict:
    if not re.fullmatch(r'[0-9a-f]{40}', revision):
        raise ValueError('release requires a full Git revision')
    if not re.fullmatch(r'v\d+\.\d+\.\d+(?:-[a-z0-9]+(?:[.-][a-z0-9]+)*)?', tag):
        raise ValueError('release requires a version tag such as v0.1.0-preview.1')
    if runs.keys() != HOSTS.keys() or len(set(runs.values())) != len(HOSTS):
        raise ValueError('release requires distinct x86-64 and AArch64 qualification runs')
    if output.exists():
        raise ValueError('release staging requires a fresh output directory')
    license_file = distribution_license(checkout)
    if license_file is None:
        raise ValueError('release requires an approved project distribution license')

    admitted = {}
    for host, run_id in runs.items():
        run = json.loads(subprocess.check_output(['gh', 'api', f'repos/{repo}/actions/runs/{run_id}']))
        validate_run(run, repo=repo, revision=revision, run_id=run_id)
        attempt = run['run_attempt']
        target = downloads / f'{host}-{run_id}-{attempt}'
        if target.exists():
            archive, record = admit_archive(target, host=host, checkout=checkout)
            installs = clean_installs(target, host=host, revision=revision, run=run_id,
                                     attempt=attempt, archive=record)
        else:
            downloads.mkdir(parents=True, exist_ok=True)
            with tempfile.TemporaryDirectory(prefix='.download-', dir=downloads) as directory:
                pending = Path(directory) / 'artifact'
                subprocess.run(['gh', 'run', 'download', str(run_id), '--repo', repo,
                                '--name', f'objc3c-linux-sdk-{host}-{run_id}-{attempt}',
                                '--dir', str(pending)], check=True)
                archive, record = admit_archive(pending, host=host, checkout=checkout)
                for distribution in release_install.DISTRIBUTIONS:
                    name = distribution.replace(':', '-')
                    subprocess.run(['gh', 'run', 'download', str(run_id), '--repo', repo,
                                    '--name', f'objc3c-linux-install-{host}-{run_id}-{attempt}-{name}',
                                    '--dir', str(pending / name)], check=True)
                installs = clean_installs(pending, host=host, revision=revision, run=run_id,
                                         attempt=attempt, archive=record)
                relative = archive.relative_to(pending)
                pending.rename(target)
                archive = target / relative
        admitted[host] = archive, {**record, 'qualification_attempt': attempt,
                                   'clean_installs': installs}
    source = {(record['upstream_commit'], record['patch_digest'], record['extension_abi'])
              for _, record in admitted.values()}
    if len(source) != 1:
        raise ValueError('native archives use different compiler source stacks')

    output.parent.mkdir(parents=True, exist_ok=True)
    report = {'schema': 'objc3c.linux-release.v1', 'tag': tag, 'revision': revision,
        'channel': 'preview', 'profiles_claimed': [], 'archives': {}}
    with tempfile.TemporaryDirectory(prefix='.release-', dir=output.parent) as directory:
        bundle = Path(directory) / 'bundle'
        bundle.mkdir()
        digests = {}
        for host, (archive, record) in admitted.items():
            name = f'objc3-sdk-{tag}-{host}.tar.zst'
            asset = bundle / name
            try:
                asset.hardlink_to(archive)
            except OSError as error:
                if error.errno != errno.EXDEV:
                    raise
                shutil.copyfile(archive, asset)
            digests[name] = _file_hash(asset)
            if digests[name] != record['sha256']:
                raise ValueError(f'{host} archive changed while staging')
            report['archives'][host] = {**record, 'file': name, 'qualification_run': runs[host],
                                        'qualification_url': f'https://github.com/{repo}/actions/runs/{runs[host]}'}
        for name, path in INSTALLERS.items():
            shutil.copyfile(checkout / path, bundle / name)
        shutil.copyfile(license_file, bundle / 'LICENSE')
        (bundle / 'linux-release.json').write_text(json.dumps(report, indent=2) + '\n')
        checksums = [f'{digests.get(path.name) or _file_hash(path)}  {path.name}'
                     for path in sorted(bundle.iterdir())]
        (bundle / 'SHA256SUMS').write_text('\n'.join(checksums) + '\n')
        bundle.rename(output)
    return report


def publish_draft(output: Path, *, repo: str, tag: str, revision: str, notes: Path) -> None:
    def api(route: str, *options: str):
        return json.loads(subprocess.check_output(['gh', 'api', f'repos/{repo}/{route}', *options]))

    def find_release():
        pages = api('releases?per_page=100', '--paginate', '--slurp')
        return next((row for page in pages for row in page if row['tag_name'] == tag), None)

    manifest = output / 'linux-release.json'
    marker = f'<!-- objc3c-release-manifest-sha256: {_file_hash(manifest)} -->'
    expected = {path.name: ('sha256:' + _file_hash(path), path.stat().st_size)
                for path in output.iterdir()}

    def missing_assets(release):
        if (release is None or not release.get('draft') or not release.get('prerelease') or
                release.get('target_commitish') != revision or marker not in release['body'].splitlines()):
            raise ValueError(f'{tag} does not identify the exact resumable preview draft')
        missing = set(expected)
        for asset in api(f'releases/{release["id"]}/assets'):
            name = asset['name']
            if name not in missing or (asset.get('digest'), asset['size']) != expected[name]:
                raise ValueError(f'{tag} already has an unexpected or mismatched asset: {name}')
            missing.remove(name)
        return sorted(missing)

    refs = api(f'git/matching-refs/tags/{tag}')
    if any(ref['ref'] == f'refs/tags/{tag}' for ref in refs):
        if api(f'commits/{tag}')['sha'] != revision:
            raise ValueError(f'{tag} already points to a different Git revision')

    release = find_release()
    if release is None:
        with tempfile.TemporaryDirectory(prefix='objc3-release-notes-') as directory:
            draft_notes = Path(directory) / 'notes.md'
            draft_notes.write_text(notes.read_text().rstrip() + f'\n\n{marker}\n')
            subprocess.run(['gh', 'release', 'create', tag, '--repo', repo, '--target', revision,
                            '--draft', '--prerelease', '--title', f'Objective-C 3.0 Linux SDK {tag}',
                            '--notes-file', str(draft_notes)], check=True)
        release = find_release()
    missing = missing_assets(release)
    if missing:
        subprocess.run(['gh', 'release', 'upload', tag, '--repo', repo,
                        *(str(output / name) for name in missing)], check=True)
        if missing_assets(release):
            raise ValueError(f'{tag} draft is missing expected assets after upload')


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', required=True)
    parser.add_argument('--revision', required=True)
    parser.add_argument('--tag', required=True)
    parser.add_argument('--x86-run', type=int, required=True)
    parser.add_argument('--arm-run', type=int, required=True)
    parser.add_argument('--downloads', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--publish-draft', type=Path, metavar='RELEASE_NOTES')
    args = parser.parse_args()
    try:
        stage(Path.cwd(), repo=args.repo, revision=args.revision, tag=args.tag,
              runs={'linux-x86_64': args.x86_run, 'linux-aarch64': args.arm_run},
              downloads=args.downloads, output=args.output)
        if args.publish_draft:
            publish_draft(args.output, repo=args.repo, tag=args.tag, revision=args.revision,
                          notes=args.publish_draft)
    except (ValueError, OSError, KeyError, tarfile.TarError, subprocess.SubprocessError) as error:
        parser.exit(1, f'{error}\n')


if __name__ == '__main__':
    main()
