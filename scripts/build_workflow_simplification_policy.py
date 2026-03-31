from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-simplification-policy'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-command-surface-inventory' / 'command_surface_inventory.json'
POLICY_JSON_PATH = PLAN_DIR / 'workflow_simplification_policy.json'
POLICY_MD_PATH = PLAN_DIR / 'workflow_simplification_policy.md'
REPORT_JSON_PATH = REPORT_DIR / 'policy_summary.json'
REPORT_MD_PATH = REPORT_DIR / 'policy_summary.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    inventory = load_json(INVENTORY_PATH)
    policy = {
        'policy_id': 'workflow-simplification-policy-v1',
        'effective_from_phase': 'workflow-simplification',
        'canonical_public_categories': [
            'build',
            'check',
            'compile',
            'inspect',
            'package',
            'proof',
            'publish',
            'test',
            'trace',
        ],
        'runner_first_rules': [
            'Normal operator flows should route through package.json scripts that delegate to scripts/objc3c_public_workflow_runner.py where a workflow action exists.',
            'Internal workflow actions may remain without public aliases when they are composition helpers or implementation details.',
            'Lint, formatter, dependency-boundary, llvm-capability, and task-hygiene commands may remain direct scripts when they are tool wrappers rather than workflow families.',
            'Generated command appendix and package.json must remain synchronized; public commands should not bypass that surface.'
        ],
        'retained_orphan_public_scripts': inventory['orphan_public_scripts'],
        'next_issues': ['workflow-alias-retirement', 'workflow-runner-unification', 'workflow-public-command-contract'],
    }
    write_text(POLICY_JSON_PATH, json.dumps(policy, indent=2) + '\n')

    summary = {
        'issue': 'workflow-simplification-policy',
        'policy_id': policy['policy_id'],
        'package_script_count': inventory['package_script_count'],
        'workflow_action_count': inventory['workflow_action_count'],
        'orphan_public_script_count': inventory['orphan_public_script_count'],
        'canonical_public_categories': policy['canonical_public_categories'],
        'retained_orphan_public_scripts': policy['retained_orphan_public_scripts'],
        'next_issues': policy['next_issues'],
    }
    write_text(REPORT_JSON_PATH, json.dumps(summary, indent=2) + '\n')

    lines = [
        '# Workflow Simplification Policy',
        '',
        f"- policy_id: `{policy['policy_id']}`",
        f"- package_script_count: `{inventory['package_script_count']}`",
        f"- workflow_action_count: `{inventory['workflow_action_count']}`",
        f"- orphan_public_script_count: `{inventory['orphan_public_script_count']}`",
        '',
        '## Canonical public categories',
    ]
    for category in policy['canonical_public_categories']:
        lines.append(f"- `{category}`")
    lines.extend(['', '## Retained orphan public scripts'])
    for script_name in policy['retained_orphan_public_scripts']:
        lines.append(f"- `{script_name}`")
    lines.extend(['', '## Runner-first rules'])
    for rule in policy['runner_first_rules']:
        lines.append(f"- {rule}")
    lines.extend(['', 'Next issues: `workflow-alias-retirement`, `workflow-runner-unification`, `workflow-public-command-contract`', ''])
    markdown = '\n'.join(lines)
    write_text(POLICY_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()