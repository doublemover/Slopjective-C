"""Publish, transport, inspect and consume content-addressed native packages."""
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
import stat
import subprocess
import sys
import tempfile
import zipfile
import zlib

if __package__:
    from .sdk_example import coroutine_compile_flags, native_module_compile_flags, runtime_link_flags, dependencies, digest, environment, hashes, verify_installed_sdk, write_json
    from .source_graph import NAME, fields, locks, source_inputs, validate_mode, validate_provider, validate_exports, within
else:
    sys.dont_write_bytecode = True
    from sdk_example import coroutine_compile_flags, native_module_compile_flags, runtime_link_flags, dependencies, digest, environment, hashes, verify_installed_sdk, write_json
    from build import NAME, fields, locks, source_inputs, validate_mode, validate_provider, validate_exports, within

SCHEMA = 'objc3c.binary-package.v3'
CONSUMER = 'objc3c.binary-consumer.v3'
HASH = re.compile('[0-9a-f]{64}', re.ASCII)
MODE = ('language', 'standard', 'ownership')
IDENTITY = {'canonical_name', 'extension_abi', 'language_profile', 'language_revision',
            'package_identity', 'runtime_abi', 'semantic_fingerprint', 'target_triple'}


def canonical_json(value: dict) -> bytes:
    return json.dumps(value, sort_keys=True).encode()


def content_hash(value: dict) -> str:
    return hashlib.sha256(canonical_json(value)).hexdigest()


def check_hash(value):
    if not isinstance(value, str) or not HASH.fullmatch(value):
        raise ValueError('expected a lowercase SHA-256 content identity')


def disjoint(output: Path, *protected: Path):
    if any(output.is_relative_to(root) or root.is_relative_to(output) for root in protected):
        raise ValueError('package output must be disjoint from the SDK and every input root')


def find_installed_shared_library(directory: Path, name: str) -> Path | None:
    """Resolve an installed ELF DSO, while admitting a genuinely static stage."""
    directory = directory.resolve(strict=True)
    if (Path(name).name != name or not name.startswith('lib') or
            not name.endswith('.so')):
        raise ValueError('installed shared-library name must be a plain lib*.so name')
    link = directory / name
    if link.is_file():
        selected = link.resolve(strict=True)
        if not selected.is_relative_to(directory):
            raise ValueError('installed shared library escapes its owning directory')
        return selected
    candidates = {}
    versioned = re.compile(re.escape(name) + r'(?:\.[0-9]+)+', re.ASCII)
    for path in directory.glob(name + '.*'):
        if not versioned.fullmatch(path.name):
            continue
        if not path.is_file():
            if path.is_symlink():
                raise ValueError('installed shared library has a broken link: ' + name)
            continue
        selected = path.resolve(strict=True)
        if not selected.is_relative_to(directory):
            raise ValueError('installed shared library escapes its owning directory')
        candidates[selected] = path
    if not candidates:
        if link.is_symlink():
            raise ValueError('installed shared library has a broken link: ' + name)
        return None
    if len(candidates) != 1:
        raise ValueError('installed shared library is missing or ambiguous: ' + name)
    return next(iter(candidates))


def installed_shared_library(directory: Path, name: str) -> Path:
    """Require one installed ELF DSO with or without its development symlink."""
    selected = find_installed_shared_library(directory, name)
    if selected is None:
        raise ValueError('installed shared library is missing or ambiguous: ' + name)
    return selected


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
        optional = {'provider_language'} & row.keys() if isinstance(row, dict) else set()
        fields(row, {*MODE, 'provider_ownership', 'exports', 'module', 'dependencies', 'artifacts'} | optional, 'binary module')
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


