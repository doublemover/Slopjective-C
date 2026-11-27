"""Exercise Darwin package admission and relocation structure on any test host."""
import io
import json
from pathlib import Path
import platform
import shutil
import struct
import subprocess
import sys
import tarfile
from zipfile import ZipFile

import pytest

from scripts.objc3c_toolchain import cli, darwin_registration, darwin_sdk as sdk, darwin_sdk_arc as arc
from scripts.objc3c_toolchain import darwin_compiler_cache as compiler_cache
from scripts.objc3c_toolchain import darwin_sdk_activation as activation
from scripts.objc3c_toolchain import darwin_sdk_replay as replay
from scripts.objc3c_toolchain.package import MANIFEST, SCHEMA, _digest, _file_hash, inventory, normalize_permissions, verify_sdk
from scripts.objc3c_toolchain.sdk_example import dependencies


def macho(kind, *, cpu=0x0100000c):
    return struct.pack('<8I', 0xfeedfacf, cpu, 0, kind, 1, 8, 0, 0) + struct.pack('<2I', 1, 8)


def ar_member(name, data):
    header = (f'{name:<16}{0:<12}{0:<6}{0:<6}{100644:<8}{len(data):<10}`\n').encode()
    return header + data + (b'\n' if len(data) % 2 else b'')


def sealed(prefix, additions=None):
    normalize_permissions(prefix)
    data = {'schema': SCHEMA, 'files': inventory(prefix), **(additions or {})}
    manifest = prefix / MANIFEST
    manifest.write_text(json.dumps({**data, 'seal_sha256': _digest(data)}) + '\n')
    manifest.chmod(0o644)
    return json.loads(manifest.read_text())


def put(prefix, relative, data, executable=False):
    path = prefix / relative
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    path.chmod(0o755 if executable else 0o644)


def thin_archive(name, data):
    extended = (name.encode() + b'\0').ljust(16, b'\0')
    return b'!<arch>\n' + ar_member('__.SYMDEF', b'') + ar_member(f'#1/{len(extended)}', extended + data)


