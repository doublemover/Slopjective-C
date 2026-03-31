from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN_DIR = ROOT / 'tmp' / 'planning' / 'source_hygiene'
REPORT_DIR = ROOT / 'tmp' / 'reports' / 'source-hygiene' / 'stable-identifier-policy'
POLICY_PATH = ROOT / 'tests' / 'tooling' / 'fixtures' / 'source_hygiene' / 'stable_identifier_authenticity_policy.json'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'source-hygiene' / 'residue-authenticity-inventory' / 'residue_authenticity_inventory.json'
PLAN_JSON_PATH = PLAN_DIR / 'stable_identifier_policy_summary.json'
PLAN_MD_PATH = PLAN_DIR / 'stable_identifier_policy_summary.md'
REPORT_JSON_PATH = REPORT_DIR / 'stable_identifier_policy_summary.json'
REPORT_MD_PATH = REPORT_DIR / 'stable_identifier_policy_summary.md'


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8', newline='\n')


def main() -> None:
    policy = json.loads(POLICY_PATH.read_text(encoding='utf-8'))
    inventory = json.loads(INVENTORY_PATH.read_text(encoding='utf-8'))

    summary = {
        'issue': 'source-hygiene-stable-identifier-policy',
        'contract_id': policy['contract_id'],
        'policy_path': POLICY_PATH.relative_to(ROOT).as_posix(),
        'inventory_path': INVENTORY_PATH.relative_to(ROOT).as_posix(),
        'durable_identifier_regex': policy['stable_identifier_policy']['durable_identifier_regex'],
        'annotation_families': policy['stable_identifier_policy']['annotation_families'],
        'authenticity_classes': sorted(policy['authenticity_classes'].keys()),
        'generated_truth_file_count': inventory['generated_truth_file_count'],
        'generated_truth_residue_hit_count': inventory['generated_truth_residue_hit_count'],
        'product_residue_hit_count': inventory['product_residue_hit_count'],
        'product_residue_bucket_counts': inventory['residue_bucket_counts'],
        'replacement_examples': policy['stable_identifier_policy']['replacement_examples'],
        'generated_truth_must_reach_zero': True,
        'synthetic_fixture_root_examples': policy['authenticity_classes']['synthetic_fixture']['allowed_roots'],
        'archive_only_root_examples': policy['authenticity_classes']['archive_reference']['allowed_roots'],
        'next_issue': 'product-decontamination',
        'checks': {
            'generated_truth_surface_listed': len(policy['scope']['generated_truth_files']) == inventory['generated_truth_file_count'],
            'archive_roots_listed': bool(policy['scope']['archive_only_roots']),
            'authenticity_class_count': len(policy['authenticity_classes']) == 4,
            'replacement_examples_present': len(policy['stable_identifier_policy']['replacement_examples']) >= 4,
            'inventory_baseline_loaded': inventory['product_residue_hit_count'] >= inventory['generated_truth_residue_hit_count'],
        },
    }
    summary['ok'] = all(summary['checks'].values())

    write_text(PLAN_JSON_PATH, json.dumps(summary, indent=2) + '\n')
    write_text(REPORT_JSON_PATH, json.dumps(summary, indent=2) + '\n')

    lines = [
        '# Stable Identifier Policy Summary',
        '',
        f"- contract_id: `{summary['contract_id']}`",
        f"- policy_path: `{summary['policy_path']}`",
        f"- inventory_path: `{summary['inventory_path']}`",
        f"- durable_identifier_regex: `{summary['durable_identifier_regex']}`",
        f"- generated_truth_file_count: `{summary['generated_truth_file_count']}`",
        f"- generated_truth_residue_hit_count: `{summary['generated_truth_residue_hit_count']}`",
        f"- product_residue_hit_count: `{summary['product_residue_hit_count']}`",
        f"- ok: `{summary['ok']}`",
        '',
        '## Authenticity classes',
    ]
    for name in summary['authenticity_classes']:
        lines.append(f'- `{name}`')
    lines.extend(['', '## Annotation families'])
    for name in summary['annotation_families']:
        lines.append(f'- `{name}`')
    lines.extend(['', '## Checks'])
    for key, value in summary['checks'].items():
        lines.append(f"- `{key}`: `{value}`")
    lines.extend(['', 'Next step: `product-decontamination`', ''])
    markdown = '\n'.join(lines)
    write_text(PLAN_MD_PATH, markdown)
    write_text(REPORT_MD_PATH, markdown)


if __name__ == '__main__':
    main()
