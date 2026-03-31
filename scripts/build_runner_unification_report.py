from __future__ import annotations

import importlib.util
import json
import sys
from collections import Counter
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'M314-B003'
PACKAGE_JSON_PATH = ROOT / 'package.json'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_public_workflow_runner.py'
B002_REPORT_PATH = ROOT / 'tmp' / 'reports' / 'm314' / 'M314-B002' / 'alias_retirement_report.json'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_runner_unification.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_runner_unification.md'
OUTPUT_JSON_PATH = REPORT_DIR / 'runner_unification_report.json'
OUTPUT_MD_PATH = REPORT_DIR / 'runner_unification_report.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def load_runner() -> Any:
    spec = importlib.util.spec_from_file_location('objc3c_public_workflow_runner_m314_b003', RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def category_for_script(script_name: str) -> str:
    return script_name.split(':', 1)[0]


def main() -> None:
    package = load_json(PACKAGE_JSON_PATH)
    scripts = package['scripts']
    b002 = load_json(B002_REPORT_PATH)
    runner = load_runner()

    public_script_to_action: dict[str, str] = {}
    maintainer_scripts: list[str] = []
    operator_scripts: list[str] = []
    for action_name, spec in runner.ACTION_SPECS.items():
        payload = runner.describe_action_payload(action_name)
        for public_script in spec.public_scripts:
            public_script_to_action[public_script] = action_name
            if payload['audience'] == 'maintainer':
                maintainer_scripts.append(public_script)
            elif payload['audience'] == 'operator':
                operator_scripts.append(public_script)

    unmapped_scripts = sorted(name for name in scripts if name not in public_script_to_action)
    category_counts = Counter(category_for_script(name) for name in scripts)
    payload = {
        'issue': 'M314-B003',
        'package_script_count': len(scripts),
        'workflow_action_count': len(runner.ACTION_SPECS),
        'public_script_count': len(public_script_to_action),
        'unmapped_script_count': len(unmapped_scripts),
        'maintainer_script_count': len(sorted(maintainer_scripts)),
        'operator_script_count': len(sorted(operator_scripts)),
        'runner_mode': runner.list_actions_payload()['mode'],
        'retired_alias_count_from_b002': b002['retired_alias_count'],
        'category_counts': dict(sorted(category_counts.items())),
        'maintainer_scripts': sorted(maintainer_scripts),
        'unmapped_scripts': unmapped_scripts,
        'next_issue': 'M314-C001',
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(PLAN_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# M314-B003 Runner Unification Report',
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
    lines.extend(['', 'Next issue: `M314-C001`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(PLAN_MD_PATH, markdown)


if __name__ == '__main__':
    main()
