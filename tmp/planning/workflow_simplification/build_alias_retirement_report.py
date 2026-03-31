from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'M314-B002'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
A001_INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm314' / 'M314-A001' / 'command_surface_inventory.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'alias_retirement_report.json'
OUTPUT_MD_PATH = REPORT_DIR / 'alias_retirement_report.md'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_alias_retirement.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_alias_retirement.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def load_runner() -> Any:
    spec = importlib.util.spec_from_file_location('objc3c_public_workflow_runner_m314_b002', RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def main() -> None:
    before = load_json(A001_INVENTORY_PATH)
    package_json = load_json(PACKAGE_JSON_PATH)
    scripts = package_json['scripts']
    runner = load_runner()

    public_script_to_action: dict[str, str] = {}
    for action_name, spec in runner.ACTION_SPECS.items():
        for public_script in spec.public_scripts:
            public_script_to_action[public_script] = action_name

    current_orphans = sorted(name for name in scripts if name not in public_script_to_action)
    previous_orphans = set(before['orphan_public_scripts'])
    retired_orphans = sorted(previous_orphans - set(current_orphans))
    retained_orphans = sorted(set(current_orphans))

    payload = {
        'issue': 'M314-B002',
        'previous_package_script_count': before['package_script_count'],
        'current_package_script_count': len(scripts),
        'retired_aliases': retired_orphans,
        'retired_alias_count': len(retired_orphans),
        'retained_direct_maintainer_wrappers': retained_orphans,
        'retained_direct_wrapper_count': len(retained_orphans),
        'lint_command': scripts['lint'],
        'next_issue': 'M314-B003',
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(PLAN_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# M314-B002 Alias Retirement Report',
        '',
        f"- previous_package_script_count: `{payload['previous_package_script_count']}`",
        f"- current_package_script_count: `{payload['current_package_script_count']}`",
        f"- retired_alias_count: `{payload['retired_alias_count']}`",
        f"- retained_direct_wrapper_count: `{payload['retained_direct_wrapper_count']}`",
        f"- lint_command: `{payload['lint_command']}`",
        '',
        '## Retired aliases',
    ]
    if retired_orphans:
        for script_name in retired_orphans:
            lines.append(f"- `{script_name}`")
    else:
        lines.append('- none')
    lines.extend(['', '## Retained direct maintainer wrappers'])
    for script_name in retained_orphans:
        lines.append(f"- `{script_name}`")
    lines.extend(['', 'Next issue: `M314-B003`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(PLAN_MD_PATH, markdown)


if __name__ == '__main__':
    main()
