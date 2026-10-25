"""Publish, install and consume content-addressed offline native packages."""
from __future__ import annotations

import argparse
from contextlib import contextmanager
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

if __package__:
    from .sdk_example import coroutine_compile_flags, runtime_link_flags, dependencies, digest, environment, hashes, verify_installed_sdk, write_json
    from .source_graph import NAME, fields, locks, source_inputs, validate_mode, validate_provider, validate_exports, within
else:
    sys.dont_write_bytecode = True
    from sdk_example import coroutine_compile_flags, runtime_link_flags, dependencies, digest, environment, hashes, verify_installed_sdk, write_json
    from build import NAME, fields, locks, source_inputs, validate_mode, validate_provider, validate_exports, within

SCHEMA = 'objc3c.binary-package.v3'
CONSUMER = 'objc3c.binary-consumer.v3'
HASH = re.compile('[0-9a-f]{64}', re.ASCII)
MODE = ('language', 'standard', 'ownership')
IDENTITY = {'canonical_name', 'extension_abi', 'language_profile', 'language_revision',
            'package_identity', 'runtime_abi', 'semantic_fingerprint', 'target_triple'}


def content_hash(value: dict) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True).encode()).hexdigest()


def check_hash(value):
    if not isinstance(value, str) or not HASH.fullmatch(value):
        raise ValueError('expected a lowercase SHA-256 content identity')


def disjoint(output: Path, *protected: Path):
    if any(output.is_relative_to(root) or root.is_relative_to(output) for root in protected):
        raise ValueError('package output must be disjoint from the SDK and every input root')


@contextmanager
def lock(root: Path, name: str):
    root.mkdir(parents=True, exist_ok=True)
    with (root / name).open('a+b') as stream:
        fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
        yield


def sdk_identity(sdk: Path) -> dict:
    verify_installed_sdk(sdk)
    return json.loads((sdk / 'objc3-sdk.json').read_text())


def package_rows(manifest: dict) -> dict:
    fields(manifest, {'schema', 'sdk_seal', 'target', 'optimization', 'packages'}, 'binary package')
    if manifest['schema'] != SCHEMA or manifest['optimization'] not in ('O0', 'O2'):
        raise ValueError('unsupported binary package schema or optimization')
    check_hash(manifest['sdk_seal'])
    if not isinstance(manifest['target'], str) or not manifest['target']:
        raise ValueError('binary package requires a target')
    if not isinstance(manifest['packages'], list) or not manifest['packages']:
        raise ValueError('binary package must contain modules')
    rows = {}
    for row in manifest['packages']:
        fields(row, {*MODE, 'provider_ownership', 'exports', 'module', 'dependencies', 'artifacts'}, 'binary module')
        module = row['module']
        fields(module, IDENTITY, 'module identity')
        if any(not isinstance(value, str) or not value for value in module.values()):
            raise ValueError('incomplete native module identity')
        name = module['canonical_name']
        if not NAME.fullmatch(name) or name in rows:
            raise ValueError('invalid or duplicate binary module')
        check_hash(module['semantic_fingerprint'])
        suffix = {'objective-c': '.m', 'objective-c++': '.mm', 'c++': '.cpp'}.get(row['language'], '') if isinstance(row['language'], str) else ''
        validate_mode(row, Path('module' + suffix))
        validate_provider(row)
        if validate_exports(row['exports']) != row['exports']:
            raise ValueError('binary exports must be canonical')
        runtime = ('no-objc-runtime' if row['language'] == 'c++' else 'gnustep-2.2') + ';' + ('arc' if row['ownership'] == 'arc' else 'manual-reference-counting')
        if (module['target_triple'] != manifest['target'] or module['runtime_abi'] != runtime or
                ';language=' + row['language'] + ';' not in module['language_profile'] or
                ';standard=' + row['standard'] + ';' not in module['language_profile']):
            raise ValueError('binary module identity differs from its mode or target')
        paths = {f'modules/{name}/module.o', f'modules/{name}/{name}.objc3i', f'modules/{name}/{name}.objc3i.json'}
        if not isinstance(row['artifacts'], dict) or set(row['artifacts']) != paths:
            raise ValueError('binary module must contain exactly its object, interface and sidecar')
        for value in row['artifacts'].values():
            check_hash(value)
        if locks(row['dependencies'], name) != row['dependencies']:
            raise ValueError('binary dependency locks must be canonical')
        rows[name] = row
    closure(rows, list(rows))
    return rows


