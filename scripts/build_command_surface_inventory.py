from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names
from scripts.objc3c_workflow.actions.command_facades_inventory import (
    command_facade_inventory_contract,
    package_bridge_names_from_scripts,
)

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-surface-inventory'
PACKAGE_JSON_PATH = ROOT / 'package.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'command_surface_inventory.json'
OUTPUT_MD_PATH = REPORT_DIR / 'command_surface_inventory.md'


def main() -> None:
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']

    package_bridge_names = package_bridge_names_from_scripts(scripts)
    public_actions = public_workflow_action_names()
    internal_actions: list[str] = []

    payload = {
        **command_facade_inventory_contract(
            scripts,
            workflow_action_count=len(public_actions),
            public_action_count=len(public_actions),
            internal_action_count=len(internal_actions),
        ),
        'issue': 'workflow-command-surface-inventory',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'next_issue': 'workflow-simplification-policy',
    }
    write_json_file(OUTPUT_JSON_PATH, payload)

    lines = [
        '# workflow-command-surface-inventory Command Surface Inventory',
        '',
        f"- package_bridge_count: `{payload['package_bridge_count']}`",
        f"- workflow_action_count: `{payload['workflow_action_count']}`",
        f"- public_action_count: `{payload['public_action_count']}`",
        f"- internal_action_count: `{payload['internal_action_count']}`",
        f"- missing_package_bridge: `{len(payload['missing_package_bridge'])}`",
        f"- unexpected_package_bridges: `{len(payload['unexpected_package_bridges'])}`",
        '',
        '## Package bridges',
    ]
    for script_name in package_bridge_names:
        lines.append(f"- `{script_name}`")
    lines.extend(['', '## Unexpected package bridges'])
    for script_name in payload['unexpected_package_bridges']:
        lines.append(f"- `{script_name}`")
    lines.extend(['', 'Next issue: `workflow-simplification-policy`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()
