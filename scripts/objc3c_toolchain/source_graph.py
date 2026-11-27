"""Build a locked offline source-package graph with the installed native SDK."""
from __future__ import annotations

import argparse
from collections import deque
from concurrent.futures import FIRST_COMPLETED, ThreadPoolExecutor, wait
import fcntl
from graphlib import TopologicalSorter
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

if __package__:
    from .sdk_example import coroutine_compile_flags, native_module_compile_flags, runtime_link_flags, discover_lookup, dependencies, digest, environment, hashes, reusable, verify_installed_sdk, write_json
else:
    # Standalone helpers live in a sealed, possibly user-owned SDK. Keep
    # bytecode out of that installation without changing library callers.
    sys.dont_write_bytecode = True
    from sdk_example import coroutine_compile_flags, native_module_compile_flags, runtime_link_flags, discover_lookup, dependencies, digest, environment, hashes, reusable, verify_installed_sdk, write_json

MAX_EXPORT_NAMES = 4096
WORKSPACE = 'objc3c.source-workspace.v4'
PACKAGE = 'objc3c.source-package.v4'
REPORT = 'objc3c.source-graph-build.v4'
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
    canonical = []
    for value in values:
        value = dict(value) if isinstance(value, dict) else value
        fields(value, {'name', 'package', 'visibility'} | ({'spi'} if isinstance(value, dict) and 'spi' in value else set()), label)
        if 'spi' in value:
            groups = value['spi']
            if (not isinstance(groups, list) or not groups or len(groups) > 64 or
                    any(not isinstance(g, str) or len(g) > 64 or not re.fullmatch(r'[A-Za-z_]\w*', g, re.ASCII) for g in groups) or
                    len(set(groups)) != len(groups)):
                raise ValueError(f'{label}: SPI grants require distinct ASCII group identifiers')
            value['spi'] = sorted(groups)
        name, identity = value['name'], value['package']
        if (not isinstance(name, str) or not NAME.fullmatch(name) or name in seen or
                not isinstance(identity, str) or not identity.strip() or
                value['visibility'] not in ('public', 'private')):
            raise ValueError(f'{label}: invalid or duplicate dependency lock')
        seen.add(name)
        canonical.append(value)
    return sorted(canonical, key=lambda row: row['name'])



def validate_mode(config: dict, source: Path) -> None:
    language, standard, ownership = config['language'], config['standard'], config['ownership']
    suffixes = {'objective-c': ('.m', '.objc3'), 'objective-c++': ('.mm',),
                'c++': ('.cpp', '.cc', '.cxx')}
    if not isinstance(language, str) or language not in suffixes or source.suffix not in suffixes[language]:
        raise ValueError('source language must match its Objective-C, Objective-C++, or C++ suffix')
    standards = ('c17',) if language == 'objective-c' else ('c++20', 'c++23')
    if standard not in standards:
        raise ValueError(f'unsupported standard for {language}: {standard}')
    if ownership not in (('none',) if language == 'c++' else ('arc', 'mrc')):
        raise ValueError('ownership must be arc/mrc for Objective-C modes or none for C++')


def metaprogramming_inputs(config: dict, root: Path) -> list[Path]:
    if 'metaprogramming' not in config:
        return []
    value = config['metaprogramming']
    fields(value, {'packages', 'lock'}, 'metaprogramming')
    packages = value['packages']
    if (config['language'] not in ('objective-c', 'objective-c++') or
            not isinstance(packages, list) or not 1 <= len(packages) <= 64):
        raise ValueError('metaprogramming requires 1..64 explicit Objective-C package manifests')
    manifests = [within(root, name, 'metaprogramming manifest') for name in packages]
    if len(set(manifests)) != len(manifests) or any(path.name != 'objc3-meta.json' for path in manifests):
        raise ValueError('metaprogramming manifests must be distinct objc3-meta.json files')
    lock = within(root, value['lock'], 'metaprogramming lock')
    if lock in manifests:
        raise ValueError('metaprogramming lock must be separate from its manifests')
    return [*manifests, lock]


