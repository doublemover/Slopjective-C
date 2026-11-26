"""Package and qualify the experimental Apple Silicon registration toolchain."""
from __future__ import annotations

import argparse
from contextlib import closing
import gzip
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import struct
import subprocess
import tarfile
import tempfile
import unicodedata

from .darwin_bootstrap import DEPLOYMENT_TARGET, ROOT, require_apple_silicon, verify_macho
from .darwin_registration import (
    BLOCK_SOURCES, BLOCK_WITNESS, CARRIER_DISPATCH_SOURCES, CARRIER_DISPATCH_WITNESS,
    MANAGED_LAYOUT_SOURCES, MANAGED_LAYOUT_WITNESS,
    RUNTIME_HEADER, TYPED_ERROR_SOURCES, TYPED_ERROR_WITNESS, TYPED_METHOD_SOURCES, TYPED_METHOD_WITNESS,
    xcode_cxx_headers,
)
from . import darwin_sdk_arc as arc_consumer
from . import darwin_sdk_driver as installed_driver
from . import darwin_sdk_portable_services as portable_services
from . import darwin_sdk_sanitizers as sanitizers
from . import darwin_sdk_task_services as task_services
from .package import MANIFEST, SCHEMA, _digest, _file_hash, inventory, normalize_permissions, verify_sdk

PROFILE = 'macos-arm64-registration-experimental'
TARGET = 'arm64-apple-macos' + DEPLOYMENT_TARGET
RUNTIME_FLAG = '-fobjc-runtime=gnustep-2.99'
RUNTIME_ABI = 'objc3-darwin-registration-v1'
PAYLOAD_SCHEMA = 'objc3c.experimental-darwin-sdk-payload.v2'
RECEIPT_SCHEMA = 'objc3c.experimental-darwin-sdk-package.v1'
ARCHIVE_ROOT = 'objc3-darwin-sdk'
ARCHIVE_NAME = ARCHIVE_ROOT + '.tar.gz'
XCODE = 'Xcode 16.4\nBuild version 16F6'
SDK_VERSION = '15.5'
HOST_PAIRS = {
    (XCODE, SDK_VERSION): 'xcode-16.4-sdk-15.5',
    ('Xcode 26.3\nBuild version 17C529', '26.2'): 'xcode-26.3-sdk-26.2',
}
DRIVER = 'bin/objc3c'
LIBRARY = 'libobjc3darwin'
SYSTEM = '/usr/lib/libSystem.B.dylib'
DEMO = 'share/objc3c/examples/registration'
DEMO_FILES = ('api.h', 'provider.m', 'main.m', 'late.m', 'late_api.h', 'late_parent.m', *arc_consumer.SOURCES,
              *TYPED_METHOD_SOURCES, *MANAGED_LAYOUT_SOURCES, *BLOCK_SOURCES,
              *TYPED_ERROR_SOURCES, *CARRIER_DISPATCH_SOURCES)
API_HEADER = 'include/objc3_darwin.h'
WITNESS = 'owned Objective-C 3 Darwin registration: PASS\nowned Objective-C 3 Darwin instances: PASS'
MAX_ENTRIES = 10000
MAX_PAYLOAD_BYTES = 1024 ** 3
HEX_SHA = re.compile(r'[a-f0-9]{64}')


def _environment():
    env = {'PATH': '/usr/bin:/bin:/usr/sbin:/sbin', 'LC_ALL': 'C'}
    if 'DEVELOPER_DIR' in os.environ:
        env['DEVELOPER_DIR'] = os.environ['DEVELOPER_DIR']
    return env


def _run(*args, cwd=None, commands=None, rejects=(), trace_libraries=False, includes_stderr=False):
    argv = list(map(str, args))
    environment = _environment()
    if trace_libraries:
        environment['DYLD_PRINT_LIBRARIES'] = '1'
    result = subprocess.run(argv, env=environment, cwd=cwd, capture_output=True,
                            text=True, errors='replace')
    if commands is not None:
        commands.append({'argv': argv, 'exit_status': result.returncode, 'expected_diagnostics': list(rejects),
                         'stdout': result.stdout, 'stderr': result.stderr})
        if trace_libraries:
            commands[-1]['dyld_print_libraries'] = True
    if rejects and (result.returncode not in (1, 2) or any(token not in result.stderr for token in rejects)):
        raise ValueError(f'{argv[0]} did not issue the expected rejection: {result.stderr.strip()}')
    if not rejects and result.returncode:
        raise ValueError(f'{argv[0]} exited {result.returncode}: {result.stderr.strip() or result.stdout.strip()}')
    return (result.stdout + (result.stderr if includes_stderr else '')).strip()


