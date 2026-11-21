"""Execute installed deferred-action ownership and unwind regressions."""
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
    from scripts.objc3c_native_validation import defer_source
    from . import binary_packages as packages
    from .sdk_example import environment
else:
    sys.dont_write_bytecode = True
    import defer_source
    import binary_packages as packages
    from sdk_example import environment


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    report = {'schema': 'objc3c.installed-defer-control-flow.v1', 'status': 'running',
              'commands': [], 'profile_conformance_claimed': False}

    def persist():
        receipt.write_text(json.dumps(report, indent=2) + '\n')

    persist()
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    compiler, frontend = sdk / 'bin/clang', sdk / 'bin/objc3c'
    runtime = sdk / 'lib/libobjc.so'
    fixtures = sdk / 'share/objc3c/examples/defer-control-flow'
    inputs = [compiler.resolve(), frontend.resolve(), runtime.resolve(),
              sdk / 'objc3-sdk.json', Path(defer_source.__file__).resolve(),
              Path(__file__).resolve(), fixtures / 'runtime.h', fixtures / 'main.m']

    def run(name, command, expected_exit_code=0):
        started = time.monotonic()
        log = output / (name + '.log')
        timed_out = False
        with log.open('w') as stream:
            process = subprocess.Popen(list(map(str, command)), env=env,
                                       stdout=stream, stderr=subprocess.STDOUT,
                                       start_new_session=True)
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
            'name': name, 'exit_code': code, 'expected_exit_code': expected_exit_code,
            'timed_out': timed_out, 'elapsed_seconds': round(time.monotonic() - started, 6),
            'log_sha256': packages.digest(log),
        })
        persist()
        if timed_out or code != expected_exit_code:
            raise ValueError(f'installed defer control flow failed: {name}; see {log}')

    try:
        env = environment(sdk)
        hashes = {str(path): packages.digest(path) for path in inputs}
        report.update(compiler_sha256=hashes[str(compiler.resolve())],
                      frontend_sha256=hashes[str(frontend.resolve())],
                      runtime_library_sha256=hashes[str(runtime.resolve())],
                      sdk_seal_sha256=json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
                      input_sha256=hashes)
        with tempfile.TemporaryDirectory(prefix='objc3-installed-defer-') as temporary:
            client = Path(temporary)
            shutil.copytree(fixtures, client / 'source')
            defer_source.qualify(run, client, compiler, sdk, report,
                                 fixtures=client / 'source')
            for standard in ('c17', 'c++20'):
                executable = client / 'driver-application'
                run('driver-' + standard + '-compile', [
                    frontend, '-std=' + standard, '-x',
                    'objective-c' if standard == 'c17' else 'objective-c++',
                    '-fobjc-arc', '-fobjc-arc-exceptions', '-fexceptions', '-fblocks',
                    '-O2', '-Wno-vla-cxx-extension', client / 'source/main.m',
                    '-o', executable,
                ])
                run('driver-' + standard + '-execute', [executable])
        if hashes != {str(path): packages.digest(path) for path in inputs}:
            raise ValueError('installed defer inputs changed during execution')
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
    print(f"{result['status']}: {result['commands_passed']} installed defer commands")


if __name__ == '__main__':
    main()
