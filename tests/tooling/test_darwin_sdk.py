"""Exercise Darwin package admission and relocation structure on any test host."""
import io
import json
from pathlib import Path
import platform
import struct
import subprocess
import sys
import tarfile

import pytest

from scripts.objc3c_toolchain import darwin_sdk as sdk, darwin_sdk_arc as arc
from scripts.objc3c_toolchain.package import MANIFEST, SCHEMA, _digest, _file_hash, inventory, normalize_permissions


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


@pytest.fixture
def structural_host(monkeypatch):
    host = {'xcode': sdk.XCODE, 'sdk_version': sdk.SDK_VERSION, 'tools': {
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
    put(path, 'lib/libobjc3darwin.a', thin_archive('runtime.o', macho(1)))
    put(path, 'lib/libobjc3darwin.dylib', macho(6))
    put(path, 'lib/clang/23/include/stddef.h', b'/* checked resource */\n')
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
    with pytest.raises(ValueError, match='fresh prefix'):
        sdk.install(archive, _file_hash(archive), prefix)


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
    result = subprocess.run([sys.executable, '-B', '-m', 'scripts.objc3c_toolchain.darwin_sdk',
        'install', '--archive', str(tmp_path / 'unavailable.tar.gz'), '--archive-sha256', 'a' * 64,
        '--prefix', str(prefix)], cwd=sdk.ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 2
    assert 'requires an Apple Silicon Mac running native arm64 Python' in result.stderr
    assert not result.stdout and not prefix.exists()


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
    with pytest.raises(ValueError, match='Xcode 16.4'):
        sdk._host()
