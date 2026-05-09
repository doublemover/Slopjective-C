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
from source_hygiene.public_command_contract import (
    build_public_command_budget_summary,
    render_public_command_budget_markdown,
)

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
COMMAND_SURFACE_PY = ROOT / 'scripts' / 'render_objc3c_public_command_surface.py'
DEFAULT_CONTRACT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'


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

    summary = build_public_command_budget_summary(
        contract,
        contract_path=DEFAULT_CONTRACT.relative_to(ROOT),
    )
    if args.summary_out:
        write_text(args.summary_out, json.dumps(summary, indent=2) + '\n')
    if args.markdown_out:
        write_text(args.markdown_out, render_public_command_budget_markdown(summary))

    if summary['failures']:
        for failure in summary['failures']:
            print(f'[fail] {failure}', file=sys.stderr)
        return 1
    print('[ok] public command budget and appendix sync passed')
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
