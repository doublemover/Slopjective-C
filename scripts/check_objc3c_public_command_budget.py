#!/usr/bin/env python3
"""Check the public command budget and appendix sync against the canonical command contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
COMMAND_SURFACE_PY = ROOT / 'scripts' / 'render_objc3c_public_command_surface.py'
DEFAULT_CONTRACT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'
CANONICAL_CATEGORIES = ['build', 'check', 'compile', 'inspect', 'package', 'proof', 'publish', 'test', 'trace']
MAX_MAINTAINER_SCRIPTS = 8


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--summary-out', type=Path)
    parser.add_argument('--markdown-out', type=Path)
    return parser.parse_args(argv)


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    subprocess.run([sys.executable, str(CONTRACT_BUILDER), '--output', str(DEFAULT_CONTRACT)], cwd=ROOT, check=True)
    contract = json.loads(DEFAULT_CONTRACT.read_text(encoding='utf-8'))
    subprocess.run([sys.executable, str(COMMAND_SURFACE_PY), '--check'], cwd=ROOT, check=True)

    operator_categories = sorted({entry['category'] for entry in contract['package_scripts'] if entry['audience'] == 'operator'})
    maintainer_categories = sorted({entry['category'] for entry in contract['package_scripts'] if entry['audience'] == 'maintainer'})
    failures: list[str] = []
    if contract['unmapped_scripts']:
        failures.append(f"unmapped package scripts present: {contract['unmapped_scripts']}")
    if contract['extra_runner_public_scripts']:
        failures.append(f"runner advertises extra public scripts: {contract['extra_runner_public_scripts']}")
    if operator_categories != CANONICAL_CATEGORIES:
        failures.append(f"operator categories drifted: expected {CANONICAL_CATEGORIES} got {operator_categories}")
    if contract['maintainer_script_count'] > MAX_MAINTAINER_SCRIPTS:
        failures.append(
            f"maintainer script budget exceeded: {contract['maintainer_script_count']} > {MAX_MAINTAINER_SCRIPTS}"
        )

    summary = {
        'status': 'PASS' if not failures else 'FAIL',
        'package_script_count': contract['package_script_count'],
        'public_script_count': contract['public_script_count'],
        'workflow_action_count': contract['workflow_action_count'],
        'internal_action_count': contract['internal_action_count'],
        'operator_script_count': contract['operator_script_count'],
        'maintainer_script_count': contract['maintainer_script_count'],
        'operator_categories': operator_categories,
        'maintainer_categories': maintainer_categories,
        'max_maintainer_scripts': MAX_MAINTAINER_SCRIPTS,
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
            f"- package_script_count: `{summary['package_script_count']}`",
            f"- public_script_count: `{summary['public_script_count']}`",
            f"- workflow_action_count: `{summary['workflow_action_count']}`",
            f"- internal_action_count: `{summary['internal_action_count']}`",
            f"- operator_script_count: `{summary['operator_script_count']}`",
            f"- maintainer_script_count: `{summary['maintainer_script_count']}` / `{summary['max_maintainer_scripts']}`",
            '',
            '## Operator categories',
        ]
        for category in operator_categories:
            lines.append(f"- `{category}`")
        lines.extend(['', '## Maintainer categories'])
        for category in maintainer_categories:
            lines.append(f"- `{category}`")
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
