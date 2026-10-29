"""Execute authored capture/spawn diagnostics, IR and PCH witnesses on Linux."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

if __package__:
    from .sdk_example import digest, environment, sdk_verification, write_json
else:
    from sdk_example import digest, environment, sdk_verification, write_json

MODES = (('objective-c', 'c17'), ('objective-c++', 'c++20'),
         ('objective-c++', 'c++23'))
STRICT_ERRORS = {
    'CAPTURE_USE_AFTER_MOVE': ('may have been moved or discarded', 16),
    'CAPTURE_DOUBLE_CLEANUP': ('manual close of registered resource', 18),
    'CAPTURE_SECOND_MOVE': ('may have been moved or discarded', 20),
    'CAPTURE_BODY_CLOSE': ('manual close of registered resource', 10),
}


def qualify(sdk: Path, output: Path, *, fixtures: Path | None = None,
            compiler: Path | None = None, filecheck: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    if output.is_relative_to(sdk) or sdk.is_relative_to(output):
        raise ValueError('witness output must be disjoint from the sealed SDK')
    fixtures = (fixtures or sdk / 'share/objc3c/examples/linux-features').resolve(strict=True)
    compiler = (compiler or sdk / 'bin/clang').resolve(strict=True)
    filecheck = (filecheck or sdk / 'bin/FileCheck').resolve(strict=True)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    files = [fixtures / 'explicit_capture_lists' / name for name in
             ('sema.m', 'lowering.m', 'strict_resource.m', 'persistence.h', 'persistence.m')]
    files.append(fixtures / 'task_spawning/diagnostics.m')
    source_hashes = {str(p.relative_to(fixtures)): digest(p) for p in files}
    compiler_hash, filecheck_hash = digest(compiler), digest(filecheck)
    commands, cases, negatives = [], [], []
    env = environment(sdk)

    def run(label, command, *, cwd, expected=0, stdin=None):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, command)), cwd=cwd, env=env,
                stdin=stdin, stdout=stream, stderr=subprocess.STDOUT, timeout=180)
        commands.append({'name': label, 'argv': list(map(str, command)),
                         'exit_code': result.returncode, 'expected': expected,
                         'log_sha256': digest(log)})
        if result.returncode != expected:
            raise ValueError('compiler witness failed: ' + label + '; see ' + str(log))
        return log

    try:
        with sdk_verification(sdk):
            for language, standard in MODES:
                for ownership in ('arc', 'mrc'):
                    label = standard + '-' + ownership
                    with tempfile.TemporaryDirectory(prefix='objc3-compiler-witness-') as temporary:
                        work = Path(temporary)
                        for source in files:
                            destination = work / source.relative_to(fixtures)
                            destination.parent.mkdir(parents=True, exist_ok=True)
                            shutil.copyfile(source, destination)
                        captures = work / 'explicit_capture_lists'
                        flags = ['--no-default-config', '-x', language, '-std=' + standard,
                                 '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2',
                                 '-fblocks', '-fexceptions', '-fobjc-exceptions',
                                 '-fobjc-arc-exceptions', '-UNDEBUG',
                                 '-fobjc-arc' if ownership == 'arc' else '-fno-objc-arc',
                                 '-fmodules', '-fmodules-cache-path=' + str(work / 'modules'),
                                 '-isystem', str(sdk / 'include')]
                        if ownership == 'arc':
                            run(label + '-capture-sema', [compiler, *flags, '-fsyntax-only',
                                '-Xclang', '-verify', captures / 'sema.m'], cwd=work)
                            ir = work / 'capture.ll'
                            run(label + '-capture-ir', [compiler, *flags, '-O0', '-S',
                                '-emit-llvm', captures / 'lowering.m', '-o', ir], cwd=work)
                            with ir.open() as stream:
                                run(label + '-capture-filecheck', [filecheck, captures / 'lowering.m'],
                                    cwd=work, stdin=stream)
                        run(label + '-spawn-diagnostics', [compiler, *flags,
                            '-fobjc3-strict-concurrency', '-fsyntax-only', '-Xclang', '-verify',
                            work / 'task_spawning/diagnostics.m'], cwd=work)
                        for variant in ('', *STRICT_ERRORS):
                            log = run(label + '-strict-' + (variant or 'success'),
                                [compiler, *flags, '-fobjc3-profile=strict-system', '-fsyntax-only',
                                 *(['-D' + variant] if variant else []), captures / 'strict_resource.m'],
                                cwd=work, expected=1 if variant else 0)
                            if variant:
                                message, line = STRICT_ERRORS[variant]
                                text = log.read_text()
                                if message not in text or f'strict_resource.m:{line}:' not in text:
                                    raise ValueError('strict capture witness lost its owning diagnostic: ' + variant)
                                negatives.append({'mode': label, 'case': variant,
                                                  'log_sha256': digest(log)})
                        for optimization in ('O0', 'O2'):
                            for transport in ('source', 'pch'):
                                name = label + '-' + optimization + '-' + transport
                                options = [*flags, '-' + optimization]
                                extra = []
                                if transport == 'pch':
                                    header = captures / 'persistence.h'
                                    pch = work / (name + '.pch')
                                    run(name + '-pch', [compiler, *options, '-x', language + '-header',
                                        '-DCAPTURE_PERSISTENCE_PCH', header, '-o', pch], cwd=work)
                                    extra = ['-DCAPTURE_PERSISTENCE_PCH', '-include-pch', pch]
                                obj = work / (name + '.o')
                                run(name + '-compile', [compiler, *options, *extra, '-c',
                                    captures / 'persistence.m', '-o', obj], cwd=work)
                                driver = work / 'driver.m'
                                expected = 42 if language == 'objective-c++' else 17
                                driver.write_text('int capture_persistence_client(void);\n'
                                    'int main(void) { @autoreleasepool {\n'
                                    'return capture_persistence_client() != ' + str(expected) + '; } }\n')
                                driver_obj = work / (name + '-driver.o')
                                run(name + '-driver', [compiler, *options, '-c', driver,
                                    '-o', driver_obj], cwd=work)
                                executable = work / (name + '.bin')
                                run(name + '-link', [sdk / 'bin/clang++', '--no-default-config',
                                    '-fuse-ld=lld', obj, driver_obj, '-L' + str(sdk / 'lib'),
                                    '-lobjc', '-Wl,-rpath,' + str(sdk / 'lib'), '-o', executable], cwd=work)
                                run(name + '-execute', [executable], cwd=work)
                                cases.append({'language': language, 'standard': standard,
                                    'ownership': ownership, 'optimization': optimization,
                                    'transport': transport, 'result': expected})
        if (source_hashes != {str(p.relative_to(fixtures)): digest(p) for p in files} or
                digest(compiler) != compiler_hash or digest(filecheck) != filecheck_hash):
            raise ValueError('compiler witness inputs changed during execution')
        report = {'schema': 'objc3c.linux-compiler-witnesses.v1', 'status': 'PASS',
                  'sdk_seal': json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
                  'compiler_sha256': compiler_hash, 'filecheck_sha256': filecheck_hash,
                  'source_hashes': source_hashes, 'commands': commands,
                  'executions': len(cases), 'cases': cases, 'rejections': negatives,
                  'profile_conformance_claimed': False}
    except Exception as error:
        write_json(receipt, {'schema': 'objc3c.linux-compiler-witnesses.v1',
            'status': 'FAIL', 'error': str(error), 'commands': commands, 'cases': cases})
        raise
    write_json(receipt, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--fixtures', type=Path)
    parser.add_argument('--compiler', type=Path)
    parser.add_argument('--filecheck', type=Path)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.output_dir, fixtures=args.fixtures,
                     compiler=args.compiler, filecheck=args.filecheck)
    print(json.dumps({'status': result['status'], 'executions': result['executions'],
                      'rejections': len(result['rejections']), 'commands': len(result['commands'])}))


if __name__ == '__main__':
    main()