def closure(rows: dict, roots: list[str]) -> list[str]:
    active, visited, order = set(), set(), []

    def visit(name):
        if name in active:
            raise ValueError('cyclic binary package dependency')
        if name in visited:
            return
        if name not in rows:
            raise ValueError('unresolved binary package dependency')
        active.add(name)
        row = rows[name]
        for edge in row['dependencies']:
            dependency = rows.get(edge['name'])
            if (dependency is None or dependency['module']['package_identity'] != edge['package'] or
                    any(dependency[key] != row[key] for key in MODE)):
                raise ValueError('binary package dependency identity or mode lock differs')
            visit(edge['name'])
        active.remove(name)
        visited.add(name)
        order.append(name)

    for name in roots:
        visit(name)
    return order


def artifacts(manifest: dict) -> dict:
    return {name: value for row in manifest['packages'] for name, value in row['artifacts'].items()}


def verify_files(root: Path, manifest: dict, *, exact: bool):
    expected = artifacts(manifest)
    modules = {row['module']['canonical_name']: row for row in manifest['packages']}
    for name, value in expected.items():
        path = root / name
        if any(part.is_symlink() for part in (path, *path.parents) if part != root.parent):
            raise ValueError('binary package artifacts cannot use symbolic links')
        if not path.is_file() or digest(path) != value:
            raise ValueError('binary package artifact hash differs: ' + name)
    for row in manifest['packages']:
        name = row['module']['canonical_name']
        sidecar = json.loads((root / f'modules/{name}/{name}.objc3i.json').read_text())
        if not isinstance(sidecar, dict) or sidecar.get('module') != row['module']:
            raise ValueError('binary package sidecar differs from its module identity')
        if row['exports'] != ['*']:
            root_kinds = {'Function', 'Var', 'Record', 'CXXRecord', 'Enum', 'Typedef',
                          'TypeAlias', 'ObjCInterface', 'ObjCProtocol', 'ObjCCategory'}
            declarations = sidecar.get('declarations')
            if (not isinstance(declarations, list) or
                    any(not isinstance(declaration, dict) or
                        not isinstance(declaration.get('kind'), str) or
                        not isinstance(declaration.get('qualified_name'), str) for declaration in declarations) or
                    {declaration.get('qualified_name') for declaration in declarations
                     if declaration.get('kind') in root_kinds} != set(row['exports'])):
                raise ValueError('binary exports differ from the native public root declarations')
        projection = sidecar.get('ownership_projection')
        projected = row['provider_ownership'] != row['ownership']
        if (not isinstance(projection, str) or bool(projection) != projected or
                sidecar.get('schema_id') != 'objc3c.semantic-interface.v2' or
                sidecar.get('schema_major') != 2):
            raise ValueError('binary provider ownership differs from its native projection')
        if projected:
            producer = json.loads(projection)
            if not isinstance(producer, dict) or not isinstance(producer.get('module'), dict):
                raise ValueError('binary projection producer must have an object identity')
            identity = producer['module']
            producer_identity = dict(row['module'])
            producer_identity['canonical_name'] += '.__ownership_producer'
            producer_identity['runtime_abi'] = 'gnustep-2.2;' + ('arc' if row['provider_ownership'] == 'arc' else 'manual-reference-counting')
            producer_identity['language_profile'] = producer_identity['language_profile'].replace(
                ';arc=' + str(row['ownership'] == 'arc').lower() + ';',
                ';arc=' + str(row['provider_ownership'] == 'arc').lower() + ';')
            producer_identity['semantic_fingerprint'] = identity.get('semantic_fingerprint')
            if (identity != producer_identity or producer.get('schema_id') != 'objc3c.semantic-interface.v2' or producer.get('schema_major') != 2 or producer.get('dependencies') != [] or
                    producer.get('ownership_projection') != '' or
                    producer.get('source_binding_sha256') != sidecar.get('source_binding_sha256') or
                    producer.get('source_binding_sha256') != digest(root / f'modules/{name}/{name}.objc3i') or
                    not isinstance(sidecar.get('required_semantics'), list) or
                    'objc3.ownership-projection.v1' not in sidecar['required_semantics']):
                raise ValueError('binary projection producer proof differs from its ownership lock')
            check_hash(identity['semantic_fingerprint'])
        expected_dependencies = [
            {'module': modules[edge['name']]['module'],
             'public_import': edge['visibility'] == 'public',
             'reexported': edge['visibility'] == 'public'} for edge in row['dependencies']]
        actual_dependencies = sidecar.get('dependencies')
        if (not isinstance(actual_dependencies, list) or
                sorted(json.dumps(edge, sort_keys=True) for edge in actual_dependencies) !=
                sorted(json.dumps(edge, sort_keys=True) for edge in expected_dependencies)):
            raise ValueError('binary package visibility or dependency identity differs from its sidecar')
    if exact:
        found = set()
        for path in root.rglob('*'):
            if path.is_symlink() or (not path.is_file() and not path.is_dir()):
                raise ValueError('binary package contains a link or special file')
            if path.is_file():
                found.add(path.relative_to(root).as_posix())
        if found != set(expected) | {'package.json'}:
            raise ValueError('binary package contains missing or unlisted files')


