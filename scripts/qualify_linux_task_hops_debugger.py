"""Inspect source-free coroutine locals after a queued actor hop in LLDB."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(repo))
from scripts.objc3c_toolchain import qualify_task_coroutines as qualification
from scripts.objc3c_toolchain.binary_packages import digest, disjoint, write_json


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    sdk = args.sdk_root.resolve(strict=True)
    args.output_dir = args.output_dir.resolve()
    disjoint(args.output_dir, sdk)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    receipt = args.output_dir / 'qualification.json'
    receipt.unlink(missing_ok=True)
    fixture = sdk / 'share/objc3c/examples/task-coroutines/provider.cpp'
    statement = '    assert(facts.actor_identity == second_id);'
    lines = fixture.read_text().splitlines()
    if lines.count(statement) != 1:
        raise ValueError('installed actor-hop checkpoint is ambiguous or absent')
    line = lines.index(statement) + 1
    modes = [mode for mode in qualification.configurations() if mode[-1] == 'O0']
    original_configurations = qualification.configurations
    original_run = subprocess.run
    predicate = '(bool)(facts.actor_identity == second_id && second_id != first_id)'

    def run(argv, **kwargs):
        if Path(argv[0]).name == 'lldb':
            # Preserve the original value/caller checks, then stop after the
            # destination actor's mailbox has resumed another native frame.
            argv = list(argv)
            if argv[-3:-1] != ['-o', 'continue']:
                raise ValueError('coroutine debugger command contract changed')
            argv[-3:-1] = [
                '-o', f'breakpoint set --file provider.cpp --line {line}',
                '-o', 'continue', '-o', 'frame select 0',
                '-o', 'expression -- ' + predicate,
                '-o', 'frame info', '-o', 'breakpoint disable 2', '-o', 'continue']
        return original_run(argv, **kwargs)

    try:
        qualification.configurations = lambda: modes
        subprocess.run = run
        report = qualification.qualify(sdk, args.output_dir)
    finally:
        qualification.configurations = original_configurations
        subprocess.run = original_run
    logs = sorted(args.output_dir.glob('*-lldb.log'))
    if len(logs) != 6 or any(
            not re.search(r'\(bool\) \$\d+ = true', path.read_text())
            for path in logs):
        report.update(schema='objc3c.task-hop-debugger.v1', status='FAIL',
                      error='LLDB lost the resumed actor-hop frame or its identities')
        write_json(receipt, report)
        raise ValueError(report['error'])
    report.update(schema='objc3c.task-hop-debugger.v1', actor_hop_debugger_cases=6,
                  checkpoint_line=line, checkpoint_predicate=predicate,
                  harness_sha256=digest(Path(__file__)))
    write_json(receipt, report)
    print(json.dumps({'status': 'PASS', 'actor_hop_debugger_cases': 6,
                      'commands': len(report['commands'])}))


if __name__ == '__main__':
    main()
