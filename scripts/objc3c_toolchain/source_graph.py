"""Build a locked offline source-package graph with the installed native SDK."""
from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

# The standalone builder imports helpers from a sealed, possibly user-owned
# SDK. Python bytecode caches must never modify that installation.
sys.dont_write_bytecode = True

if __package__:
    from .sdk_example import dependencies, digest, environment, hashes, reusable, write_json
else:
    from sdk_example import dependencies, digest, environment, hashes, reusable, write_json

WORKSPACE = 'objc3c.source-workspace.v1'
PACKAGE = 'objc3c.source-package.v1'
REPORT = 'objc3c.source-graph-build.v1'
APP = '@application'
NAME = re.compile(r'[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*', re.ASCII)


def fields(value: dict, required: set[str], label: str) -> None:
    if not isinstance(value, dict) or set(value) != required:
        raise ValueError(f'{label}: expected fields {sorted(required)}')


def within(root: Path, name: str, label: str, *, directory=False) -> Path:
    if not isinstance(name, str) or not name or Path(name).is_absolute():
        raise ValueError(f'{label}: expected a relative path')
    path = (root / name).resolve(strict=True)
    if not path.is_relative_to(root) or (not path.is_dir() if directory else not path.is_file()):
        raise ValueError(f'{label}: path must remain inside its source root')
    return path


def locks(values, label: str) -> list[dict]:
    if not isinstance(values, list):
        raise ValueError(f'{label}: dependencies must be an array')
    seen = set()
    for value in values:
        fields(value, {'name', 'package', 'visibility'}, label)
        name, identity = value['name'], value['package']
        if (not isinstance(name, str) or not NAME.fullmatch(name) or name in seen or
                not isinstance(identity, str) or not identity.strip() or
                value['visibility'] not in ('public', 'private')):
            raise ValueError(f'{label}: invalid or duplicate dependency lock')
        seen.add(name)
    return sorted(values, key=lambda row: row['name'])


def load_graph(workspace: Path) -> dict:
    workspace = workspace.resolve(strict=True)
    data = json.loads(workspace.read_text())
    fields(data, {'schema', 'packages', 'application'}, 'workspace')
    if data['schema'] != WORKSPACE or not isinstance(data['packages'], list):
        raise ValueError('unsupported source workspace schema')
    root = workspace.parent
    packages = {}
    source_roots = []
    manifests = {workspace: digest(workspace)}
    for name in data['packages']:
        path = within(root, name, 'package manifest')
        config = json.loads(path.read_text())
        fields(config, {'schema', 'name', 'package', 'source', 'ownership', 'dependencies'}, str(path))
        module = config['name']
        if (config['schema'] != PACKAGE or not isinstance(module, str) or not NAME.fullmatch(module) or
                module in packages or
                not isinstance(config['package'], str) or not config['package'].strip()):
            raise ValueError('invalid or duplicate source package identity')
        directory = path.parent
        if any(directory.is_relative_to(other) or other.is_relative_to(directory) for other in source_roots):
            raise ValueError('package source roots must be separate')
        source_roots.append(directory)
        source = within(directory, config['source'], 'package source')
        if source.suffix not in ('.m', '.objc3'):
            raise ValueError('source packages require Objective-C .m or .objc3 input')
        if config['ownership'] not in ('arc', 'mrc'):
            raise ValueError('ownership must be arc or mrc')
        config['dependencies'] = locks(config['dependencies'], module)
        packages[module] = {**config, 'root': directory, 'input': source, 'manifest': path}
        manifests[path] = digest(path)
    app = data['application']
    fields(app, {'root', 'source', 'ownership', 'dependencies'}, 'application')
    app_root = within(root, app['root'], 'application root', directory=True)
    if any(app_root.is_relative_to(other) or other.is_relative_to(app_root) for other in source_roots):
        raise ValueError('application and package source roots must be separate')
    if app['ownership'] not in ('arc', 'mrc'):
        raise ValueError('ownership must be arc or mrc')
    app = {**app, 'root': app_root, 'input': within(app_root, app['source'], 'application source'),
           'dependencies': locks(app['dependencies'], 'application')}
    if app['input'].suffix not in ('.m', '.objc3'):
        raise ValueError('applications require Objective-C .m or .objc3 input')
    ordered, active, visited = [], set(), set()

    def visit(config):
        for dependency in config['dependencies']:
            name = dependency['name']
            if name not in packages or packages[name]['package'] != dependency['package']:
                raise ValueError(f'package identity lock cannot be resolved: {name}')
            selected = packages[name]
            if selected['ownership'] != config['ownership']:
                raise ValueError(f'ownership lock differs across package edge: {name}')
            if name in active:
                raise ValueError(f'cyclic source-package dependency: {name}')
            if name not in visited:
                active.add(name)
                visit(selected)
                active.remove(name)
                visited.add(name)
                ordered.append(name)

    # Validate even unused declarations; build only the application's closure.
    for config in packages.values():
        visit(config)
    ordered.clear()
    visited.clear()
    visit(app)
    return {'workspace': workspace, 'packages': packages, 'application': app,
            'order': ordered, 'manifests': manifests}