def verify_package(root: Path, expected: str) -> dict:
    check_hash(expected)
    if root.is_symlink():
        raise ValueError('binary package root cannot be a symbolic link')
    manifest = json.loads((root / 'package.json').read_text())
    package_rows(manifest)
    if content_hash(manifest) != expected:
        raise ValueError('binary package content identity differs from the requested lock')
    verify_files(root, manifest, exact=True)
    return manifest


def copy_package(source: Path, manifest: dict, store: Path, before_publish=None) -> Path:
    identity = content_hash(manifest)
    with lock(store, '.package.lock'):
        destination = store / identity
        if destination.exists() or destination.is_symlink():
            verify_package(destination, identity)
            if before_publish:
                before_publish()
            return destination
        with tempfile.TemporaryDirectory(prefix='.package-', dir=store) as temporary:
            candidate = Path(temporary) / 'candidate'
            candidate.mkdir()
            for name in artifacts(manifest):
                target = candidate / name
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(source / name, target)
            write_json(candidate / 'package.json', manifest)
            verify_package(candidate, identity)
            verify_files(source, manifest, exact=(source / 'package.json').is_file())
            for path in candidate.rglob('*'):
                if path.is_file():
                    path.chmod(0o444)
            if before_publish:
                before_publish()
            candidate.rename(destination)
        return destination


def publish(sdk: Path, build: Path, store: Path) -> dict:
    sdk, build, store = sdk.resolve(strict=True), build.resolve(strict=True), store.resolve()
    disjoint(store, sdk, build)
    identity = sdk_identity(sdk)
    with lock(build, '.build.lock'):
        report = json.loads((build / 'build-report.json').read_text())
        inventory = json.loads((build / 'package-installation.json').read_text())
        fields(inventory, {'schema', 'sdk_seal', 'target', 'optimization', 'application_mode', 'packages', 'content_sha256'}, 'installation inventory')
        unhashed = {key: value for key, value in inventory.items() if key != 'content_sha256'}
        if (inventory['schema'] != 'objc3c.package-installation.v4' or content_hash(unhashed) != inventory['content_sha256'] or
                not isinstance(report, dict) or report.get('schema') != 'objc3c.source-graph-build.v4' or report.get('status') != 'PASS' or
                report.get('installation') != inventory):
            raise ValueError('publication requires a completed, matching source build inventory')
        manifest = {key: inventory[key] for key in ('sdk_seal', 'target', 'optimization', 'packages')}
        manifest['schema'] = SCHEMA
        package_rows(manifest)
        if manifest['sdk_seal'] != identity['seal_sha256'] or manifest['target'] != identity['target']:
            raise ValueError('binary publication requires the exact producing SDK')
        verify_files(build, manifest, exact=False)
        destination = copy_package(build, manifest, store, before_publish=lambda: sdk_identity(sdk))
        return {'status': 'PASS', 'content_sha256': content_hash(manifest), 'package': str(destination)}


def install(source: Path, expected: str, store: Path) -> dict:
    if source.is_symlink():
        raise ValueError('binary package root cannot be a symbolic link')
    source, store = source.resolve(strict=True), store.resolve()
    disjoint(store, source)
    manifest = verify_package(source, expected)
    destination = copy_package(source, manifest, store)
    verify_package(source, expected)
    return {'status': 'PASS', 'content_sha256': expected, 'package': str(destination)}