def verify_sidecars(manifest: dict, read):
    modules = {row['module']['canonical_name']: row for row in manifest['packages']}
    for row in manifest['packages']:
        name = row['module']['canonical_name']
        interface = f'modules/{name}/{name}.objc3i'
        sidecar = json.loads(read(interface + '.json'))
        if not isinstance(sidecar, dict) or sidecar.get('module') != row['module']:
            raise ValueError('binary package sidecar differs from its module identity')
        # Schema 2.2 binds export ownership in native metadata. A declaration
        # kind alone cannot distinguish namespace descendants, static members
        # and friends from independent public roots.
        declarations = sidecar.get('declarations')
        if (sidecar.get('schema_major') != 2 or sidecar.get('schema_minor') != 2 or
                not isinstance(declarations, list) or
                any(not isinstance(declaration, dict) or
                    any(not isinstance(declaration.get(key), str) or not declaration[key]
                        for key in ('kind', 'qualified_name', 'export_root'))
                    for declaration in declarations)):
            raise ValueError('binary exports require native schema 2.2 root identities')
        names = {declaration['qualified_name'] for declaration in declarations}
        roots = {declaration['export_root'] for declaration in declarations}
        namespaces = {declaration['qualified_name'] for declaration in declarations
                      if declaration['kind'] == 'Namespace'}
        if not roots <= names:
            raise ValueError('binary exports reference an absent native root')
        if row['exports'] != ['*']:
            selected = set(row['exports'])
            if not selected <= roots or any(
                    not any(root == wanted or root.startswith(wanted + '::') or
                            (root in namespaces and wanted.startswith(root + '::'))
                            for wanted in selected)
                    for root in roots):
                raise ValueError('binary exports differ from the native public root declarations')
        projection = sidecar.get('ownership_projection')
        cross_language = row.get('provider_language') == 'c++'
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
            producer_runtime = 'no-objc-runtime' if cross_language else 'gnustep-2.2'
            producer_identity['runtime_abi'] = producer_runtime + ';' + ('arc' if row['provider_ownership'] == 'arc' else 'manual-reference-counting')
            producer_identity['language_profile'] = producer_identity['language_profile'].replace(
                ';arc=' + str(row['ownership'] == 'arc').lower() + ';',
                ';arc=' + str(row['provider_ownership'] == 'arc').lower() + ';')
            if cross_language:
                producer_identity['language_revision'] = 'objc3-disabled'
                producer_identity['language_profile'] = producer_identity['language_profile'].replace(
                    'objc3=enabled;', 'objc3=disabled;', 1).replace(';language=objective-c++;', ';language=c++;', 1)
            producer_identity['semantic_fingerprint'] = identity.get('semantic_fingerprint')
            if (identity != producer_identity or producer.get('schema_id') != 'objc3c.semantic-interface.v2' or producer.get('schema_major') != 2 or producer.get('dependencies') != [] or
                    producer.get('ownership_projection') != '' or
                    producer.get('source_binding_sha256') != sidecar.get('source_binding_sha256') or
                    producer.get('source_binding_sha256') != row['artifacts'][interface] or
                    not isinstance(sidecar.get('required_semantics'), list) or
                    'objc3.ownership-projection.v1' not in sidecar['required_semantics'] or
                    ('objc3.cxx-abi-projection.v1' in sidecar['required_semantics']) != cross_language):
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


def verify_files(root: Path, manifest: dict, *, exact: bool):
    expected = artifacts(manifest)
    for name, value in expected.items():
        path = root / name
        if any(part.is_symlink() for part in (path, *path.parents) if part != root.parent):
            raise ValueError('binary package artifacts cannot use symbolic links')
        if not path.is_file() or digest(path) != value:
            raise ValueError('binary package artifact hash differs: ' + name)
    verify_sidecars(manifest, lambda name: (root / name).read_bytes())
    if exact:
        found = set()
        for path in root.rglob('*'):
            if path.is_symlink() or (not path.is_file() and not path.is_dir()):
                raise ValueError('binary package contains a link or special file')
            if path.is_file():
                found.add(path.relative_to(root).as_posix())
        if found != set(expected) | {'package.json'}:
            raise ValueError('binary package contains missing or unlisted files')


def locked_manifest(data: bytes, expected: str) -> dict:
    check_hash(expected)
    manifest = json.loads(data)
    if content_hash(manifest) != expected:
        raise ValueError('binary package content identity differs from the requested lock')
    package_rows(manifest)
    return manifest


def verify_package(root: Path, expected: str) -> dict:
    if root.is_symlink():
        raise ValueError('binary package root cannot be a symbolic link')
    path = root / 'package.json'
    if path.is_symlink() or not path.is_file():
        raise ValueError(f'binary package requires a regular manifest: {path}')
    manifest = locked_manifest(path.read_bytes(), expected)
    verify_files(root, manifest, exact=True)
    return manifest


def seal_candidate(candidate: Path, destination: Path) -> None:
    for path in candidate.rglob('*'):
        if path.is_file():
            path.chmod(0o444)
    candidate.rename(destination)


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
            if before_publish:
                before_publish()
            seal_candidate(candidate, destination)
        return destination


