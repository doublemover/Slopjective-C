from __future__ import annotations

import json
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-B003'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-A001' / 'validation_surface_inventory.json'
POLICY_PATH = PLAN_DIR / 'validation_consolidation_policy.json'
NAMESPACE_JSON_PATH = PLAN_DIR / 'legacy_validation_surface_map.json'
NAMESPACE_MD_PATH = PLAN_DIR / 'legacy_validation_surface_map.md'
SUMMARY_JSON_PATH = REPORT_DIR / 'legacy_validation_surface_map.json'
SUMMARY_MD_PATH = REPORT_DIR / 'legacy_validation_surface_map.md'

CLASSIFICATIONS: dict[str, dict[str, Any]] = {
    'scripts/check_activation_triggers.py': {
        'state': 'migration-only',
        'namespace_bucket': 'legacy/bootstrap-preflight',
        'successor_surface': 'scripts/run_activation_preflight.py',
        'rationale': 'Still used by an explicit runner and strict hardening fixture checks, but not part of the public acceptance-first validation path.',
    },
    'scripts/check_bootstrap_readiness.py': {
        'state': 'migration-only',
        'namespace_bucket': 'legacy/bootstrap-preflight',
        'successor_surface': 'scripts/run_bootstrap_readiness.py',
        'rationale': 'Still used by a dedicated bootstrap-readiness runner, but not wired into the public validation topology.',
    },
    'scripts/check_conformance_corpus_surface.py': {
        'state': 'active',
        'namespace_bucket': 'retained-static/source-surface-contract',
        'successor_surface': 'scripts/check_objc3c_conformance_corpus_integration.py + validate-conformance-corpus',
        'rationale': 'This is an actual retained source-surface contract check referenced by integration, packaging, build, docs, and checked-in corpus contracts.',
    },
    'scripts/check_getting_started_surface.py': {
        'state': 'active',
        'namespace_bucket': 'active/child-executable',
        'successor_surface': 'scripts/check_getting_started_integration.py + validate-getting-started',
        'rationale': 'This remains the bounded child executable that the integrated getting-started flow calls and the tutorial docs reference directly.',
    },
    'scripts/check_objc3c_end_to_end_determinism.py': {
        'state': 'migration-only',
        'namespace_bucket': 'legacy/tooling-utility',
        'successor_surface': 'future developer-tooling workflow integration under validate-developer-tooling',
        'rationale': 'This is a tested determinism utility with no current public workflow entrypoint, so it should be treated as a migration-only utility until it is wired in or retired.',
    },
    'scripts/check_objc3c_library_cli_parity.py': {
        'state': 'migration-only',
        'namespace_bucket': 'legacy/tooling-utility',
        'successor_surface': 'future developer-tooling workflow integration under validate-developer-tooling',
        'rationale': 'The parity checker is documented and tested, but it is not currently part of the shared public validation workflow.',
    },
    'scripts/check_open_blocker_audit_contract.py': {
        'state': 'active',
        'namespace_bucket': 'active/internal-contract',
        'successor_surface': 'scripts/run_open_blocker_audit.py',
        'rationale': 'This is an internal contract checker called by the open-blocker audit runner and tested directly; it is not stale, just internal-first.',
    },
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def collect_refs(target: str) -> list[str]:
    target_name = Path(target).name
    for query in (target, target_name):
        result = subprocess.run(
            ['rg', '-n', query, '.'],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=False,
        )
        if result.returncode not in {0, 1}:
            raise RuntimeError(result.stderr.strip() or f'rg failed for {query}')
        refs: list[str] = []
        for line in result.stdout.splitlines():
            line = line.strip()
            if not line:
                continue
            if line.startswith('.\\'):
                line = line[2:]
            refs.append(line.replace('\\', '/'))
        if refs:
            return refs
    return []


def main() -> None:
    inventory = load_json(INVENTORY_PATH)
    policy = load_json(POLICY_PATH)
    unreferenced = inventory['unreferenced_check_surfaces']

    rows = []
    state_counts: dict[str, int] = {}
    for path in unreferenced:
        classification = CLASSIFICATIONS[path]
        refs = collect_refs(path)
        row = {
            'path': path,
            'state': classification['state'],
            'namespace_bucket': classification['namespace_bucket'],
            'successor_surface': classification['successor_surface'],
            'rationale': classification['rationale'],
            'reference_count': len(refs),
            'references': refs,
            'inventory_gap': classification['state'] == 'active',
        }
        rows.append(row)
        state_counts[row['state']] = state_counts.get(row['state'], 0) + 1

    payload = {
        'issue': 'M313-B003',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'policy_id': policy['policy_id'],
        'inventory_issue': inventory['issue'],
        'legacy_surface_count': len(rows),
        'state_counts': state_counts,
        'surfaces': rows,
        'next_issues': ['M313-C001', 'M313-C003'],
    }

    write_text(NAMESPACE_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(SUMMARY_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# Legacy Validation Surface Map',
        '',
        f"- issue: `{payload['issue']}`",
        f"- policy_id: `{payload['policy_id']}`",
        f"- legacy_surface_count: `{payload['legacy_surface_count']}`",
        '',
        '## State counts',
    ]
    for state, count in sorted(state_counts.items()):
        lines.append(f"- `{state}`: `{count}`")
    lines.extend(['', '## Classified surfaces'])
    for row in rows:
        lines.append(f"- `{row['path']}`")
        lines.append(f"  - state: `{row['state']}`")
        lines.append(f"  - namespace_bucket: `{row['namespace_bucket']}`")
        lines.append(f"  - successor_surface: `{row['successor_surface']}`")
        lines.append(f"  - reference_count: `{row['reference_count']}`")
        lines.append(f"  - inventory_gap: `{str(row['inventory_gap']).lower()}`")
    lines.extend(['', 'Next issues: `M313-C001`, `M313-C003`', ''])
    markdown = '\n'.join(lines)
    write_text(NAMESPACE_MD_PATH, markdown)
    write_text(SUMMARY_MD_PATH, markdown)


if __name__ == '__main__':
    main()