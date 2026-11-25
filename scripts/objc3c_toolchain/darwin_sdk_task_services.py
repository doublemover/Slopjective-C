"""Ship and exercise the Darwin value-only task service archive."""
from __future__ import annotations

from pathlib import Path
import re
import shutil
import tempfile

from .darwin_bootstrap import ROOT, verify_macho
from .package import _file_hash
from .sdk_example import dependencies

KEY = 'task_services'
SCHEMA = 'objc3c.experimental-darwin-task-services.v1'
ABI = 4
DIRECTORY = 'Installed task services'
LIBRARY = 'lib/libobjc3darwin_tasks.a'
HEADER = 'include/objc3_runtime_services.h'
EXAMPLE = 'share/objc3c/examples/task-services/example.c'
WITNESS = 'owned Objective-C 3 Darwin task services: PASS'
SOURCE = ROOT / 'tests/clang_foundation/darwin_task_services/example.c'
NATIVE = ROOT / 'native/objc3c/src'
INPUTS = {name + '.o': NATIVE / 'runtime/concurrency' / (name + '.cpp')
          for name in ('service_executor', 'service_task', 'service_async_join',
                       'service_group', 'service_actor', 'service_public')}
INPUTS['error_service.o'] = NATIVE / 'runtime/services/error_service.cpp'


def manifest_files(data: dict) -> set[str]:
    payload = data[KEY]
    files = {LIBRARY, HEADER, EXAMPLE}
    if (not isinstance(payload, dict) or set(payload) != {'schema', 'abi', 'inputs', 'objects', 'files'} or
            payload['schema'] != SCHEMA or payload['abi'] != ABI or
            any(not isinstance(payload[table], dict) for table in ('inputs', 'objects', 'files')) or
            payload['files'].keys() != files or payload['objects'].keys() != INPUTS.keys() or
            any(not isinstance(digest, str) or not re.fullmatch(r'[a-f0-9]{64}', digest)
                for table in ('inputs', 'objects', 'files') for digest in payload[table].values()) or
            not {path.relative_to(ROOT).as_posix() for path in (*INPUTS.values(), SOURCE,
                       NATIVE / 'runtime/public/objc3_runtime_services.h')} <= payload['inputs'].keys() or
            any(data['files'].get(path, {}).get('sha256') != digest for path, digest in payload['files'].items())):
        raise ValueError('Darwin task service inventory differs from the sealed payload')
    return files


def validate_archive(prefix, data, commands):
    from .darwin_sdk import _archive_objects, _run

    _archive_objects(prefix / LIBRARY, INPUTS)
    with tempfile.TemporaryDirectory(prefix='objc3-task-members-') as scratch:
        _run(prefix / 'bin/llvm-ar', 'x', prefix / LIBRARY, *INPUTS, cwd=scratch, commands=commands)
        actual = {name: verify_macho(Path(scratch) / name, 'object')['sha256'] for name in INPUTS}
    if actual != data[KEY]['objects']:
        raise ValueError('Darwin task service archive members differ from the qualified products')


def toolchain(host, commands):
    from .darwin_sdk import TARGET, _run

    apple = Path(_run('/usr/bin/xcrun', '--sdk', 'macosx', '--find', 'clang++', commands=commands)).resolve(strict=True)
    if not apple.is_relative_to(Path(host['developer_dir'])):
        raise ValueError('Darwin task services require the admitted Xcode C++ compiler')
    return [apple, '--no-default-config', '--target=' + TARGET, '-isysroot', host['sdk_path']]


def stage_payload(prefix, host, commands):
    from .darwin_sdk import _run

    compiler = toolchain(host, commands)
    public_header = NATIVE / 'runtime/public/objc3_runtime_services.h'
    tracked = {SOURCE, public_header}
    with tempfile.TemporaryDirectory(prefix='objc3-task-objects-') as scratch:
        build = Path(scratch)
        for name, path in INPUTS.items():
            depfile = build / (name + '.d')
            _run(*compiler, '-std=c++20', '-O2', '-DOBJC3_VALUE_ONLY_SERVICE_RUNTIME',
                 '-I', NATIVE, '-MMD', '-MF', depfile,
                 '-c', path, '-o', build / name, commands=commands)
            tracked.update(dependency for dependency in dependencies(depfile, build) if dependency.is_relative_to(NATIVE))
        objects = {name: verify_macho(build / name, 'object')['sha256'] for name in INPUTS}
        _run(prefix / 'bin/llvm-ar', '--format=darwin', 'rcs', prefix / LIBRARY,
             *(build / name for name in INPUTS), commands=commands)
    for path, source in ((HEADER, public_header), (EXAMPLE, SOURCE)):
        target = prefix / path
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, target)
    return {'schema': SCHEMA, 'abi': ABI, 'objects': objects,
            'inputs': {path.relative_to(ROOT).as_posix(): _file_hash(path) for path in sorted(tracked)},
            'files': {name: _file_hash(prefix / name) for name in (LIBRARY, HEADER, EXAMPLE)}}


def qualify_consumer(prefix, data, host, output, commands):
    from .darwin_sdk import SYSTEM, _deployment, _load_paths, _run

    output.mkdir()
    source = output / 'example.c'
    shutil.copyfile(prefix / EXAMPLE, source)
    obj, executable = output / 'example.o', output / 'example'
    _run(prefix / 'bin/objc3c', '-std=c17', '-I', prefix / 'include', '-c', source, '-o', obj, commands=commands)
    _run(*toolchain(host, commands), '--ld-path=' + host['tools']['ld']['path'], obj, prefix / LIBRARY,
         '-o', executable, commands=commands)
    otool = host['tools']['otool']['path']
    paths = _load_paths(otool, executable, [SYSTEM, '/usr/lib/libc++.1.dylib'], commands)
    if _run(executable, commands=commands) != WITNESS:
        raise ValueError('installed Darwin task services did not complete the C witness')
    return {'schema': SCHEMA, 'abi': ABI, 'status': 'PASS', 'inputs': data[KEY]['files'],
            'object': verify_macho(obj, 'object'), 'executable': verify_macho(executable, 'executable'),
            'load_paths': paths, 'deployment': _deployment(otool, executable, commands, sdk_version=host['sdk_version'])}