def export_archive(source: Path, expected: str, output: Path) -> dict:
    manifest = verify_package(source, expected)
    if output.is_symlink():
        raise ValueError('binary package archive output cannot be a symbolic link')
    output = output.resolve()
    disjoint(output, source.resolve(strict=True))
    with tempfile.TemporaryDirectory(prefix='.package-export-', dir=output.parent) as temporary:
        candidate = Path(temporary) / 'package.zip'
        with zipfile.ZipFile(candidate, 'w') as archive:
            files = artifacts(manifest)
            for name in ('package.json', *sorted(files)):
                member = zipfile.ZipInfo(name)
                member.create_system = 3  # Unix file type and mode.
                member.external_attr = (stat.S_IFREG | 0o644) << 16
                member.compress_type = zipfile.ZIP_DEFLATED
                if name == 'package.json':
                    archive.writestr(member, canonical_json(manifest))
                    continue
                with (source / name).open('rb') as payload, archive.open(member, 'w', force_zip64=True) as target:
                    hashed = hashlib.sha256()
                    while chunk := payload.read(1024 * 1024):
                        hashed.update(chunk)
                        target.write(chunk)
                    if hashed.hexdigest() != files[name]:
                        raise ValueError('binary package artifact changed during export: ' + name)
        signature = digest(candidate)
        candidate.replace(output)
    return {'status': 'PASS', 'content_sha256': expected, 'archive_sha256': signature, 'archive': str(output)}


def read_archive(source: Path, expected: str, *, candidate: Path | None = None) -> dict:
    check_hash(expected)
    with zipfile.ZipFile(source) as archive:
        members = {}
        for member in archive.infolist():
            name = member.filename
            kind = stat.S_IFMT(member.external_attr >> 16)
            if (name != member.orig_filename or member.is_dir() or kind not in (0, stat.S_IFREG) or
                    member.external_attr & 0x10 or member.flag_bits & 1 or
                    member.compress_type not in (zipfile.ZIP_STORED, zipfile.ZIP_DEFLATED)):
                raise ValueError('binary package archive requires ordinary regular-file members')
            if name in members:
                raise ValueError('duplicate binary package archive member: ' + name)
            members[name] = member
        if 'package.json' not in members:
            raise ValueError('binary package archive is missing package.json')
        manifest = locked_manifest(archive.read(members['package.json']), expected)
        files = artifacts(manifest)
        if members.keys() != files.keys() | {'package.json'}:
            raise ValueError('binary package archive contains missing or unlisted members')
        for name, signature in files.items():
            with archive.open(members[name]) as payload:
                if candidate is None:
                    if hashlib.file_digest(payload, 'sha256').hexdigest() != signature:
                        raise ValueError('binary package archive artifact hash differs: ' + name)
                else:
                    # Every destination name comes from the checked manifest schema.
                    path = candidate / name
                    path.parent.mkdir(parents=True, exist_ok=True)
                    with path.open('xb') as target:
                        shutil.copyfileobj(payload, target)
        if candidate is None:
            verify_sidecars(manifest, lambda name: archive.read(members[name]))
        else:
            write_json(candidate / 'package.json', manifest)
            verify_files(candidate, manifest, exact=True)
    return manifest


def inspect(source: Path, expected: str, selections: list[tuple[str, str]]) -> dict:
    if source.is_symlink():
        raise ValueError('binary package input cannot be a symbolic link')
    manifest = read_archive(source, expected) if source.is_file() else verify_package(source, expected)
    rows = package_rows(manifest)
    roots = []
    for name, package in selections:
        if name not in rows or rows[name]['module']['package_identity'] != package:
            raise ValueError('inspection module identity lock cannot be resolved: ' + name)
        if name in roots:
            raise ValueError('duplicate inspection module lock: ' + name)
        roots.append(name)
    order = closure(rows, roots or sorted(rows))
    return {'schema': 'objc3c.binary-package-inspection.v1', 'status': 'PASS', 'content_sha256': expected,
            'sdk_seal': manifest['sdk_seal'], 'target': manifest['target'], 'optimization': manifest['optimization'],
            'roots': roots, 'modules': [rows[name] for name in order]}


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
    if source.is_file():
        with lock(store, '.package.lock'), tempfile.TemporaryDirectory(prefix='.package-', dir=store) as temporary:
            candidate = Path(temporary) / 'candidate'
            candidate.mkdir()
            read_archive(source, expected, candidate=candidate)
            destination = store / expected
            if destination.exists() or destination.is_symlink():
                verify_package(destination, expected)
            else:
                seal_candidate(candidate, destination)
    else:
        manifest = verify_package(source, expected)
        destination = copy_package(source, manifest, store)
        verify_package(source, expected)
    return {'status': 'PASS', 'content_sha256': expected, 'package': str(destination)}


