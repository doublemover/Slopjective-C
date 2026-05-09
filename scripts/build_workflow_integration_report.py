from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_text_file as write_text, write_json_file
from objc3c_tooling.public_runner import load_public_workflow_runner

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'workflow_simplification'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm314' / 'workflow-integration'
RUNNER_PATH = ROOT / 'scripts' / 'objc3c_workflow' / 'runner.py'
DISPATCH_PATH = ROOT / 'scripts' / 'objc3c_workflow' / 'action_dispatch.py'
CONTRACT_BUILDER = ROOT / 'scripts' / 'build_objc3c_public_command_contract.py'
DEFAULT_CONTRACT_PATH = ROOT / 'tmp' / 'artifacts' / 'public-command-surface' / 'objc3c-public-command-contract.json'
MAINTAINER_RUNBOOK_PATH = ROOT / 'docs' / 'runbooks' / 'objc3c_maintainer_workflows.md'
README_PATH = ROOT / 'README.md'
PLAN_JSON_PATH = PLAN_DIR / 'workflow_integration.json'
PLAN_MD_PATH = PLAN_DIR / 'workflow_integration.md'
REPORT_JSON_PATH = REPORT_DIR / 'workflow_integration_report.json'
REPORT_MD_PATH = REPORT_DIR / 'workflow_integration_report.md'



def load_runner() -> Any:
    return load_public_workflow_runner(
        runner_path=DISPATCH_PATH,
        module_name='objc3c_workflow_runner_m314_d001',
    )


def main() -> None:
    subprocess.run([sys.executable, str(CONTRACT_BUILDER)], cwd=ROOT, check=True)
    subprocess.run([sys.executable, str(RUNNER_PATH), 'check-public-command-budget'], cwd=ROOT, check=True)

    runner = load_runner()
    contract = json.loads(DEFAULT_CONTRACT_PATH.read_text(encoding='utf-8'))
    maintainer_runbook = MAINTAINER_RUNBOOK_PATH.read_text(encoding='utf-8')
    readme = README_PATH.read_text(encoding='utf-8')

    package_bridge = runner.describe_package_script_payload('objc3c')
    operator_examples = {
        action: runner.describe_action_payload(action)
        for action in ('build-public-command-surface', 'check-public-command-surface', 'validate-repo-superclean')
    }
    maintainer_public_examples = {
        action: runner.describe_action_payload(action)
        for action in ('check-dependency-boundaries', 'check-task-hygiene', 'lint')
    }
    internal_maintainer_actions = {
        action: runner.describe_action_payload(action)
        for action in ('build-public-command-contract', 'check-public-command-contract', 'check-public-command-budget')
    }

    doc_assertions = {
        'maintainer_runbook_mentions_build_public_command_contract': 'npm run objc3c -- build-public-command-contract' in maintainer_runbook,
        'maintainer_runbook_mentions_check_public_command_contract': 'npm run objc3c -- check-public-command-contract' in maintainer_runbook,
        'maintainer_runbook_mentions_check_public_command_budget': 'npm run objc3c -- check-public-command-budget' in maintainer_runbook,
        'maintainer_runbook_uses_wrapper_for_dependency_boundaries': 'npm run objc3c -- check-dependency-boundaries' in maintainer_runbook,
        'maintainer_runbook_uses_wrapper_for_task_hygiene': 'npm run objc3c -- check-task-hygiene' in maintainer_runbook,
        'maintainer_runbook_uses_wrapper_for_external_validation_surface': 'npm run objc3c -- check-external-validation-surface' in maintainer_runbook,
        'readme_mentions_internal_maintainer_actions': 'check-public-command-budget' in readme and 'build-public-command-contract' in readme,
    }

    payload = {
        'issue': 'workflow-integration',
        'package_script_count': contract['package_script_count'],
        'package_bridge_count': contract['package_bridge_count'],
        'operator_script_count': contract['operator_script_count'],
        'maintainer_script_count': contract['maintainer_script_count'],
        'runner_mode': contract['runner_mode'],
        'maintainer_runbook_path': MAINTAINER_RUNBOOK_PATH.relative_to(ROOT).as_posix(),
        'readme_path': README_PATH.relative_to(ROOT).as_posix(),
        'package_bridge': {
            'package_script': package_bridge['package_script'],
            'action': package_bridge['action'],
            'backend': package_bridge['backend'],
        },
        'operator_examples': {
            action: {
                'action': payload['action'],
                'category': payload['category'],
                'backend': payload['backend'],
            }
            for action, payload in operator_examples.items()
        },
        'maintainer_public_examples': {
            action: {
                'action': payload['action'],
                'category': payload['category'],
                'backend': payload['backend'],
            }
            for action, payload in maintainer_public_examples.items()
        },
        'internal_maintainer_actions': {
            action: {
                'category': payload['category'],
                'backend': payload['backend'],
            }
            for action, payload in internal_maintainer_actions.items()
        },
        'doc_assertions': doc_assertions,
        'next_issue': 'workflow-prototype-retirement',
    }
    write_json_file(PLAN_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = [
        '# workflow-integration Workflow Integration Report',
        '',
        f"- package_script_count: `{payload['package_script_count']}`",
        f"- package_bridge_count: `{payload['package_bridge_count']}`",
        f"- operator_script_count: `{payload['operator_script_count']}`",
        f"- maintainer_script_count: `{payload['maintainer_script_count']}`",
        f"- runner_mode: `{payload['runner_mode']}`",
        '',
        '## Package bridge',
        f"- `{payload['package_bridge']['package_script']}` -> `{payload['package_bridge']['backend']}`",
        '',
        '## Operator examples',
    ]
    for script, details in payload['operator_examples'].items():
        lines.append(f"- `npm run objc3c -- {script}` -> `{details['backend']}` (`{details['category']}`)")
    lines.extend(['', '## Maintainer public examples'])
    for script, details in payload['maintainer_public_examples'].items():
        lines.append(f"- `npm run objc3c -- {script}` -> `{details['backend']}` (`{details['category']}`)")
    lines.extend(['', '## Internal maintainer actions'])
    for action, details in payload['internal_maintainer_actions'].items():
        lines.append(f"- `npm run objc3c -- {action}` -> `{details['backend']}` (`{details['category']}`)")
    lines.extend(['', '## Documentation assertions'])
    for key, value in payload['doc_assertions'].items():
        lines.append(f"- `{key}`: `{value}`")
    lines.extend(['', 'Next issue: `workflow-prototype-retirement`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
