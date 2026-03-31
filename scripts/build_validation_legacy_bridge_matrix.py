from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'validation_consolidation'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'm313' / 'M313-C003'
LEGACY_MAP_PATH = PLAN_DIR / 'legacy_validation_surface_map.json'
OUTPUT_JSON_PATH = PLAN_DIR / 'validation_legacy_bridge_matrix.json'
OUTPUT_MD_PATH = PLAN_DIR / 'validation_legacy_bridge_matrix.md'
REPORT_JSON_PATH = REPORT_DIR / 'validation_legacy_bridge_matrix.json'
REPORT_MD_PATH = REPORT_DIR / 'validation_legacy_bridge_matrix.md'


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding='utf-8'))


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


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
        'issue': 'M313-C003',
        'generated_at': datetime.now(timezone.utc).isoformat(),
        'bridge_count': len(bridges),
        'bridges': bridges,
        'next_issues': ['M313-D002'],
    }
    write_text(OUTPUT_JSON_PATH, json.dumps(payload, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(payload, indent=2) + '\n')

    lines = [
        '# Validation Legacy Bridge Matrix',
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
    lines.extend(['', 'Next issue: `M313-D002`', ''])
    markdown = '\n'.join(lines)
    write_text(OUTPUT_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()