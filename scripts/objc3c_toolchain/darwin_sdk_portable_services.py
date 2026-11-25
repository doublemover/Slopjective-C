"""Stage and replay the experimental Darwin C++ collections and text payload."""
from __future__ import annotations

from pathlib import Path
import re
import shutil
import tempfile

from .darwin_bootstrap import ROOT, verify_macho
from .darwin_registration import NATIVE, PORTABLE_HEADERS, PORTABLE_SERVICES, xcode_cxx_headers
from .package import _file_hash
from .sdk_example import dependencies

KEY = 'portable_services'
SCHEMA = 'objc3c.experimental-darwin-portable-services.v1'
ABI = 'objc3-portable-collections-text-v2'
LIBRARY = 'lib/libobjc3darwin_portable.a'
HEADERS = {
    'include/objc3c/runtime/' + name: 'native/objc3c/src/runtime/public/' + name
    for name in ('objc3_collections.h', 'objc3_stdlib_status.h', 'objc3_text.h')
}
EXAMPLE = 'share/objc3c/examples/collections-text/example.cpp'
EXAMPLE_SOURCE = 'tests/clang_foundation/darwin_portable_services/example.cpp'
DIRECTORY = 'Portable services'
WITNESS = 'owned Objective-C 3 Darwin collections and text: PASS'
ARCHIVE_LIMIT = 4 * 1024**2
MEMBERS = {service + '.o': label for label, (service, _) in PORTABLE_SERVICES.items()}
SYSTEM_CXX = '/usr/lib/libc++.1.dylib'


def _witness_sources() -> set[str]:
    paths = {NATIVE / name for name in PORTABLE_HEADERS}
    for service, fixture in PORTABLE_SERVICES.values():
        paths.add(NATIVE / 'src/runtime/stdlib' / (service + '.cpp'))
        paths.add(NATIVE / 'tests' / (fixture + '.cpp'))
    return {path.relative_to(ROOT).as_posix() for path in paths}


def manifest_files(data: dict) -> set[str]:
    """Admit exactly the public headers, static archive and installed example."""
    if KEY not in data:
        return set()
    payload = data[KEY]
    if (not isinstance(payload, dict) or set(payload) != {'schema', 'abi', 'build_inputs', 'objects', 'files'} or
            payload.get('schema') != SCHEMA or payload.get('abi') != ABI):
        raise ValueError('unexpected Darwin collections/text payload descriptor')
    libcxx = data.get('compiler_host_libcxx', {})
    if (not isinstance(libcxx, dict) or
            set(libcxx) != {'header_config_sha256', 'library_stub_sha256', 'reported_version'} or
            any(not isinstance(libcxx[key], str) or not re.fullmatch('[a-f0-9]{64}', libcxx[key])
                for key in ('header_config_sha256', 'library_stub_sha256')) or
            not isinstance(libcxx['reported_version'], str) or
            not re.fullmatch(r'Xcode libc\+\+ [0-9]+', libcxx['reported_version'])):
        raise ValueError('Darwin collections/text payload lacks the admitted Xcode libc++ identity')
    inputs, objects, files = (payload[key] for key in ('build_inputs', 'objects', 'files'))
    for inventory, expected in ((inputs, _witness_sources() | {EXAMPLE_SOURCE}),
                                (objects, set(MEMBERS)), (files, {LIBRARY, *HEADERS, EXAMPLE})):
        if (not isinstance(inventory, dict) or inventory.keys() != expected or
                any(not isinstance(value, str) or not re.fullmatch('[a-f0-9]{64}', value)
                    for value in inventory.values())):
            raise ValueError('Darwin collections/text payload inventory differs from its reviewed scope')
    if any(files[path] != inputs[source] for path, source in {**HEADERS, EXAMPLE: EXAMPLE_SOURCE}.items()):
        raise ValueError('Darwin collections/text public source differs from the qualified inputs')
    for path, signature in files.items():
        row = data['files'].get(path, {})
        if row != {'kind': 'file', 'mode': 0o644, 'sha256': signature}:
            raise ValueError('Darwin collections/text payload differs from the sealed file inventory: ' + path)
    return set(files)


