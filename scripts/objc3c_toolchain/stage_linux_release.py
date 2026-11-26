"""Assemble a draft release from two successful Linux SDK qualification runs."""

import argparse
from compression import zstd
from datetime import datetime
import errno
import gzip
import json
from pathlib import Path
import re
import shutil
import subprocess
import tarfile
import tempfile
import time
from urllib.request import urlopen

from .core import ToolchainError, planned_development_identity
from .package import SCHEMA, _digest, _file_hash, distribution_license
from . import release_install
from scripts.objc3c_toolchain_runtime.build_linux import runtime_profile


HOSTS = {'linux-x86_64': 'x86_64-unknown-linux-gnu',
         'linux-aarch64': 'aarch64-unknown-linux-gnu'}
INSTALLERS = {'install-linux.sh': 'scripts/install_objc3c_linux.sh',
              'install-sdk.py': 'scripts/objc3c_toolchain/install_sdk.py'}
MANIFEST = 'objc3-sdk/objc3-sdk.json'
ARCHIVE_LICENSE = 'objc3-sdk/share/licenses/slopjective-c/LICENSE'
FOUNDATION = 'objc3-sdk/include/clang/Basic/ObjC3Foundation.h'
QUALIFICATION = '.github/workflows/platform-host-evidence.yml'
TERMINAL_GATE = 'Require complete native SDK release evidence'
LLVM_MANIFEST = 'toolchains/llvm/manifest.json'
RELEASE_VISIBILITY_DELAYS = (0, 1, 2, 4, 8, 15)
ASSET_NAME = re.compile(r'[A-Za-z0-9][A-Za-z0-9._-]*')


def validate_run(run: dict, *, repo: str, revision: str, run_id: int) -> None:
    if (run.get('id') != run_id or run.get('path') != QUALIFICATION or
            run.get('event') != 'workflow_dispatch' or run.get('head_sha') != revision or
            (run.get('head_repository') or {}).get('full_name') != repo or
            type(run.get('run_attempt')) is not int or run['run_attempt'] < 1 or
            run.get('status') != 'completed' or run.get('conclusion') != 'success'):
        raise ValueError(f'run {run_id} is not successful release qualification for {repo}@{revision}')


def qualification_jobs(repo: str, run: dict) -> list[dict]:
    run_id, attempt, revision = run['id'], run['run_attempt'], run['head_sha']
    route = f'repos/{repo}/actions/runs/{run_id}/attempts/{attempt}/jobs?per_page=100'
    pages = [json.loads(line) for line in subprocess.check_output(
        ['gh', 'api', route, '--paginate', '--jq', '{total_count,jobs}']).splitlines()]
    jobs = [job for page in pages for job in page['jobs']]
    if not jobs or any(page['total_count'] != len(jobs) for page in pages):
        raise ValueError(f'run {run_id} attempt {attempt} has an incomplete job inventory')
    ids = set()
    checks = []
    for job in jobs:
        conclusion = job.get('conclusion')
        if (job.get('run_id') != run_id or job.get('run_attempt') != attempt or
                job.get('head_sha') != revision or job.get('status') != 'completed' or
                conclusion not in ('success', 'skipped') or type(job.get('id')) is not int or
                not job.get('name') or job['id'] in ids):
            raise ValueError(f'run {run_id} attempt {attempt} has a substituted or unsuccessful job')
        ids.add(job['id'])
        row = {key: job[key] for key in ('id', 'name', 'conclusion')}
        if conclusion == 'success':
            start = datetime.fromisoformat(job['started_at'])
            end = datetime.fromisoformat(job['completed_at'])
            row['started_at'] = job['started_at']
            row['completed_at'] = job['completed_at']
            row['seconds'] = int((end - start).total_seconds())
            if row['seconds'] < 0:
                raise ValueError(f'run {run_id} attempt {attempt} has invalid job timing')
        checks.append(row)
    gates = [row['conclusion'] for row in checks if row['name'] == TERMINAL_GATE]
    if gates != ['success']:
        raise ValueError(f'run {run_id} attempt {attempt} lacks the successful terminal release gate')
    return sorted(checks, key=lambda row: row['id'])


