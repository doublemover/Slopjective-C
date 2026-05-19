from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-ci-topology'
SUITE_MATRIX_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_ci_topology.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_ci_topology.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_ci_topology.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_ci_topology.md'

TOPOLOGY = {
    'test-smoke': ['aggregate-validation', 'docs', 'repo-shape', 'showcase', 'onboarding'],
    'test-full': [
        'aggregate-validation',
        'docs',
        'repo-shape',
        'showcase',
        'onboarding',
        'stdlib',
        'performance',
        'compiler-throughput',
        'runtime-architecture',
        'release-foundation',
        'packaging-channels',
        'release-operations',
    ],
    'test-nightly': [
        'aggregate-validation',
        'docs',
        'repo-shape',
        'showcase',
        'onboarding',
        'stdlib',
        'performance',
        'compiler-throughput',
        'runtime-architecture',
        'release-foundation',
        'packaging-channels',
        'release-operations',
        'bonus-experiences',
        'conformance-corpus',
        'stress',
        'external-validation',
        'public-conformance',
        'performance-governance',
        'distribution-credibility',
        'runtime-closure',
    ],
}




def main() -> None:
    suite_matrix = load_json(SUITE_MATRIX_PATH)
    known_families = {row['suite_family'] for row in suite_matrix['suite_families']}
    for action, families in TOPOLOGY.items():
        missing = [family for family in families if family not in known_families]
        if missing:
            raise RuntimeError(f'{action} topology references unknown families: {missing}')

    payload = {
        'issue': 'validation-ci-topology',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'topology': [
            {
                'action': action,
                'public_command': f'npm run objc3c -- {action}',
                'family_count': len(families),
                'families': families,
            }
            for action, families in TOPOLOGY.items()
        ],
        'next_issues': ['validation-ci-topology-integration'],
    }
    write_json_file(OUTPUT_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = ['# Validation CI Topology', '', f"- issue: `{payload['issue']}`", '', '## Aggregate schedules']
    for row in payload['topology']:
        lines.append(f"- `{row['public_command']}` -> `{row['family_count']}` families")
        lines.append(f"  - families: {', '.join(f'`{family}`' for family in row['families'])}")
    lines.extend(['', 'Next issue: `validation-ci-topology-integration`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
