#!/usr/bin/env python3
"""Build the canonical public command contract from package.json and the live workflow runner."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence
from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.json_io import load_json_any as load_json
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_payloads,
    public_workflow_list_payload,
)
from scripts.objc3c_workflow.actions.command_facades_inventory import (
    package_bridge_inventory_fields,
    package_bridge_payloads_from_scripts,
)

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[1]
PACKAGE_JSON = ROOT / 'package.json'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-public-command-contract-v1.schema.json'
DEFAULT_OUTPUT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=DEFAULT_OUTPUT)
    add_check_argument(parser)
    return parser.parse_args(argv)


def build_contract() -> dict[str, object]:
    package = load_json(PACKAGE_JSON)
    schema = load_json(SCHEMA_PATH)
    list_payload = public_workflow_list_payload()
    package_scripts = package['scripts']

    bridge_inventory = package_bridge_inventory_fields(package_scripts)
    action_payloads = sorted(public_workflow_action_payloads(), key=lambda payload: str(payload.get('action')))
    package_bridge_payloads = package_bridge_payloads_from_scripts(package_scripts)
    operator_action_count = sum(1 for payload in action_payloads if payload.get('audience') == 'operator')
    maintainer_action_count = sum(1 for payload in action_payloads if payload.get('audience') == 'maintainer')

    return {
        'contract_id': 'objc3c-public-command-contract-v1',
        'issue': 'workflow-public-command-contract',
        'runner_mode': list_payload['mode'],
        'runner_path': list_payload['runner_path'],
        'schema_path': schema['$id'],
        'package_bridge_count': bridge_inventory['package_bridge_count'],
        'workflow_action_count': list_payload['action_count'],
        'internal_action_count': list_payload['internal_action_count'],
        'operator_action_count': operator_action_count,
        'maintainer_action_count': maintainer_action_count,
        'missing_package_bridge': bridge_inventory['missing_package_bridge'],
        'unexpected_package_bridges': bridge_inventory['unexpected_package_bridges'],
        'actions': action_payloads,
        'package_bridges': package_bridge_payloads,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    payload = build_contract()
    rendered = json.dumps(payload, indent=2) + '\n'
    if args.check:
        if not args.output.is_file():
            print(f'[fail] public command contract missing: {args.output}', file=sys.stderr)
            return 1
        existing = args.output.read_text(encoding='utf-8')
        if existing != rendered:
            print(f'[fail] public command contract out of sync: {args.output}', file=sys.stderr)
            return 1
        print(f'[ok] public command contract in sync: {args.output}')
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(rendered, encoding='utf-8', newline='\n')
    print(f'[ok] wrote {args.output}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