@pytest.mark.parametrize('entry', ('clang', 'driver'))
def test_relocated_compiler_finds_the_public_registration_header(tmp_path, entry):
    clang = shutil.which('clang')
    if clang is None:
        pytest.skip('requires stock Clang')
    resource = Path(subprocess.check_output([clang, '-print-resource-dir'], text=True).strip())
    prefix = tmp_path / 'Relocated Objective-C 3 SDK'
    public_header = prefix / sdk.API_HEADER
    public_header.parent.mkdir(parents=True)
    shutil.copyfile(darwin_registration.RUNTIME_HEADER, public_header)
    consumer = tmp_path / 'Fresh consumer'
    consumer.mkdir()
    source = consumer / 'main.m'
    (consumer / 'api.h').write_bytes((darwin_registration.FIXTURES / 'api.h').read_bytes())
    source.write_text('#include "api.h"\n'
                      'const objc3_darwin_class *lookup(void) { return objc3_darwin_class_v1("Probe"); }\n')
    depfile = consumer / 'headers.d'
    flags = ['-ffreestanding', '-nostdlibinc', '-fsyntax-only', '-Werror', '-MD', '-MF',
             str(depfile), str(source)]
    if entry == 'clang':
        _, *args = sdk.relocated_clang_arguments(prefix, tmp_path, resource)
        args.extend(flags)
    else:
        resource_dir = 'lib/clang/' + resource.name
        (prefix / resource_dir).parent.mkdir(parents=True)
        (prefix / resource_dir).symlink_to(resource, target_is_directory=True)
        manifest = {'profile': sdk.PROFILE, 'target': sdk.TARGET,
                    'compiler_runtime_flag': sdk.RUNTIME_FLAG, 'resource_dir': resource_dir}
        _, *args = sdk.installed_driver.driver_command(prefix, manifest, tmp_path, Path('/usr/bin/ld'),
                                                       flags, 'dynamic')
        # Stock Clang exercises the same header search without the ObjC3 frontend flags.
        args = [arg for arg in args if arg not in ('-fobjc-version=3', sdk.RUNTIME_FLAG)]
    result = subprocess.run([clang, *args], cwd=consumer, capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    assert public_header.resolve() in dependencies(depfile, consumer)


def test_task_service_object_link_retains_cxx_driver_mode_after_resolving_symlink(tmp_path, monkeypatch):
    clang = shutil.which('clang')
    if clang is None:
        pytest.skip('requires stock Clang')
    developer = tmp_path / 'Xcode Developer'
    apple = developer / 'bin/clang'
    apple.parent.mkdir(parents=True)
    apple.touch()
    cxx = apple.with_name('clang++')
    cxx.symlink_to(apple.name)
    monkeypatch.setattr(sdk, '_run', lambda *args, **kwargs: str(cxx))
    host = {'developer_dir': str(developer), 'sdk_path': str(tmp_path)}
    resolved, *args = sdk.task_services.toolchain(host, [])
    assert resolved == apple

    obj = tmp_path / 'client.o'
    library = tmp_path / 'libobjc3darwin_tasks.a'
    obj.touch()
    library.touch()
    result = subprocess.run([clang, *args, '-###', obj, library, '-o', tmp_path / 'client'],
                            capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    assert '"-lc++"' in result.stderr, result.stderr


@pytest.fixture
def structural_host(monkeypatch):
    host = {'xcode': sdk.XCODE, 'sdk_version': sdk.SDK_VERSION, 'sdk_settings_sha256': '1' * 64,
            'libcxx': {'config_relative': 'config', 'config_sha256': '2' * 64, 'stub_sha256': '3' * 64}, 'tools': {
        name: {'path': '/Xcode/bin/' + name, 'sha256': '0' * 64}
        for name in ('ld', 'lipo', 'otool', 'dsymutil')}}
    monkeypatch.setattr(sdk, '_host', lambda commands=None: host)

    def load_commands(*args, **_):
        assert args[0] == '/Xcode/bin/otool'
        if args[1] == '-l':
            return f'{args[2]}:\nLoad command 0\n      cmd LC_BUILD_VERSION\n  platform MACOS\n     minos 15.0\n       sdk 15.5'
        paths = [sdk.SYSTEM]
        if str(args[2]).endswith('.dylib'):
            paths.append('@rpath/libobjc3darwin.dylib')
        return '\n'.join([str(args[2]) + ':', *(f'\t{path} (version)' for path in paths)])

    monkeypatch.setattr(sdk, '_run', load_commands)
    return host


def staged_product(path, host, *, wrong_arch=False, wrong_sdk=False, extra=None):
    for name in ('clang', 'llvm-ar', 'llvm-dwarfdump'):
        put(path, 'bin/' + name, macho(2, cpu=0x01000007 if wrong_arch else 0x0100000c), True)
    put(path, sdk.DRIVER, Path(sdk.installed_driver.__file__).read_bytes(), True)
    put(path, 'lib/libobjc3darwin.a', thin_archive('runtime.o', macho(1)))
    put(path, 'lib/libobjc3darwin.dylib', macho(6))
    put(path, 'lib/clang/23/include/stddef.h', b'/* checked resource */\n')
    put(path, sdk.API_HEADER, b'/* runtime API */\n')
    put(path, 'lib/clang/23/lib/darwin/libclang_rt.osx.a', thin_archive('builtins.o', macho(1)))
    for name in sdk.DEMO_FILES:
        put(path, sdk.DEMO + '/' + name, b'/* packaged witness */\n')
    for project in ('llvm', 'clang', 'compiler-rt'):
        put(path, f'share/licenses/{project}/LICENSE.TXT', b'license\n')
    put(path, 'share/licenses/slopjective-c/LICENSE', b'license\n')
    if extra is not None:
        put(path, extra, macho(2), True)
    requirements = sdk._host_requirements(host)
    if wrong_sdk:
        requirements['sdk_version'] = '15.1'
    return sealed(path, {'payload_schema': sdk.PAYLOAD_SCHEMA, 'profile': sdk.PROFILE, 'target': sdk.TARGET,
                         'runtime_abi': sdk.RUNTIME_ABI, 'compiler_runtime_flag': sdk.RUNTIME_FLAG,
                         'support_truth': False, 'host_requirements': requirements, 'resource_dir': 'lib/clang/23'})


def test_archive_replays_in_a_fresh_relocated_prefix(tmp_path, structural_host):
    stage = tmp_path / 'Owned producer stage'
    manifest = staged_product(stage, structural_host)
    archive, second = (tmp_path / name for name in ('sdk.tar.gz', 'repeat.tar.gz'))
    sdk._write_archive(stage, archive)
    sdk._write_archive(stage, second)
    assert _file_hash(archive) == _file_hash(second)
    prefix = tmp_path / 'Cold consumer prefix with spaces'
    receipt = sdk.install(archive, _file_hash(archive), prefix)
    assert receipt['status'] == 'PASS' and receipt['support_truth'] is False
    assert receipt['seal_sha256'] == manifest['seal_sha256']
    assert (prefix / 'bin/clang').read_bytes() == macho(2)
    assert not (prefix / 'bin/clang').is_symlink()
    driver = prefix / sdk.DRIVER
    assert driver.stat().st_mode & 0o111
    assert _file_hash(driver) == manifest['files'][sdk.DRIVER]['sha256']
    args = sdk.installed_driver.driver_command(prefix, manifest, Path('/Apple SDK'), Path('/Xcode/bin/ld'),
                                               ['unit.o', '-o', 'app'], 'dynamic')
    assert args[-5:] == [str(prefix / 'lib/libobjc3darwin.dylib'), '-Xlinker', '-rpath', '-Xlinker', str(prefix / 'lib')]
    with pytest.raises(ValueError, match='fresh prefix'):
        sdk.install(archive, _file_hash(archive), prefix)


def test_compiler_cache_binds_native_products_to_exact_inputs(tmp_path):
    products = tmp_path / 'source'
    put(products, 'bin/clang', macho(2), True)
    for name in compiler_cache.LICENSES:
        put(products, name, b'license')
    cache = tmp_path / 'cache'
    inputs = {'xcode': sdk.XCODE, 'configuration_sha256': 'a' * 64, 'patch_digest': 'b' * 64}
    assert compiler_cache.restore(cache, inputs) is None
    saved = compiler_cache.save(cache, inputs, products / 'bin/clang', products)
    assert compiler_cache.restore(cache, inputs) == saved
    assert compiler_cache.restore(cache, {**inputs, 'patch_digest': 'c' * 64}) is None
    (saved / 'bin/clang').write_bytes(b'different compiler')
    with pytest.raises(ValueError, match='product seal'):
        compiler_cache.restore(cache, inputs)


def test_installed_driver_builds_and_links_using_the_sealed_profile(tmp_path, structural_host):
    prefix = tmp_path / 'Relocated Objective-C 3 SDK'
    manifest = staged_product(prefix, structural_host)
    driver = sdk.installed_driver.driver_command
    selected, linker = Path('/Apple SDK'), Path('/Xcode/bin/ld')
    compile_args = driver(prefix, manifest, selected, linker, ['-c', 'unit.m', '-o', 'unit.o'], 'dynamic')
    assert '-fobjc-runtime=gnustep-2.99' in compile_args and str(selected) in compile_args
    assert '--ld-path=' + str(linker) not in compile_args
    assert driver(prefix, manifest, selected, linker, ['unit.o'], 'static')[-1] == str(prefix / 'lib/libobjc3darwin.a')
    assert str(prefix / 'lib/libobjc3darwin.dylib') in driver(prefix, manifest, selected, linker, ['unit.o'], 'dynamic')
    with pytest.raises(ValueError, match='overrides the installed'):
        driver(prefix, manifest, selected, linker, ['-fobjc-runtime=macosx-15.0', 'unit.m'], 'dynamic')


def test_package_manifest_admits_only_its_sealed_portable_files(tmp_path, structural_host):
    services = sdk.portable_services
    stage = tmp_path / 'stage'
    manifest = staged_product(stage, structural_host)
    sources = {name: _file_hash(sdk.ROOT / name) for name in services._witness_sources()}
    for target, source in {**services.HEADERS, services.EXAMPLE: services.EXAMPLE_SOURCE}.items():
        put(stage, target, (sdk.ROOT / source).read_bytes())
        sources[source] = _file_hash(sdk.ROOT / source)
    put(stage, services.LIBRARY, thin_archive('runtime.o', macho(1)))
    manifest['files'] = inventory(stage)
    manifest['compiler_host_libcxx'] = {'header_config_sha256': 'a'*64, 'library_stub_sha256': 'b'*64,
                                      'reported_version': 'Xcode libc++ 190102'}
    manifest[services.KEY] = {'schema': services.SCHEMA, 'abi': services.ABI, 'build_inputs': sources,
                              'objects': {name: 'c'*64 for name in services.MEMBERS},
                              'files': {name: manifest['files'][name]['sha256']
                                        for name in (services.LIBRARY, services.EXAMPLE, *services.HEADERS)}}
    assert sdk._validate_manifest(manifest, structural_host) == manifest['resource_dir']
    manifest.pop(services.KEY)
    with pytest.raises(ValueError, match='payload inventory'):
        sdk._validate_manifest(manifest, structural_host)


@pytest.mark.parametrize(('fault', 'diagnostic'), (
    ('arch', 'arm64 Mach-O executable'), ('sdk', 'admitted experimental Darwin host'),
    ('extra', 'payload inventory'), ('sha256', 'SHA-256 does not match')))
def test_failed_install_owns_no_visible_prefix(tmp_path, structural_host, fault, diagnostic):
    stage = tmp_path / 'stage'
    staged_product(stage, structural_host, wrong_arch=fault == 'arch', wrong_sdk=fault == 'sdk',
                   extra='bin/unexpected' if fault == 'extra' else None)
    archive = tmp_path / 'sdk.tar.gz'
    sdk._write_archive(stage, archive)
    prefix = tmp_path / 'Consumer prefix'
    expected = '0' * 64 if fault == 'sha256' else _file_hash(archive)
    with pytest.raises(ValueError, match=diagnostic):
        sdk.install(archive, expected, prefix)
    assert not prefix.exists()
    assert not list(tmp_path.glob('.objc3-darwin-*'))


def test_relocated_qualification_retains_real_archive_and_destination_controls(tmp_path, structural_host):
    staged = tmp_path / 'stage'
    manifest = staged_product(staged, structural_host)
    archive = tmp_path / 'sdk.tar.gz'
    sdk._write_archive(staged, archive)
    checksum = _file_hash(archive)
    active = tmp_path / 'Existing qualified prefix'
    sdk.install(archive, checksum, active)

    result = sdk._qualify_installer_controls(archive, checksum, tmp_path, manifest, structural_host, [])
    assert result['schema'] == 'objc3c.experimental-darwin-sdk-installer-controls.v1'
    assert result['archive_sha256'] == _file_hash(archive) == checksum
    rows = result['observations']
    assert set(rows) == {'archive-digest', 'incomplete-prefix', 'damaged-prefix', 'apple-linker-identity'}
    for name, row in rows.items():
        assert row['status'] == 'REJECTED'
        assert row['prefix_before_sha256'] == row['prefix_after_sha256']
        if name.endswith('-prefix'):
            assert row['prefix_before_sha256'] is not None
            assert row['diagnostic'] == 'install requires a fresh prefix'
            assert row['requested_archive_sha256'] == checksum
        else:
            assert row['prefix_before_sha256'] is None
            assert row['requested_archive_sha256'] != checksum
    linker = rows['apple-linker-identity']
    assert linker['tool'] == 'ld'
    assert linker['observed_tool_sha256'] == structural_host['tools']['ld']['sha256']
    assert linker['required_tool_sha256'] != linker['observed_tool_sha256']
    assert rows['damaged-prefix']['prefix_before_sha256'] != rows['incomplete-prefix']['prefix_before_sha256']
    assert (active / 'bin/clang').read_bytes() == macho(2)
    assert not list(tmp_path.glob('.installer-controls-*'))


def test_install_command_rejects_the_actual_non_apple_silicon_host(tmp_path):
    if platform.system() == 'Darwin' and platform.machine() == 'arm64':
        pytest.skip('requires a non-Apple-Silicon host for this command-level rejection')
    prefix = tmp_path / 'Never activated'
    result = subprocess.run([sys.executable, '-B', '-m', 'scripts.objc3c_toolchain',
        'darwin', 'install', '--archive', str(tmp_path / 'unavailable.tar.gz'), '--archive-sha256', 'a' * 64,
        '--prefix', str(prefix)], cwd=sdk.ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 2
    assert 'requires an Apple Silicon Mac running native arm64 Python' in result.stderr
    assert not result.stdout and not prefix.exists()


def test_public_build_command_always_packs_and_selects_optional_native_sanitizers(tmp_path, monkeypatch, capsys):
    bootstrap, output = tmp_path / 'bootstrap.json', tmp_path / 'SDK output'

    def qualify(report, destination, *, fetch, package_sdk, sdk_sanitizers):
        assert (report, destination) == (bootstrap, output)
        assert fetch and package_sdk and sdk_sanitizers

    monkeypatch.setattr(darwin_registration, 'qualify', qualify)
    assert cli.main(['darwin', 'build-sdk', '--bootstrap-report', str(bootstrap),
                     '--output-dir', str(output), '--fetch', '--sdk-sanitizers']) == 0
    assert capsys.readouterr().out.strip() == str(output / 'report.json')


def test_installed_arc_load_trace_resolves_native_root_aliases(tmp_path):
    private = tmp_path.resolve() / 'private' / 'var'
    runtime = private / 'Relocated SDK/lib/libobjc3darwin.dylib'
    put(private, 'Relocated SDK/lib/libobjc3darwin.dylib', b'owned runtime')
    public = tmp_path / 'var'
    public.symlink_to(private, target_is_directory=True)
    trace = f'dyld[123]: <UUID> {public / runtime.relative_to(private)}\n'
    assert arc.loaded_runtime(trace, runtime) == str(runtime)
    with pytest.raises(ValueError, match='exact installed'):
        arc.loaded_runtime('', runtime)


@pytest.mark.parametrize('fault', (None, 'stale-source', 'wrong-header', 'foreign-import', 'wrong-runtime'))
def test_installed_arc_witness_binds_the_archive_compiler_headers_and_loaded_runtime(
        tmp_path, structural_host, monkeypatch, fault):
    tmp_path = tmp_path.resolve()
    prefix = tmp_path / 'Relocated Objective-C 3 SDK'
    manifest = staged_product(prefix, structural_host)
    compiler = prefix / 'bin/clang'
    runtime = prefix / 'lib/libobjc3darwin.dylib'
    resource = prefix / manifest['resource_dir']
    stddef = resource / 'include/stddef.h'
    xcode = tmp_path / 'Apple SDK'
    pthread = xcode / 'usr/include/pthread.h'
    put(xcode, 'usr/include/pthread.h', b'Apple pthread header')
    host = {**structural_host, 'sdk_path': str(xcode)}
    output = tmp_path / arc.DIRECTORY
    monkeypatch.setattr(arc, 'qualify_cpp_unwinding', lambda *args: {'status': 'PASS'})
    rpath = '@loader_path/../Relocated Objective-C 3 SDK/lib'
    commands = []

    def quote(path):
        return str(path).replace(' ', '\\ ')

    def execute(*args, cwd=None, commands=None, trace_libraries=False):
        tool, output_bytes = args[0], ''
        stderr = ''
        if tool == compiler:
            assert args[1:7] == ('--no-default-config', '--target=' + sdk.TARGET, '-isysroot', xcode,
                                 '-resource-dir', resource)
            if args[-1] == '-print-file-name=libclang_rt.osx.a':
                output_bytes = str(resource / 'lib/darwin/libclang_rt.osx.a')
            else:
                if '-c' in args:
                    name = args[args.index('-c') + 1]
                    assert name in (prefix / sdk.DEMO / 'arc.m', prefix / sdk.DEMO / 'arc_threads.c')
                    assert ('-fobjc-arc' in args) == (name.suffix == '.m')
                    assert '-O2' in args
                    if '-MF' in args:
                        depfile = args[args.index('-MF') + 1]
                        included = pthread if fault == 'wrong-header' else stddef
                        depfile.write_text(f'arc-threads.o: {quote(name)} {quote(pthread)} {quote(included)}\n')
                else:
                    assert runtime in args and '-Wl,-rpath,' + rpath in args
                args[-1].write_bytes(macho(1 if '-c' in args else 2))
        elif tool == '/usr/bin/nm':
            assert args[1:] == ('-u', '-j', output / 'arc')
            names = sorted('_' + name for name in arc.REQUIRED_IMPORTS)
            if fault == 'foreign-import':
                names.append('_objc_release')
            output_bytes = '\n'.join(names)
        elif tool == structural_host['tools']['otool']['path']:
            if args[1] == '-L':
                output_bytes = f'{args[2]}:\n\t{sdk.SYSTEM} (version)\n\t@rpath/{runtime.name} (version)'
            else:
                output_bytes = (f'Load command 0\ncmd LC_RPATH\ncmdsize 72\npath {rpath} (offset 12)\n'
                                'Load command 1\ncmd LC_BUILD_VERSION\nplatform MACOS\nminos 15.0\nsdk 15.5')
        elif tool == output / 'arc':
            assert trace_libraries and cwd == output
            loaded = runtime
            if fault == 'wrong-runtime':
                loaded = tmp_path / 'Other runtime' / runtime.name
                put(loaded.parent, runtime.name, runtime.read_bytes())
            stderr = f'dyld[123]: <UUID> {sdk.SYSTEM}\ndyld[123]: <UUID> {loaded}\n'
            output_bytes = arc.WITNESS
        else:
            assert args == (prefix / 'bin/llvm-dwarfdump', '--verify', output / 'arc.o')
        if commands is not None:
            commands.append({'argv': list(map(str, args)), 'exit_status': 0, 'stdout': output_bytes, 'stderr': stderr})
        return output_bytes

    monkeypatch.setattr(sdk, '_run', execute)
    if fault == 'stale-source':
        (prefix / sdk.DEMO / 'arc.m').write_text('unqualified source')
    if fault:
        diagnostics = {'stale-source': 'differs from its producer receipt',
                       'wrong-header': 'pinned Apple SDK headers',
                       'foreign-import': 'inconsistent owned runtime imports', 'wrong-runtime': 'exact installed'}
        with pytest.raises(ValueError, match=diagnostics[fault]):
            arc.qualify_consumer(prefix, manifest, host, output, commands)
        if fault == 'stale-source':
            assert not commands
    else:
        receipt = arc.qualify_consumer(prefix, manifest, host, output, commands)
        assert receipt['status'] == 'PASS' and receipt['support_truth'] is False
        assert receipt['seal_sha256'] == manifest['seal_sha256'] and receipt['optimization'] == 'O2'
        assert receipt['loaded_runtime'] == {'path': str(runtime), 'sha256': _file_hash(runtime)}
        assert receipt['inputs'][sdk.DEMO + '/arc.m'] == _file_hash(prefix / sdk.DEMO / 'arc.m')
        assert receipt['thread_dependencies'] == {'sha256': _file_hash(output / 'arc-threads.d'),
                                                   'stddef': str(stddef), 'pthread': str(pthread)}
        assert receipt['artifacts']['arc']['rpaths'] == [rpath]
        assert commands[-1]['argv'] == [str(output / 'arc')]


def test_cpp_arc_qualification_uses_the_dynamic_library_macho_kind(tmp_path, structural_host, monkeypatch):
    prefix = tmp_path / 'Relocated SDK'
    output = tmp_path / 'Fresh consumer'
    output.mkdir()
    clang, driver = prefix / 'bin/clang', prefix / sdk.DRIVER
    runtime = prefix / 'lib/libobjc3darwin.dylib'
    put(prefix, 'lib/libobjc3darwin.dylib', macho(6))
    inputs = {sdk.DRIVER: driver, **{f'{sdk.DEMO}/{name}': prefix / sdk.DEMO / name
                                  for name in arc.ARCXX_SOURCES}}

    def execute(*args, cwd=None, commands=None, rejects=(), trace_libraries=False):
        assert cwd == output
        if '-dynamiclib' in args:
            assert args[0] == clang
            args[-1].write_bytes(macho(6))
        elif '-fsyntax-only' in args:
            assert rejects and '-fobjc-exceptions' in args
        elif '-o' in args:
            assert args[0] == driver
            args[-1].write_bytes(macho(1 if '-c' in args else 2))
        elif args[0] == '/usr/bin/nm':
            return '___gxx_personality_v0\n_objc3_darwin_arc_release_v1'
        else:
            assert trace_libraries and args[0].name in ('arc-unwind-O0', 'arc-unwind-O2')
            commands.append({'stderr': f'dyld[123]: <UUID> {runtime}\n'})
            return arc.ARCXX_WITNESS

    monkeypatch.setattr(sdk, '_run', execute)
    monkeypatch.setattr(sdk, '_load_paths', lambda tool, artifact, expected, commands: expected)
    report = arc.qualify_cpp_unwinding(prefix, structural_host, output, [], [clang], runtime, inputs)
    assert report['helper']['kind'] == 'dynamic_library'
    assert set(report['configurations']) == {'O0', 'O2'}


def payload_archive(destination, manifest, *members):
    def add(tar, name, payload=b'', kind=tarfile.REGTYPE, link=''):
        member = tarfile.TarInfo(sdk.ARCHIVE_ROOT + '/' + name)
        member.mode, member.type, member.linkname = 0o644, kind, link
        member.size = len(payload)
        tar.addfile(member, io.BytesIO(payload))

    with tarfile.open(destination, 'w:gz') as archive:
        add(archive, MANIFEST, json.dumps(manifest).encode())
        for descriptor in members:
            add(archive, *descriptor)


@pytest.mark.parametrize('fault', ('escape', 'symlink', 'hardlink', 'duplicate', 'missing', 'case', 'hash', 'bad-seal'))
def test_archive_rejects_invalid_transports(tmp_path, fault):
    src = tmp_path / 'source'
    put(src, 'hello.h', b'hello')
    if fault == 'case':
        put(src, 'HELLo.h', b'also hello')
    manifest = sealed(src)
    archives = {
        'escape': (('../escape', b'hello'),),
        'symlink': (('hello.h', b'', tarfile.SYMTYPE, '../escape'),),
        'hardlink': (('hello.h', b'', tarfile.LNKTYPE, '../escape'),),
        'duplicate': (('hello.h', b'hello'), ('hello.h', b'hello')),
        'missing': (), 'case': (('hello.h', b'hello'), ('HELLo.h', b'also hello')),
        'hash': (('hello.h', b'altered'),), 'bad-seal': (('hello.h', b'hello'),),
    }
    if fault == 'bad-seal':
        manifest['seal_sha256'] = '0' * 64
    archive = tmp_path / 'sdk.tar.gz'
    payload_archive(archive, manifest, *archives[fault])
    with archive.open('rb') as compressed, pytest.raises(ValueError):
        sdk._unpack(compressed, tmp_path / 'unpacked')
    assert not (tmp_path / 'escape').exists()


def managed_packages(tmp_path, host):
    products = []
    for index in range(2):
        stage = tmp_path / f'candidate-stage-{index}'
        initial = staged_product(stage, host)
        put(stage, 'share/licenses/slopjective-c/LICENSE', f'candidate {index}\n'.encode())
        manifest = sealed(stage, {key: value for key, value in initial.items() if key not in ('files', 'seal_sha256')})
        archive = tmp_path / f'candidate-{index}.tar.gz'
        sdk._write_archive(stage, archive)
        products.append((archive, _file_hash(archive), manifest['seal_sha256']))
    return products


def test_managed_install_reuses_updates_and_removes_only_the_admitted_sdk(tmp_path, structural_host, monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    root = tmp_path / 'Managed Darwin SDK'
    first, second = managed_packages(tmp_path, structural_host)
    installed = activation.install(*first[:2], root)
    current = root / 'current'
    before = current.resolve()
    keep = root / 'user-notes.txt'
    keep.write_text('preserve user files\n')
    assert installed['action'] == 'installed'
    assert installed['seal_sha256'] == first[2]
    assert installed['active_prefix'] == str(current)
    stamp = (before / 'bin/clang').stat().st_ino
    assert activation.install(*first[:2], root)['action'] == 'reused'
    assert current.resolve() == before and (before / 'bin/clang').stat().st_ino == stamp

    updated = activation.install(*second[:2], root)
    assert updated['action'] == 'updated' and updated['seal_sha256'] == second[2]
    assert not before.exists()
    assert current.resolve() == root / ('sdk-' + second[2]) / 'sdk'
    with pytest.raises(ValueError, match='differs from the active Darwin SDK'):
        activation.uninstall(root, first[2])
    removed = activation.uninstall(root, second[2])
    assert removed['action'] == 'uninstalled' and removed['active_prefix'] is None
    assert not current.is_symlink()
    assert not (root / ('sdk-' + second[2])).exists()
    assert keep.read_text() == 'preserve user files\n'


def test_managed_install_readmits_repacked_archive_once(tmp_path, structural_host, monkeypatch):
    root = tmp_path / 'Managed Darwin SDK'
    first, _ = managed_packages(tmp_path, structural_host)
    activation.install(*first[:2], root)
    compiler = root / 'current/bin/clang'
    original_inode = compiler.stat().st_ino

    repacked = tmp_path / 'repacked.tar.gz'
    archive = bytearray(first[0].read_bytes())
    struct.pack_into('<I', archive, 4, 1)
    repacked.write_bytes(archive)
    checksum = _file_hash(repacked)
    assert checksum != first[1]
    receipt = activation.install(repacked, checksum, root)
    assert receipt['action'] == 'reused' and receipt['archive_sha256'] == checksum
    assert receipt['seal_sha256'] == first[2] and compiler.stat().st_ino == original_inode

    def reject_extraction(*args):
        pytest.fail('already admitted repacked archive was extracted again')

    monkeypatch.setattr(sdk, 'install', reject_extraction)
    assert activation.install(repacked, checksum, root) == receipt


def test_managed_install_failure_leaves_previous_activation(tmp_path, structural_host, monkeypatch):
    root = tmp_path / 'Managed Darwin SDK'
    first, second = managed_packages(tmp_path, structural_host)
    activation.install(*first[:2], root)
    current = root / 'current'
    before = current.resolve()
    prior_replace = activation.os.replace

    def fail_to_publish(source, destination):
        if destination == current:
            raise OSError('interrupted before activation')
        return prior_replace(source, destination)

    monkeypatch.setattr(activation.os, 'replace', fail_to_publish)
    with pytest.raises(OSError, match='interrupted before activation'):
        activation.install(*second[:2], root)
    assert current.resolve() == before
    assert verify_sdk(before, first[2])['seal_sha256'] == first[2]
    assert {path.name for path in root.glob('sdk-*')} == {'sdk-' + first[2]}
    assert not list(root.glob('.incoming-*'))


def test_managed_install_retries_verified_cleanup_after_activation(tmp_path, structural_host, monkeypatch):
    root = tmp_path / 'Managed Darwin SDK'
    first, second = managed_packages(tmp_path, structural_host)
    activation.install(*first[:2], root)
    retired = root / ('sdk-' + first[2])
    previous_unlink = Path.unlink
    deleted = []

    def fail_after_one_file(path, *args, **kwargs):
        if path.is_relative_to(retired):
            if deleted:
                raise PermissionError('retired generation partially removed')
            deleted.append(path)
        return previous_unlink(path, *args, **kwargs)

    with monkeypatch.context() as failure:
        failure.setattr(Path, 'unlink', fail_after_one_file)
        updated = activation.install(*second[:2], root)
    assert updated['action'] == 'updated'
    assert updated['cleanup_error'] == 'retired generation partially removed'
    assert len(deleted) == 1 and not deleted[0].exists()
    assert activation._retirement_path(retired).is_file()
    assert retired.is_dir() and (root / 'current').resolve() == root / ('sdk-' + second[2]) / 'sdk'
    added = retired / 'sdk/unmanaged-content'
    added.write_text('leave this data intact')
    with pytest.raises(ValueError, match='retirement preserved modified or unowned files'):
        activation.install(*second[:2], root)
    assert added.read_text() == 'leave this data intact'
    added.unlink()
    modified = retired / 'sdk/bin/clang'
    original_bytes = modified.read_bytes()
    modified.write_bytes(b'modified compiler')
    with pytest.raises(ValueError, match='retirement preserved modified or unowned files'):
        activation.install(*second[:2], root)
    assert modified.read_bytes() == b'modified compiler'
    modified.write_bytes(original_bytes)
    assert activation.install(*second[:2], root)['action'] == 'reused'
    assert not retired.exists()
    assert not activation._retirement_path(retired).exists()


def test_managed_uninstall_retries_after_its_payload_manifest_was_removed(tmp_path, structural_host, monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    root = tmp_path / 'Managed Darwin SDK'
    first, _ = managed_packages(tmp_path, structural_host)
    activation.install(*first[:2], root)
    prefix = (root / 'current').resolve()
    previous_rmdir = Path.rmdir

    def stop_after_payload_erasure(path):
        if path == prefix:
            raise PermissionError('controlled removal failure after manifest')
        return previous_rmdir(path)

    with monkeypatch.context() as failure:
        failure.setattr(Path, 'rmdir', stop_after_payload_erasure)
        with pytest.raises(PermissionError, match='after manifest'):
            activation.uninstall(root, first[2])
    assert not (root / 'current').is_symlink()
    assert prefix.is_dir() and not list(prefix.iterdir())
    assert activation._retirement_path(prefix.parent).is_file()
    assert activation.uninstall(root, first[2])['action'] == 'uninstalled'
    assert not prefix.parent.exists()
    assert not activation._retirement_path(prefix.parent).exists()


def test_managed_install_can_reuse_an_intact_sdk_after_interrupted_unpublish(tmp_path, structural_host, monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    root = tmp_path / 'Managed Darwin SDK'
    first, _ = managed_packages(tmp_path, structural_host)
    activation.install(*first[:2], root)
    current = root / 'current'
    generation = current.resolve().parent
    previous_unlink = Path.unlink

    def interrupt_unpublish(path, *args, **kwargs):
        if path == current:
            raise PermissionError('controlled stop before unpublishing')
        return previous_unlink(path, *args, **kwargs)

    with monkeypatch.context() as failure:
        failure.setattr(Path, 'unlink', interrupt_unpublish)
        with pytest.raises(PermissionError, match='before unpublishing'):
            activation.uninstall(root, first[2])
    assert activation._retirement_path(generation).is_file()
    assert activation.install(*first[:2], root)['action'] == 'reused'
    assert current.resolve().parent == generation
    assert not activation._retirement_path(generation).exists()


def test_managed_uninstall_preserves_modified_or_foreign_paths(tmp_path, structural_host, monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    root = tmp_path / 'Managed Darwin SDK'
    first, _ = managed_packages(tmp_path, structural_host)
    unrelated = tmp_path / 'unrelated'
    unrelated.mkdir()
    with pytest.raises(ValueError, match='no managed installation'):
        activation.uninstall(unrelated, first[2])
    assert not list(unrelated.iterdir())
    activation.install(*first[:2], root)
    current = root / 'current'
    before = current.resolve()
    user_data = before / 'user-content'
    user_data.write_text('personal file\n')
    with pytest.raises(ValueError, match='differ from the seal'):
        activation.uninstall(root, first[2])
    assert current.resolve() == before and user_data.read_text() == 'personal file\n'
    current.unlink()
    current.symlink_to(tmp_path / 'candidate-stage-0')
    with pytest.raises(ValueError, match='outside the managed installation'):
        activation.uninstall(root, first[2])
    assert current.resolve() == tmp_path / 'candidate-stage-0'


def test_managed_replay_admits_only_the_exact_run_attempt_and_artifact_pins(tmp_path):
    values = {'run_id': 17, 'run_attempt': 2, 'evidence_artifact_id': 301, 'terminal_artifact_id': 302,
              'revision': 'a' * 40, 'evidence_zip_sha256': 'b' * 64, 'terminal_zip_sha256': 'c' * 64,
              'terminal_sha256': 'd' * 64}
    pin = replay.parse_pin(json.dumps(values))
    repo = {'full_name': 'owner/Repository', 'id': 11}
    run = {'id': 17, 'run_attempt': 2, 'head_sha': 'a' * 40, 'path': replay.SOURCE_WORKFLOW,
           'status': 'completed', 'conclusion': 'success', 'repository': repo, 'head_repository': repo}
    replay.verify_run(run, repo['full_name'], pin)
    with pytest.raises(ValueError, match='successful exact attempt'):
        replay.verify_run({**run, 'run_attempt': 1}, repo['full_name'], pin)
    artifact = {'id': 302, 'name': 'darwin-terminal-17-2', 'digest': 'sha256:' + 'c' * 64,
                'expired': False, 'size_in_bytes': 383,
                'workflow_run': {'id': 17, 'head_sha': 'a' * 40, 'repository_id': 11, 'head_repository_id': 11}}
    replay.verify_artifact(artifact, pin, run, role='terminal')
    for field, wrong in (('id', 301), ('name', 'darwin-terminal-17-1'), ('digest', 'sha256:' + 'b' * 64),
                         ('expired', True)):
        with pytest.raises(ValueError, match='immutable run, attempt and digest pins'):
            replay.verify_artifact({**artifact, field: wrong}, pin, run, role='terminal')
    archive = tmp_path / 'terminal.zip'
    with ZipFile(archive, 'w') as contents:
        contents.writestr('darwin-terminal.json', '{}')
    replay.extract(archive, _file_hash(archive), tmp_path / 'accepted', terminal=True)
    with ZipFile(archive, 'a') as contents:
        contents.writestr('../foreign', '{}')
    with pytest.raises(ValueError, match='unowned or excessive'):
        replay.extract(archive, _file_hash(archive), tmp_path / 'rejected', terminal=True)
    assert not (tmp_path / 'rejected').exists()


@pytest.mark.parametrize('update', [False, True])
def test_managed_replay_runs_exact_admitted_generation_then_reclaims_it(tmp_path, structural_host, monkeypatch, capsys, update):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    primary, replacement = managed_packages(tmp_path, structural_host)
    incoming = tmp_path / 'pinned inputs'
    if update:
        (incoming / 'replacement').mkdir(parents=True)

    def admit(directory):
        archive, digest, seal = primary if directory.name == 'primary' else replacement
        return archive, {'archive_sha256': digest, 'seal_sha256': seal}, {'source': directory.name}

    attempted = []

    def execute(prefix, manifest, host, output, commands):
        assert host == structural_host
        seal = verify_sdk(prefix)['seal_sha256']
        assert manifest['seal_sha256'] == seal
        attempted.append(seal)
        return {'status': 'PASS', 'seal_sha256': seal}

    monkeypatch.setattr(replay, '_admit', admit)
    monkeypatch.setattr(replay.darwin_sdk_arc, 'qualify_consumer', execute)

    run_tools = sdk._run

    def run_public_cli(*args, cwd=None, commands=None):
        if args[0] != sys.executable:
            return run_tools(*args, cwd=cwd, commands=commands)
        assert cwd == sdk.ROOT and args[:5] == (sys.executable, '-B', '-m', 'scripts.objc3c_toolchain', 'darwin')
        argv = list(map(str, args))
        assert cli.main(argv[4:]) == 0
        captured = capsys.readouterr()
        commands.append({'argv': argv, 'exit_status': 0, 'stdout': captured.out, 'stderr': captured.err})
        return captured.out

    monkeypatch.setattr(sdk, '_run', run_public_cli)
    output = tmp_path / 'managed replay evidence'
    report = replay.qualify(incoming, output, receiver_revision='a' * 40)
    assert report['status'] == 'PASS' and not report['support_truth']
    assert report['receiver_revision'] == 'a' * 40
    assert attempted == [primary[2], *([replacement[2]] if update else [])]
    assert report['operations']['uninstall']['seal_sha256'] == attempted[-1]
    assert set(report['operations']) == {'install', 'install_arc', 'reuse', 'uninstall'} | (
        {'update', 'update_arc'} if update else set())
    assert [row['argv'][5] for row in report['commands']] == ['install'] * (3 if update else 2) + ['uninstall']
    assert not list((output / 'Managed Apple Silicon SDK').glob('sdk-*'))


def test_archive_admission_checks_every_arm64_object(tmp_path):
    archive = tmp_path / 'sdk.a'
    archive.write_bytes(thin_archive('runtime.o', macho(1)))
    assert sdk._archive_objects(archive, ['runtime.o']) == ['runtime.o']
    archive.write_bytes(archive.read_bytes() + ar_member('wrong.o', macho(1, cpu=0x01000007)))
    with pytest.raises(ValueError, match='arm64 Mach-O object'):
        sdk._archive_objects(archive)


@pytest.mark.parametrize('wrapped', [False, True])
def test_builtin_staging_uses_archive_format_even_with_one_arm64_slice(tmp_path, monkeypatch, wrapped):
    source = tmp_path / 'libclang_rt.osx.a'
    destination = tmp_path / 'SDK/libclang_rt.osx.a'
    payload = thin_archive('builtins.o', macho(1))
    if wrapped:
        # The official LLVM archive uses one arm64 slice in a FAT container.
        offset = 4096
        header = struct.pack('>7I', 0xcafebabe, 1, 0x0100000c, 0, offset, len(payload), 12)
        source.write_bytes(header.ljust(offset, b'\0') + payload)
    else:
        source.write_bytes(payload)

    calls = []
    def lipo(*args, commands):
        assert args[0] == '/Xcode/bin/lipo'
        calls.append(args[1:])
        if args[1:] == ('-archs', source):
            return 'arm64'
        assert args[1:] == (source, '-thin', 'arm64', '-output', destination)
        assert wrapped
        magic, count, cpu, _, start, length, _ = struct.unpack('>7I', source.read_bytes()[:28])
        assert (magic, count, cpu) == (0xcafebabe, 1, 0x0100000c)
        destination.write_bytes(source.read_bytes()[start:start + length])
        return ''

    monkeypatch.setattr(sdk, '_run', lipo)
    sdk._stage_builtins(source, destination, '/Xcode/bin/lipo', [])
    assert len(calls) == (2 if wrapped else 1)
    assert destination.read_bytes() == payload
    assert sdk._archive_objects(destination) == ['builtins.o']


def test_host_probe_rejects_wrong_xcode(monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    expected = {'/usr/bin/sw_vers': '15.5', '/usr/bin/xcodebuild': 'Xcode 16.3', '/usr/bin/xcrun': '15.5'}
    monkeypatch.setattr(sdk, '_run', lambda tool, *_args, **_kwargs: expected[tool])
    with pytest.raises(ValueError, match='pinned Xcode and macOS SDK pair'):
        sdk._host()
