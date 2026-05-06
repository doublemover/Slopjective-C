from __future__ import annotations

from collections import Counter
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file
from objc3c_tooling.public_runner import load_public_workflow_runner

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-runner-unification'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_workflow' / 'runner.py'
B002_REPORT_PATH = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-alias-retirement' / 'alias_retirement_report.json'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_runner_unification.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_runner_unification.md'
OUTPUT_JSON_PATH = REPORT_DIR / 'runner_unification_report.json'
OUTPUT_MD_PATH = REPORT_DIR / 'runner_unification_report.md'




def load_runner() -> Any:
    return load_public_workflow_runner(
        runner_path=RUNNER_PATH,
        module_name='objc3c_workflow_runner_m314_b003',
    )


def category_for_script(script_name: str) -> str:
    return script_name.split(':', 1)[0]


def main() -> None:
    package = load_json(PACKAGE_JSON_PATH)
    scripts = package['scripts']
    b002 = load_json(B002_REPORT_PATH)
    runner = load_runner()

    maintainer_scripts: list[str] = []
    operator_scripts = ['objc3c'] if 'objc3c' in scripts else []
    unmapped_scripts = sorted(name for name in scripts if name != 'objc3c')
    category_counts = Counter(category_for_script(name) for name in scripts)
    payload = {
        'issue': 'workflow-runner-unification',
        'package_script_count': len(scripts),
        'workflow_action_count': len(runner.ACTION_SPECS),
        'public_script_count': len(operator_scripts),
        'unmapped_script_count': len(unmapped_scripts),
        'maintainer_script_count': len(sorted(maintainer_scripts)),
        'operator_script_count': len(sorted(operator_scripts)),
        'runner_mode': runner.list_actions_payload()['mode'],
        'retired_alias_count_from_b002': b002['retired_alias_count'],
        'category_counts': dict(sorted(category_counts.items())),
        'maintainer_scripts': sorted(maintainer_scripts),
        'unmapped_scripts': unmapped_scripts,
        'next_issue': 'workflow-public-command-contract',
    }
    write_json_file(OUTPUT_JSON_PATH, payload)
    write_json_file(PLAN_JSON_PATH, payload)

    lines = [
        '# workflow-runner-unification Runner Unification Report',
        '',
        f"- package_script_count: `{payload['package_script_count']}`",
        f"- workflow_action_count: `{payload['workflow_action_count']}`",
        f"- public_script_count: `{payload['public_script_count']}`",
        f"- unmapped_script_count: `{payload['unmapped_script_count']}`",
        f"- maintainer_script_count: `{payload['maintainer_script_count']}`",
        f"- operator_script_count: `{payload['operator_script_count']}`",
        f"- runner_mode: `{payload['runner_mode']}`",
        '',
        '## Maintainer-only package scripts',
    ]
    for script_name in payload['maintainer_scripts']:
        lines.append(f"- `{script_name}`")
    lines.extend(['', '## Unmapped package scripts'])
    if payload['unmapped_scripts']:
        for script_name in payload['unmapped_scripts']:
            lines.append(f"- `{script_name}`")
    else:
        lines.append('- none')
    lines.extend(['', 'Next issue: `workflow-public-command-contract`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(PLAN_MD_PATH, markdown)


if __name__ == '__main__':
    main()