def clean_installs(root: Path, *, host: str, revision: str, run: int, attempt: int,
                   archive: dict) -> list[dict]:
    records = []
    for distribution in release_install.DISTRIBUTIONS:
        path = root / distribution.replace(':', '-') / 'result.json'
        if (path.is_symlink() or path.parent.is_symlink() or not path.is_file() or
                not path.resolve(strict=True).is_relative_to(root.resolve(strict=True))):
            raise ValueError(f'{host} clean-install receipt is missing or outside its owned directory')
        row = json.loads(path.read_bytes())
        release_install.validate_clean_install(row, host=host, distribution=distribution, revision=revision,
                                               run=run, attempt=attempt, archive=archive)
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
    if (download.is_symlink() or archive.is_symlink() or receipts[0].is_symlink() or
            not archive.resolve(strict=True).is_relative_to(download.resolve(strict=True)) or
            not receipts[0].resolve(strict=True).is_relative_to(download.resolve(strict=True))):
        raise ValueError(f'{host} downloaded archive or receipt is outside its owned directory')
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
    configuration = manifest.get('build_config_digest')
    if (manifest.get('schema') != SCHEMA or manifest.get('target') != HOSTS[host] or
            seal != receipt.get('seal_sha256') or seal != _digest(manifest) or
            not build_id or build_id != receipt.get('source_build_id') or
            not isinstance(configuration, str) or not re.fullmatch(r'[a-f0-9]{64}', configuration)):
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
        'build_config_digest': configuration,
    }


def download_source(url: str, output: Path, *, root: str, license_path: str) -> None:
    with urlopen(url, timeout=60) as response, output.open('xb') as stream:
        shutil.copyfileobj(response, stream)
    with gzip.open(output, 'rb') as contents, tarfile.open(fileobj=contents, mode='r|') as archive:
        found = False
        for member in archive:
            if not member.name.startswith(root + '/') and member.name != root:
                raise ValueError(f'{output.name} contains files outside the pinned source root')
            if member.name == f'{root}/{license_path}':
                if found or not member.isfile() or member.size == 0:
                    raise ValueError(f'{output.name} has an invalid license file')
                found = True
        if not found:
            raise ValueError(f'{output.name} is missing {license_path}')
        # Read past the end of the tar entries to verify the gzip trailer.
        while contents.read(1024 * 1024):
            pass


def admit_checkout(checkout: Path, *, revision: str, archives: dict) -> None:
    head = subprocess.check_output(['git', '-C', str(checkout), 'rev-parse', 'HEAD'], text=True).strip()
    if head != revision:
        raise ValueError('source checkout differs from the release revision')
    if subprocess.run(['git', '-C', str(checkout), 'diff', '--quiet', 'HEAD']).returncode:
        raise ValueError('source checkout has tracked changes')
    identity = planned_development_identity(checkout)
    archive_keys = {'upstream_commit': 'upstream_commit', 'patch_digest': 'patch_digest',
                    'source_build_id': 'build_id', 'build_config_digest': 'build_config_digest'}
    if any(row[archive_key] != identity[source_key] for row in archives.values()
           for archive_key, source_key in archive_keys.items()):
        raise ValueError('native archives differ from the pinned compiler source stack')


def source_inventory(checkout: Path, *, revision: str, tag: str, archives: dict) -> list[dict]:
    project = f'slopjective-c-{tag}-source.tar.gz'
    sources = [{'component': 'Slopjective-C', 'revision': revision, 'file': project}]
    llvm = json.loads((checkout / LLVM_MANIFEST).read_bytes())['upstream']
    dependencies = {('llvm-project', llvm['url'], llvm['commit'], 'llvm/LICENSE.TXT')}
    for host in archives:
        profile = runtime_profile(checkout, host)
        dependencies.update({
            ('libobjc2', profile['runtime_source'], profile['runtime_commit'], 'COPYING'),
            ('robin-map', profile['robin_map_source'], profile['robin_map_commit'], 'LICENSE'),
        })
    for component, remote, commit, license_path in sorted(dependencies):
        repository = re.fullmatch(r'https://github\.com/([A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+)\.git', remote)
        if (not repository or repository[1].rsplit('/', 1)[1] != component or
                not re.fullmatch(r'[0-9a-f]{40}', commit)):
            raise ValueError(f'{component} has an invalid source pin')
        url = f'https://codeload.github.com/{repository[1]}/tar.gz/{commit}'
        root = f'{component}-{commit}'
        sources.append({'component': component, 'revision': commit, 'file': f'{root}-source.tar.gz',
                        'url': url, 'root': root, 'license_path': license_path})
    return sources