def source_inputs(root: Path) -> dict:
    selected = []
    for path in sorted(root.rglob('*')):
        if path.is_symlink() and not path.resolve(strict=True).is_relative_to(root):
            raise ValueError(f'source link escapes package root: {path}')
        if path.is_symlink() and path.is_dir():
            raise ValueError(f'source directory links are unsupported: {path}')
        if path.is_file():
            selected.append(path)
    return hashes(selected)


def build_graph(sdk: Path, workspace: Path, output: Path, optimization='O2') -> dict:
    sdk = sdk.resolve(strict=True)
    graph = load_graph(workspace)
    output = output.resolve()
    if optimization not in ('O0', 'O2'):
        raise ValueError('optimization must be O0 or O2')
    # The workspace is immutable while compiling, including unused manifests.
    for source in (sdk, graph['workspace'].parent):
        if output.is_relative_to(source) or source.is_relative_to(output):
            raise ValueError('build output must be separate from the SDK and source workspace')
    output.mkdir(parents=True, exist_ok=True)
    with (output / '.build.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        return _build(sdk, graph, output, optimization)


def _build(sdk: Path, graph: dict, output: Path, optimization: str) -> dict:
    started = time.monotonic()
    env = environment(sdk)
    env.pop('LD_AUDIT', None)

    def verify():
        subprocess.run([sys.executable, '-B', str(sdk / 'share/objc3c/verify-sdk.py'), str(sdk)],
                       env=env, check=True, capture_output=True, timeout=120)

    verify()
    sdk_data = json.loads((sdk / 'objc3-sdk.json').read_text())
    target, seal = sdk_data['target'], sdk_data['seal_sha256']
    report_path = output / 'build-report.json'
    prior = json.loads(report_path.read_text()) if report_path.exists() else {}
    previous = prior.get('nodes', {}) if prior.get('schema') == REPORT else {}
    packages, app = graph['packages'], graph['application']
    configs = {name: packages[name] for name in graph['order']}
    inputs = {name: source_inputs(config['root']) for name, config in configs.items()}
    inputs[APP] = source_inputs(app['root'])
    recipe = hashes([Path(__file__), Path(__file__).with_name('sdk_example.py')])
    nodes, identities = {}, {}
    directories = {name: output / 'modules' / name for name in graph['order']}
    paths = {name: directories[name] / (name + '.objc3i') for name in directories}
    for directory in directories.values():
        directory.mkdir(parents=True, exist_ok=True)
    directories[APP] = output / 'application'
    directories[APP].mkdir(exist_ok=True)

    for name, config in [*configs.items(), (APP, app)]:
        directory = directories[name]
        obj, depfile = directory / 'module.o', directory / 'module.d'
        command = [str(sdk / 'bin/objc3c'), str(config['input']), '--objc3',
                   '--target', target, '--runtime', 'gnustep-2.0', '--emit-object', str(obj)]
        if config['ownership'] == 'arc':
            command.append('--arc')
        products = [obj, depfile]
        if name != APP:
            interface = paths[name]
            sidecar = interface.with_suffix('.objc3i.json')
            command += ['--emit-interface', str(interface), '--module', name, '--package', config['package']]
            products += [interface, sidecar]
        direct = config['dependencies']
        # Every transitive artifact is physically available, while only direct
        # roots and native re-export edges grant semantic visibility.
        imported_inputs = []
        if direct:
            command.append('--source-imports')
            closure = set()

            def include(module):
                if module not in closure:
                    closure.add(module)
                    for edge in packages[module]['dependencies']:
                        include(edge['name'])

            for edge in direct:
                include(edge['name'])
            for module in graph['order']:
                if module in closure:
                    command += ['--module-search-path', str(directories[module])]
                    imported_inputs += [paths[module], paths[module].with_suffix('.objc3i.json')]
            for dependency in direct:
                module = dependency['name']
                identity = identities[module]
                command += ['--import-module', module, identity['semantic_fingerprint'], dependency['package']]
                command.append('--reexport-import' if dependency['visibility'] == 'public' else '--private-import')
            dependency_record = directory / 'dependencies.json'
            command += ['--emit-dependency-record', str(dependency_record)]
            products.append(dependency_record)
        command += ['--', '--no-default-config', '-std=c17', '-fblocks', '-' + optimization,
                    '-MD', '-MF', str(depfile), '-MT', 'dependencies']
        # A source implementation edit rebuilds that package. Importers depend
        # on interface contents rather than the implementation object bytes.
        imported_hashes = hashes(imported_inputs)
        key = hashlib.sha256(json.dumps({'sdk_seal': seal, 'recipe': recipe, 'argv': command,
              'sources': inputs[name], 'imports': imported_hashes, 'locks': direct}, sort_keys=True).encode()).hexdigest()
        saved = previous.get(name, {})
        tick = time.monotonic()
        if reusable(saved, key):
            nodes[name] = {**saved, 'reused': True}
        else:
            report_path.unlink(missing_ok=True)
            log = directory / 'compile.log'
            with log.open('w') as stream:
                subprocess.run(command, cwd=output, env=env, stdout=stream, stderr=subprocess.STDOUT,
                               check=True, timeout=180)
            tracked = dependencies(depfile, output) + list(map(Path, inputs[name])) + imported_inputs
            nodes[name] = {'key': key, 'reused': False, 'argv': command,
                           'outputs': hashes(products), 'dependencies': hashes(tracked)}
        nodes[name]['elapsed_seconds'] = round(time.monotonic() - tick, 6)
        if name != APP:
            identity = json.loads(sidecar.read_text())['module']
            runtime = 'gnustep-2.0;' + ('arc' if config['ownership'] == 'arc' else 'manual-reference-counting')
            if (identity['canonical_name'] != name or identity['package_identity'] != config['package'] or
                    identity['target_triple'] != target or identity['runtime_abi'] != runtime):
                raise ValueError(f'compiled interface identity differs from package lock: {name}')
            identities[name] = identity

    executable = output / 'application.bin'
    # System CRT/library selection is refreshed on every invocation.
    report_path.unlink(missing_ok=True)
    subprocess.run([str(sdk / 'bin/clang++'), '--no-default-config', '--target=' + target,
                    '-fuse-ld=lld', *[str(directories[name] / 'module.o') for name in graph['order']],
                    str(directories[APP] / 'module.o'), '-L' + str(sdk / 'lib'), '-lobjc',
                    '-Wl,-rpath,' + str(sdk / 'lib'), '-o', str(executable)],
                   env=env, check=True, timeout=180)
    run_env = dict(env)
    run_env.pop('LD_LIBRARY_PATH', None)
    subprocess.run([str(executable)], env=run_env, check=True, timeout=30)
    if hashes(graph['manifests']) != {str(path): sha for path, sha in graph['manifests'].items()}:
        raise ValueError('workspace/package manifests changed during compilation')
    for name, config in [*configs.items(), (APP, app)]:
        if source_inputs(config['root']) != inputs[name] or not reusable(nodes[name], nodes[name]['key']):
            raise ValueError(f'package inputs or products changed during compilation: {name}')
    verify()
    # Relocatable installation inventory: identities and direct visibility locks
    # accompany content hashes of each installed object/interface/sidecar.
    inventory = {'schema': 'objc3c.package-installation.v1', 'sdk_seal': seal,
                 'target': target, 'optimization': optimization, 'packages': [
                     {'module': identities[name], 'dependencies': packages[name]['dependencies'],
                      'artifacts': {str(path.relative_to(output)): digest(path) for path in
                                    (directories[name] / 'module.o', paths[name], paths[name].with_suffix('.objc3i.json'))}}
                     for name in graph['order']]}
    inventory['content_sha256'] = hashlib.sha256(json.dumps(inventory, sort_keys=True).encode()).hexdigest()
    write_json(output / 'package-installation.json', inventory)
    report = {'schema': REPORT, 'status': 'PASS', 'sdk_seal': seal, 'target': target,
              'workspace_manifests': {str(path): sha for path, sha in graph['manifests'].items()},
              'nodes': nodes, 'order': graph['order'], 'modules': identities,
              'executable': str(executable), 'executable_sha256': digest(executable),
              'installation': inventory, 'elapsed_seconds': round(time.monotonic() - started, 6)}
    write_json(report_path, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--workspace', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--optimization', choices=('O0', 'O2'), default='O2')
    args = parser.parse_args()
    default = (Path.cwd() / 'out/toolchains/linux-sdk/current' if __package__ else Path(__file__).resolve().parents[3])
    sdk = args.sdk_root or Path(os.environ.get('OBJC3C_SDK_ROOT', default))
    try:
        report = build_graph(sdk, args.workspace, args.output_dir, args.optimization)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(1, f'package build failed: {error}\n')
    print(json.dumps({'status': report['status'], 'report': str(args.output_dir / 'build-report.json'),
                      'reused': [name for name, node in report['nodes'].items() if node['reused']]}))


if __name__ == '__main__':
    main()