def _archive_members(prefix: Path, expected: dict[str, str], commands: list | None) -> None:
    from .darwin_sdk import _archive_objects, _run

    archive = prefix / LIBRARY
    if not 0 < archive.stat().st_size <= ARCHIVE_LIMIT:
        raise ValueError('Darwin collections/text archive exceeds the reviewed size bound')
    _archive_objects(archive, MEMBERS)
    with tempfile.TemporaryDirectory(prefix='objc3-portable-members-') as directory:
        _run(prefix / 'bin/llvm-ar', 'x', archive, *MEMBERS, cwd=directory, commands=commands)
        actual = {name: verify_macho(Path(directory) / name, 'object')['sha256'] for name in MEMBERS}
    if actual != expected:
        raise ValueError('Darwin collections/text archive members differ from the qualified native objects')


def stage_payload(prefix: Path, registration: dict, commands: list | None = None) -> dict:
    """Archive the same two Mach-O objects exercised by the registration owner."""
    from .darwin_sdk import _checked_file, _run

    prefix = prefix.resolve(strict=True)
    witness = registration['portable_services']
    expected_sources = {name: _file_hash(ROOT / name) for name in sorted(_witness_sources())}
    if (registration.get('status') != 'registration-verified' or registration.get('support_truth') is not False or
            witness['status'] != 'PASS' or witness['inputs'] != expected_sources or
            witness['suites'].keys() != PORTABLE_SERVICES.keys()):
        raise ValueError('Darwin collections/text payload requires the exact portable registration witnesses')
    build = (Path(registration['installation_root']).resolve(strict=True).parent / 'build').resolve(strict=True)
    archive = prefix / LIBRARY
    if any((prefix / name).exists() for name in (LIBRARY, EXAMPLE, *HEADERS)):
        raise ValueError('Darwin collections/text payload requires a fresh staging layout')
    objects = {}
    for name, suite in MEMBERS.items():
        reported = witness['suites'][suite]['objects'][name]
        object_file = _checked_file(build / name, reported['sha256'], build)
        if verify_macho(object_file, 'object')['sha256'] != reported['sha256']:
            raise ValueError('Darwin collections/text object changed during qualification')
        objects[name] = reported['sha256']
    archive.parent.mkdir(parents=True, exist_ok=True)
    _run(prefix / 'bin/llvm-ar', '--format=darwin', 'rcs', archive,
         *(build / name for name in MEMBERS), commands=commands)
    _archive_members(prefix, objects, commands)

    expected_sources[EXAMPLE_SOURCE] = _file_hash(ROOT / EXAMPLE_SOURCE)
    files = {LIBRARY: _file_hash(archive)}
    for target, source in {**HEADERS, EXAMPLE: EXAMPLE_SOURCE}.items():
        path = prefix / target
        path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(_checked_file(ROOT / source, expected_sources[source], ROOT), path)
        _checked_file(path, expected_sources[source], prefix)
        files[target] = expected_sources[source]
    return {'schema': SCHEMA, 'abi': ABI, 'build_inputs': expected_sources, 'objects': objects, 'files': files}


def validate_payload(prefix: Path, data: dict, commands: list | None = None) -> None:
    from .darwin_sdk import _checked_file

    for path in manifest_files(data):
        if (prefix / path).is_symlink():
            raise ValueError('Darwin collections/text file must remain a regular payload: ' + path)
        _checked_file(prefix / path, data[KEY]['files'][path], prefix)
    if KEY in data:
        _archive_members(prefix, data[KEY]['objects'], commands)


