"""Measure verified installed compiler command latency and keep runtime separate."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import resource
import statistics
import subprocess
import sys
import tempfile
import time

from scripts.objc3c_toolchain.install_sdk import verify


def measure(sdks: list[Path], repetitions: int, report: Path) -> dict:
    if sys.platform != 'linux' or repetitions < 3:
        raise ValueError('native Linux and at least three measured repetitions are required')
    admitted = [verify(prefix) for prefix in sdks]
    rows = []
    with tempfile.TemporaryDirectory(prefix='objc3-loop-') as temporary:
        root = Path(temporary)
        source = root / 'main.c'
        source.write_text('int main(void) { return 6 * 7 == 42 ? 0 : 1; }\n')
        states = []
        for index, (prefix, data) in enumerate(zip(sdks, admitted, strict=True)):
            resources = [p for p in (prefix / 'lib/clang').iterdir() if p.is_dir()]
            if len(resources) != 1:
                raise ValueError('a single coherent Clang resource directory is required')
            application = root / f'application-{index}'
            compiler = str(prefix / 'bin/objc3c')
            commands = {
                'python_startup': [sys.executable, '-B', '-c', 'pass'],
                'compiler_version': [compiler, '--version'],
                'direct_syntax': [str(prefix / 'bin/clang'), '-resource-dir', str(resources[0]),
                                  '-std=c17', '-fsyntax-only', str(source)],
                'public_syntax': [compiler, str(source), '-std=c17', '-fsyntax-only'],
                'public_compile_and_link': [compiler, str(source), '-std=c17', '-O0', '-o', str(application)],
                'program_runtime': [str(application)],
            }
            states.append({'prefix': prefix, 'seal': data['seal_sha256'],
                           'commands': commands, 'samples': {name: [] for name in commands}})
        # Alternate SDK order to reduce thermal/load-order bias. Each group has
        # one unmeasured warmup; runtime executes a previously linked program.
        for repetition in range(repetitions + 1):
            order = states if repetition % 2 == 0 else reversed(states)
            for state in order:
                env = {**os.environ, 'LD_LIBRARY_PATH': str(state['prefix'] / 'lib')}
                for name, command in state['commands'].items():
                    started = time.perf_counter()
                    result = subprocess.run(command, env=env, cwd=root, stdout=subprocess.DEVNULL,
                                            stderr=subprocess.PIPE, text=True, timeout=60)
                    elapsed = time.perf_counter() - started
                    if result.returncode:
                        raise ValueError(f'{name} failed ({result.returncode}): {result.stderr}')
                    if repetition:
                        state['samples'][name].append(elapsed)
        for state in states:
            current = verify(state['prefix'])
            if current['seal_sha256'] != state['seal']:
                raise ValueError('SDK changed during the latency measurement')
            rows.append({'sdk': str(state['prefix']), 'seal_sha256': state['seal'],
                         'samples_seconds': state['samples'],
                         'median_seconds': {name: statistics.median(values)
                                            for name, values in state['samples'].items()}})
    result = {'schema': 'objc3.development-latency.v1', 'recorded_utc': datetime.now(timezone.utc).isoformat(),
              'repetitions': repetitions, 'python': sys.version.split()[0],
              'script_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'scope': 'Warm installed command latency; runtime excludes compilation/linking and SDK admission.',
              'cpu_quota': Path('/sys/fs/cgroup/cpu.max').read_text().strip(),
              'cpu_model': next(line.split(':', 1)[1].strip() for line in
                                Path('/proc/cpuinfo').read_text().splitlines()
                                if line.startswith('model name')),
              'peak_child_rss_kib': resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss,
              'memory_scope': 'Maximum individual descendant RSS for benchmark commands; not aggregate tree memory.',
              'results': rows}
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(result, indent=2) + '\n')
    return result


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk', type=Path, required=True)
    parser.add_argument('--baseline-sdk', type=Path)
    parser.add_argument('--repetitions', type=int, default=5)
    parser.add_argument('--report', type=Path, required=True)
    parser.add_argument('--budget-report', type=Path,
                        help='Check recorded median budgets on the same CPU model and quota')
    args = parser.parse_args(argv)
    try:
        budget = None
        if args.budget_report:
            if args.budget_report.resolve() == args.report.resolve():
                raise ValueError('budget authority and output report must be different files')
            budget_bytes = args.budget_report.read_bytes()
            budget = json.loads(budget_bytes)
        paths = ([args.baseline_sdk.resolve()] if args.baseline_sdk else []) + [args.sdk.resolve()]
        result = measure(paths, args.repetitions, args.report)
        if budget is not None:
            if any(result[key] != budget[key] for key in ('cpu_model', 'cpu_quota')):
                raise ValueError('CPU model or quota differs from the recorded budget context')
            limits = budget['budgets']['median_seconds']
            actual = result['results'][-1]['median_seconds']
            failures = [name for name, limit in limits.items() if actual[name] > limit]
            result['budget_check'] = {'status': 'FAIL' if failures else 'PASS', 'failures': failures,
                                      'authority': str(args.budget_report),
                                      'authority_sha256': hashlib.sha256(budget_bytes).hexdigest()}
            args.report.write_text(json.dumps(result, indent=2) + '\n')
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        parser.exit(2, f'latency measurement failed: {error}\n')
    print(json.dumps({'report': str(args.report), 'results': [
        {'sdk': row['sdk'], 'median_seconds': row['median_seconds']} for row in result['results']]}, indent=2))
    return int(result.get('budget_check', {}).get('status') == 'FAIL')


if __name__ == '__main__':
    raise SystemExit(main())
