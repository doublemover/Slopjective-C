"""Check the real executor queue lock domain with paired warning controls."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

from .binary_packages import digest, write_json


CASES = {
    1: ('missing-lock-write', "writing variable 'stopping' requires holding mutex"),
    2: ('missing-required-lock', "calling function 'requires_queue' requires holding mutex"),
    3: ('callback-under-lock', "cannot call function 'outside_queue' while mutex"),
    4: ('unmodeled-predicate-lock', "reading variable 'stopping' requires holding mutex"),
}


def validate_diagnostic(source: Path, code: int, diagnostic: str, *, expected=None, marker=None):
    if expected is None:
        if code != 0 or diagnostic.strip():
            raise ValueError('correct lock path did not compile cleanly')
        return
    errors = [line for line in diagnostic.splitlines() if ': error:' in line or ': fatal error:' in line]
    if (code != 1 or expected not in diagnostic or not errors or
            any(not re.search(r'\[-Werror,-Wthread-safety[-a-z]*\]$', line) for line in errors)):
        raise ValueError('missing or unrelated lock detector failure')
    if marker:
        line = next(index for index, text in enumerate(source.read_text().splitlines(), 1) if marker in text)
        if str(source)+':'+str(line)+':' not in diagnostic:
            raise ValueError('lock warning is missing the expected source location')


def qualify(repo: Path, compiler: Path, output: Path) -> dict:
    repo, compiler, output = repo.resolve(strict=True), compiler.resolve(strict=True), output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    fixture = repo / 'tests/clang_foundation/executor_lock_contract/contract.cpp'
    executor = repo / 'native/objc3c/src/runtime/concurrency/service_executor.cpp'
    headers = sorted((repo / 'native/objc3c/src/runtime').rglob('*.h'))
    inputs = [compiler, Path(__file__), fixture, executor, *headers]
    report = {'schema': 'objc3c.executor-lock-contract.v1', 'status': 'running',
              'subject': 'executor queue/stopping mutex ownership; not whole-program race analysis',
              'compiler': str(compiler), 'inputs': {str(path): digest(path) for path in inputs}, 'cases': []}
    receipt = output / 'qualification.json'
    write_json(receipt, report)
    environment = {key: value for key, value in os.environ.items()
                   if key not in ('CPATH', 'CPLUS_INCLUDE_PATH', 'C_INCLUDE_PATH', 'OBJC_INCLUDE_PATH',
                                  'CCC_OVERRIDE_OPTIONS', 'CLANG_CONFIG_FILE_SYSTEM_DIR', 'CLANG_CONFIG_FILE_USER_DIR')}
    flags = ['--no-default-config', '-std=c++20', '-fsyntax-only', '-fno-color-diagnostics',
             '-Wall', '-Wextra', '-pedantic', '-Wthread-safety', '-Werror=thread-safety',
             '-Werror=unknown-attributes', '-Werror=ignored-attributes',
             '-I'+str(repo / 'native/objc3c/src'), '-I'+str(executor.parent)]

    def compile_case(name, source, arguments, *, expected=None, marker=None):
        log = output / (name+'.log')
        argv = [str(compiler), *flags, *arguments, str(source)]
        with log.open('w') as stream:
            result = subprocess.run(argv, cwd=repo, env=environment, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=60)
        diagnostic = log.read_text()
        row = {'name': name, 'argv': argv, 'exit_code': result.returncode, 'log_sha256': digest(log)}
        report['cases'].append(row)
        write_json(receipt, report)
        validate_diagnostic(source, result.returncode, diagnostic, expected=expected, marker=marker)
        row['status'] = 'PASS'

    try:
        version = subprocess.check_output([compiler, '--version'], env=environment, text=True, timeout=30)
        pinned = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())['upstream']['tag'].removeprefix('llvmorg-')
        if not re.search(r'clang version '+re.escape(pinned)+r'(?![0-9.])', version):
            raise ValueError('lock qualification requires the pinned Clang implementation build compiler')
        report['compiler_version'] = version
        compile_case('correct-contract', fixture, ['-DOBJC3_LOCK_DEFECT=0'])
        for mode, (marker, expected) in CASES.items():
            compile_case(marker, fixture, ['-DOBJC3_LOCK_DEFECT='+str(mode)], expected=expected, marker=marker)
        compile_case('current-executor', executor, [])
        with tempfile.TemporaryDirectory(prefix='objc3-executor-lock-') as temporary:
            # Mutate the real implementation. The checker must reject a queue
            # access whose surrounding lock was accidentally removed.
            source = Path(temporary) / 'executor-missing-lock.cpp'
            original = executor.read_text()
            guard = 'objc3::services::ExecutorQueueLock lock(executor->mutex);'
            if guard not in original:
                raise ValueError('executor queue guard mutation no longer targets the current implementation')
            source.write_text(original.replace(guard, '// deliberately omitted queue guard', 1))
            compile_case('implementation-missing-lock', source, [],
                         expected="reading variable 'queue' requires holding mutex")
        if any(digest(Path(path)) != value for path, value in report['inputs'].items()):
            raise ValueError('lock contract inputs changed during qualification')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(receipt, report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--compiler', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    print(json.dumps(qualify(args.repo, args.compiler, args.output)))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
