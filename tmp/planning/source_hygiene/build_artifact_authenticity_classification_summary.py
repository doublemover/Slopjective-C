#!/usr/bin/env python3
from __future__ import annotations

import fnmatch
import json
import subprocess
from pathlib import Path
from typing import Any

ROOT = Path(r'C:/Users/sneak/Development/Slopjective-C')
POLICY_PATH = ROOT / 'tests/tooling/fixtures/source_hygiene/stable_identifier_authenticity_policy.json'
CLASS_PATH = ROOT / 'tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json'
INVENTORY_PATH = ROOT / 'tmp/reports/m315/M315-A001/residue_authenticity_inventory.json'
OUT_DIR = ROOT / 'tmp/reports/m315/M315-B003'
JSON_OUT = OUT_DIR / 'artifact_authenticity_classification_summary.json'
MD_OUT = OUT_DIR / 'artifact_authenticity_classification_summary.md'


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding='utf-8'))


def tracked_files() -> list[str]:
    result = subprocess.run(
        ['git', 'ls-files'],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding='utf-8',
        check=True,
    )
    return [line.strip().replace('\\', '/') for line in result.stdout.splitlines() if line.strip()]


def match_globs(paths: list[str], globs: list[str]) -> list[str]:
    matched = set()
    for path in paths:
        for pattern in globs:
            if fnmatch.fnmatch(path, pattern):
                matched.add(path)
                break
    return sorted(matched)


def count_ll_headers(paths: list[str]) -> int:
    count = 0
    for rel in paths:
        if not rel.endswith('.ll'):
            continue
        text = (ROOT / rel).read_text(encoding='utf-8')
        if 'fixture_family_id' in text and 'provenance_class: synthetic_fixture' in text:
            count += 1
    return count


def count_json_envelopes(paths: list[str]) -> int:
    count = 0
    for rel in paths:
        if not rel.endswith('.json'):
            continue
        try:
            payload = json.loads((ROOT / rel).read_text(encoding='utf-8'))
        except Exception:
            continue
        envelope = payload.get('artifact_authenticity') if isinstance(payload, dict) else None
        if isinstance(envelope, dict) and envelope.get('provenance_class') == 'synthetic_fixture':
            count += 1
    return count


def main() -> int:
    policy = read_json(POLICY_PATH)
    classification = read_json(CLASS_PATH)
    inventory = read_json(INVENTORY_PATH)
    paths = tracked_files()

    generated_truth_paths = classification['classes']['generated_truth']['paths']
    generated_truth_present = [path for path in generated_truth_paths if path in paths]

    synthetic_rules = classification['classes']['synthetic_fixture']['rules']
    synthetic_rule_counts = []
    total_synthetic_matches = set()
    parity_paths: list[str] = []
    for rule in synthetic_rules:
        matched = match_globs(paths, rule['path_globs'])
        total_synthetic_matches.update(matched)
        if rule['rule_id'] == 'objc3c.fixture.synthetic.librarycliparity.v1':
            parity_paths = matched
        synthetic_rule_counts.append({
            'rule_id': rule['rule_id'],
            'match_count': len(matched),
            'sample': matched[:10],
            'label_rollout_status': rule['label_rollout_status'],
        })

    archive_roots = classification['classes']['archive_reference']['roots']
    archive_matches = sorted(
        path for path in paths if any(path.startswith(root) for root in archive_roots)
    )

    parity_ll_labeled = count_ll_headers(parity_paths)
    parity_json_labeled = count_json_envelopes(parity_paths)

    summary = {
        'issue': 'M315-B003',
        'contract_id': classification['contract_id'],
        'policy_id': policy['contract_id'],
        'inventory_issue': inventory['issue'],
        'generated_truth_expected_count': len(generated_truth_paths),
        'generated_truth_present_count': len(generated_truth_present),
        'synthetic_rule_counts': synthetic_rule_counts,
        'synthetic_total_match_count': len(total_synthetic_matches),
        'archive_reference_match_count': len(archive_matches),
        'library_cli_parity_labeled_ll_count': parity_ll_labeled,
        'library_cli_parity_labeled_json_count': parity_json_labeled,
        'checks': {
            'classification_contract_linked_from_policy': policy.get('classification_contract') == 'tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json',
            'generated_truth_paths_complete': len(generated_truth_present) == len(generated_truth_paths),
            'synthetic_replay_ll_rule_matches_inventory': next((entry['match_count'] for entry in synthetic_rule_counts if entry['rule_id'] == 'objc3c.fixture.synthetic.replayll.v1'), -1) == inventory['authenticity_classes']['synthetic_or_replay_ll']['count'],
            'synthetic_test_json_rule_covers_tests_json': next((entry['match_count'] for entry in synthetic_rule_counts if entry['rule_id'] == 'objc3c.fixture.synthetic.testjson.v1'), -1) >= inventory['authenticity_classes']['tracked_test_and_conformance_json']['count'],
            'library_cli_parity_has_labeled_examples': parity_json_labeled >= 3,
            'archive_reference_rule_has_matches': len(archive_matches) >= inventory['archive_file_count'],
        },
    }
    summary['ok'] = all(summary['checks'].values())

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    MD_OUT.write_text(
        '# M315-B003 Artifact Authenticity Classification Summary\n\n'
        f"- Contract: `{classification['contract_id']}`\n"
        f"- Generated truth paths present: `{len(generated_truth_present)}/{len(generated_truth_paths)}`\n"
        f"- Synthetic total match count: `{len(total_synthetic_matches)}`\n"
        f"- Archive reference match count: `{len(archive_matches)}`\n"
        f"- library_cli_parity labeled LL count: `{parity_ll_labeled}`\n"
        f"- library_cli_parity labeled JSON count: `{parity_json_labeled}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding='utf-8',
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
