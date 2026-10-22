"""Execute installed recursive C++ carrier lifetime and transfer regressions."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import resource
import shutil
import signal
import subprocess
import sys
import tempfile
import time

if __package__:
    from . import binary_packages as packages
    from .sdk_example import environment
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from sdk_example import environment


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    report = {'schema': 'objc3c.installed-recursive-carriers.v1', 'status': 'running',
              'commands': [], 'configurations': [], 'executions': 0, 'rejections': 0,
              'profile_conformance_claimed': False,
              'scope': ['direct nested C++ carrier construction and destruction',
                        'recursive selected-arm copy/move and const move',
                        'failed construction and full-expression rollback',
                        'native exception unwind and single source evaluation',
                        'separately compiled reference boundaries and PCH replay',
                        'mixed ARC/C++ selected-leaf lifetime partitions',
                        'strong, weak, unsafe and Block ownership with C++ alternatives',
                        'native destructor actions and deferred managed releases']}

    def persist():
        receipt.write_text(json.dumps(report, indent=2) + '\n')

    persist()
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    compiler, frontend = sdk / 'bin/clang', sdk / 'bin/objc3c'
    fixtures = sdk / 'share/objc3c/examples/recursive-carriers'
    inputs = [compiler.resolve(), frontend.resolve(),
              (sdk / 'lib/libobjc.so').resolve(), sdk / 'objc3-sdk.json',
              sdk / 'share/objc3c/verify-sdk.py', Path(__file__).resolve()]
    for name in ('main.mm', 'boundary.h', 'provider.mm', 'consumer.mm',
                 'pch.h', 'pch.mm', 'rejections.mm', 'mixed-boundary.h',
                 'mixed-provider.mm', 'mixed-main.mm', 'mixed-consumer.mm',
                 'mixed-pch.h', 'mixed-pch.mm'):
        inputs.append(fixtures / name)

    def run(name, command, *, cwd=None, expected_exit_code=0):
        started = time.monotonic()
        log = output / (name + '.log')
        timed_out = False
        with log.open('w') as stream:
            process = subprocess.Popen(list(map(str, command)), env=env,
                                       cwd=cwd or output, stdout=stream,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            try:
                code = process.wait(timeout=120)
            except subprocess.TimeoutExpired:
                timed_out = True
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                code = process.wait()
        report['commands'].append({
            'name': name, 'command': list(map(str, command)), 'exit_code': code,
            'expected_exit_code': expected_exit_code, 'timed_out': timed_out,
            'elapsed_seconds': round(time.monotonic() - started, 6),
            'log_sha256': packages.digest(log),
        })
        persist()
        if timed_out or code != expected_exit_code:
            raise ValueError(f'installed recursive carriers failed: {name}; see {log}')
        return log.read_text()

    def execute(name, executable, client):
        run(name, [executable], cwd=client)
        report['executions'] += 1

    try:
        env = environment(sdk)
        hashes = {str(path): packages.digest(path) for path in inputs}
        report.update(compiler_sha256=hashes[str(compiler.resolve())],
                      frontend_sha256=hashes[str(frontend.resolve())],
                      sdk_seal_sha256=json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
                      input_sha256=hashes)
        run('verify-sdk', [sys.executable, '-B', sdk / 'share/objc3c/verify-sdk.py', sdk])
        with tempfile.TemporaryDirectory(prefix='objc3-installed-recursive-carriers-') as temporary:
            client = Path(temporary)
            shutil.copytree(fixtures, client / 'source')
            source = client / 'source'
            link = ['-fuse-ld=lld', '-no-pie', '-L' + str(sdk / 'lib'), '-lobjc',
                    '-Wl,-rpath,' + str(sdk / 'lib')]
            for standard in ('c++20', 'c++23'):
                for arc in (False, True):
                    ownership = 'arc' if arc else 'mrc'
                    flags = ['-std=' + standard, '-fobjc-version=3',
                             '-fobjc-runtime=gnustep-2.2', '-fexceptions',
                             '-fcxx-exceptions', '-fobjc-exceptions',
                             '-fobjc-arc-exceptions', '-fblocks', '-fno-crash-diagnostics', '-g',
                             '-fobjc-arc' if arc else '-fno-objc-arc']
                    common = [compiler, '--driver-mode=g++', '--no-default-config', *flags]
                    for optimization in ('O0', 'O2'):
                        name = f'{standard}-{ownership}-{optimization}'
                        compile_flags = [*common, '-' + optimization]
                        executable = client / (name + '-runtime')
                        run(name + '-runtime-compile', [*compile_flags, '-x', 'objective-c++',
                            source / 'main.mm', *link, '-o', executable], cwd=client)
                        execute(name + '-runtime-execute', executable, client)
                        provider, consumer = client / (name + '-provider.o'), client / (name + '-consumer.o')
                        run(name + '-provider', [*compile_flags, '-x', 'objective-c++', '-c',
                            source / 'provider.mm', '-o', provider], cwd=client)
                        run(name + '-consumer', [*compile_flags, '-x', 'objective-c++', '-c',
                            source / 'consumer.mm', '-o', consumer], cwd=client)
                        executable = client / (name + '-separate')
                        run(name + '-link', [*common, provider, consumer, *link, '-o', executable], cwd=client)
                        execute(name + '-separate-execute', executable, client)
                        pch, consumer = client / (name + '.pch'), client / (name + '-pch.o')
                        run(name + '-pch-header', [*compile_flags, '-x', 'objective-c++-header',
                            source / 'pch.h', '-o', pch], cwd=client)
                        run(name + '-pch-consumer', [*compile_flags, '-x', 'objective-c++', '-c',
                            '-include-pch', pch, source / 'pch.mm', '-o', consumer], cwd=client)
                        executable = client / (name + '-pch')
                        run(name + '-pch-link', [*common, provider, consumer, *link, '-o', executable], cwd=client)
                        execute(name + '-pch-execute', executable, client)
                        report['configurations'].append({'standard': standard, 'ownership': ownership,
                                                          'optimization': optimization})
                        if arc:
                            mixed_provider = client / (name + '-mixed-provider.o')
                            run(name + '-mixed-provider', [*compile_flags, '-x', 'objective-c++', '-c',
                                source / 'mixed-provider.mm', '-o', mixed_provider], cwd=client)
                            for fixture in ('main', 'consumer'):
                                mixed_object = client / (name + '-mixed-' + fixture + '.o')
                                run(name + '-mixed-' + fixture + '-compile', [
                                    *compile_flags, '-x', 'objective-c++', '-c',
                                    source / ('mixed-' + fixture + '.mm'), '-o', mixed_object], cwd=client)
                                executable = client / (name + '-mixed-' + fixture)
                                run(name + '-mixed-' + fixture + '-link', [
                                    *common, mixed_provider, mixed_object, *link, '-o', executable], cwd=client)
                                execute(name + '-mixed-' + fixture + '-execute', executable, client)
                            mixed_pch = client / (name + '-mixed.pch')
                            run(name + '-mixed-pch-header', [*compile_flags, '-x', 'objective-c++-header',
                                source / 'mixed-pch.h', '-o', mixed_pch], cwd=client)
                            mixed_object = client / (name + '-mixed-pch.o')
                            run(name + '-mixed-pch-consumer', [*compile_flags, '-x', 'objective-c++', '-c',
                                '-include-pch', mixed_pch, source / 'mixed-pch.mm', '-o', mixed_object], cwd=client)
                            executable = client / (name + '-mixed-pch')
                            run(name + '-mixed-pch-link', [*common, mixed_provider, mixed_object,
                                *link, '-o', executable], cwd=client)
                            execute(name + '-mixed-pch-execute', executable, client)
                    name = f'{standard}-{ownership}'
                    executable = client / (name + '-driver')
                    run(name + '-driver-compile', [frontend, *flags, '-O2',
                        source / 'main.mm', '-o', executable], cwd=client)
                    execute(name + '-driver-execute', executable, client)
                    rejections = {
                        'PRIVATE': 'has private destructor',
                        'DELETED_COPY': 'deleted constructor',
                        'CALL_ABI': 'by-value call ABI',
                        'VOLATILE': 'volatile carrier copy and move construction is not supported',
                        'CXX_NEW': 'nontrivial carrier allocation or storage reuse',
                        'CXX_PLACEMENT': 'nontrivial carrier allocation or storage reuse',
                        'CXX_DELETE': 'nontrivial carrier dynamic destruction',
                    }
                    if arc:
                        rejections['ARC_RECORD'] = 'not supported in an Objective-C 3 value carrier'
                        rejections.update({
                            'MIXED_ASSIGNMENT': 'ARC carrier assignment',
                            'MIXED_CALL_ABI': 'by-value call ABI',
                            'MIXED_ARRAY': 'ARC array local initialization',
                            'MIXED_VOLATILE': 'volatile carrier copy and move construction is not supported',
                            'MIXED_CAPTURE': 'by-value call ABI',
                            'MIXED_RECORD': 'mixed ARC/C++ carrier enclosing storage',
                            'MIXED_LAMBDA': 'mixed ARC/C++ carrier enclosing storage',
                            'MIXED_NEW': 'nontrivial carrier allocation or storage reuse',
                            'MIXED_NEW_VALUE': 'nontrivial carrier allocation or storage reuse',
                            'MIXED_NEW_ARRAY': 'nontrivial carrier allocation or storage reuse',
                            'MIXED_PLACEMENT': 'nontrivial carrier allocation or storage reuse',
                            'MIXED_DELETE': 'nontrivial carrier dynamic destruction',
                        })
                        executable = client / (name + '-mixed-driver')
                        run(name + '-mixed-driver-compile', [frontend, *flags, '-O2',
                            source / 'mixed-provider.mm', source / 'mixed-main.mm',
                            '-o', executable], cwd=client)
                        execute(name + '-mixed-driver-execute', executable, client)
                    else:
                        rejections['MIXED_MRC'] = 'not supported in an Objective-C 3 value carrier'
                    for macro, diagnostic in rejections.items():
                        rejected = client / (name + '-' + macro + '.o')
                        log = run(name + '-reject-' + macro, [*common, '-x', 'objective-c++',
                            '-DREJECT_' + macro, '-c', source / 'rejections.mm', '-o', rejected],
                            cwd=client, expected_exit_code=1)
                        if diagnostic not in log or rejected.exists():
                            raise ValueError(f'rejection {macro} lost its diagnostic or emitted an object')
                        report['rejections'] += 1
        if hashes != {str(path): packages.digest(path) for path in inputs}:
            raise ValueError('installed recursive carrier inputs changed during execution')
        if (len(report['configurations']), report['executions'], report['rejections'],
                len(report['commands'])) != (8, 42, 56, 193):
            raise ValueError('installed recursive carrier execution matrix is incomplete')
        report.update(status='PASS', temporary_artifacts_reclaimed=True,
                      commands_passed=len(report['commands']))
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        persist()
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.output)
    print(f"{result['status']}: {result['commands_passed']} installed recursive carrier commands")


if __name__ == '__main__':
    main()