def consume(sdk: Path, package: Path, expected: str, application: Path, output: Path) -> dict:
    sdk, application, output = sdk.resolve(strict=True), application.resolve(strict=True), output.resolve()
    manifest = verify_package(package, expected)
    package = package.resolve(strict=True)
    root = application.parent
    disjoint(output, sdk, package, root)
    identity = sdk_identity(sdk)
    if manifest['sdk_seal'] != identity['seal_sha256'] or manifest['target'] != identity['target']:
        raise ValueError('consumer requires the exact producing SDK seal and target')
    config = json.loads(application.read_text())
    fields(config, {'schema', 'source', *MODE, 'dependencies'}, 'binary consumer')
    if config['schema'] != CONSUMER:
        raise ValueError('unsupported binary consumer schema')
    source = within(root, config['source'], 'consumer source')
    validate_mode(config, source)
    direct = locks(config['dependencies'], 'consumer')
    rows = package_rows(manifest)
    for edge in direct:
        row = rows.get(edge['name'])
        if (row is None or row['module']['package_identity'] != edge['package'] or
                any(row[key] != config[key] for key in MODE)):
            raise ValueError('consumer dependency identity or mode lock differs')
    order = closure(rows, [edge['name'] for edge in direct])
    sources = source_inputs(root)
    with lock(output, '.consumer.lock'):
        report_path = output / 'consumer-report.json'
        report_path.unlink(missing_ok=True)
        obj, executable, depfile = output / 'application.o', output / 'application.bin', output / 'application.d'
        command = [str(sdk / 'bin/objc3c'), str(source), '--target', manifest['target'], '--emit-object', str(obj)]
        if config['language'] != 'c++':
            command += ['--objc3', '--runtime', 'gnustep-2.2', '--source-imports']
        if config['language'] == 'objective-c++':
            command.append('--objcxx')
        if config['ownership'] == 'arc':
            command.append('--arc')
        for name in order:
            command += ['--module-search-path', str(package / 'modules' / name)]
        for edge in direct:
            row = rows[edge['name']]['module']
            command += ['--import-module', edge['name'], row['semantic_fingerprint'], edge['package'],
                        '--reexport-import' if edge['visibility'] == 'public' else '--private-import']
            for group in edge.get('spi', []):
                command += ['--import-spi', group]
        command += ['--', '--no-default-config', '-std=' + config['standard'], *coroutine_compile_flags(config['language']), '-fblocks',
                    '-' + manifest['optimization'], '-isystem', str(sdk / 'include'),
                    '-MD', '-MF', str(depfile), '-MT', 'dependencies']
        env = environment(sdk)
        env.pop('LD_AUDIT', None)
        commands = [command, [str(sdk / 'bin/clang++'), '--no-default-config', '--target=' + manifest['target'],
                    '-fuse-ld=lld', str(obj), *[str(package / 'modules' / name / 'module.o') for name in order],
                    '-L' + str(sdk / 'lib'), *runtime_link_flags(sdk), '-Wl,-rpath,' + str(sdk / 'lib'), '-o', str(executable)]]
        for index, argv in enumerate(commands):
            with (output / f'command-{index}.log').open('w') as stream:
                subprocess.run(argv, cwd=output, env=env, stdout=stream, stderr=subprocess.STDOUT, check=True, timeout=180)
            if index == 0:
                tracked = hashes(dependencies(depfile, output))
        run_env = dict(env)
        run_env.pop('LD_LIBRARY_PATH', None)
        with (output / 'execution.log').open('w') as stream:
            subprocess.run([str(executable)], cwd=output, env=run_env, stdout=stream, stderr=subprocess.STDOUT, check=True, timeout=30)
        verify_package(package, expected)
        sdk_identity(sdk)
        if source_inputs(root) != sources:
            raise ValueError('consumer sources changed during compilation')
        if hashes(map(Path, tracked)) != tracked:
            raise ValueError('consumer header dependencies changed during compilation')
        report = {'schema': 'objc3c.binary-consumer-build.v3', 'status': 'PASS', 'package_content_sha256': expected,
                  'sdk_seal': identity['seal_sha256'], 'modules': order, 'sources': sources, 'dependencies': tracked, 'commands': commands,
                  'provider_compilations': 0, 'executable_sha256': digest(executable)}
        write_json(report_path, report)
        return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    commands = parser.add_subparsers(dest='command', required=True)
    publication = commands.add_parser('publish')
    publication.add_argument('--build-dir', type=Path, required=True)
    publication.add_argument('--store', type=Path, required=True)
    installation = commands.add_parser('install')
    installation.add_argument('--package', type=Path, required=True)
    installation.add_argument('--content-sha256', required=True)
    installation.add_argument('--store', type=Path, required=True)
    consumer = commands.add_parser('consume')
    consumer.add_argument('--package', type=Path, required=True)
    consumer.add_argument('--content-sha256', required=True)
    consumer.add_argument('--application', type=Path, required=True)
    consumer.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        if args.command == 'install':
            result = install(args.package, args.content_sha256, args.store)
        else:
            default = Path.cwd() / 'out/toolchains/linux-sdk/current' if __package__ else Path(__file__).resolve().parents[3]
            sdk = args.sdk_root or Path(os.environ.get('OBJC3C_SDK_ROOT') or default)
            if args.command == 'publish':
                result = publish(sdk, args.build_dir, args.store)
            else:
                result = consume(sdk, args.package, args.content_sha256, args.application, args.output_dir)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(1, f'binary package {args.command} failed: {error}\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