def consume(sdk: Path, package: Path, expected: str, application: Path, output: Path,
            *, additional: tuple[tuple[Path, str], ...] = ()) -> dict:
    sdk, application, output = sdk.resolve(strict=True), application.resolve(strict=True), output.resolve()
    verified = [(path.resolve(strict=True), signature, verify_package(path, signature))
                for path, signature in ((package, expected), *additional)]
    root = application.parent
    disjoint(output, sdk, root, *(path for path, _, _ in verified))
    identity = sdk_identity(sdk)
    target = identity['target']
    optimization = verified[0][2]['optimization']
    rows, owners = {}, {}
    for path, signature, manifest in verified:
        if manifest['sdk_seal'] != identity['seal_sha256'] or manifest['target'] != target:
            raise ValueError('consumer requires the exact producing SDK seal and target')
        if manifest['optimization'] != optimization:
            raise ValueError('consumer packages must share the same optimization')
        for name, row in package_rows(manifest).items():
            if name in rows:
                if rows[name] != row:
                    raise ValueError('conflicting immutable providers for module: ' + name)
            else:
                rows[name] = row
                owners[name] = (path, signature)
    config = json.loads(application.read_text())
    fields(config, {'schema', 'source', *MODE, 'dependencies'}, 'binary consumer')
    if config['schema'] != CONSUMER:
        raise ValueError('unsupported binary consumer schema')
    source = within(root, config['source'], 'consumer source')
    validate_mode(config, source)
    direct = locks(config['dependencies'], 'consumer')
    for edge in direct:
        row = rows.get(edge['name'])
        if (row is None or row['module']['package_identity'] != edge['package'] or
                any(row[key] != config[key] for key in MODE)):
            raise ValueError('consumer dependency identity or mode lock differs')
    order = closure(rows, [edge['name'] for edge in direct])
    modules = {name: owners[name][0] / 'modules' / name for name in order}
    sources = source_inputs(root)
    with lock(output, '.consumer.lock'):
        report_path = output / 'consumer-report.json'
        report_path.unlink(missing_ok=True)
        obj, executable, depfile = output / 'application.o', output / 'application.bin', output / 'application.d'
        command = [str(sdk / 'bin/objc3c'), str(source), '--target', target, '--emit-object', str(obj)]
        if config['language'] != 'c++':
            command += ['--objc3', '--runtime', 'gnustep-2.2', '--source-imports']
        if config['language'] == 'objective-c++':
            command.append('--objcxx')
        if config['ownership'] == 'arc':
            command.append('--arc')
        for directory in modules.values():
            command += ['--module-search-path', str(directory)]
        for edge in direct:
            row = rows[edge['name']]['module']
            command += ['--import-module', edge['name'], row['semantic_fingerprint'], edge['package'],
                        '--reexport-import' if edge['visibility'] == 'public' else '--private-import']
            for group in edge.get('spi', []):
                command += ['--import-spi', group]
        command += ['--', '--no-default-config', '-std=' + config['standard'], *coroutine_compile_flags(config['language']),
                    *native_module_compile_flags(config['language'], output), '-fblocks',
                    '-' + optimization, '-isystem', str(sdk / 'include'),
                    '-MD', '-MF', str(depfile), '-MT', 'dependencies']
        env = environment(sdk)
        env.pop('LD_AUDIT', None)
        commands = [command, [str(sdk / 'bin/clang++'), '--no-default-config', '--target=' + target,
                    '-fuse-ld=lld', str(obj), *[str(directory / 'module.o') for directory in modules.values()],
                    '-L' + str(sdk / 'lib'), *runtime_link_flags(sdk), '-Wl,-rpath,' + str(sdk / 'lib'), '-o', str(executable)]]
        for index, argv in enumerate(commands):
            with (output / f'command-{index}.log').open('w') as stream:
                subprocess.run(argv, cwd=output, env=env, stdout=stream, stderr=subprocess.STDOUT, check=True)
            if index == 0:
                tracked = hashes(dependencies(depfile, output))
        run_env = dict(env)
        run_env.pop('LD_LIBRARY_PATH', None)
        with (output / 'execution.log').open('w') as stream:
            subprocess.run([str(executable)], cwd=output, env=run_env, stdout=stream, stderr=subprocess.STDOUT, check=True, timeout=30)
        for path, signature, _ in verified:
            verify_package(path, signature)
        sdk_identity(sdk)
        if source_inputs(root) != sources:
            raise ValueError('consumer sources changed during compilation')
        if hashes(map(Path, tracked)) != tracked:
            raise ValueError('consumer header dependencies changed during compilation')
        report = {'schema': 'objc3c.binary-consumer-build.v3', 'status': 'PASS', 'package_content_sha256': expected,
                  'package_content_sha256s': sorted({signature for _, signature, _ in verified}),
                  'module_package_content_sha256': {name: owners[name][1] for name in order},
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
    export = commands.add_parser('export', help='write a deterministic ZIP from a verified package directory')
    inspection = commands.add_parser('inspect', help='show a verified package or a selected module closure')
    registry_publish = commands.add_parser('registry-publish', help='prepare a static registry index and archive')
    fetch = commands.add_parser('fetch', help='resolve and install an exact package from a pinned registry')
    installation = commands.add_parser('install', help='install a verified package directory or ZIP')
    consumer = commands.add_parser('consume')
    for command in (export, inspection, registry_publish, installation, consumer):
        command.add_argument('--package', type=Path, required=True)
        command.add_argument('--content-sha256', required=True)
    export.add_argument('--output', type=Path, required=True)
    inspection.add_argument('--module', nargs=2, metavar=('NAME', 'PACKAGE'), action='append', default=[])
    registry_publish.add_argument('--registry-dir', type=Path, required=True)
    fetch.add_argument('--index-url', required=True)
    fetch.add_argument('--index-sha256', required=True)
    fetch.add_argument('--content-sha256', required=True)
    fetch.add_argument('--module', nargs=2, metavar=('NAME', 'PACKAGE'), action='append', required=True)
    fetch.add_argument('--store', type=Path, required=True)
    fetch.add_argument('--offline', action='store_true', help='verify the cached index and package without network access')
    installation.add_argument('--store', type=Path, required=True)
    consumer.add_argument('--application', type=Path, required=True)
    consumer.add_argument('--output-dir', type=Path, required=True)
    consumer.add_argument('--add-package', nargs=2, metavar=('DIRECTORY', 'CONTENT_SHA256'),
                          action='append', default=[], help='add another exact immutable package lock')
    args = parser.parse_args(argv)
    if args.command in ('fetch', 'registry-publish'):
        if __package__:
            from . import package_registry
        else:
            import package_registry
    try:
        if args.command == 'install':
            result = install(args.package, args.content_sha256, args.store)
        elif args.command == 'export':
            result = export_archive(args.package, args.content_sha256, args.output)
        elif args.command == 'inspect':
            result = inspect(args.package, args.content_sha256, args.module)
        elif args.command == 'registry-publish':
            result = package_registry.publish(args.package, args.content_sha256, args.registry_dir)
        else:
            sdk = args.sdk_root
            if sdk is None:
                default = (Path.cwd() / 'out/toolchains/linux-sdk/current' if __package__
                           else Path(__file__).resolve().parents[3])
                sdk = Path(os.environ.get('OBJC3C_SDK_ROOT') or default)
            if args.command == 'publish':
                result = publish(sdk, args.build_dir, args.store)
            elif args.command == 'fetch':
                result = package_registry.fetch(sdk, args.index_url, args.index_sha256, args.content_sha256,
                                                args.module, args.store, offline=args.offline)
            else:
                additional = tuple((Path(path), signature) for path, signature in args.add_package)
                result = consume(sdk, args.package, args.content_sha256, args.application, args.output_dir,
                                 additional=additional)
    except (OSError, ValueError, subprocess.SubprocessError, zipfile.BadZipFile, zlib.error) as error:
        parser.exit(1, f'binary package {args.command} failed: {error}\n')
    print(json.dumps(result, sort_keys=True))


if __name__ == '__main__':
    main()
