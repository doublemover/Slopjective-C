from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text
from objc3c_tooling.public_runner import load_public_workflow_runner

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-alias-retirement'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
A001_INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-surface-inventory' / 'command_surface_inventory.json'
OUTPUT_JSON_PATH = REPORT_DIR / 'alias_retirement_report.json'
OUTPUT_MD_PATH = REPORT_DIR / 'alias_retirement_report.md'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_alias_retirement.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_alias_retirement.md'




def load_runner() -> Any:
    return load_public_workflow_runner(
        runner_path=RUNNER_PATH,
        module_name='objc3c_public_workflow_runner_m314_b002',
    )


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
        'issue': 'workflow-alias-retirement',
        'previous_package_script_count': before['package_script_count'],
        'current_package_script_count': len(scripts),
        'retired_aliases': retired_orphans,
        'retired_alias_count': len(retired_orphans),
        'retained_direct_maintainer_wrappers': retained_orphans,
        'retained_direct_wrapper_count': len(retained_orphans),
        'lint_command': scripts['lint'],
        'next_issue': 'workflow-runner-unification',
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(PLAN_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# workflow-alias-retirement Alias Retirement Report',
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
    lines.extend(['', 'Next issue: `workflow-runner-unification`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(PLAN_MD_PATH, markdown)


if __name__ == '__main__':
    main()
