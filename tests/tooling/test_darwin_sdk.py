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

from scripts.objc3c_toolchain import darwin_sdk as sdk
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


def test_host_probe_rejects_wrong_xcode(monkeypatch):
    monkeypatch.setattr(sdk, 'require_apple_silicon', lambda: None)
    expected = {'/usr/bin/sw_vers': '15.5', '/usr/bin/xcodebuild': 'Xcode 16.3', '/usr/bin/xcrun': '15.5'}
    monkeypatch.setattr(sdk, '_run', lambda tool, *_args, **_kwargs: expected[tool])
    with pytest.raises(ValueError, match='Xcode 16.4'):
        sdk._host()
