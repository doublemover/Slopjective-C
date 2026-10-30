"""Bounded, recorded LLVM subprocesses and explicit failure classifications."""
from __future__ import annotations

import os
from pathlib import Path
import resource
import signal
import subprocess
import time

from .binary_packages import digest, write_json
from .sanitizer_policy import require_clean_diagnostic


class StageFailure(ValueError):
    def __init__(self, name: str, classification: str, message: str):
        super().__init__(name+': '+message)
        self.stage, self.classification = name, classification


def classify(stage: str, code: int, text: str, *, timed_out=False) -> str:
    if timed_out:
        return 'timeout'
    if 'Bad machine code:' in text:
        return 'backend-verifier'
    if ('Instruction does not dominate all uses!' in text or
            'Broken module found' in text or 'input module is broken' in text):
        return 'invalid-ir' if stage == 'input-verifier' else 'pass-verifier'
    if code < 0 or any(value in text for value in
                      ('AddressSanitizer:', 'runtime error:', 'PLEASE submit a bug report')):
        return 'frontend-crash' if stage.startswith('frontend') else 'tool-crash'
    if stage == 'runtime-oracle':
        return 'semantic-mismatch'
    if stage == 'debug':
        return 'dwarf-debugger'
    if stage.startswith('frontend'):
        return 'frontend-diagnostic'
    return 'infrastructure'


def command(argv, *, cwd: Path, environment: dict, log: Path, timeout: float) -> dict:
    """Kill the entire process group on timeout; retain diagnostics for all exits."""
    started = time.monotonic()
    args = list(map(str, argv))
    timed_out = False
    def no_core():
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    with log.open('w') as stream:
        try:
            with subprocess.Popen(args, cwd=cwd, env=environment, stdout=stream,
                                  stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                                  start_new_session=True, preexec_fn=no_core) as process:
                try:
                    code = process.wait(timeout=timeout)
                except subprocess.TimeoutExpired:
                    timed_out = True
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
                    code = 124
                    stream.write('\nTimed out after '+str(timeout)+' seconds.\n')
        except OSError as error:
            stream.write(str(error)+'\n')
            code = 127
    return {'argv': args, 'cwd': str(cwd), 'exit_code': code, 'timed_out': timed_out,
            'seconds': round(time.monotonic()-started, 6), 'log': str(log),
            'log_sha256': digest(log)}


class Execution:
    def __init__(self, output: Path, environment: dict, report: dict, *, timeout=180):
        self.output, self.environment, self.report, self.timeout = output, environment, report, timeout
        (output / 'logs').mkdir(parents=True)
        report['commands'] = []

    def save(self):
        write_json(self.output / 'qualification.json', self.report)

    def run(self, name, argv, *, cwd=None, outputs=(), stage='infrastructure', expected=0,
            contains=(), timeout=None):
        if any(row['name'] == name for row in self.report['commands']):
            raise ValueError('duplicate LLVM assurance stage: '+name)
        paths = [Path(path) for path in outputs]
        if any(path.exists() for path in paths):
            raise StageFailure(name, 'infrastructure', 'output already exists; stale output cannot pass')
        row = command(argv, cwd=cwd or self.output, environment=self.environment,
            log=self.output / 'logs' / (name+'.log'), timeout=timeout or self.timeout)
        text = Path(row['log']).read_text(errors='replace')
        row.update(name=name, stage=stage, expected_exit_code=expected,
                   outputs={str(path): digest(path) for path in paths if path.is_file()})
        self.report['commands'].append(row)
        self.save()
        if row['timed_out'] or row['exit_code'] != expected:
            raise StageFailure(name, classify(stage, row['exit_code'], text, timed_out=row['timed_out']),
                               'unexpected exit '+str(row['exit_code'])+'; see '+row['log'])
        if expected == 0:
            try:
                require_clean_diagnostic(text)
            except ValueError as error:
                raise StageFailure(name, 'tool-crash', str(error)) from error
            if 'Could not parse -passes-ep-' in text or "I'm going to ignore it" in text:
                raise StageFailure(name, 'infrastructure', 'LLVM ignored an invalid pipeline extension')
        if any(not path.is_file() or path.stat().st_size == 0 for path in paths):
            raise StageFailure(name, 'infrastructure', 'missing or empty required output')
        if any(value not in text for value in contains):
            raise StageFailure(name, 'infrastructure', 'required diagnostic/control signature absent')
        row['status'] = 'PASS'
        self.save()
        return text