def validate_provider(config: dict) -> None:
    mode = config['provider_ownership']
    language = config.get('provider_language', config['language'])
    if 'provider_language' in config and (language, config['language']) != ('c++', 'objective-c++'):
        raise ValueError('language projection requires a C++ provider and an Objective-C++ consumer')
    if mode not in (('none',) if language == 'c++' else ('arc', 'mrc')):
        raise ValueError('provider ownership must be explicit and match the language')
    if mode != config['ownership'] and config['dependencies']:
        raise ValueError('ownership lock projection requires a closed provider without dependencies')


def validate_exports(values) -> list[str]:
    if (not isinstance(values, list) or not values or len(values) > MAX_EXPORT_NAMES or
            any(not isinstance(name, str) or not name or len(name.encode('utf-8')) > 256 or
                any(ord(char) < 33 or ord(char) == 127 for char in name) for name in values) or
            len(set(values)) != len(values) or ('*' in values and len(values) != 1)):
        raise ValueError('exports require distinct declaration names or a single wildcard')
    return sorted(values)


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
        optional = {'metaprogramming', 'provider_language'} & config.keys() if isinstance(config, dict) else set()
        fields(config, {'schema', 'name', 'package', 'source', 'ownership', 'provider_ownership', 'exports', 'language', 'standard', 'dependencies'} | optional, str(path))
        config['exports'] = validate_exports(config['exports'])
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
        if 'provider_language' in config:
            validate_mode(config, Path('consumer.mm'))
            validate_mode({**config, 'language': config['provider_language'],
                           'ownership': config['provider_ownership']}, source)
            if 'metaprogramming' in config:
                raise ValueError('a plain C++ ABI projection cannot invoke Objective-C metaprogramming')
        else:
            validate_mode(config, source)
        config['dependencies'] = locks(config['dependencies'], module)
        validate_provider(config)
        metaprogramming_inputs(config, directory)
        packages[module] = {**config, 'root': directory, 'input': source, 'manifest': path}
        manifests[path] = digest(path)
    app = data['application']
    fields(app, {'root', 'source', 'ownership', 'language', 'standard', 'dependencies'} | ({'metaprogramming'} if isinstance(app, dict) and 'metaprogramming' in app else set()), 'application')
    app_root = within(root, app['root'], 'application root', directory=True)
    if any(app_root.is_relative_to(other) or other.is_relative_to(app_root) for other in source_roots):
        raise ValueError('application and package source roots must be separate')
    app = {**app, 'root': app_root, 'input': within(app_root, app['source'], 'application source'),
           'dependencies': locks(app['dependencies'], 'application')}
    validate_mode(app, app['input'])
    metaprogramming_inputs(app, app_root)
    ordered, active, visited = [], set(), set()

    def visit(config):
        for dependency in config['dependencies']:
            name = dependency['name']
            if name not in packages or packages[name]['package'] != dependency['package']:
                raise ValueError(f'package identity lock cannot be resolved: {name}')
            selected = packages[name]
            if selected['language'] != config['language']:
                raise ValueError(f'language lock differs across package edge: {name}')
            if selected['standard'] != config['standard']:
                raise ValueError(f'standard lock differs across package edge: {name}')
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


