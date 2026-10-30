"""Independent negative controls for execution, debug validation and reduction."""
from __future__ import annotations

import copy
import json
from pathlib import Path
import sys

from .binary_packages import digest, write_json
from .llvm_ir_execution import Execution, StageFailure
from .llvm_reduce import interesting, reduce, semantic_spec


def qualify(repo: Path, output: Path, tools: dict, environment: dict) -> dict:
    if output.exists():
        raise ValueError('LLVM controls require a fresh evidence directory')
    output.mkdir(parents=True)
    receipt = {'schema': 'objc3c.llvm-assurance-controls.v1', 'status': 'running', 'controls': []}
    executor = Execution(output, environment, receipt)
    try:
        fixtures = repo / 'tests/tooling/fixtures/llvm_assurance'
        spec = semantic_spec(tools, environment, expected_exit=7)
        spec_path = output / 'interestingness.json'
        write_json(spec_path, spec)
        for name, source, expected in (
                ('intended-semantic-exit', fixtures / 'reduction-wrong-exit.ll', True),
                ('correct-semantic-exit', fixtures / 'valid.ll', False),
                ('unrelated-invalid-ir', fixtures / 'dominance-invalid.ll', False)):
            actual = interesting(spec, source, logs=output / name)
            if actual != expected:
                raise ValueError('interestingness accepted an unrelated result or lost the intended failure: '+name)
            receipt['controls'].append({'name': name, 'status': 'PASS', 'interesting': actual,
                                        'input_sha256': digest(source)})
        unrelated_link = output / 'unresolved.ll'
        unrelated_link.write_text('target triple = "x86_64-unknown-linux-gnu"\n'
            'declare i32 @unresolved_control()\n'
            'define i32 @main() { %v = call i32 @unresolved_control()\n ret i32 %v }\n')
        if interesting(spec, unrelated_link, logs=output / 'unrelated-link-failure'):
            raise ValueError('a link failure was accepted as a semantic mismatch')
        receipt['controls'].append({'name': 'unrelated-link-failure', 'status': 'PASS'})
        altered = copy.deepcopy(spec)
        altered['tools'][str(tools['opt'])] = '0'*64
        try:
            interesting(altered, fixtures / 'reduction-wrong-exit.ll')
        except ValueError as error:
            if 'tool or dependency changed' not in str(error):
                raise
        else:
            raise ValueError('substituted LLVM tool was accepted')
        receipt['controls'].append({'name': 'tool-substitution', 'status': 'PASS'})
        reduction = reduce(spec_path, fixtures / 'reduction-wrong-exit.ll', output / 'reduction', tools['llvm-reduce'])
        # Canonical IR printing can add more header bytes than a reduction
        # removes. Require actual program structure removal, not byte shrinkage.
        reduced_text = (output / 'reduction/reduced.ll').read_text()
        if '@unused(' in reduced_text or '@main(' not in reduced_text:
            raise ValueError('reducer control did not remove the irrelevant function')
        receipt['reduction'] = reduction
        for name, argv, classification, timeout in (
                ('missing-output', [sys.executable, '-c', 'pass'], 'infrastructure', 10),
                ('compiler-crash', [sys.executable, '-c',
                    'import os,signal; os.kill(os.getpid(),signal.SIGKILL)'], 'frontend-crash', 10),
                ('tool-timeout', [sys.executable, '-c', 'import time; time.sleep(20)'], 'timeout', .1)):
            try:
                executor.run(name, argv, stage='frontend-lowering', outputs=[output / (name+'.o')], timeout=timeout)
            except StageFailure as error:
                if error.classification != classification:
                    raise
            else:
                raise ValueError('execution control falsely passed: '+name)
            receipt['controls'].append({'name': name, 'status': 'PASS', 'classification': classification})
        # The assembler input is well formed; only DWARF's unit header is bad.
        assembly, malformed = output / 'malformed-debug.s', output / 'malformed-debug.o'
        assembly.write_text('.section .debug_info,"",@progbits\n.long 0xfffffffe\n.long 0\n')
        executor.run('malformed-debug-assemble', [tools['clang'], '--no-default-config',
            '--target=x86_64-unknown-linux-gnu', '-c', assembly, '-o', malformed], outputs=[malformed])
        executor.run('malformed-debug-verify', [tools['llvm-dwarfdump'], '--verify', malformed],
                     expected=1, contains=('error:', '.debug_info'))
        receipt['controls'].append({'name': 'malformed-dwarf', 'status': 'PASS'})
        receipt['status'] = 'PASS'
    except (OSError, ValueError) as error:
        receipt.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(output / 'qualification.json', receipt)
    return receipt
