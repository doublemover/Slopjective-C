"""Real coverage reader controls for binary identity, missing data and recovery."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess
import time

from .binary_packages import digest, write_json
from .llvm_ir_execution import command
from .llvm_tools import foundation
from .sanitizer_policy import strict_environment
from .source_coverage import raw_profiles, read_export


def qualify(repo, output):
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('coverage controls require fresh owned output')
    plan, _ = foundation(repo)
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    tools = {name: bootstrap / 'bin' / name for name in ('clang++', 'llvm-cov', 'llvm-profdata')}
    source = repo / 'tests/tooling/fixtures/source_coverage'
    inputs = {str(path): digest(path) for path in [Path(__file__), *tools.values(), *source.glob('*.cpp')]}
    output.mkdir(parents=True)
    env = strict_environment(tools['clang++'], bootstrap / 'bin/llvm-symbolizer')
    report = {'schema': 'objc3c.source-coverage-controls.v1', 'status': 'running', 'commands': [], 'controls': [], 'inputs': inputs}
    started = time.monotonic()
    def save():
        report['seconds'] = round(time.monotonic()-started, 6);write_json(output / 'qualification.json', report)
    def run(name, argv, *, expected=0, environment=env):
        row = {'name': name, **command(argv, cwd=output, environment=environment, log=output / (name+'.log'), timeout=60)}
        report['commands'].append(row);save()
        if row['exit_code'] != expected or row['timed_out']:
            raise ValueError('coverage control had unexpected disposition: '+name)
        return Path(row['log'])
    try:
        binary, wrong = output / 'control', output / 'wrong'
        flags = ['-std=c++20', '-O0', '-g0', '-UNDEBUG', '-fuse-ld=lld', '-fprofile-instr-generate', '-fcoverage-mapping',
            '-fprofile-update=atomic', '-Wl,--build-id=sha1']
        for name, destination, extra in (('build', binary, []), ('wrong-build', wrong, ['-DWRONG_BINARY'])):
            run(name, [tools['clang++'], '--no-default-config', *flags, *extra,
                source / 'control.cpp', source / 'unreached.cpp', '-o', destination])
        original_hash, wrong_hash = digest(binary), digest(wrong)
        raw = output / 'raw';raw.mkdir()
        # Simultaneous processes with the same binary must retain two files.
        processes = []
        for suffix in ('normal', 'recovery'):
            stream = (output / (suffix+'.log')).open('w')
            args = [binary, *(['recovery'] if suffix == 'recovery' else [])]
            processes.append((subprocess.Popen(list(map(str, args)), cwd=output,
                env={**env, 'LLVM_PROFILE_FILE': str(raw / '%p-%m.profraw')}, stdout=stream,
                stderr=subprocess.STDOUT), stream, suffix, args))
        try:
            for process, stream, suffix, args in processes:
                code = process.wait(timeout=20);stream.close()
                log = output / (suffix+'.log')
                report['commands'].append({'name': suffix, 'argv': list(map(str, args)), 'exit_code': code,
                    'pid': process.pid,
                    'log': str(log), 'log_sha256': digest(log), 'environment': {**env, 'LLVM_PROFILE_FILE': str(raw / '%p-%m.profraw')}})
                if code or json.loads(log.read_text()) != {'value': 42}:
                    raise ValueError('coverage control did not execute its semantic witness')
        finally:
            for process, stream, _, _ in processes:
                if process.poll() is None: process.kill();process.wait()
                stream.close()
        profiles = raw_profiles(raw)
        if len(profiles) != 2: raise ValueError('concurrent coverage profiles overwrote one another')
        report['controls'].append({'name': 'concurrent-process-profile-isolation', 'status': 'PASS', 'profiles': [p.name for p in profiles]})
        merged = output / 'merged.profdata'
        run('merge', [tools['llvm-profdata'], 'merge', '--sparse', *profiles, '-o', merged])
        exported = run('export', [tools['llvm-cov'], 'export', binary, '--check-binary-ids', '--instr-profile='+str(merged)])
        eligible = {str(path.relative_to(repo)) for path in source.glob('*.cpp')}
        rows = read_export(exported, repo=repo, build=output, eligible=eligible)
        unreachable = rows[str((source / 'unreached.cpp').relative_to(repo))]
        if unreachable['state'] != 'zero-hit' or unreachable['summary']['functions']['covered'] != 0:
            raise ValueError('coverage control lost its eligible unreachable function')
        document = json.loads(exported.read_text())
        recovery = [row for data in document['data'] for row in data['functions'] if 'recovery' in row['name']]
        if len(recovery) != 1 or not any(branch[4] and branch[5] for branch in recovery[0]['branches']):
            raise ValueError('coverage did not observe both ordinary and recovery branches')
        normal_pid = next(process.pid for process, _, suffix, _ in processes if suffix == 'normal')
        normal = [path for path in profiles if path.name.startswith(str(normal_pid)+'-')]
        if len(normal) != 1: raise ValueError('coverage lost the ordinary-process profile attribution')
        normal_profile = output / 'ordinary.profdata'
        run('ordinary-merge', [tools['llvm-profdata'], 'merge', '--sparse', *normal, '-o', normal_profile])
        ordinary_export = run('ordinary-export', [tools['llvm-cov'], 'export', binary,
            '--check-binary-ids', '--instr-profile='+str(normal_profile)])
        ordinary = json.loads(ordinary_export.read_text())
        before = [row for data in ordinary['data'] for row in data['functions'] if 'recovery' in row['name']]
        if (len(before) != 1 or not any(not branch[4] or not branch[5] for branch in before[0]['branches'])):
            raise ValueError('recovery control did not demonstrate an initially uncovered branch')
        report['controls'].append({'name': 'zero-hit-and-targeted-recovery-branch', 'status': 'PASS',
            'ordinary_branches': before[0]['branches'], 'combined_branches': recovery[0]['branches'], 'source_coverage': rows})
        for name, argv, diagnostic in (
            ('wrong-binary', [tools['llvm-cov'], 'export', wrong, '--check-binary-ids', '--instr-profile='+str(merged)], 'binary'),
            ('missing-profile', [tools['llvm-cov'], 'export', binary, '--instr-profile='+str(output / 'missing.profdata')], 'No such file'),
            ('corrupt-profile', [tools['llvm-profdata'], 'merge', output / 'corrupt.profraw', '-o', output / 'must-not-exist'], 'error')):
            if name == 'corrupt-profile': (output / 'corrupt.profraw').write_bytes(b'not a raw LLVM profile')
            log = run(name, argv, expected=1)
            if diagnostic.lower() not in log.read_text().lower(): raise ValueError('coverage reader failed for an unrelated reason')
            report['controls'].append({'name': name, 'status': 'PASS'})
        if (digest(binary) != original_hash or digest(wrong) != wrong_hash or
                any(digest(Path(path)) != value for path, value in inputs.items())):
            raise ValueError('coverage control binary/tool/source changed')
        report.update(status='PASS', binary_sha256=original_hash, merged_profile_sha256=digest(merged))
    except Exception as error:
        report.update(status='FAIL', error=str(error));raise
    finally: save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd());parser.add_argument('--output', type=Path, required=True)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'controls': len(result['controls'])}))


if __name__ == '__main__': main()
