from __future__ import annotations

import importlib.util
import json
import sys
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'M314-A001'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
OUTPUT_JSON_PATH = REPORT_DIR / 'command_surface_inventory.json'
OUTPUT_MD_PATH = REPORT_DIR / 'command_surface_inventory.md'


def load_runner() -> Any:
    spec = importlib.util.spec_from_file_location('objc3c_public_workflow_runner_inventory', RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def category_for_script(script_name: str) -> str:
    return script_name.split(':', 1)[0]


def main() -> None:
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']
    runner = load_runner()
    action_specs = runner.ACTION_SPECS

    public_script_to_action: dict[str, str] = {}
    for action_name, spec in action_specs.items():
        for public_script in spec.public_scripts:
            public_script_to_action[public_script] = action_name

    category_counts = Counter(category_for_script(name) for name in scripts)
    public_actions = sorted({action for action in public_script_to_action.values()})
    internal_actions = sorted(action for action in action_specs if action not in public_actions)
    orphan_public_scripts = sorted(name for name in scripts if name not in public_script_to_action)

    payload = {
        'issue': 'M314-A001',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'package_script_count': len(scripts),
        'workflow_action_count': len(action_specs),
        'public_action_count': len(public_actions),
        'internal_action_count': len(internal_actions),
        'orphan_public_script_count': len(orphan_public_scripts),
        'category_counts': dict(sorted(category_counts.items())),
        'orchestration_model': {
            'public_entrypoint_owner': 'package.json -> scripts/objc3c_public_workflow_runner.py',
            'internal_action_owner': 'ACTION_SPECS actions without public_scripts aliases',
            'appendix_generator': 'scripts/render_objc3c_public_command_surface.py',
        },
        'orphan_public_scripts': orphan_public_scripts,
        'next_issue': 'M314-B001',
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# M314-A001 Command Surface Inventory',
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
    lines.extend(['', 'Next issue: `M314-B001`', ''])
    write_text(OUTPUT_MD_PATH, '\n'.join(lines))


if __name__ == '__main__':
    main()