def _installed_cxx(data: dict, host: dict, commands: list) -> list[str]:
    from .darwin_sdk import TARGET, _run

    developer = Path(host['developer_dir']).resolve(strict=True)
    sdk = Path(host['sdk_path']).resolve(strict=True)
    apple = Path(_run('/usr/bin/xcrun', '--sdk', 'macosx', '--find', 'clang++', commands=commands)).resolve(strict=True)
    if not apple.is_file() or not apple.is_relative_to(developer):
        raise ValueError('the Apple C++ compiler escaped the admitted Xcode')
    _run(apple, '--target=' + TARGET, '-isysroot', sdk, '-stdlib=libc++', '-E', '-v', '-x', 'c++',
         '/dev/null', '-o', '/dev/null', commands=commands)
    headers = xcode_cxx_headers(commands[-1]['stderr'], developer)
    expected = data['compiler_host_libcxx']
    if (_file_hash(headers / '__config') != expected['header_config_sha256'] or
            _file_hash(sdk / 'usr/lib/libc++.tbd') != expected['library_stub_sha256']):
        raise ValueError('relocated collections/text consumer has a different Xcode libc++ identity')
    return ['-stdlib=libc++', '-nostdinc++', '-isystem', str(headers)]


def _check_consumer_inputs(source: Path, depfile: Path, prefix: Path, sdk: Path | None) -> None:
    required = {source, *(prefix / header for header in HEADERS)}
    if sdk is not None:
        required.add((sdk / 'SDKSettings.json').resolve(strict=True))
    if set(dependencies(depfile, source.parent)) != required:
        raise ValueError('Darwin collections/text consumer used inputs outside its installed example, headers or SDK settings')


def qualify_consumer(prefix: Path, data: dict, host: dict, output: Path, commands: list) -> dict:
    """Compile and run the C++ example with only its installed SDK inputs."""
    from .darwin_sdk import SYSTEM, TARGET, _deployment, _load_paths, _rpaths, _run

    prefix = prefix.resolve(strict=True)
    validate_payload(prefix, data, commands)
    output = output.resolve()
    output.mkdir()
    source, depfile, obj, binary = (output / name for name in ('example.cpp', 'example.d', 'example.o', 'example'))
    shutil.copyfile(prefix / EXAMPLE, source)
    defaults = [prefix / 'bin/clang', '--no-default-config', '--driver-mode=g++', '--target=' + TARGET,
                '-isysroot', host['sdk_path'], '-resource-dir', prefix / data['resource_dir'],
                *_installed_cxx(data, host, commands)]
    _run(*defaults, '-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror', '-I', prefix / 'include',
         '-MMD', '-MF', depfile, '-c', source, '-o', obj, cwd=output, commands=commands)
    sdk = Path(host['sdk_path'])
    _check_consumer_inputs(source, depfile, prefix, sdk)
    link = [*defaults, '--ld-path=' + host['tools']['ld']['path'], '-Wl,-dead_strip', obj]
    _run(*link, '-o', output / 'without-archive', cwd=output, commands=commands,
         rejects=('objc3_runtime_stdlib_map_i32_i32_v2_create', 'objc3_runtime_stdlib_text_v2_create_utf8_checked'))
    _run(*link, prefix / LIBRARY, '-o', binary, cwd=output, commands=commands)
    otool = host['tools']['otool']['path']
    executable = {**verify_macho(binary, 'executable'),
                  'load_paths': _load_paths(otool, binary, [SYSTEM, SYSTEM_CXX], commands),
                  'rpaths': _rpaths(otool, binary, [], commands),
                  'deployment': _deployment(otool, binary, commands, sdk_version=host['sdk_version'])}
    if _run(binary, cwd=output, commands=commands) != WITNESS or commands[-1]['stderr']:
        raise ValueError('the installed Darwin collections/text example did not complete cleanly')
    if _file_hash(source) != data[KEY]['files'][EXAMPLE]:
        raise ValueError('the portable collections/text example changed during compilation or execution')
    validate_payload(prefix, data, commands)
    return {'schema': SCHEMA, 'status': 'PASS', 'support_truth': False,
            'abi': ABI, 'inputs': data[KEY]['files'], 'object': verify_macho(obj, 'object'),
            'sdk_settings_sha256': _file_hash(sdk / 'SDKSettings.json'),
            'executable': executable, 'stdout': WITNESS}