def build_graph(sdk: Path, workspace: Path, output: Path, optimization='O2', *, jobs: int = 1) -> dict:
    if type(jobs) is not int or jobs < 1:
        raise ValueError('jobs must be a positive integer')
    sdk = sdk.resolve(strict=True)
    graph = load_graph(workspace)
    output = output.resolve()
    if optimization not in ('O0', 'O2'):
        raise ValueError('optimization must be O0 or O2')
    # Build directories may live in the workspace, but never inside (or above)
    # a source root. Protect unused packages as well as the selected closure.
    protected = [sdk, graph['application']['root'],
                 *[config['root'] for config in graph['packages'].values()]]
    for source in protected:
        if output.is_relative_to(source) or source.is_relative_to(output):
            raise ValueError('build output must be separate from the SDK and every source root')
    output.mkdir(parents=True, exist_ok=True)
    with (output / '.build.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        return _build(sdk, graph, output, optimization, jobs)


def dependency_graph(configs: dict) -> dict:
    return {name: tuple(edge['name'] for edge in config['dependencies'])
            for name, config in configs.items()}


def workspace_plan(workspace: Path) -> dict:
    graph = load_graph(workspace)
    configs, order = graph['packages'], graph['order']
    selected = {name: configs[name] for name in order}
    schedule = TopologicalSorter(dependency_graph({**selected, APP: graph['application']}))
    schedule.prepare()
    stages = []
    while schedule.is_active():
        ready = schedule.get_ready()
        stages.append(list(ready))
        schedule.done(*ready)

    workspace = graph['workspace']
    root = workspace.parent
    mode = ('language', 'standard', 'ownership')
    keys = (*mode, 'dependencies')
    app = {key: graph['application'][key] for key in keys}
    app['source'] = graph['application']['input'].relative_to(root).as_posix()
    packages = {}
    for name, config in sorted(configs.items()):
        packages[name] = {key: config[key] for key in (*keys, 'package', 'provider_ownership', 'exports')}
        if 'provider_language' in config:
            packages[name]['provider_language'] = config['provider_language']
        packages[name]['manifest'] = config['manifest'].relative_to(root).as_posix()
        packages[name]['source'] = config['input'].relative_to(root).as_posix()
    return {'schema': 'objc3c.source-workspace-plan.v1', 'status': 'PASS',
            'workspace': workspace.name, 'application': app, 'packages': packages,
            'selected': order, 'unused': sorted(configs.keys() - selected.keys()),
            'readiness_stages': stages,
            'manifests': {path.relative_to(root).as_posix(): signature
                          for path, signature in sorted(graph['manifests'].items())}}


def _build_nodes(configs, compile_node, jobs):
    dependencies = dependency_graph(configs)
    schedule = TopologicalSorter(dependencies)
    schedule.prepare()
    ready = deque(schedule.get_ready())
    results = {}
    active = {}
    with ThreadPoolExecutor(max_workers=min(jobs, len(configs))) as workers:
        while ready or active:
            while ready and len(active) < jobs:
                name = ready.popleft()
                imports = {module: results[module][1] for module in dependencies[name]}
                active[workers.submit(compile_node, name, configs[name], imports)] = name
            finished, _ = wait(active, return_when=FIRST_COMPLETED)
            for completed in finished:
                name = active.pop(completed)
                results[name] = completed.result()
                schedule.done(name)
            ready.extend(schedule.get_ready())
    return {name: results[name] for name in configs}


def _build(sdk: Path, graph: dict, output: Path, optimization: str, jobs: int) -> dict:
    started = time.monotonic()
    env = environment(sdk)
    env.pop('LD_AUDIT', None)

    def verify():
        verify_installed_sdk(sdk, env=env)

    verify()
    sdk_data = json.loads((sdk / 'objc3-sdk.json').read_text())
    target, seal = sdk_data['target'], sdk_data['seal_sha256']
    report_path = output / 'build-report.json'
    prior = json.loads(report_path.read_text()) if report_path.exists() else {}
    previous = prior.get('nodes', {}) if prior.get('schema') == REPORT else {}
    packages, app = graph['packages'], graph['application']
    configs = {name: packages[name] for name in graph['order']}
    configs[APP] = app
    inputs = {name: source_inputs(config['root']) for name, config in configs.items()}
    recipe = hashes([Path(__file__), Path(__file__).with_name('sdk_example.py')])
    directories = {name: output / 'modules' / name for name in graph['order']}
    paths = {name: directories[name] / (name + '.objc3i') for name in directories}
    for directory in directories.values():
        directory.mkdir(parents=True, exist_ok=True)
    directories[APP] = output / 'application'
    directories[APP].mkdir(exist_ok=True)

    def compile_node(name, config, imported_identities):
        directory = directories[name]
        obj, depfile = directory / 'module.o', directory / 'module.d'
        command = [str(sdk / 'bin/objc3c'), str(config['input']),
                   '--target', target, '--emit-object', str(obj)]
        provider_language = config.get('provider_language', config['language'])
        if provider_language != 'c++':
            command += ['--objc3', '--runtime', 'gnustep-2.2']
        if provider_language == 'objective-c++':
            command.append('--objcxx')
        provider_mode = config.get('provider_ownership', config['ownership'])
        if provider_mode == 'arc':
            command.append('--arc')
        if provider_language != config['language']:
            command += ['--project-objcxx', config['ownership']]
        elif provider_mode != config['ownership']:
            command += ['--project-ownership', config['ownership']]
        meta_inputs = metaprogramming_inputs(config, config['root'])
        if meta_inputs:
            for manifest in meta_inputs[:-1]:
                command += ['--meta-package', str(manifest)]
            command += ['--meta-lock', str(meta_inputs[-1])]
        products = [obj, depfile]
        if name != APP:
            interface = paths[name]
            sidecar = interface.with_suffix('.objc3i.json')
            command += ['--emit-interface', str(interface), '--module', name, '--package', config['package'], '--module-owned-names', '--module-source-root', str(config['root'])]
            products += [interface, sidecar]
        direct = config['dependencies']
        # Every transitive artifact is physically available, while only direct
        # roots and native re-export edges grant semantic visibility.
        imported_inputs = []
        closure = set()
        if direct:
            if config['language'] != 'c++':
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
                identity = imported_identities[module]
                command += ['--import-module', module, identity['semantic_fingerprint'], dependency['package']]
                command.append('--reexport-import' if dependency['visibility'] == 'public' else '--private-import')
                for group in dependency.get('spi', []):
                    command += ['--import-spi', group]
        if direct or meta_inputs:
            dependency_record = directory / 'dependencies.json'
            command += ['--emit-dependency-record', str(dependency_record)]
            products.append(dependency_record)
        if name != APP:
            for declaration in config['exports']:
                command += ['--export-declaration', declaration]
        command += ['--', '--no-default-config', '-std=' + config['standard'], *coroutine_compile_flags(config['language']),
                    *native_module_compile_flags(provider_language, output), '-fblocks', '-' + optimization,
                    '-isystem', str(sdk / 'include'),
                    '-MD', '-MF', str(depfile), '-MT', 'dependencies']
        flags = command[command.index('--') + 1:]
        modules = sorted(closure)
        interfaces = [path for path in imported_inputs if path.suffix == '.objc3i']
        lookup_inputs = discover_lookup(sdk, config, flags, target, output, directory, env, modules, interfaces)
        # A source implementation edit rebuilds that package. Importers depend
        # on interface contents rather than the implementation object bytes.
        imported_hashes = hashes(imported_inputs)
        key = hashlib.sha256(json.dumps({'sdk_seal': seal, 'recipe': recipe, 'argv': command,
              'sources': inputs[name], 'imports': imported_hashes, 'locks': direct,
              'header_lookup': lookup_inputs}, sort_keys=True).encode()).hexdigest()
        saved = previous.get(name, {})
        tick = time.monotonic()
        if reusable(saved, key):
            node = {**saved, 'reused': True}
        else:
            log = directory / 'compile.log'
            outcome = directory / 'compile-result.json'
            outcome.unlink(missing_ok=True)
            with log.open('w') as stream:
                completed = subprocess.run(command, cwd=output, env=env, stdout=stream,
                                           stderr=subprocess.STDOUT, check=False)
            outcome.write_text(json.dumps({'argv': command, 'exit_code': completed.returncode}) + '\n')
            completed.check_returncode()
            tracked = dependencies(depfile, output) + list(map(Path, inputs[name])) + imported_inputs
            node = {'key': key, 'reused': False, 'argv': command,
                    'header_lookup': lookup_inputs,
                    'outputs': hashes(products), 'dependencies': hashes(tracked)}
        node['elapsed_seconds'] = round(time.monotonic() - tick, 6)
        identity = None
        if name != APP:
            identity = json.loads(sidecar.read_text())['module']
            runtime = ('no-objc-runtime' if config['language'] == 'c++' else 'gnustep-2.2') + ';' + ('arc' if config['ownership'] == 'arc' else 'manual-reference-counting')
            if (identity['canonical_name'] != name or identity['package_identity'] != config['package'] or
                    identity['target_triple'] != target or identity['runtime_abi'] != runtime or
                    ';language=' + config['language'] + ';' not in identity['language_profile'] or
                    ';standard=' + config['standard'] + ';' not in identity['language_profile']):
                raise ValueError(f'compiled interface identity differs from package lock: {name}')
        return node, identity, (flags, modules, interfaces)

    # Preflight has succeeded. Clear previous completion records before any
    # worker can change a product and join all workers before releasing the lock.
    inventory_path = output / 'package-installation.json'
    report_path.unlink(missing_ok=True)
    inventory_path.unlink(missing_ok=True)
    built = _build_nodes(configs, compile_node, jobs)
    nodes = {name: row[0] for name, row in built.items()}
    identities = {name: built[name][1] for name in graph['order']}
    executable = output / 'application.bin'
    # System CRT/library selection is refreshed on every invocation.
    subprocess.run([str(sdk / 'bin/clang++'), '--no-default-config', '--target=' + target,
                    '-fuse-ld=lld', *[str(directories[name] / 'module.o') for name in graph['order']],
                    str(directories[APP] / 'module.o'), '-L' + str(sdk / 'lib'), *runtime_link_flags(sdk),
                    '-Wl,-rpath,' + str(sdk / 'lib'), '-o', str(executable)],
                   env=env, check=True)
    run_env = dict(env)
    run_env.pop('LD_LIBRARY_PATH', None)
    subprocess.run([str(executable)], env=run_env, check=True, timeout=30)
    if hashes(graph['manifests']) != {str(path): sha for path, sha in graph['manifests'].items()}:
        raise ValueError('workspace/package manifests changed during compilation')
    for name, config in configs.items():
        node, _, (flags, modules, interfaces) = built[name]
        if (source_inputs(config['root']) != inputs[name] or not reusable(node, node['key'])
                or discover_lookup(sdk, config, flags, target, output, directories[name], env, modules, interfaces)
                != node['header_lookup']):
            raise ValueError(f'package inputs or products changed during compilation: {name}')
    verify()
    # Relocatable installation inventory: identities and direct visibility locks
    # accompany content hashes of each installed object/interface/sidecar.
    inventory = {'schema': 'objc3c.package-installation.v4', 'sdk_seal': seal,
                 'target': target, 'optimization': optimization,
                 'application_mode': {key: app[key] for key in ('language', 'standard', 'ownership')}, 'packages': [
                     {'module': identities[name], 'language': packages[name]['language'],
                      'standard': packages[name]['standard'], 'ownership': packages[name]['ownership'],
                      'provider_ownership': packages[name]['provider_ownership'], 'exports': packages[name]['exports'],
                      **({'provider_language': packages[name]['provider_language']} if 'provider_language' in packages[name] else {}),
                      'dependencies': packages[name]['dependencies'],
                      'artifacts': {str(path.relative_to(output)): digest(path) for path in
                                    (directories[name] / 'module.o', paths[name], paths[name].with_suffix('.objc3i.json'))}}
                     for name in graph['order']]}
    inventory['content_sha256'] = hashlib.sha256(json.dumps(inventory, sort_keys=True).encode()).hexdigest()
    write_json(inventory_path, inventory)
    report = {'schema': REPORT, 'status': 'PASS', 'sdk_seal': seal, 'target': target,
              'workspace_manifests': {str(path): sha for path, sha in graph['manifests'].items()},
              'nodes': nodes, 'order': graph['order'], 'modules': identities,
              'executable': str(executable), 'executable_sha256': digest(executable),
              'installation': inventory, 'jobs': jobs, 'elapsed_seconds': round(time.monotonic() - started, 6)}
    write_json(report_path, report)
    return report


def add_build_arguments(parser):
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--workspace', type=Path, required=True)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--output-dir', type=Path)
    action.add_argument('--plan', action='store_true', help='validate and print the locked graph without building')
    parser.add_argument('--optimization', choices=('O0', 'O2'), default='O2')
    parser.add_argument('--jobs', type=int, default=1, help='maximum concurrent package compile workers')


def run_workspace(args):
    if args.plan:
        return workspace_plan(args.workspace)
    sdk = args.sdk_root
    if sdk is None:
        default = (Path.cwd() / 'out/toolchains/linux-sdk/current' if __package__
                   else Path(__file__).resolve().parents[3])
        sdk = Path(os.environ.get('OBJC3C_SDK_ROOT', default))
    report = build_graph(sdk, args.workspace, args.output_dir, args.optimization, jobs=args.jobs)
    return {'status': report['status'], 'report': str(args.output_dir / 'build-report.json'),
            'reused': [name for name, node in report['nodes'].items() if node['reused']]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    add_build_arguments(parser)
    args = parser.parse_args()
    try:
        result = run_workspace(args)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(1, f'package build failed: {error}\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