def stage_sources(checkout: Path, bundle: Path, *, revision: str, tag: str,
                  archives: dict[str, dict]) -> list[dict]:
    sources = source_inventory(checkout, revision=revision, tag=tag, archives=archives)
    project = bundle / sources[0]['file']
    with project.open('xb') as stream:
        subprocess.run(['git', '-C', str(checkout), 'archive', '--format=tar.gz',
                        f'--prefix=Slopjective-C-{revision}/', revision], stdout=stream, check=True)
    for source in sources[1:]:
        download_source(source['url'], bundle / source['file'], root=source.pop('root'),
                        license_path=source.pop('license_path'))
    for source in sources:
        file = bundle / source['file']
        source.update(sha256=_file_hash(file), bytes=file.stat().st_size)
    return sources


def bundle_assets(output: Path) -> dict[str, tuple[str, int]]:
    if output.is_symlink() or not output.is_dir():
        raise ValueError('release needs an owned staged bundle')
    files = {path.name: path for path in output.iterdir()}
    if any(not ASSET_NAME.fullmatch(name) or path.is_symlink() or not path.is_file()
           for name, path in files.items()):
        raise ValueError('release bundle contains an invalid asset path')
    sums = files.get('SHA256SUMS')
    if sums is None or not 0 < sums.stat().st_size <= 64 * 1024:
        raise ValueError('release bundle needs its complete bounded checksum index')
    records = {}
    for line in sums.read_text().splitlines():
        pair = re.fullmatch(r'([a-f0-9]{64})  ([A-Za-z0-9][A-Za-z0-9._-]*)', line)
        if not pair or pair[2] in records:
            raise ValueError('release bundle has an invalid or duplicate checksum entry')
        records[pair[2]] = pair[1]
    if set(records) != set(files) - {'SHA256SUMS'} or 'linux-release.json' not in records:
        raise ValueError('release checksum index differs from the staged asset inventory')
    for name, signature in records.items():
        if _file_hash(files[name]) != signature:
            raise ValueError(f'release staged asset differs from its checksum: {name}')
    return {name: ('sha256:' + (records[name] if name in records else _file_hash(path)), path.stat().st_size)
            for name, path in files.items()}


def resume_staged(output: Path, checkout: Path, license_file: Path, report: dict) -> dict:
    assets = bundle_assets(output)
    existing = json.loads((output / 'linux-release.json').read_bytes())
    sources = source_inventory(checkout, revision=report['revision'], tag=report['tag'], archives=report['archives'])
    expected_names = {'linux-release.json', 'SHA256SUMS', 'LICENSE', *INSTALLERS,
                      *(row['file'] for row in report['archives'].values()), *(row['file'] for row in sources)}
    if set(assets) != expected_names:
        raise ValueError('staged release differs from the exact expected asset inventory')
    for source in sources:
        source.pop('root', None)
        source.pop('license_path', None)
        signature, size = assets[source['file']]
        source.update(sha256=signature.removeprefix('sha256:'), bytes=size)
    if existing != {**report, 'sources': sources}:
        raise ValueError('staged release differs from the exact qualified producers or source pins')
    for archive in report['archives'].values():
        if assets[archive['file']] != ('sha256:' + archive['sha256'], archive['bytes']):
            raise ValueError('staged SDK archive differs from the qualified producer')
    for name, source in {'LICENSE': license_file, **{label: checkout / filename for label, filename in INSTALLERS.items()}}.items():
        if (output / name).read_bytes() != source.read_bytes():
            raise ValueError(f'staged release file differs from the qualified revision: {name}')
    return existing


