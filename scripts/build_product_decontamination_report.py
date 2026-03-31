#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / 'tests' / 'tooling' / 'fixtures' / 'source_hygiene' / 'stable_identifier_authenticity_policy.json'
INVENTORY_PATH = ROOT / 'tmp' / 'reports' / 'source-hygiene' / 'residue-authenticity-inventory' / 'residue_authenticity_inventory.json'
OUT_DIR = ROOT / 'tmp' / 'reports' / 'source-hygiene' / 'product-decontamination'
JSON_OUT = OUT_DIR / 'product_decontamination_report.json'
MD_OUT = OUT_DIR / 'product_decontamination_report.md'


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding='utf-8'))


def tracked_files() -> set[str]:
    result = subprocess.run(
        ['git', 'ls-files'],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding='utf-8',
        check=True,
    )
    return {line.strip().replace('\\', '/') for line in result.stdout.splitlines() if line.strip()}


def root_matches(root: str, paths: set[str]) -> list[str]:
    normalized = root.replace('\\', '/')
    if normalized in paths:
        return [normalized]
    prefix = normalized if normalized.endswith('/') else normalized + '/'
    return sorted(path for path in paths if path.startswith(prefix))


def main() -> int:
    policy = read_json(POLICY_PATH)
    inventory = read_json(INVENTORY_PATH)
    tracked = tracked_files()

    product_paths: list[str] = []
    for root in policy['scope']['product_surface_roots']:
        product_paths.extend(root_matches(root, tracked))
    product_paths = sorted(dict.fromkeys(product_paths))

    generated_truth = sorted(dict.fromkeys(policy['scope']['generated_truth_files']))
    milestone_pattern = re.compile(r'\bM\d{3}\b|\[M\d{3}\]')
    forbidden_tokens = [
        *( (f'lane-{letter.lower()}', re.compile('Lane-' + letter)) for letter in 'ABCDE' ),
        ('workpack-iteration', re.compile('workpack ' + 'iteration')),
        ('closeout-signoff', re.compile('closeout ' + 'signoff')),
    ]

    milestone_hits = []
    forbidden_hits = []
    for rel in product_paths:
        text = (ROOT / rel).read_text(encoding='utf-8')
        matches = sorted(set(milestone_pattern.findall(text)))
        if matches:
            milestone_hits.append({'path': rel, 'matches': matches})
        token_matches = [name for name, pattern in forbidden_tokens if pattern.search(text)]
        if token_matches:
            forbidden_hits.append({'path': rel, 'matches': token_matches})

    generated_truth_hits = [entry for entry in milestone_hits if entry['path'] in generated_truth]
    summary = {
        'issue': 'source-hygiene-product-decontamination',
        'contract_id': 'objc3c.sourcehygiene.productdecontamination.v1',
        'policy_id': policy['contract_id'],
        'inventory_baseline_issue': inventory['issue'],
        'baseline_product_residue_hit_count': inventory['product_residue_hit_count'],
        'baseline_generated_truth_residue_hit_count': inventory['generated_truth_residue_hit_count'],
        'product_surface_file_count': len(product_paths),
        'generated_truth_file_count': len(generated_truth),
        'product_milestone_residue_hit_count': len(milestone_hits),
        'generated_truth_milestone_residue_hit_count': len(generated_truth_hits),
        'forbidden_annotation_hit_count': len(forbidden_hits),
        'product_milestone_residue_hits': milestone_hits,
        'generated_truth_milestone_residue_hits': generated_truth_hits,
        'forbidden_annotation_hits': forbidden_hits,
        'modified_surface_families': [
            'docs-and-runbooks',
            'generated-native-docs',
            'stdlib-contracts',
            'native-runtime-and-lowering-contracts',
            'schema-surface',
            'script-metadata-and-report-roots',
        ],
        'ok': not milestone_hits and not forbidden_hits,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    md = f"""# Product Decontamination Report

- Policy: `{policy['contract_id']}`
- Baseline product residue hits: `{inventory['product_residue_hit_count']}`
- Baseline generated-truth residue hits: `{inventory['generated_truth_residue_hit_count']}`
- Current product milestone residue hits: `{len(milestone_hits)}`
- Current generated-truth milestone residue hits: `{len(generated_truth_hits)}`
- Current forbidden annotation hits: `{len(forbidden_hits)}`
- Status: `{'PASS' if summary['ok'] else 'FAIL'}`
"""
    MD_OUT.write_text(md, encoding='utf-8')
    print(json.dumps(summary, indent=2))
    return 0 if summary['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