def _host(commands=None):
    require_apple_silicon()
    def apple(*args):
        return _run(*args, commands=commands)

    macos = apple('/usr/bin/sw_vers', '-productVersion')
    if not re.fullmatch(r'[0-9]+(?:\.[0-9]+){1,2}', macos) or int(macos.split('.')[0]) < 15:
        raise ValueError('the experimental package requires native macOS arm64 15.0 or later')
    xcode = apple('/usr/bin/xcodebuild', '-version')
    sdk_version = apple('/usr/bin/xcrun', '--sdk', 'macosx', '--show-sdk-version')
    row = HOST_PAIRS.get((xcode, sdk_version))
    if row is None:
        raise ValueError('the experimental package requires a pinned Xcode and macOS SDK pair')
    developer = Path(apple('/usr/bin/xcode-select', '-p')).resolve(strict=True)
    sdk = Path(apple('/usr/bin/xcrun', '--sdk', 'macosx', '--show-sdk-path')).resolve(strict=True)
    if not sdk.is_dir() or not sdk.is_relative_to(developer):
        raise ValueError('the selected macOS SDK must belong to the active Xcode tree')
    tools = {}
    for name in ('ld', 'lipo', 'otool', 'dsymutil'):
        tool = Path(apple('/usr/bin/xcrun', '--sdk', 'macosx', '--find', name)).resolve(strict=True)
        if not tool.is_file() or not os.access(tool, os.X_OK) or not tool.is_relative_to(developer):
            raise ValueError(f'expected executable {name} inside the active Xcode tree')
        tools[name] = {'path': str(tool), 'sha256': _file_hash(tool)}
    lipo = tools['lipo']['path']
    for name, tool in tools.items():
        if not {'arm64', 'arm64e'}.intersection(apple(lipo, '-archs', tool['path']).split()):
            raise ValueError(f'Xcode {name} is missing native arm64 support')
    clang = Path(apple('/usr/bin/xcrun', '--sdk', 'macosx', '--find', 'clang++')).resolve(strict=True)
    if not clang.is_relative_to(developer):
        raise ValueError('Apple Clang++ is outside the selected Xcode tree')
    search = _run(clang, '--target=' + TARGET, '-isysroot', sdk, '-stdlib=libc++', '-E', '-v',
                  '-x', 'c++', '/dev/null', '-o', '/dev/null', commands=commands, includes_stderr=True)
    config = xcode_cxx_headers(search, developer) / '__config'
    return {'os': 'macOS', 'arch': 'arm64', 'version': macos, 'xcode': xcode, 'row': row,
            'sdk_version': sdk_version, 'sdk_path': str(sdk), 'developer_dir': str(developer), 'tools': tools,
            'sdk_settings_sha256': _file_hash(sdk / 'SDKSettings.json'),
            'libcxx': {'config_relative': config.relative_to(developer).as_posix(),
                       'config_sha256': _file_hash(config), 'stub_sha256': _file_hash(sdk / 'usr/lib/libc++.tbd')}}


def _host_requirements(host):
    return {'xcode': host['xcode'], 'sdk_version': host['sdk_version'], 'deployment_target': DEPLOYMENT_TARGET,
            'row': HOST_PAIRS[(host['xcode'], host['sdk_version'])],
            'sdk_settings_sha256': host['sdk_settings_sha256'],
            'libcxx': host['libcxx'],
            'tools': {name: tool['sha256'] for name, tool in host['tools'].items()}}


def _load_paths(tool, artifact, expected, commands=None):
    lines = _run(tool, '-L', artifact, commands=commands).splitlines()
    dependencies = [line.strip().split(' (', 1)[0] for line in lines[1:]]
    if not lines or sorted(dependencies) != sorted(expected):
        raise ValueError(f'unexpected Mach-O runtime dependencies at {artifact}: {dependencies}')
    return dependencies


def _system_tool_dependencies(otool, tool, commands=None):
    lines = _run(otool, '-L', tool, commands=commands).splitlines()
    dependencies = [line.strip().split(' (', 1)[0] for line in lines[1:]]
    if not dependencies or any(not path.startswith(('/usr/lib/', '/System/Library/')) or '..' in PurePosixPath(path).parts
                               for path in dependencies):
        raise ValueError(f'{tool} has dependencies outside the macOS system libraries: {dependencies}')
    return dependencies


def _rpaths(otool, artifact, expected, commands):
    lines = _run(otool, '-l', artifact, commands=commands).splitlines()
    paths = [next((item.strip().removeprefix('path ').split(' (offset ', 1)[0]
                   for item in lines[index + 1:index + 4] if item.strip().startswith('path ')), '')
             for index, line in enumerate(lines) if line.strip() == 'cmd LC_RPATH']
    if paths != expected:
        raise ValueError(f'unexpected Mach-O rpaths at {artifact}: {paths}')
    return paths


def _deployment(otool, artifact, commands, *, sdk_version):
    lines = [line.strip() for line in _run(otool, '-l', artifact, commands=commands).splitlines()]
    versions = [dict(item.split(maxsplit=1) for item in lines[index + 1:index + 8]
                     if item.startswith(('platform ', 'minos ', 'sdk ')))
                for index, line in enumerate(lines) if line == 'cmd LC_BUILD_VERSION']
    if (len(versions) != 1 or versions[0].get('platform') not in ('macos', 'MACOS', '1') or
            versions[0].get('minos') not in (DEPLOYMENT_TARGET, DEPLOYMENT_TARGET + '.0') or
            versions[0].get('sdk') not in (sdk_version, sdk_version + '.0')):
        raise ValueError(f'unexpected Mach-O platform or deployment target at {artifact}: {versions}')
    return versions[0]


def _archive_objects(path, expected_members=None):
    """Check arm64 object headers within an ordinary Darwin BSD archive."""
    names = set()
    size = path.stat().st_size
    with path.open('rb') as source:
        if source.read(8) != b'!<arch>\n':
            raise ValueError(f'expected an arm64 Darwin archive at {path}')
        while source.tell() < size:
            header = source.read(60)
            if len(header) != 60 or header[-2:] != b'`\n' or not header[48:58].strip().isdigit():
                raise ValueError(f'invalid Darwin archive record at {path}')
            count = int(header[48:58])
            end = source.tell() + count + count % 2
            if end > size:
                raise ValueError(f'truncated Darwin archive record at {path}')
            name = header[:16].decode('ascii').strip()
            if name.startswith('#1/'):
                length = name[3:]
                if not length.isdecimal() or not 1 <= int(length) <= min(count, 4096):
                    raise ValueError(f'invalid Darwin archive name at {path}')
                length = int(length)
                name = source.read(length).rstrip(b'\0').decode('ascii')
                count -= length
            name = name.removesuffix('/')
            if name not in ('__.SYMDEF', '__.SYMDEF SORTED', '__.SYMDEF_64', '__.SYMDEF_64 SORTED'):
                header = source.read(min(count, 32))
                if len(header) != 32 or struct.unpack('<4I', header[:16])[:2] != (0xfeedfacf, 0x0100000c) or int.from_bytes(header[12:16], 'little') != 1:
                    raise ValueError(f'expected an arm64 Mach-O object in {path}: {name}')
                if not name.endswith('.o') or '/' in name or name in names:
                    raise ValueError(f'invalid or duplicate Darwin archive member at {path}: {name}')
                names.add(name)
            source.seek(end)
    if not names or (expected_members is not None and names != set(expected_members)):
        raise ValueError(f'unexpected Darwin archive members at {path}')
    return sorted(names)


