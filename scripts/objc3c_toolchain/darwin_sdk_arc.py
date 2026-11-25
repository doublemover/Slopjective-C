"""Compile and run the reviewed ARC consumer from a relocated Darwin SDK."""
from __future__ import annotations

from pathlib import Path
import os
import re

from .darwin_bootstrap import verify_macho
from .package import _file_hash
from .sdk_example import dependencies


SCHEMA = 'objc3c.experimental-darwin-sdk-arc.v1'
SOURCES = ('arc.m', 'arc_threads.c')
DIRECTORY = 'Fresh ARC consumer'
WITNESS = 'owned Objective-C 3 Darwin ARC: PASS'
REQUIRED_IMPORTS = {'objc3_darwin_allocate_v1', 'objc3_darwin_arc_release_v1',
                    'objc3_darwin_arc_load_weak_retained_v1', 'objc3_darwin_arc_pool_push_v1'}


def loaded_runtime(transcript: str, runtime: Path) -> str:
    paths = []
    for line in transcript.splitlines():
        entry = re.fullmatch(r'dyld\[\d+\]:\s+(?:<[^>]+>\s+)?(/.+)', line)
        if entry and Path(entry[1]).name == runtime.name:
            paths.append(Path(entry[1]).resolve(strict=True))
    if paths != [runtime.resolve(strict=True)]:
        raise ValueError('ARC executable did not load the exact installed Objective-C 3 runtime')
    return str(paths[0])


def qualify_consumer(prefix: Path, manifest: dict, host: dict, output: Path, commands: list) -> dict:
    from .darwin_sdk import DEMO, LIBRARY, RUNTIME_FLAG, SYSTEM, TARGET
    from .darwin_sdk import _checked_file, _deployment, _load_paths, _rpaths, _run

    output.mkdir()
    resource = manifest['resource_dir']
    builtin = f'{resource}/lib/darwin/libclang_rt.osx.a'
    relative = [*(f'{DEMO}/{name}' for name in SOURCES), 'bin/clang', 'bin/llvm-dwarfdump',
                f'{resource}/include/stddef.h', builtin, f'lib/{LIBRARY}.dylib']
    inputs = {name: manifest['files'][name]['sha256'] for name in relative}
    installed = {name: _checked_file(prefix / name, signature, prefix) for name, signature in inputs.items()}
    clang = installed['bin/clang']
    runtime = installed[f'lib/{LIBRARY}.dylib']
    sdk = Path(host['sdk_path']).resolve(strict=True)
    defaults = [clang, '--no-default-config', '--target=' + TARGET, '-isysroot', sdk,
                '-resource-dir', prefix / resource]

    def native(*args, **kwargs):
        return _run(*args, cwd=output, commands=commands, **kwargs)

    builtins = Path(native(*defaults, '-print-file-name=libclang_rt.osx.a'))
    if not builtins.is_absolute() or builtins.resolve(strict=True) != installed[builtin]:
        raise ValueError('installed Clang selected different Darwin compiler builtins')
    flags = ['-std=c17', '-Wall', '-Wextra', '-Werror', '-O2', '-g']
    arc, threads, binary = (output / name for name in ('arc.o', 'arc-threads.o', 'arc'))
    native(*defaults, *flags, '-fobjc-version=3', RUNTIME_FLAG, '-fobjc-arc',
           '-c', installed[f'{DEMO}/arc.m'], '-o', arc)
    depfile = output / 'arc-threads.d'
    native(*defaults, *flags, '-MD', '-MF', depfile, '-c', installed[f'{DEMO}/arc_threads.c'], '-o', threads)
    includes = set(dependencies(depfile, output))
    stddef = installed[f'{resource}/include/stddef.h']
    pthread = (sdk / 'usr/include/pthread.h').resolve(strict=True)
    if not {stddef, pthread} <= includes:
        raise ValueError('ARC thread witness did not read the packaged Clang and pinned Apple SDK headers')
    rpath = '@loader_path/' + os.path.relpath(runtime.parent, output)
    native(*defaults, '--ld-path=' + host['tools']['ld']['path'], arc, threads, runtime,
           '-Wl,-rpath,' + rpath, '-o', binary)

    artifacts = {path.name: verify_macho(path, 'object') for path in (arc, threads)}
    native(installed['bin/llvm-dwarfdump'], '--verify', arc)
    imports = sorted({name.removeprefix('_') for name in native('/usr/bin/nm', '-u', '-j', binary).splitlines()})
    if (not REQUIRED_IMPORTS <= set(imports) or
            any(name.startswith(('objc_', '__objc_', '_Block_', '_NSConcrete')) for name in imports)):
        raise ValueError('installed ARC executable has inconsistent owned runtime imports')
    otool = host['tools']['otool']['path']
    executable = verify_macho(binary, 'executable')
    executable.update(imports=imports,
        load_paths=_load_paths(otool, binary, [SYSTEM, '@rpath/' + runtime.name], commands),
        rpaths=_rpaths(otool, binary, [rpath], commands),
        deployment=_deployment(otool, binary, commands, sdk_version=host['sdk_version']))
    artifacts[binary.name] = executable
    if native(binary, trace_libraries=True) != WITNESS:
        raise ValueError('installed Darwin ARC lifetime witness failed')
    loaded = loaded_runtime(commands[-1]['stderr'], runtime)
    return {'schema': SCHEMA, 'status': 'PASS', 'support_truth': False, 'optimization': 'O2',
            'seal_sha256': manifest['seal_sha256'], 'inputs': inputs, 'artifacts': artifacts,
            'thread_dependencies': {'sha256': _file_hash(depfile), 'stddef': str(stddef), 'pthread': str(pthread)},
            'loaded_runtime': {'path': loaded, 'sha256': inputs[f'lib/{LIBRARY}.dylib']}}
