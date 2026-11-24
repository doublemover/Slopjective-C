"""Package and verify optional ASan and UBSan consumer witnesses on Apple Silicon."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess

from .darwin_bootstrap import ROOT, verify_macho
from .package import _file_hash


KEY = 'consumer_sanitizers'
SCHEMA = 'objc3c.experimental-darwin-consumer-sanitizers.v1'
WITNESS = 'owned Objective-C 3 Darwin sanitizer witness: PASS'
WITNESS_DIRECTORY = 'Sanitizer witness'
FIXTURE = 'share/objc3c/examples/sanitizers/stateless.m'
SOURCE = ROOT / 'tests/clang_foundation/darwin_sanitizers/stateless.m'
UPSTREAM = {'version': '23.1.5', 'revision': '392dfff97d29b59d77e5cac6d4b6ef2585fe7117',
            'archive': 'LLVM-23.1.5-macOS-ARM64.tar.zst',
            'archive_sha256': '503de30ef90311e0b143623801b19af2250adac6d69faed65ad0051170202e3b'}
RUNTIMES = {
    'asan': ('libclang_rt.asan_osx_dynamic.dylib', '636326e28e884e3d671c1a5bcce5998e1700cf6c0451bcf4643fab3fdae979dd'),
    'ubsan': ('libclang_rt.ubsan_osx_dynamic.dylib', 'd9e5dbf99657c44ec1351d510de04b4d943d85dc0db72f794d9d7065a06f40ca'),
}
SYSTEM_DEPENDENCIES = ['/usr/lib/libSystem.B.dylib', '/usr/lib/libc++.1.dylib', '/usr/lib/libc++abi.dylib']
FLAGS = {
    'plain': (),
    'asan': ('-fsanitize=address', '-fno-sanitize-ignorelist', '-DOBJC3_ASAN_WITNESS'),
    'ubsan': ('-fsanitize=undefined', '-fno-sanitize-recover=undefined', '-DOBJC3_UBSAN_WITNESS'),
}
RUNTIME_OPTIONS = {
    'plain': {},
    'asan': {'ASAN_OPTIONS': 'detect_leaks=0:halt_on_error=1:abort_on_error=0:exitcode=86:symbolize=0'},
    'ubsan': {'UBSAN_OPTIONS': 'halt_on_error=1:exitcode=86:print_stacktrace=0:symbolize=0'},
}
MACHINE_IMPORTS = {'_objc3_darwin_class_v1', '_objc3_darwin_allocate_v1', '_objc3_darwin_deallocate_v1',
                   '_objc3_darwin_register_v1', '_objc3_darwin_lookup_v1'}
SANITIZER_IMPORTS = {'plain': set(), 'asan': {'___asan_init', '___asan_report_load4'},
                     'ubsan': {'___ubsan_handle_add_overflow_abort'}}


def descriptor(resource_dir: str) -> dict:
    return {'schema': SCHEMA, 'compiler_rt': UPSTREAM.copy(), 'fixture': FIXTURE,
            'scope': 'Objective-C 3 MRC consumer object',
            'runtimes': {mode: {'path': f'{resource_dir}/lib/darwin/{name}', 'sha256': digest}
                         for mode, (name, digest) in RUNTIMES.items()}}


def manifest_files(data: dict, resource_dir: str) -> set[str]:
    if KEY not in data:
        return set()
    claimed = data[KEY]
    if (resource_dir != 'lib/clang/23' or claimed != descriptor(resource_dir) or
            data.get('bootstrap_llvm') != {key: UPSTREAM[key] for key in ('version', 'revision')}):
        raise ValueError('Darwin sanitizer payload differs from the pinned upstream compiler-rt variant')
    files = data['files']
    for runtime in claimed['runtimes'].values():
        if files.get(runtime['path'], {}).get('sha256') != runtime['sha256']:
            raise ValueError('Darwin sanitizer runtime bytes differ from the pinned upstream asset')
    return {FIXTURE, *(runtime['path'] for runtime in claimed['runtimes'].values())}


def stage_payload(prefix: Path, resource: Path, resource_dir: str) -> dict:
    staged = descriptor(resource_dir)
    for item in staged['runtimes'].values():
        staged_path = Path(item['path'])
        source = resource / 'lib/darwin' / staged_path.name
        if source.is_symlink() or not source.is_file() or _file_hash(source) != item['sha256']:
            raise ValueError(f'pinned Darwin compiler-rt dylib is absent or changed: {source}')
        verify_macho(source, 'dynamic_library')
        destination = prefix / staged_path
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
    destination = prefix / FIXTURE
    destination.parent.mkdir(parents=True)
    shutil.copy2(SOURCE, destination)
    return staged


def validate_runtimes(prefix: Path, data: dict, host: dict, commands: list | None) -> None:
    from .darwin_sdk import _load_paths, _rpaths

    otool = host['tools']['otool']['path']
    for item in data[KEY]['runtimes'].values():
        dylib = prefix / item['path']
        if verify_macho(dylib, 'dynamic_library')['sha256'] != item['sha256']:
            raise ValueError(f'Darwin sanitizer dylib changed: {dylib}')
        _load_paths(otool, dylib, ['@rpath/' + dylib.name, *SYSTEM_DEPENDENCIES], commands)
        _rpaths(otool, dylib, [], commands)


def validate_imports(mode: str, output: str) -> list[str]:
    names = {line.strip() for line in output.splitlines() if line.strip()}
    sanitizer_names = {name for name in names if name.startswith(('___asan_', '___ubsan_'))}
    if (not (MACHINE_IMPORTS | SANITIZER_IMPORTS[mode]) <= names or
            (mode in ('plain', 'ubsan') and any(name.startswith('___asan_') for name in sanitizer_names)) or
            (mode in ('plain', 'asan') and any(name.startswith('___ubsan_') for name in sanitizer_names))):
        raise ValueError(f'{mode} Mach-O imports do not prove the expected sanitizer and owned dispatch instrumentation')
    return sorted(names)


def validate_execution(stdout: str, stderr: str, code: int, runtime: Path | None) -> str | None:
    lines = stdout.splitlines()
    if code != 0 or stderr or len(lines) != (2 if runtime else 1) or lines[0] != WITNESS:
        raise ValueError('Darwin sanitizer witness did not execute cleanly')
    if runtime is None:
        return None
    loaded = Path(lines[1])
    if not loaded.is_absolute() or loaded.resolve() != runtime.resolve(strict=True):
        raise ValueError('the consumer loaded a sanitizer dylib outside the admitted SDK')
    return str(loaded.resolve(strict=True))


def qualify_consumers(prefix: Path, data: dict, host: dict, output: Path, commands: list) -> dict:
    from .darwin_sdk import LIBRARY, RUNTIME_FLAG, SYSTEM, TARGET, _deployment, _environment, _load_paths, _rpaths, _run

    output.mkdir()
    tool = Path(_run('/usr/bin/xcrun', '--sdk', 'macosx', '--find', 'nm', commands=commands)).resolve(strict=True)
    if not tool.is_relative_to(host['developer_dir']) or not tool.is_file() or not os.access(tool, os.X_OK):
        raise ValueError('Darwin sanitizer inspection requires nm from the admitted Xcode')
    if 'arm64' not in _run(host['tools']['lipo']['path'], '-archs', tool, commands=commands).split():
        raise ValueError('Darwin sanitizer inspection requires a native arm64 nm slice')
    resource = prefix / data['resource_dir']
    defaults = [prefix / 'bin/clang', '--no-default-config', '--target=' + TARGET, '-isysroot', host['sdk_path'],
                '-resource-dir', resource]
    compile_flags = [*defaults, '-fobjc-version=3', RUNTIME_FLAG, '-fno-objc-arc', '-fno-objc-exceptions',
                     '-Wall', '-Wextra', '-Werror', '-O2', '-gdwarf-4']
    link = [*defaults, '--ld-path=' + host['tools']['ld']['path']]
    otool = host['tools']['otool']['path']
    routes = {}
    for mode, flags in FLAGS.items():
        obj, binary = output / (mode + '.o'), output / mode
        _run(*compile_flags, *flags, '-c', prefix / FIXTURE, '-o', obj, cwd=output, commands=commands)
        inspected = verify_macho(obj, 'object')
        inspected['imports'] = validate_imports(mode, _run(tool, '-u', '-j', obj, commands=commands))
        runtime = prefix / data[KEY]['runtimes'][mode]['path'] if mode != 'plain' else None
        rpaths, dependencies, sanitizer_arguments = [], [SYSTEM], []
        if runtime:
            reported = Path(_run(*defaults, flags[0], '-print-file-name=' + runtime.name, commands=commands))
            if not reported.is_absolute() or reported.resolve(strict=True) != runtime.resolve(strict=True):
                raise ValueError('the admitted compiler resolved a different sanitizer runtime')
            relative = os.path.relpath(runtime.parent, output)
            rpaths.append('@loader_path/' + relative)
            dependencies.append('@rpath/' + runtime.name)
            sanitizer_arguments = [runtime, '-Wl,-rpath,' + rpaths[0]]
        _run(*link, obj, prefix / 'lib' / (LIBRARY + '.a'), *sanitizer_arguments, '-o', binary,
             cwd=output, commands=commands)
        executable = verify_macho(binary, 'executable')
        executable.update(load_paths=_load_paths(otool, binary, dependencies, commands),
                          rpaths=_rpaths(otool, binary, rpaths, commands),
                          deployment=_deployment(otool, binary, commands))
        options = RUNTIME_OPTIONS[mode]
        result = subprocess.run([str(binary)], cwd=output, env={**_environment(), **options}, capture_output=True,
                                text=True, timeout=30, errors='replace')
        commands.append({'argv': [str(binary)], 'exit_status': result.returncode, 'stdout': result.stdout,
                         'stderr': result.stderr, 'expected_diagnostics': [], 'sanitizer_options': options})
        loaded = validate_execution(result.stdout, result.stderr, result.returncode, runtime)
        routes[mode] = {'object': inspected, 'executable': executable, 'loaded_runtime': loaded,
                        'runtime_sha256': _file_hash(Path(loaded)) if loaded else None}
    return {'schema': SCHEMA, 'status': 'PASS', 'support_truth': False, 'scope': data[KEY]['scope'],
            'nm': {'path': str(tool), 'sha256': _file_hash(tool)}, 'routes': routes}