def _checked_file(path, expected_sha256, root=None):
    if not isinstance(expected_sha256, str) or not HEX_SHA.fullmatch(expected_sha256):
        raise ValueError('missing or malformed artifact SHA-256')
    physical = path.resolve(strict=True)
    if (root is not None and not physical.is_relative_to(root)) or not physical.is_file() or _file_hash(physical) != expected_sha256:
        raise ValueError(f'Darwin source artifact differs from its producer receipt: {path}')
    return physical


def _copied_file(source, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)


def _stage_builtins(source, destination, lipo, commands):
    arches = _run(lipo, '-archs', source, commands=commands).split()
    if 'arm64' not in arches:
        raise ValueError('Darwin compiler builtins lack arm64')
    with source.open('rb') as archive:
        header = archive.read(8)
    if header == b'!<arch>\n':
        _copied_file(source, destination)
    elif header[:4] in (b'\xca\xfe\xba\xbe', b'\xca\xfe\xba\xbf'):
        # The pinned archive is a FAT container even though lipo lists one slice.
        destination.parent.mkdir(parents=True, exist_ok=True)
        _run(lipo, source, '-thin', 'arm64', '-output', destination, commands=commands)
    else:
        raise ValueError('Darwin compiler builtins have an unsupported archive format')
    _archive_objects(destination)


def _validate_manifest(data, host):
    if (data.get('payload_schema') != PAYLOAD_SCHEMA or data.get('profile') != PROFILE or
            data.get('target') != TARGET or data.get('runtime_abi') != RUNTIME_ABI or
            data.get('compiler_runtime_flag') != RUNTIME_FLAG or data.get('support_truth') is not False or
            data.get('host_requirements') != _host_requirements(host)):
        raise ValueError('this package does not match the admitted experimental Darwin host and ABI')
    resource = data.get('resource_dir')
    if not isinstance(resource, str) or not re.fullmatch(r'lib/clang/[1-9][0-9]*', resource):
        raise ValueError('the package has an invalid Clang resource path')
    required = {DRIVER, 'bin/clang', 'bin/llvm-ar', 'bin/llvm-dwarfdump', f'lib/{LIBRARY}.a', f'lib/{LIBRARY}.dylib', API_HEADER,
                resource + '/include/stddef.h', resource + '/lib/darwin/libclang_rt.osx.a',
                *(DEMO + '/' + name for name in DEMO_FILES), 'share/licenses/slopjective-c/LICENSE',
                *(f'share/licenses/{name}/LICENSE.TXT' for name in ('llvm', 'clang', 'compiler-rt'))}
    required.update(sanitizers.manifest_files(data, resource))
    required.update(portable_services.manifest_files(data))
    if task_services.KEY in data:
        required.update(task_services.manifest_files(data))
    files = {name for name, row in data['files'].items() if row['kind'] == 'file'}
    if not required <= files or any(not name.startswith(resource + '/include/') for name in files - required):
        raise ValueError('the Darwin SDK payload inventory differs from the experimental profile')
    return resource


def _validate_product(prefix, data, host, commands=None):
    resource = _validate_manifest(data, host)
    otool = host['tools']['otool']['path']
    if not os.access(prefix / DRIVER, os.X_OK):
        raise ValueError('installed Objective-C 3 driver must be executable')
    for tool in ('clang', 'llvm-ar', 'llvm-dwarfdump'):
        binary = prefix / 'bin' / tool
        if not os.access(binary, os.X_OK):
            raise ValueError(f'packaged {tool} must be executable')
        verify_macho(binary, 'executable')
        _system_tool_dependencies(otool, binary, commands)
    archive = prefix / 'lib' / (LIBRARY + '.a')
    dylib = prefix / 'lib' / (LIBRARY + '.dylib')
    verify_macho(dylib, 'dynamic_library')
    _archive_objects(archive, ['runtime.o'])
    _archive_objects(prefix / resource / 'lib/darwin/libclang_rt.osx.a')
    _load_paths(otool, dylib, [SYSTEM, '@rpath/' + dylib.name], commands)
    _rpaths(otool, dylib, [], commands)
    _deployment(otool, dylib, commands, sdk_version=host['sdk_version'])
    if sanitizers.KEY in data:
        sanitizers.validate_runtimes(prefix, data, host, commands)
    if portable_services.KEY in data:
        portable_services.validate_payload(prefix, data, commands)
    if task_services.KEY in data:
        task_services.validate_archive(prefix, data, commands)


def _write_archive(prefix, destination):
    def regular(path):
        if not path.is_file() or path.is_symlink():
            raise ValueError(f'Darwin SDK payload must contain regular files: {path}')
        return path

    def normalize(info):
        info.uid = info.gid = 0
        info.uname = info.gname = ''
        info.mtime = 0
        return info

    with destination.open('xb') as disk, gzip.GzipFile(filename='', mode='wb', fileobj=disk, mtime=0) as compressed:
        with tarfile.open(fileobj=compressed, mode='w|') as archive:
            archive.add(regular(prefix / MANIFEST), arcname=f'{ARCHIVE_ROOT}/{MANIFEST}', filter=normalize)
            for path in sorted(prefix.rglob('*')):
                if path.is_dir() and not path.is_symlink():
                    continue
                if path != prefix / MANIFEST:
                    archive.add(regular(path), arcname=f'{ARCHIVE_ROOT}/{path.relative_to(prefix)}', filter=normalize)


