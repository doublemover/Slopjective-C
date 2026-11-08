"""Plan and admit versioned Linux dependency caches without product build outputs."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import stat
import subprocess

from . import source_cache

SCHEMA = 'objc3c.linux-dependency-cache.v1'


def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def identity(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def layout(repo: Path, root: Path, *, include_swift=False):
    manifest = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())
    tools = manifest['host_tools']
    script = repo / 'scripts/setup_cloud_environment.sh'
    def recipe(*functions):
        value = subprocess.check_output(['bash', '-c',
            'source "$1"; shift; declare -f "$@"', 'cache-plan', str(script),
            'download_file', 'download_verified', 'host_version', *functions], text=True)
        return hashlib.sha256(value.encode()).hexdigest()
    entries = {}
    def add(name, paths, inputs):
        entries[name] = {'key': 'objc3-deps-v1-debian13-x64-' + name + '-' + identity(inputs),
                         'paths': [str(path) for path in paths],
                         'seal': str(root / '.dependency-cache' / (name + '.json'))}
    llvm = manifest['upstream']['tag'].removeprefix('llvmorg-')
    add('llvm', [root / ('llvm-' + llvm)], {
        'recipe': recipe('install_llvm', 'extract_llvm', 'prune_llvm'),
        'upstream': manifest['upstream']})
    add('python', [root / 'python', root / 'venv315'], {
        'recipe': recipe('install_python'), 'prefix': str(root),
        'versions': {key: tools[key] for key in ('python', 'uv', 'pip')},
        'lock': digest(repo / 'uv.lock')})
    add('node', [root / ('node-v' + tools['node'] + '-linux-x64')], {
        'recipe': recipe('install_node'), 'node': tools['node'], 'npm': tools['npm']})
    add('cmake', [root / ('cmake-' + tools['cmake'] + '-linux-x86_64')], {
        'recipe': recipe('install_cmake'), 'installer': digest(script.parent / 'objc3c_toolchain/cmake_linux.sh'),
        'version': tools['cmake']})
    add('ccache', [root / ('ccache-' + tools['ccache'] + '-linux-x86_64-glibc')], {
        'recipe': recipe('install_ccache'), 'version': tools['ccache']})
    profile = json.loads((repo / 'scripts/objc3c_toolchain_runtime/runtime-profile.json').read_text())['profiles']['linux-x86_64']
    sources = [
        {'path': str(repo / 'out/toolchains/upstream-cache.git'),
         'url': manifest['upstream']['url'], 'tag': manifest['upstream']['tag'],
         'commit': manifest['upstream']['commit'], 'tag_object': manifest['upstream']['tag_object']},
        {'path': str(repo / ('out/toolchains/source-cache/libobjc2-v' + profile['runtime_release'] + '.git')),
         'url': profile['runtime_source'], 'tag': 'v' + profile['runtime_release'], 'commit': profile['runtime_commit']},
        {'path': str(repo / ('out/toolchains/source-cache/robin-map-' + profile['robin_map_release'] + '.git')),
         'url': profile['robin_map_source'], 'tag': profile['robin_map_release'], 'commit': profile['robin_map_commit']},
    ]
    entries['sources'] = {'key': 'objc3-deps-v1-debian13-x64-sources-' + identity({
        'pins': [{key: value for key, value in item.items() if key != 'path'} for item in sources],
        'owner': digest(Path(source_cache.__file__))}), 'paths': [item['path'] for item in sources]}
    # Package caches contain verified download objects, never node_modules or a product SDK.
    entries['npm'] = {'key': 'objc3-deps-v1-debian13-x64-npm-' + digest(repo / 'package-lock.json'),
                      'paths': [str(root / 'npm-cache')]}
    if include_swift:
        pin = json.loads((repo / 'toolchains/swift/manifest.json').read_text())
        entries['swift'] = {'key': 'objc3-deps-v1-debian13-x64-swift-' + identity(pin),
                            'paths': [str(root / ('swift-' + pin['version']))]}
    return {'schema': SCHEMA, 'root': str(root), 'repo': str(repo),
            'entries': entries, 'sources': sources, 'include_swift': include_swift}


def quick_layout(repo: Path, root: Path):
    tools = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())['host_tools']
    python = identity({key: tools[key] for key in ('python', 'uv')})
    cmake = identity({'version': tools['cmake'],
        'installer': digest(repo / 'scripts/objc3c_toolchain/cmake_linux.sh')})
    return {'schema': SCHEMA, 'root': str(root), 'repo': str(repo), 'sources': [],
        'include_swift': False, 'entries': {
            'interpreter': {'key': 'objc3-deps-v1-ubuntu26-x64-python-' + python,
                'paths': [str(Path.home() / '.local/share/uv/python')]},
            'python_packages': {'key': 'objc3-deps-v1-ubuntu26-x64-python-packages-' + python + '-' + digest(repo / 'uv.lock'),
                'paths': [str(Path.home() / '.cache/uv'), str(Path.home() / '.cache/pip')]},
            'cmake': {'key': 'objc3-deps-v1-ubuntu26-x64-cmake-' + cmake,
                'paths': [str(root / 'objc3-tooling-cmake')],
                'seal': str(root / 'objc3-tooling-cmake-seal.json')}}}


def inventory(paths):
    result = {}
    roots = [Path(path).resolve() for path in paths]
    for index, directory in enumerate(map(Path, paths)):
        if directory.is_symlink() or not directory.is_dir():
            raise ValueError('dependency cache root is absent or is a link: ' + str(directory))
        for path in sorted(directory.rglob('*')):
            name = str(index) + '/' + path.relative_to(directory).as_posix()
            mode = stat.S_IMODE(path.lstat().st_mode)
            if path.is_symlink():
                # Python venv links may point into the separately cached interpreter.
                target = path.resolve(strict=True)
                if not any(target.is_relative_to(root) for root in roots):
                    raise ValueError('dependency cache link escaped its admitted roots: ' + name)
                result[name] = {'link': os.readlink(path), 'mode': mode}
            elif path.is_file():
                result[name] = {'sha256': digest(path), 'mode': mode}
            elif not path.is_dir():
                raise ValueError('unexpected dependency cache member: ' + str(path))
    return result


def admit(plan):
    admitted = []
    for name, entry in plan['entries'].items():
        if 'seal' not in entry:
            continue
        seal = Path(entry['seal'])
        present = any(Path(path).exists() for path in entry['paths'])
        if not present and not seal.exists():
            continue
        if not seal.is_file() or seal.is_symlink():
            raise ValueError('restored dependency has no admission seal: ' + name)
        record = json.loads(seal.read_text())
        if (record.get('schema') != SCHEMA or record.get('key') != entry['key'] or
                inventory(entry['paths']) != record.get('files')):
            raise ValueError('restored dependency differs from its pinned cache identity: ' + name)
        admitted.append(name)
    if 'swift' in plan['entries'] and Path(plan['entries']['swift']['paths'][0]).exists():
        from .swift_toolchain import verify
        verify(Path(plan['repo']), Path(plan['entries']['swift']['paths'][0]))
        admitted.append('swift')
    return {'status': 'PASS', 'admitted': admitted}


def seal(plan):
    for entry in plan['entries'].values():
        if 'seal' not in entry or Path(entry['seal']).exists():
            continue
        files = inventory(entry['paths'])
        path = Path(entry['seal']);path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps({'schema': SCHEMA, 'key': entry['key'], 'files': files}, sort_keys=True) + '\n')


def prepare_sources(plan, *, fetch):
    records = []
    for item in plan['sources']:
        records.append(source_cache.ensure(Path(item['path']),
            **{key: value for key, value in item.items() if key != 'path'}, fetch=fetch))
    return records


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['plan', 'admit', 'seal', 'sources'])
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--root', type=Path, default=Path('/opt/objc3c-host'))
    parser.add_argument('--plan', type=Path, required=True)
    parser.add_argument('--github-output', type=Path)
    parser.add_argument('--include-swift', action='store_true')
    parser.add_argument('--quick', action='store_true')
    parser.add_argument('--fetch', action='store_true')
    args = parser.parse_args(argv)
    if args.command == 'plan':
        value = (quick_layout(args.repo.resolve(), args.root.resolve()) if args.quick else
                 layout(args.repo.resolve(), args.root.resolve(), include_swift=args.include_swift))
        args.plan.write_text(json.dumps(value, indent=2) + '\n')
        if args.github_output:
            with args.github_output.open('a') as output:
                for name, entry in value['entries'].items():
                    output.write(f'{name}_key={entry["key"]}\n{name}_paths<<OBJC3_CACHE_PATHS\n')
                    output.write('\n'.join([*entry['paths'], *([entry['seal']] if 'seal' in entry else [])]))
                    output.write('\nOBJC3_CACHE_PATHS\n')
    else:
        value = json.loads(args.plan.read_text())
        if value.get('schema') != SCHEMA:
            raise ValueError('unknown dependency cache plan')
        if args.command == 'admit': print(json.dumps(admit(value)))
        elif args.command == 'seal': seal(value)
        else: print(json.dumps(prepare_sources(value, fetch=args.fetch)))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
