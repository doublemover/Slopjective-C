from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-ci-topology'
SUITE_MATRIX_PATH = PLAN_DIR / 'validation_acceptance_suite_matrix.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_ci_topology.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_ci_topology.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_ci_topology.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_ci_topology.md'

TOPOLOGY = {
    'test:fast': ['aggregate-validation', 'docs', 'repo-shape', 'showcase', 'onboarding'],
    'test:objc3c:full': [
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
    'test:objc3c:nightly': [
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


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    suite_matrix = load_json(SUITE_MATRIX_PATH)
    known_families = {row['suite_family'] for row in suite_matrix['suite_families']}
    for script_name, families in TOPOLOGY.items():
        missing = [family for family in families if family not in known_families]
        if missing:
            raise RuntimeError(f'{script_name} topology references unknown families: {missing}')

    payload = {
        'issue': 'validation-ci-topology',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'topology': [
            {
                'package_script': script_name,
                'family_count': len(families),
                'families': families,
            }
            for script_name, families in TOPOLOGY.items()
        ],
        'next_issues': ['validation-ci-topology-integration'],
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = ['# Validation CI Topology', '', f"- issue: `{payload['issue']}`", '', '## Aggregate schedules']
    for row in payload['topology']:
        lines.append(f"- `{row['package_script']}` -> `{row['family_count']}` families")
        lines.append(f"  - families: {', '.join(f'`{family}`' for family in row['families'])}")
    lines.extend(['', 'Next issue: `validation-ci-topology-integration`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()