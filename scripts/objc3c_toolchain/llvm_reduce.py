"""Reproduce and reduce one exact LLVM failure without accepting unrelated exits.

Specs contain explicit argv arrays and hashed tools/dependencies, not shell text.
Candidates run in private scratch directories; original evidence is never edited.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import sys
import tempfile

from .binary_packages import digest, write_json
from .llvm_ir_execution import command
from .sanitizer_policy import OPTIONS, strict_environment

REJECT = ('unknown pass name', 'Unknown command line argument', 'error while loading shared libraries',
          'No such file or directory', 'error: expected', 'error: use of undefined value',
          'undefined reference', 'unable to find library', 'not a valid bitcode file')


def validate(spec: dict):
    rows, failure = spec['commands'], spec['failure']
    if (spec.get('schema') != 'objc3c.llvm-interestingness.v1' or not rows or
            len({row['name'] for row in rows}) != len(rows) or
            failure['stage'] != rows[-1]['name'] or
            failure['kind'] not in ('diagnostic', 'semantic-exit') or
            type(failure['exit_code']) is not int or failure['exit_code'] in (0, 124, 127) or
            not 0 < spec['timeout_seconds'] <= 60 or
            set(spec['environment']) - {*OPTIONS, 'HOME', 'TMPDIR', 'TMP', 'TEMP', 'LANG', 'LC_CTYPE', 'TZ',
                                       'LC_ALL', 'SOURCE_DATE_EPOCH', 'PATH', 'LLVM_SYMBOLIZER_PATH',
                                       'ASAN_SYMBOLIZER_PATH', 'LD_LIBRARY_PATH'}):
        raise ValueError('invalid or unbounded LLVM interestingness specification')
    if any(spec['environment'].get(key) != value for key, value in OPTIONS.items()):
        raise ValueError('interestingness must preserve the strict tool sanitizer policy')
    if failure['kind'] == 'diagnostic':
        if not failure.get('contains') or any(not isinstance(text, str) or not text.strip()
                                             for text in failure['contains']):
            raise ValueError('diagnostic interestingness requires an exact intended signature')
    elif (not 0 < failure['exit_code'] <= 255 or 'stdout' not in failure or rows[0]['name'] != 'input-verifier' or
          rows[0]['argv'][1:3] != ['-passes=verify', '-disable-output'] or
          rows[-1]['argv'][0] != '{work}/program'):
        raise ValueError('semantic reduction requires verified input and an exact program result')
    for row in rows:
        argv = row['argv']
        if (not argv or not all(isinstance(arg, str) and arg for arg in argv) or
                (argv[0] not in spec['tools'] and argv[0] != '{work}/program')):
            raise ValueError('interestingness command lacks a bound executable')
        for path in row.get('outputs', []):
            if not path.startswith('{work}/') or '..' in Path(path).parts:
                raise ValueError('interestingness output escaped private scratch')


def inputs_unchanged(spec: dict):
    for path, signature in {**spec['tools'], **spec.get('inputs', {})}.items():
        if digest(Path(path)) != signature:
            raise ValueError('interestingness tool or dependency changed: '+path)


def matches(failure: dict, result: dict, text: str) -> bool:
    if result['timed_out'] or result['exit_code'] != failure['exit_code'] or any(value in text for value in REJECT):
        return False
    if failure['kind'] == 'semantic-exit':
        return text == failure['stdout']
    return all(value in text for value in failure['contains'])


def interesting(spec: dict, candidate: Path, *, logs: Path | None = None) -> bool:
    validate(spec)
    inputs_unchanged(spec)
    candidate = candidate.resolve(strict=True)
    # An already-failing parse/link/timeout cannot become the expected semantic
    # mismatch. Every prerequisite must succeed and produce its required files.
    attempts = 2 if spec['failure']['kind'] == 'semantic-exit' else 1
    for attempt in range(attempts):
        with tempfile.TemporaryDirectory(prefix='objc3-llvm-interesting-') as temporary:
            work = Path(temporary)
            for index, row in enumerate(spec['commands']):
                def expand(value):
                    return value.replace('{input}', str(candidate)).replace('{work}', str(work))
                argv = [expand(value) for value in row['argv']]
                log = work / ('step-'+str(index)+'.log')
                result = command(argv, cwd=work, environment=spec['environment'], log=log,
                                 timeout=spec['timeout_seconds'])
                text = log.read_text(errors='replace')
                if logs is not None:
                    logs.mkdir(parents=True, exist_ok=True)
                    shutil.copyfile(log, logs / (str(attempt)+'-'+row['name']+'.log'))
                    write_json(logs / (str(attempt)+'-'+row['name']+'.json'), result)
                if row['name'] == spec['failure']['stage']:
                    if not matches(spec['failure'], result, text):
                        return False
                elif result['timed_out'] or result['exit_code'] != 0 or any(value in text for value in REJECT):
                    return False
                if any(not Path(expand(path)).is_file() or Path(expand(path)).stat().st_size == 0
                       for path in row.get('outputs', [])):
                    return False
    inputs_unchanged(spec)
    return True


def semantic_spec(tools: dict, environment: dict, *, expected_exit: int, inputs=(), link=()) -> dict:
    """Prepare a deterministic split-pipeline oracle; caller chooses the wrong result.

    Inputs can include other fixture objects and exact runtime libraries. They
    are bound by content. This is also the synthetic reducer capability control,
    not a claim that the control was emitted by the Objective-C frontend.
    """
    rows = [
        {'name': 'input-verifier', 'argv': [str(tools['opt']), '-passes=verify', '-disable-output', '{input}']},
        {'name': 'optimizer', 'argv': [str(tools['opt']), '-passes=default<O2>', '-verify-each', '{input}',
                                     '-o', '{work}/optimized.bc'], 'outputs': ['{work}/optimized.bc']},
        {'name': 'backend', 'argv': [str(tools['llc']), '-verify-machineinstrs', '-O2', '-relocation-model=pic',
                                   '-filetype=obj', '{work}/optimized.bc', '-o', '{work}/candidate.o'],
         'outputs': ['{work}/candidate.o']},
        {'name': 'link', 'argv': [str(tools['clang++']), '--no-default-config', '-fuse-ld=lld',
                                '-B'+str(tools['ld.lld'].parent), '{work}/candidate.o', *map(str, link),
                                '-o', '{work}/program'], 'outputs': ['{work}/program']},
        {'name': 'oracle', 'argv': ['{work}/program']},
    ]
    libraries = {}
    # Include every loaded non-system compiler library in the binding. Do not
    # fingerprint an executable while silently changing its LLVM DSO underneath.
    for root in {path.parent.parent for path in tools.values()}:
        for name in ('libLLVM.so', 'libclang-cpp.so'):
            path = root / 'lib' / name
            if path.is_file():
                libraries[str(path.resolve())] = digest(path)
    return {'schema': 'objc3c.llvm-interestingness.v1', 'timeout_seconds': 30,
            'tools': {str(path): digest(path) for path in tools.values()},
            'inputs': {**libraries, **{str(path.resolve()): digest(path) for path in inputs}},
            'environment': environment, 'commands': rows,
            'failure': {'stage': 'oracle', 'kind': 'semantic-exit', 'exit_code': expected_exit, 'stdout': ''}}


def reduce(spec_path: Path, original: Path, output: Path, reducer: Path, *, timeout=180) -> dict:
    spec_path, original, reducer = (path.resolve(strict=True) for path in (spec_path, original, reducer))
    output = output.resolve()
    if output.exists() or original.is_relative_to(output) or spec_path.is_relative_to(output):
        raise ValueError('reduction needs a fresh directory disjoint from original evidence')
    spec = json.loads(spec_path.read_text())
    validate(spec)
    if spec['tools'].get(str(reducer)) != digest(reducer):
        raise ValueError('llvm-reduce is not bound to the interestingness tool bundle')
    output.mkdir(parents=True)
    shutil.copyfile(original, output / 'original.ll')
    shutil.copyfile(spec_path, output / 'interestingness.json')
    original_hash = digest(original)
    receipt = {'schema': 'objc3c.llvm-reduction.v1', 'status': 'running',
               'original': str(original), 'original_sha256': original_hash,
               'spec_sha256': digest(spec_path), 'reducer_sha256': digest(reducer)}
    try:
        if not interesting(spec, output / 'original.ll', logs=output / 'original-replay'):
            raise ValueError('original does not reproduce the exact intended failure')
        # Executable wrapper has only fixed, shell-quoted Python/module paths;
        # spec commands remain structured argv arrays. No candidate is embedded
        # into shell text. llvm-reduce appends its candidate as a separate argv.
        import shlex
        wrapper = output / 'interesting.sh'
        repo = Path(__file__).resolve().parents[2]
        wrapper.write_text('#!/bin/sh\ncd '+shlex.quote(str(repo))+' || exit 1\nexec '+
            shlex.quote(sys.executable)+' -B -m scripts.objc3c_toolchain.llvm_reduce interesting --spec '+
            shlex.quote(str(output / 'interestingness.json'))+' --input "$1"\n')
        wrapper.chmod(0o700)
        reduced = output / 'reduced.ll'
        result = command([reducer, '--test='+str(wrapper), '--max-pass-iterations=1',
            '--delta-passes=functions,instructions', output / 'original.ll', '-o', reduced],
            cwd=output, environment=spec['environment'], log=output / 'reducer.log', timeout=timeout)
        receipt['command'] = result
        if result['timed_out'] or result['exit_code'] != 0 or not reduced.is_file():
            raise ValueError('reducer did not complete; original evidence retained')
        if not interesting(spec, reduced, logs=output / 'reduced-replay'):
            raise ValueError('reduced candidate lost the exact failure')
        if digest(original) != original_hash or digest(output / 'original.ll') != original_hash:
            raise ValueError('reduction mutated original evidence')
        receipt.update(status='PASS', reduced_sha256=digest(reduced),
                       original_bytes=original.stat().st_size, reduced_bytes=reduced.stat().st_size)
    except (ValueError, OSError) as error:
        receipt.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(output / 'reduction.json', receipt)
    return receipt


def bisect(spec_path: Path, original: Path, output: Path, *, limit: int) -> dict:
    """Replay one opt-bisect boundary as a triage lead, not pass attribution."""
    if limit < -1:
        raise ValueError('opt-bisect limit must be -1 (inventory) or nonnegative')
    output = output.resolve()
    original, spec_path = original.resolve(strict=True), spec_path.resolve(strict=True)
    if output.exists() or original.is_relative_to(output) or spec_path.is_relative_to(output):
        raise ValueError('opt-bisect requires a fresh directory disjoint from original evidence')
    spec = json.loads(spec_path.read_text())
    validate(spec)
    optimizers = [row for row in spec['commands'] if row['name'] == 'optimizer']
    if len(optimizers) != 1 or not any(arg.startswith('-passes=') for arg in optimizers[0]['argv']):
        raise ValueError('specification lacks an applicable explicit opt pipeline')
    if any(arg.startswith('-opt-bisect') for arg in optimizers[0]['argv']):
        raise ValueError('specification already contains an opt-bisect override')
    optimizers[0]['argv'].insert(1, '-opt-bisect-limit='+str(limit))
    output.mkdir(parents=True)
    signature = digest(original)
    shutil.copyfile(original, output / 'original.ll')
    write_json(output / 'bisect-interestingness.json', spec)
    result = interesting(spec, output / 'original.ll', logs=output / 'replay')
    if digest(original) != signature:
        raise ValueError('opt-bisect mutated original evidence')
    receipt = {'schema': 'objc3c.llvm-opt-bisect.v1', 'status': 'PASS', 'limit': limit,
               'exact_failure_reproduced': result, 'original_sha256': signature,
               'boundary': 'Pass participation is optional; boundary changes are triage leads, not proof of a guilty pass.'}
    write_json(output / 'bisect.json', receipt)
    return receipt


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('interesting', 'reduce', 'bisect'))
    parser.add_argument('--spec', type=Path, required=True)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--reducer', type=Path)
    parser.add_argument('--timeout', type=float, default=180)
    parser.add_argument('--limit', type=int)
    args = parser.parse_args(argv)
    if args.action == 'interesting':
        try:
            return 0 if interesting(json.loads(args.spec.read_text()), args.input) else 1
        except (OSError, ValueError, KeyError):
            return 1
    if args.action == 'bisect':
        if args.output is None or args.limit is None:
            parser.error('bisect requires output and an explicit limit')
        print(json.dumps(bisect(args.spec, args.input, args.output, limit=args.limit)))
        return 0
    if args.output is None or args.reducer is None or args.timeout <= 0:
        parser.error('reduce requires output, reducer and a positive timeout')
    result = reduce(args.spec, args.input, args.output, args.reducer, timeout=args.timeout)
    print(json.dumps(result))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
