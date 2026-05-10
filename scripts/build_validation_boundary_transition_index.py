from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_any as load_json, write_text_file as write_text, write_json_file

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'validation-boundary-transition-index'
LEGACY_MAP_PATH = PLAN_DIR / 'legacy_validation_surface_map.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_boundary_transition_index.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_boundary_transition_index.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_boundary_transition_index.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_boundary_transition_index.md'




def main() -> None:
    legacy_map = load_json(LEGACY_MAP_PATH)
    bridges = []
    for surface in legacy_map['surfaces']:
        if surface['state'] != 'migration-only':
            continue
        bridges.append({
            'path': surface['path'],
            'namespace_bucket': surface['namespace_bucket'],
            'successor_surface': surface['successor_surface'],
            'allowed_callers': surface['references'],
            'removal_condition': 'remove once the named successor owns the workflow and docs/tests stop calling the legacy surface directly',
        })

    payload = {
        'issue': 'validation-boundary-transition-index',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'bridge_count': len(bridges),
        'bridges': bridges,
        'next_issues': ['validation-ci-topology-integration'],
    }
    write_json_file(OUTPUT_JSON_PATH, payload)
    write_json_file(REPORT_JSON_PATH, payload)

    lines = [
        '# Validation Boundary Transition Index',
        '',
        f"- issue: `{payload['issue']}`",
        f"- bridge_count: `{payload['bridge_count']}`",
        '',
        '## Migration-only bridges',
    ]
    for bridge in bridges:
        lines.append(f"- `{bridge['path']}`")
        lines.append(f"  - namespace_bucket: `{bridge['namespace_bucket']}`")
        lines.append(f"  - successor_surface: `{bridge['successor_surface']}`")
        lines.append(f"  - allowed_caller_count: `{len(bridge['allowed_callers'])}`")
    lines.extend(['', 'Next issue: `validation-ci-topology-integration`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