def stage(checkout: Path, *, repo: str, revision: str, tag: str,
          runs: dict[str, int], downloads: Path, output: Path) -> dict:
    if not re.fullmatch(r'[0-9a-f]{40}', revision):
        raise ValueError('release requires a full Git revision')
    if not re.fullmatch(r'v\d+\.\d+\.\d+(?:-[a-z0-9]+(?:[.-][a-z0-9]+)*)?', tag):
        raise ValueError('release requires a version tag such as v0.1.0-preview.1')
    if runs.keys() != HOSTS.keys() or len(set(runs.values())) != len(HOSTS):
        raise ValueError('release requires distinct x86-64 and AArch64 qualification runs')
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
                                   'clean_installs': installs,
                                   'qualification_jobs': qualification_jobs(repo, run)}
    source = {(record['upstream_commit'], record['patch_digest'], record['extension_abi'])
              for _, record in admitted.values()}
    if len(source) != 1:
        raise ValueError('native archives use different compiler source stacks')

    output.parent.mkdir(parents=True, exist_ok=True)
    report = {'schema': release_install.RELEASE_SCHEMA, 'tag': tag, 'revision': revision,
        'channel': 'preview', 'profiles_claimed': [], 'archives': {}}
    admit_checkout(checkout, revision=revision, archives={host: record for host, (_, record) in admitted.items()})
    for host, (_, record) in admitted.items():
        public = {key: value for key, value in record.items() if key != 'build_config_digest'}
        report['archives'][host] = {**public, 'file': f'objc3-sdk-{tag}-{host}.tar.zst',
            'qualification_run': runs[host], 'qualification_url': f'https://github.com/{repo}/actions/runs/{runs[host]}'}
    if output.exists() or output.is_symlink():
        return resume_staged(output, checkout, license_file, report)
    with tempfile.TemporaryDirectory(prefix='.release-', dir=output.parent) as directory:
        bundle = Path(directory) / 'bundle'
        bundle.mkdir()
        digests = {}
        for host, (archive, record) in admitted.items():
            name = report['archives'][host]['file']
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
        for name, path in INSTALLERS.items():
            shutil.copyfile(checkout / path, bundle / name)
        shutil.copyfile(license_file, bundle / 'LICENSE')
        report['sources'] = stage_sources(checkout, bundle, revision=revision, tag=tag,
                                          archives=report['archives'])
        digests.update((source['file'], source['sha256']) for source in report['sources'])
        (bundle / 'linux-release.json').write_text(json.dumps(report, indent=2) + '\n')
        checksums = [f'{digests.get(path.name) or _file_hash(path)}  {path.name}'
                     for path in sorted(bundle.iterdir())]
        (bundle / 'SHA256SUMS').write_text('\n'.join(checksums) + '\n')
        bundle.rename(output)
    return report