def package_registration(clang: Path, llvm_source: Path, bootstrap_report: Path, registration: dict,
                         output: Path, *, sdk_sanitizers: bool = False) -> dict:
    """Package live compiler and runtime outputs from the registration owner."""
    commands = []
    host = _host(commands)
    bootstrap = json.loads(bootstrap_report.read_bytes())
    profile = registration['profile']
    revision = registration['revision']
    if (registration.get('schema') != 'objc3c.darwin-registration-qualification.v1' or
            registration.get('status') != 'registration-verified' or registration.get('support_truth') is not False or
            bootstrap.get('schema') != 'objc3c.darwin-bootstrap-probe.v1' or
            bootstrap.get('status') != 'bootstrap-verified' or bootstrap.get('support_truth') is not False or
            registration['bootstrap_sha256'] != _file_hash(bootstrap_report) or
            profile.get('profile') != PROFILE or profile.get('target') != TARGET or
            profile.get('compiler_runtime_flag') != RUNTIME_FLAG or profile.get('runtime_abi') != RUNTIME_ABI or
            not re.fullmatch('[a-f0-9]{40}', revision) or bootstrap.get('checkout_revision') != revision):
        raise ValueError('package generation requires a matching experimental Darwin registration and bootstrap receipt')
    producer = registration['compiler_build']
    compiler = _checked_file(clang, producer['artifact']['sha256'])
    layout = Path(registration['installation_root']).resolve(strict=True)
    llvm = bootstrap['llvm_identity']
    llvm_root = Path(llvm['root']).resolve(strict=True)
    pinned = json.loads((ROOT / 'toolchains/llvm/manifest.json').read_bytes())['upstream']
    if llvm['revision'] != pinned['commit'] or llvm['version'] != pinned['tag'].removeprefix('llvmorg-'):
        raise ValueError('Darwin bootstrap LLVM identity differs from the source pin')
    verify_macho(compiler, 'executable')
    _system_tool_dependencies(host['tools']['otool']['path'], compiler, commands)
    version = _run(compiler, '--no-default-config', '--version', commands=commands).split('\n', 1)[0]
    if not all(marker in version for marker in (llvm['revision'], f'clang version {llvm["version"]}',
            f'ObjC3-patches={producer["patch_digest"]}', f'ObjC3-config={producer["configuration_sha256"]}')):
        raise ValueError('Darwin compiler does not report the receipt source and configuration identity')
    resource = Path(registration['compiler_builtins']['path']).resolve(strict=True).parents[2]
    builtins = _checked_file(resource / 'lib/darwin/libclang_rt.osx.a',
                             registration['compiler_builtins']['sha256'], llvm_root)
    resource_dir = str(resource.relative_to(llvm_root))
    if not re.fullmatch(r'lib/clang/[1-9][0-9]*', resource_dir):
        raise ValueError('Darwin bootstrap Clang resource is outside the admitted LLVM layout')
    output = output.resolve()
    output.mkdir(parents=True)
    with tempfile.TemporaryDirectory(prefix='.stage-', dir=output) as directory:
        prefix = Path(directory) / ARCHIVE_ROOT
        _copied_file(Path(installed_driver.__file__), prefix / DRIVER)
        (prefix / DRIVER).chmod(0o755)
        _copied_file(compiler, prefix / 'bin/clang')
        _checked_file(prefix / 'bin/clang', producer['artifact']['sha256'])
        for name in ('llvm-ar', 'llvm-dwarfdump'):
            row = llvm['tools'][name]
            tool = Path(row['path'])
            tool_binary = tool.resolve(strict=True)
            if not tool_binary.is_relative_to(llvm_root):
                raise ValueError(f'Darwin bootstrap tool escapes its LLVM root: {name}')
            verify_macho(tool, 'executable')
            _system_tool_dependencies(host['tools']['otool']['path'], tool, commands)
            if _run(tool, '--version', commands=commands) != row['version']:
                raise ValueError(f'Darwin bootstrap tool differs from its receipt: {name}')
            _copied_file(tool_binary, prefix / 'bin' / name)
        for name, key in ((LIBRARY + '.a', LIBRARY + '.a'), (LIBRARY + '.dylib', 'lib/' + LIBRARY + '.dylib')):
            source = _checked_file(layout / 'lib' / name, registration['artifacts'][key]['sha256'], layout)
            _copied_file(source, prefix / 'lib' / name)
            _checked_file(prefix / 'lib' / name, registration['artifacts'][key]['sha256'])
        for header in sorted((resource / 'include').rglob('*')):
            if header.is_symlink() or not (header.is_file() or header.is_dir()):
                raise ValueError(f'unsupported resource tree member: {header}')
            if header.is_file():
                _copied_file(header, prefix / resource_dir / header.relative_to(resource))
        package_builtins = prefix / resource_dir / 'lib/darwin' / builtins.name
        _stage_builtins(builtins, package_builtins, host['tools']['lipo']['path'], commands)
        for name in DEMO_FILES:
            fixture = ROOT / 'tests/clang_foundation/darwin_registration' / name
            _copied_file(_checked_file(fixture, registration['fixtures'][name]), prefix / DEMO / name)
            _checked_file(prefix / DEMO / name, registration['fixtures'][name])
        _copied_file(_checked_file(RUNTIME_HEADER, registration['runtime_header_sha256']), prefix / API_HEADER)
        for project in ('llvm', 'clang', 'compiler-rt'):
            _copied_file(llvm_source / project / 'LICENSE.TXT', prefix / 'share/licenses' / project / 'LICENSE.TXT')
        _copied_file(ROOT / 'LICENSE', prefix / 'share/licenses/slopjective-c/LICENSE')
        portable_payload = portable_services.stage_payload(prefix, registration, commands)
        task_payload = task_services.stage_payload(prefix, host, commands)
        sanitizer_payload = sanitizers.stage_payload(prefix, resource, resource_dir) if sdk_sanitizers else None
        normalize_permissions(prefix)
        data = {'schema': SCHEMA, 'payload_schema': PAYLOAD_SCHEMA, 'profile': PROFILE, 'target': TARGET,
                'runtime_abi': RUNTIME_ABI, 'compiler_runtime_flag': RUNTIME_FLAG, 'support_truth': False,
                'revision': revision, 'bootstrap_sha256': registration['bootstrap_sha256'],
                'registration_evidence_sha256': _digest(registration),
                'runtime_source_sha256': registration['runtime_source_sha256'], 'compiler_version': version,
                'bootstrap_llvm': {key: llvm[key] for key in ('version', 'revision')},
                'bootstrap_builtins_sha256': registration['compiler_builtins']['sha256'],
                'compiler_build': {key: producer[key] for key in ('base_build_id', 'patch_digest', 'configuration_sha256')},
                'host_requirements': _host_requirements(host), 'resource_dir': resource_dir,
                'files': inventory(prefix), portable_services.KEY: portable_payload, task_services.KEY: task_payload}
        if cxx := registration.get('cxx_toolchain'):
            if (cxx['header_config_sha256'] != host['libcxx']['config_sha256'] or
                    cxx['library_stub_sha256'] != host['libcxx']['stub_sha256']):
                raise ValueError('Darwin package and compiler build selected different libc++ inputs')
            data['compiler_host_libcxx'] = {key: cxx[key] for key in
                                          ('header_config_sha256', 'library_stub_sha256', 'reported_version')}
        if sanitizer_payload is not None:
            data[sanitizers.KEY] = sanitizer_payload
        seal = _digest(data)
        (prefix / MANIFEST).write_text(json.dumps({**data, 'seal_sha256': seal}, indent=2) + '\n')
        (prefix / MANIFEST).chmod(0o644)
        verify_sdk(prefix, seal)
        _validate_product(prefix, data, host, commands)
        archive = output / ARCHIVE_NAME
        pending = Path(directory) / ARCHIVE_NAME
        _write_archive(prefix, pending)
        pending.rename(archive)
    receipt = {'schema': RECEIPT_SCHEMA, 'status': 'PASS', 'support_truth': False, 'profile': PROFILE,
               'revision': revision, 'archive': archive.name, 'archive_sha256': _file_hash(archive),
               'archive_bytes': archive.stat().st_size, 'seal_sha256': seal, 'host': host, 'commands': commands,
               portable_services.KEY: portable_payload, task_services.KEY: task_payload}
    if sanitizer_payload is not None:
        receipt[sanitizers.KEY] = sanitizer_payload
    (output / 'package.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return receipt


def _relative(relative):
    path = PurePosixPath(relative)
    if (not relative or relative == '.' or path.is_absolute() or '..' in path.parts or
            path.as_posix() != relative or '\\' in relative or any(ord(c) < 32 for c in relative)):
        raise ValueError(f'invalid Darwin archive path: {relative!r}')
    return path


def _unpack(archive, prefix, host=None):
    with tarfile.open(fileobj=archive, mode='r|gz') as tar:
        first = tar.next()
        if first is None or first.name != f'{ARCHIVE_ROOT}/{MANIFEST}' or first.type != tarfile.REGTYPE or first.size > 8 * 1024 ** 2:
            raise ValueError('Darwin archive must begin with its regular payload manifest')
        with closing(tar.extractfile(first)) as source:
            manifest = source.read()
        data = json.loads(manifest)
        if not isinstance(data, dict) or data.get('schema') != SCHEMA:
            raise ValueError('Darwin archive has an unsupported manifest')
        seal = data.pop('seal_sha256', None)
        if seal != _digest(data):
            raise ValueError('Darwin archive manifest seal is invalid')
        entries = data.get('files')
        if not isinstance(entries, dict) or not 1 <= len(entries) <= MAX_ENTRIES or entries.get('.') != {'kind': 'directory', 'mode': 0o755}:
            raise ValueError('Darwin archive has an invalid inventory')
        admitted, directories, folded = {}, [], {MANIFEST.casefold()}
        for relative, row in entries.items():
            if relative == '.':
                continue
            path = _relative(relative)
            if not isinstance(row, dict):
                raise ValueError(f'invalid Darwin archive inventory record: {relative}')
            key = unicodedata.normalize('NFC', relative).casefold()
            if key in folded or entries.get(str(path.parent)) != {'kind': 'directory', 'mode': 0o755}:
                raise ValueError(f'Darwin archive has a colliding path or missing parent: {relative}')
            folded.add(key)
            if row == {'kind': 'directory', 'mode': 0o755}:
                directories.append(path)
            elif (row.get('kind') == 'file' and row.get('mode') in (0o644, 0o755) and
                  isinstance(row.get('sha256'), str) and HEX_SHA.fullmatch(row['sha256'])):
                admitted[relative] = row
            else:
                raise ValueError(f'invalid Darwin archive inventory record: {relative}')
        if host is not None:
            _validate_manifest(data, host)
        prefix.mkdir(mode=0o755)
        prefix.chmod(0o755)
        for directory in sorted(directories, key=lambda path: len(path.parts)):
            (prefix / directory).mkdir(mode=0o755)
            (prefix / directory).chmod(0o755)
        total = 0
        while member := tar.next():
            name = member.name.removeprefix(ARCHIVE_ROOT + '/')
            total += member.size
            row = admitted.pop(name, None)
            if (name == member.name or member.type != tarfile.REGTYPE or row is None or member.size < 0 or
                    stat.S_IMODE(member.mode) != row['mode'] or total > MAX_PAYLOAD_BYTES):
                raise ValueError(f'unexpected Darwin archive member: {member.name}')
            path = prefix / _relative(name)
            with closing(tar.extractfile(member)) as source, path.open('xb') as destination:
                shutil.copyfileobj(source, destination)
            path.chmod(row['mode'])
        if admitted:
            raise ValueError('Darwin archive omitted sealed payload files')
        (prefix / MANIFEST).write_bytes(manifest)
        (prefix / MANIFEST).chmod(0o644)
        return verify_sdk(prefix)


def install(archive: Path, expected_sha256: str, prefix: Path, *, commands=None) -> dict:
    host = _host(commands)
    if prefix.is_symlink() or prefix.exists():
        raise ValueError('install requires a fresh prefix')
    if not HEX_SHA.fullmatch(expected_sha256):
        raise ValueError('installation requires the lowercase package archive SHA-256')
    archive = archive.resolve(strict=True)
    if not archive.is_file():
        raise ValueError('installation requires a regular archive')
    prefix = prefix.parent.resolve(strict=True) / prefix.name
    with tempfile.TemporaryDirectory(prefix='.objc3-darwin-', dir=prefix.parent) as directory:
        staged = Path(directory) / ARCHIVE_ROOT
        with archive.open('rb') as media:
            if hashlib.file_digest(media, 'sha256').hexdigest() != expected_sha256:
                raise ValueError('Darwin package archive SHA-256 does not match the receipt')
            media.seek(0)
            product = _unpack(media, staged, host)
            media.seek(0)
            if hashlib.file_digest(media, 'sha256').hexdigest() != expected_sha256:
                raise ValueError('Darwin package archive changed during extraction')
        _validate_product(staged, product, host, commands)
        prefix.mkdir()
        try:
            staged.rename(prefix)
        except OSError:
            prefix.rmdir()
            raise
    receipt = {'schema': 'objc3c.experimental-darwin-sdk-install.v1', 'status': 'PASS', 'support_truth': False,
               'archive_sha256': expected_sha256, 'seal_sha256': product['seal_sha256'], 'prefix': str(prefix), 'host': host}
    if sanitizers.KEY in product:
        receipt[sanitizers.KEY] = product[sanitizers.KEY]
    if portable_services.KEY in product:
        receipt[portable_services.KEY] = product[portable_services.KEY]
    if task_services.KEY in product:
        receipt[task_services.KEY] = product[task_services.KEY]
    return receipt


def _qualify_installer_controls(archive, expected_sha256, output, manifest, host, commands):
    def different(signature):
        return ('1' if signature.startswith('0') else '0') + signature[1:]

    def snapshot(prefix):
        if not prefix.exists() and not prefix.is_symlink():
            return None
        seal = prefix / MANIFEST
        return _digest({'inventory': inventory(prefix), 'manifest': _file_hash(seal) if seal.is_file() else None})

    observations = {}
    with tempfile.TemporaryDirectory(prefix='.installer-controls-', dir=output) as work:
        work = Path(work)

        def reject(name, media, checksum, prefix, diagnostic):
            before = snapshot(prefix)
            try:
                install(media, checksum, prefix, commands=commands)
            except ValueError as error:
                if str(error) != diagnostic:
                    raise ValueError(f'Darwin installer control {name} failed unexpectedly: {error}') from error
            else:
                raise ValueError(f'Darwin installer accepted negative control {name}')
            after = snapshot(prefix)
            if before != after or list(work.glob('.objc3-darwin-*')):
                raise ValueError(f'Darwin installer control {name} changed the destination or left staging files')
            observations[name] = {'status': 'REJECTED', 'diagnostic': diagnostic,
                                  'requested_archive_sha256': checksum,
                                  'prefix_before_sha256': before, 'prefix_after_sha256': after}

        reject('archive-digest', archive, different(expected_sha256), work / 'Wrong digest',
               'Darwin package archive SHA-256 does not match the receipt')
        incomplete = work / 'Incomplete prefix'
        incomplete.mkdir()
        reject('incomplete-prefix', archive, expected_sha256, incomplete, 'install requires a fresh prefix')
        damaged = work / 'Damaged prefix'
        (damaged / 'bin').mkdir(parents=True)
        (damaged / 'bin/clang').write_bytes(b'damaged installed compiler\n')
        (damaged / MANIFEST).write_text(json.dumps(manifest) + '\n')
        reject('damaged-prefix', archive, expected_sha256, damaged, 'install requires a fresh prefix')

        actual = host['tools']['ld']['sha256']
        requested = different(actual)
        foreign = dict(manifest)
        foreign.pop('seal_sha256')
        requirements = foreign['host_requirements']
        foreign['host_requirements'] = {**requirements, 'tools': {**requirements['tools'], 'ld': requested}}
        seal = _digest(foreign)
        fixture = work / 'Mismatched Apple linker manifest'
        fixture.mkdir()
        (fixture / MANIFEST).write_text(json.dumps({**foreign, 'seal_sha256': seal}) + '\n')
        media = work / 'wrong-host.tar.gz'
        # The installer admits the sealed host identity before extracting the payload.
        _write_archive(fixture, media)
        reject('apple-linker-identity', media, _file_hash(media), work / 'Wrong linker',
               'this package does not match the admitted experimental Darwin host and ABI')
        observations['apple-linker-identity'].update(tool='ld', observed_tool_sha256=actual,
            required_tool_sha256=requested, manifest_seal_sha256=seal)
    return {'schema': 'objc3c.experimental-darwin-sdk-installer-controls.v1',
            'archive_sha256': expected_sha256, 'observations': observations}


def relocated_clang_arguments(prefix: Path, sdk_path: Path, resource: Path) -> list[str | Path]:
    return [prefix / 'bin/clang', '--no-default-config', '--target=' + TARGET,
            '-isysroot', sdk_path, '-resource-dir', resource, '-I', prefix / 'include']


def qualify(archive: Path, expected_sha256: str, output: Path) -> dict:
    require_apple_silicon()
    output = output.resolve()
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.experimental-darwin-sdk-qualification.v1', 'status': 'FAIL',
              'support_truth': False, 'commands': []}
    commands = report['commands']
    try:
        installed = install(archive, expected_sha256, output / 'Relocated Objective-C 3 SDK', commands=commands)
        report['installation'] = installed
        prefix = Path(installed['prefix'])
        manifest = verify_sdk(prefix, installed['seal_sha256'])
        host = installed['host']
        project = output / 'Fresh native consumer'
        project.mkdir()
        clang, ar, dwarf = (prefix / 'bin' / name for name in ('clang', 'llvm-ar', 'llvm-dwarfdump'))
        resource = prefix / manifest['resource_dir']
        def native(*args, rejects=(), trace_libraries=False):
            return _run(*args, cwd=project, commands=commands, rejects=rejects, trace_libraries=trace_libraries)

        if native(clang, '--no-default-config', '--version').split('\n', 1)[0] != manifest['compiler_version']:
            raise ValueError('relocated compiler source/configuration identity changed')
        defaults = relocated_clang_arguments(prefix, Path(host['sdk_path']), resource)
        objc = [*defaults, '-fobjc-version=3', RUNTIME_FLAG, '-fno-objc-arc', '-fno-objc-exceptions', '-O2', '-gdwarf-4']
        link = [*defaults, '--ld-path=' + host['tools']['ld']['path']]
        driver = prefix / DRIVER
        if native(driver, '--version').split('\n', 1)[0] != manifest['compiler_version']:
            raise ValueError('installed objc3c driver selected a different compiler')
        for name in ('provider', 'main', 'late', 'late_parent'):
            native(driver, '-fno-objc-arc', '-fno-objc-exceptions', '-O2', '-gdwarf-4',
                   '-c', prefix / DEMO / (name + '.m'), '-o', name + '.o')
        native(driver, '-fobjc-runtime=macosx-15.0', '-c', prefix / DEMO / 'provider.m', '-o', 'apple.o',
               rejects=('overrides the installed Objective-C 3 SDK or runtime',))
        native(*objc, '-fobjc-arc', '-fobjc-exceptions', '-c', prefix / DEMO / 'provider.m', '-o', 'arc.o',
               rejects=('experimental Darwin ARC', '-fobjc-exceptions'))
        native(*objc, '--target=x86_64-apple-macos15.0', '-c', prefix / DEMO / 'provider.m', '-o', 'x86.o',
               rejects=(RUNTIME_FLAG, 'x86_64'))
        native(ar, '--format=darwin', 'rcs', 'libprovider.a', 'provider.o')
        _archive_objects(project / 'libprovider.a', ['provider.o'])
        native(*link, 'main.o', 'libprovider.a', '-o', 'no-runtime',
               rejects=('objc3_darwin_lookup_v1', 'objc3_darwin_register_v1'))
        native(driver, '--runtime-link=static', 'main.o', 'libprovider.a', '-o', 'static')
        runtime = prefix / 'lib' / (LIBRARY + '.dylib')
        rpath = '@loader_path/../' + prefix.name + '/lib'
        parent_id = '@rpath/liblate-parent.dylib'
        native(*link, '-dynamiclib', 'late_parent.o', runtime, '-Wl,-install_name,' + parent_id,
               '-Wl,-rpath,' + rpath, '-o', 'liblate-parent.dylib')
        native(*link, '-dynamiclib', 'late.o', 'liblate-parent.dylib', runtime, '-Wl,-install_name,@rpath/liblate.dylib',
               '-Wl,-rpath,@loader_path', '-Wl,-rpath,' + rpath, '-o', 'liblate.dylib')
        native(driver, 'main.o', 'libprovider.a', '-o', 'dynamic')
        debug = project / 'static.dSYM'
        native(host['tools']['dsymutil']['path'], 'static', '-o', debug)
        debug_file = debug / 'Contents/Resources/DWARF/static'
        native(dwarf, '--verify', debug_file)
        if 'DW_TAG_compile_unit' not in native(dwarf, '--debug-info', debug_file):
            raise ValueError('relocated compiler output lacks native DWARF compilation units')
        reports = {}
        for name, kind, dependencies in (
                ('provider.o', 'object', None), ('static', 'executable', [SYSTEM]),
                ('dynamic', 'executable', [SYSTEM, '@rpath/' + runtime.name]),
                ('liblate-parent.dylib', 'dynamic_library', [SYSTEM, parent_id, '@rpath/' + runtime.name]),
                ('liblate.dylib', 'dynamic_library', [SYSTEM, '@rpath/liblate.dylib', parent_id, '@rpath/' + runtime.name])):
            artifact = project / name
            reports[name] = verify_macho(artifact, kind)
            if dependencies is not None:
                reports[name]['load_paths'] = _load_paths(host['tools']['otool']['path'], artifact, dependencies, commands)
                expected_rpaths = {'static': [], 'dynamic': [str(prefix / 'lib')],
                                   'liblate-parent.dylib': [rpath], 'liblate.dylib': ['@loader_path', rpath]}
                reports[name]['rpaths'] = _rpaths(host['tools']['otool']['path'], artifact, expected_rpaths[name], commands)
                reports[name]['deployment'] = _deployment(host['tools']['otool']['path'], artifact, commands,
                                                           sdk_version=host['sdk_version'])
        reports['debug'] = verify_macho(debug_file, 'debug_info')
        if native(project / 'static') != WITNESS or native(project / 'dynamic', project / 'liblate.dylib') != WITNESS:
            raise ValueError('cold relocated compiler/runtime witness failed')
        for family, sources, witness, flags in (
                ('typed-method', TYPED_METHOD_SOURCES, TYPED_METHOD_WITNESS, []),
                ('managed-layout', MANAGED_LAYOUT_SOURCES, MANAGED_LAYOUT_WITNESS, []),
                ('block-lifetime', BLOCK_SOURCES, BLOCK_WITNESS, ['-fblocks']),
                ('typed-errors', TYPED_ERROR_SOURCES, TYPED_ERROR_WITNESS, []),
                ('carrier-dispatch', CARRIER_DISPATCH_SOURCES, CARRIER_DISPATCH_WITNESS, [])):
            objects = []
            for source in (name for name in sources if name.endswith('.m')):
                name = source.removesuffix('.m') + '.o'
                native(*objc, '-fobjc-arc', *flags, '-I', prefix / 'include', '-c', prefix / DEMO / source, '-o', name)
                reports[name] = verify_macho(project / name, 'object')
                objects.append(name)
            binary = project / family
            native(*link, *objects, runtime, '-Wl,-rpath,' + rpath, '-o', binary)
            reports[family] = verify_macho(binary, 'executable')
            otool = host['tools']['otool']['path']
            reports[family].update(load_paths=_load_paths(otool, binary, [SYSTEM, '@rpath/' + runtime.name], commands),
                                   rpaths=_rpaths(otool, binary, [rpath], commands),
                                   deployment=_deployment(otool, binary, commands, sdk_version=host['sdk_version']))
            if native(binary, trace_libraries=True) != witness:
                raise ValueError(f'installed Darwin {family} witness failed')
            reports[family]['loaded_runtime'] = arc_consumer.loaded_runtime(commands[-1]['stderr'], runtime)
        report['arc'] = arc_consumer.qualify_consumer(prefix, manifest, host, output / arc_consumer.DIRECTORY, commands)
        if portable_services.KEY in manifest:
            report[portable_services.KEY] = portable_services.qualify_consumer(
                prefix, manifest, host, output / portable_services.DIRECTORY, commands)
        if task_services.KEY in manifest:
            report[task_services.KEY] = task_services.qualify_consumer(
                prefix, manifest, host, output / task_services.DIRECTORY, commands)
        if sanitizers.KEY in manifest:
            report[sanitizers.KEY] = sanitizers.qualify_consumers(
                prefix, manifest, host, output / sanitizers.WITNESS_DIRECTORY, commands)
        verify_sdk(prefix, installed['seal_sha256'])
        report['installer_controls'] = _qualify_installer_controls(archive, expected_sha256, output,
                                                                  manifest, host, commands)
        report.update(status='PASS', artifacts=reports)
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        report['error'] = str(error)
        raise
    finally:
        (output / 'qualification.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main(argv=None, *, prog=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, prog=prog)
    commands = parser.add_subparsers(dest='action', required=True)
    pack = commands.add_parser('pack', help='assemble from a verified registration producer and its live compiler')
    for name in ('stage-compiler', 'llvm-source', 'bootstrap-report', 'registration-report', 'output-dir'):
        pack.add_argument('--' + name, required=True, type=Path)
    pack.add_argument('--sdk-sanitizers', action='store_true', help='include the pinned ASan and UBSan consumer variant')
    for action in ('install', 'qualify'):
        subparser = commands.add_parser(action)
        subparser.add_argument('--archive', type=Path, required=True)
        subparser.add_argument('--archive-sha256', required=True)
        if action == 'install':
            destination = subparser.add_mutually_exclusive_group(required=True)
            destination.add_argument('--prefix', type=Path, help='install into a fresh standalone prefix')
            destination.add_argument('--root', type=Path, help='install or update a managed SDK root')
        else:
            subparser.add_argument('--output-dir', type=Path, required=True)
    uninstall = commands.add_parser('uninstall', help='remove an exact admitted managed SDK')
    uninstall.add_argument('--root', type=Path, required=True)
    uninstall.add_argument('--seal-sha256', required=True)
    args = parser.parse_args(argv)
    try:
        if args.action == 'pack':
            result = package_registration(args.stage_compiler, args.llvm_source, args.bootstrap_report,
                                          json.loads(args.registration_report.read_bytes()), args.output_dir,
                                          sdk_sanitizers=args.sdk_sanitizers)
        elif args.action == 'install' and args.prefix is not None:
            result = install(args.archive, args.archive_sha256, args.prefix)
        elif args.action in ('install', 'uninstall'):
            from . import darwin_sdk_activation
            if args.action == 'install':
                result = darwin_sdk_activation.install(args.archive, args.archive_sha256, args.root)
            else:
                result = darwin_sdk_activation.uninstall(args.root, args.seal_sha256)
        else:
            result = qualify(args.archive, args.archive_sha256, args.output_dir)
    except (OSError, ValueError, KeyError, subprocess.SubprocessError, tarfile.TarError) as error:
        parser.exit(2, f'objc3c experimental Darwin SDK: {error}\n')
    print(json.dumps(result, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
