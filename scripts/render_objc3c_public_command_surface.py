#!/usr/bin/env python3
"""Render the synchronized public command surface runbook from the canonical command contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import markdown_table
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
DEFAULT_CONTRACT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'
DEFAULT_OUTPUT = ROOT / 'docs' / 'runbooks' / 'objc3c_public_command_surface.md'


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument('--contract', type=Path, default=DEFAULT_CONTRACT)
    add_check_argument(parser)
    return parser.parse_args(argv)


def load_contract(contract_path: Path) -> dict[str, object]:
    subprocess.run(python_script_command(CONTRACT_BUILDER, '--output', contract_path), cwd=ROOT, check=True)
    return json.loads(contract_path.read_text(encoding='utf-8'))


def render_rows(entries: list[dict[str, object]]) -> list[str]:
    return markdown_table(
        ['Action', 'Category', 'Tier', 'Guarantee owner', 'Extra args', 'Backend'],
        [
            [
                f"`{entry['action']}`",
                f"`{entry['category']}`",
                f"`{entry.get('validation_tier', '') or '-'}`",
                f"`{entry.get('guarantee_owner', '') or '-'}`",
                f"`{'pass-through' if entry['pass_through_args'] else 'fixed-shape'}`",
                f"`{entry['backend']}`",
            ]
            for entry in entries
        ],
    )


def render_markdown(contract_path: Path) -> str:
    contract = load_contract(contract_path)
    action_entries: list[dict[str, object]] = contract['actions']  # type: ignore[assignment]
    operator_entries = [entry for entry in action_entries if entry.get('audience') == 'operator']
    maintainer_entries = [entry for entry in action_entries if entry.get('audience') == 'maintainer']

    lines: list[str] = [
        '# Objective-C 3 Public Command Surface',
        '',
        'This runbook is generated from the canonical public command contract.',
        'It is an operator-facing appendix, not the primary onboarding or project-explanation surface.',
        '',
        f"- Package bridge count: `{contract['package_bridge_count']}`",
        f"- Operator action count: `{contract['operator_action_count']}`",
        f"- Maintainer action count: `{contract['maintainer_action_count']}`",
        f"- Runner path: `{contract['runner_path']}`",
        f"- Contract builder: `{CONTRACT_BUILDER.relative_to(ROOT).as_posix()}`",
        f"- Contract artifact: `{contract_path.relative_to(ROOT).as_posix()}`",
        '',
        '## Operator Commands',
        '',
    ]
    lines.extend(render_rows(operator_entries))
    lines.extend(['', '## Maintainer Commands', ''])
    lines.extend(render_rows(maintainer_entries))
    lines.extend(
        [
            '',
            '## Operator Notes',
            '',
            '- Use the operator commands above for normal public workflows.',
            '- Treat this file as a generated machine-facing appendix for exact command mapping, not as the reader-facing project introduction.',
            '- Maintainer commands are intentionally narrower wrappers for repo hygiene, markdown upkeep, release-evidence checks, and dependency/capability audits.',
            '- Canonical user-facing command names come from `package.json` and route through `npm run objc3c -- <action>`.',
            '- Canonical checked-in doc outputs are `site/index.md`, `docs/objc3c-native.md`, and `docs/runbooks/objc3c_public_command_surface.md`; edit their source roots instead of the generated files.',
            '- `native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots; `tmp/` and `artifacts/` are output roots, not naming roots.',
            '- Composite validation entrypoints write an integrated runner summary to `tmp/reports/objc3c-public-workflow/<action>.json`.',
            '- Those integrated summaries record the exact child-suite report paths emitted by smoke, replay, runtime-acceptance, and other live validation scripts.',
            '- `compile-objc3c` and the fixture-backed suite actions accept pass-through arguments through the npm bridge.',
            '- No additional package-script compatibility names remain supported.',
            '',
        ]
    )
    return '\n'.join(lines)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    rendered = render_markdown(args.contract)
    if args.check:
        existing = args.output.read_text(encoding='utf-8')
        if existing != rendered:
            print(f'[fail] runbook out of sync: {args.output}', file=sys.stderr)
            return 1
        print(f'[ok] runbook in sync: {args.output}')
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(rendered, encoding='utf-8', newline='\n')
    print(f'[ok] wrote {args.output}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