def publish_draft(output: Path, *, repo: str, tag: str, revision: str, notes: Path) -> None:
    if not re.fullmatch(r'[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+', repo):
        raise ValueError('draft publication needs the exact owner/repository')

    def api(route: str):
        return json.loads(subprocess.check_output(['gh', 'api', f'repos/{repo}/{route}']))

    def paged(route: str):
        rows = subprocess.check_output(['gh', 'api', f'repos/{repo}/{route}?per_page=100',
                                        '--paginate', '--jq', '.[]'])
        return map(json.loads, rows.splitlines())

    def find_release():
        matches = [row for row in paged('releases') if row['tag_name'] == tag]
        if len(matches) > 1:
            raise ValueError(f'{tag} identifies multiple releases')
        return matches[0] if matches else None

    expected = bundle_assets(output)
    manifest_prefix = '<!-- objc3c-release-manifest-sha256: '
    manifest_marker = f'{manifest_prefix}{expected["linux-release.json"][0][7:]} -->'
    bundle_prefix = '<!-- objc3c-release-bundle-sha256: '
    bundle_marker = f'{bundle_prefix}{expected["SHA256SUMS"][0][7:]} -->'

    def require_draft(release, release_id):
        lines = (release.get('body') or '').splitlines()
        manifest_markers = [line for line in lines if line.startswith(manifest_prefix)]
        bundle_markers = [line for line in lines if line.startswith(bundle_prefix)]
        if (type(release_id) is not int or release_id <= 0 or release.get('id') != release_id or
                release.get('tag_name') != tag or release.get('draft') is not True or
                release.get('prerelease') is not True or release.get('target_commitish') != revision or
                manifest_markers != [manifest_marker] or bundle_markers not in ([], [bundle_marker])):
            raise ValueError(f'{tag} does not identify the exact resumable preview draft')

    def verify_release(release_id):
        release = api(f'releases/{release_id}')
        require_draft(release, release_id)

    def uploaded(asset):
        return (asset.get('state') == 'uploaded' and
                (asset.get('digest'), asset['size']) == expected[asset['name']])

    def missing_assets(release_id):
        verify_release(release_id)
        missing = set(expected)
        seen = set()
        starters = []
        for asset in paged(f'releases/{release_id}/assets'):
            name = asset['name']
            if name not in expected or name in seen:
                raise ValueError(f'{tag} already has an unexpected or duplicate asset: {name}')
            seen.add(name)
            if uploaded(asset):
                missing.remove(name)
            elif (asset.get('state') == 'starter' and asset.get('size') == 0 and
                  asset.get('digest') in ('', None) and type(asset.get('id')) is int and asset['id'] > 0):
                starters.append(asset)
            else:
                raise ValueError(f'{tag} already has a mismatched asset: {name}')
        return sorted(missing), starters

    def verify_tag():
        refs = api(f'git/matching-refs/tags/{tag}')
        if (any(ref['ref'] == f'refs/tags/{tag}' for ref in refs) and
                api(f'commits/{tag}')['sha'] != revision):
            raise ValueError(f'{tag} already points to a different Git revision')

    verify_tag()
    release = find_release()
    if release is None:
        with tempfile.TemporaryDirectory(prefix='objc3-release-notes-') as directory:
            draft_notes = Path(directory) / 'notes.md'
            draft_notes.write_text(notes.read_text().rstrip() + f'\n\n{manifest_marker}\n{bundle_marker}\n')
            subprocess.run(['gh', 'release', 'create', tag, '--repo', repo, '--target', revision,
                            '--draft', '--prerelease', '--title', f'Objective-C 3.0 Linux SDK {tag}',
                            '--notes-file', str(draft_notes)], check=True)
        for delay in RELEASE_VISIBILITY_DELAYS:
            time.sleep(delay)
            release = find_release()
            if release is not None:
                break
        else:
            raise ValueError(f'{tag} was created but did not become visible in the release list')
    release_id = release.get('id')
    require_draft(release, release_id)
    missing, starters = missing_assets(release_id)
    if starters:
        for asset in starters:
            verify_release(release_id)
            route = f'releases/assets/{asset["id"]}'
            current = api(route)
            if current.get('id') == asset['id'] and current.get('name') == asset['name'] and uploaded(current):
                continue
            if current != asset:
                raise ValueError(f'{tag} interrupted asset changed before cleanup: {asset["name"]}')
            # GitHub may leave a zero-byte starter when upload fails. Only a
            # still-empty marker identified in this exact draft can be removed.
            subprocess.run(['gh', 'api', '--method', 'DELETE', f'repos/{repo}/{route}', '--silent'], check=True)
        for delay in RELEASE_VISIBILITY_DELAYS:
            time.sleep(delay)
            missing, starters = missing_assets(release_id)
            if not starters:
                break
        else:
            raise ValueError(f'{tag} interrupted assets are still visible')
    for name in missing:
        verify_release(release_id)
        require_draft(find_release() or {}, release_id)
        subprocess.run(['gh', 'release', 'upload', tag, '--repo', repo, str(output / name)], check=True)
    if missing:
        for delay in RELEASE_VISIBILITY_DELAYS:
            time.sleep(delay)
            if not missing_assets(release_id)[0]:
                break
        else:
            raise ValueError(f'{tag} draft is missing expected assets after upload')
    verify_tag()


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
    except (ValueError, OSError, EOFError, KeyError, ToolchainError, tarfile.TarError,
            subprocess.SubprocessError) as error:
        parser.exit(1, f'{error}\n')


if __name__ == '__main__':
    main()
