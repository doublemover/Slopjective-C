#!/usr/bin/env python3
"""Build the canonical public command contract from package.json and the live workflow runner."""

from __future__ import annotations

import argparse
import importlib.util
import json
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
PACKAGE_JSON = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
SCHEMA_PATH = ROOT / 'schemas' / 'objc3c-public-command-contract-v1.schema.json'
DEFAULT_OUTPUT = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument('--check', action='store_true')
    return parser.parse_args(argv)


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def load_runner() -> Any:
    spec = importlib.util.spec_from_file_location('objc3c_public_workflow_runner_contract_builder', RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def build_contract() -> dict[str, object]:
    package = load_json(PACKAGE_JSON)
    schema = load_json(SCHEMA_PATH)
    runner = load_runner()
    list_payload = runner.list_actions_payload()
    package_scripts = package['scripts']

    public_script_to_action = runner.public_script_to_action_map()
    distinct_runner_scripts = sorted(public_script_to_action)
    package_script_names = sorted(package_scripts)
    unmapped_scripts = sorted(set(package_script_names) - set(distinct_runner_scripts))
    extra_runner_public_scripts = sorted(set(distinct_runner_scripts) - set(package_script_names))

    action_payloads = [runner.describe_action_payload(action_name) for action_name in sorted(runner.ACTION_SPECS)]
    package_script_payloads = [runner.describe_package_script_payload(script_name) for script_name in package_script_names]
    operator_script_count = sum(1 for payload in package_script_payloads if payload['audience'] == 'operator')
    maintainer_script_count = sum(1 for payload in package_script_payloads if payload['audience'] == 'maintainer')

    return {
        'contract_id': 'objc3c-public-command-contract-v1',
        'runner_mode': list_payload['mode'],
        'runner_path': list_payload['runner_path'],
        'schema_path': schema['$id'],
        'package_script_count': len(package_script_names),
        'workflow_action_count': list_payload['action_count'],
        'public_script_count': len(distinct_runner_scripts),
        'internal_action_count': list_payload['internal_action_count'],
        'operator_script_count': operator_script_count,
        'maintainer_script_count': maintainer_script_count,
        'unmapped_scripts': unmapped_scripts,
        'extra_runner_public_scripts': extra_runner_public_scripts,
        'actions': action_payloads,
        'package_scripts': package_script_payloads,
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
