#!/usr/bin/env python3
"""Check the public command budget and appendix sync against the canonical command contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Sequence
from objc3c_tooling.json_io import write_text_file as write_text
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
COMMAND_SURFACE_PY = ROOT / 'scripts' / 'render_objc3c_public_command_surface.py'
DEFAULT_CONTRACT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'
CANONICAL_BRIDGE = 'objc3c'
MAX_PACKAGE_BRIDGES = 1


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--summary-out', type=Path)
    parser.add_argument('--markdown-out', type=Path)
    return parser.parse_args(argv)



def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    subprocess.run(python_script_command(CONTRACT_BUILDER, '--output', DEFAULT_CONTRACT), cwd=ROOT, check=True)
    contract = json.loads(DEFAULT_CONTRACT.read_text(encoding='utf-8'))
    subprocess.run(python_script_command(COMMAND_SURFACE_PY, '--check'), cwd=ROOT, check=True)

    package_bridges = [entry['package_bridge'] for entry in contract['package_bridges']]
    failures: list[str] = []
    if contract['missing_package_bridge']:
        failures.append(f"missing package bridge: {contract['missing_package_bridge']}")
    if contract['unexpected_package_bridges']:
        failures.append(f"unexpected package bridges present: {contract['unexpected_package_bridges']}")
    if package_bridges != [CANONICAL_BRIDGE]:
        failures.append(f"package bridge drifted: expected {[CANONICAL_BRIDGE]} got {package_bridges}")
    if contract['package_bridge_count'] > MAX_PACKAGE_BRIDGES:
        failures.append(f"package bridge budget exceeded: {contract['package_bridge_count']} > {MAX_PACKAGE_BRIDGES}")

    summary = {
        'status': 'PASS' if not failures else 'FAIL',
        'package_bridge_count': contract['package_bridge_count'],
        'workflow_action_count': contract['workflow_action_count'],
        'internal_action_count': contract['internal_action_count'],
        'operator_action_count': contract['operator_action_count'],
        'maintainer_action_count': contract['maintainer_action_count'],
        'package_bridges': package_bridges,
        'max_package_bridges': MAX_PACKAGE_BRIDGES,
        'failures': failures,
        'contract_path': DEFAULT_CONTRACT.relative_to(ROOT).as_posix(),
    }
    if args.summary_out:
        write_text(args.summary_out, json.dumps(summary, indent=2) + '\n')
    if args.markdown_out:
        lines = [
            '# Public Command Budget Report',
            '',
            f"- status: `{summary['status']}`",
            f"- package_bridge_count: `{summary['package_bridge_count']}`",
            f"- workflow_action_count: `{summary['workflow_action_count']}`",
            f"- internal_action_count: `{summary['internal_action_count']}`",
            f"- operator_action_count: `{summary['operator_action_count']}`",
            f"- maintainer_action_count: `{summary['maintainer_action_count']}`",
            '',
            '## Package bridges',
        ]
        for package_bridge in package_bridges:
            lines.append(f"- `{package_bridge}`")
        lines.extend(['', '## Failures'])
        if failures:
            for failure in failures:
                lines.append(f'- {failure}')
        else:
            lines.append('- none')
        lines.append('')
        write_text(args.markdown_out, '\n'.join(lines))

    if failures:
        for failure in failures:
            print(f'[fail] {failure}', file=sys.stderr)
        return 1
    print('[ok] public command budget and appendix sync passed')
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
