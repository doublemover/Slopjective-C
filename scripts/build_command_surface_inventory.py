from __future__ import annotations

from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from objc3c_tooling.public_runner import load_public_workflow_runner

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-surface-inventory'
PACKAGE_JSON_PATH = ROOT / 'package.json'
DISPATCH_PATH = ROOT / 'scripts' / 'objc3c_workflow' / 'action_dispatch.py'
OUTPUT_JSON_PATH = REPORT_DIR / 'command_surface_inventory.json'
OUTPUT_MD_PATH = REPORT_DIR / 'command_surface_inventory.md'


def load_runner() -> Any:
    return load_public_workflow_runner(
        runner_path=DISPATCH_PATH,
        module_name='objc3c_workflow_runner_inventory',
    )




def category_for_script(script_name: str) -> str:
    return script_name.split(':', 1)[0]


def main() -> None:
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']
    runner = load_runner()
    action_specs = runner.ACTION_SPECS

    category_counts = Counter(category_for_script(name) for name in scripts)
    public_actions = sorted(action_specs)
    internal_actions: list[str] = []
    orphan_public_scripts = sorted(name for name in scripts if name != "objc3c")

    payload = {
        'issue': 'workflow-command-surface-inventory',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'package_script_count': len(scripts),
        'workflow_action_count': len(action_specs),
        'public_action_count': len(public_actions),
        'internal_action_count': len(internal_actions),
        'orphan_public_script_count': len(orphan_public_scripts),
        'category_counts': dict(sorted(category_counts.items())),
        'orchestration_model': {
            'public_entrypoint_owner': 'package.json -> python -m scripts.objc3c_workflow',
            'internal_action_owner': 'ACTION_SPECS actions are reached through the objc3c package bridge',
            'appendix_generator': 'scripts/render_objc3c_public_command_surface.py',
        },
        'orphan_public_scripts': orphan_public_scripts,
        'next_issue': 'workflow-simplification-policy',
    }
    write_json_file(OUTPUT_JSON_PATH, payload)

    lines = [
        '# workflow-command-surface-inventory Command Surface Inventory',
        '',
        f"- package_script_count: `{payload['package_script_count']}`",
        f"- workflow_action_count: `{payload['workflow_action_count']}`",
        f"- public_action_count: `{payload['public_action_count']}`",
        f"- internal_action_count: `{payload['internal_action_count']}`",
        f"- orphan_public_script_count: `{payload['orphan_public_script_count']}`",
        '',
        '## Script category counts',
    ]
    for key, count in payload['category_counts'].items():
        lines.append(f"- `{key}`: `{count}`")
    lines.extend(['', '## Orphan public scripts'])
    for script_name in orphan_public_scripts:
        lines.append(f"- `{script_name}`")
    lines.extend(['', 'Next issue: `workflow-simplification-policy`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()
