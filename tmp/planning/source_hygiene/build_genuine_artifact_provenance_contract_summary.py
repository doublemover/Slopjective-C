#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(r'C:/Users/sneak/Development/Slopjective-C')
POLICY_PATH = ROOT / 'tests/tooling/fixtures/source_hygiene/stable_identifier_authenticity_policy.json'
CLASS_PATH = ROOT / 'tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json'
CONTRACT_PATH = ROOT / 'tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json'
SCHEMA_PATH = ROOT / 'schemas/objc3c-artifact-authenticity-v1.schema.json'
PARITY_MANIFESTS = [
    ROOT / 'tests/tooling/fixtures/native/library_cli_parity/library/module.manifest.json',
    ROOT / 'tests/tooling/fixtures/native/library_cli_parity/cli/module.manifest.json',
    ROOT / 'tests/tooling/fixtures/native/library_cli_parity/golden_summary.json',
]
OUT_DIR = ROOT / 'tmp/reports/m315/M315-C001'
JSON_OUT = OUT_DIR / 'genuine_artifact_provenance_contract_summary.json'
MD_OUT = OUT_DIR / 'genuine_artifact_provenance_contract_summary.md'


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding='utf-8'))


def main() -> int:
    policy = read_json(POLICY_PATH)
    classification = read_json(CLASS_PATH)
    contract = read_json(CONTRACT_PATH)
    schema = read_json(SCHEMA_PATH)
    parity_envelopes = [read_json(path)['artifact_authenticity'] for path in PARITY_MANIFESTS]

    required_fields = contract['required_envelope_fields']
    schema_required = set(schema['required'])
    schema_props = set(schema['properties'])
    policy_required = set(policy['authenticity_classes']['genuine_generated_output']['required_provenance_fields'])

    checks = {
        'policy_links_classification_contract': policy.get('classification_contract') == 'tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json',
        'classification_links_genuine_contract': classification.get('genuine_provenance_contract') == 'tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json',
        'schema_id_matches_contract': schema['properties']['authenticity_schema_id']['const'] == 'objc3c.artifact.authenticity.schema.v1',
        'genuine_contract_required_fields_in_schema': set(required_fields).issubset(schema_props | schema_required),
        'genuine_contract_covers_policy_fields': policy_required.issubset(set(required_fields)),
        'schema_has_genuine_generated_output_enum': 'genuine_generated_output' in schema['properties']['provenance_class']['enum'],
        'parity_examples_use_schema_id': all(env['authenticity_schema_id'] == 'objc3c.artifact.authenticity.schema.v1' for env in parity_envelopes),
        'parity_examples_stay_synthetic': all(env['provenance_class'] == 'synthetic_fixture' for env in parity_envelopes),
    }

    summary = {
        'issue': 'M315-C001',
        'schema_path': 'schemas/objc3c-artifact-authenticity-v1.schema.json',
        'contract_path': 'tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json',
        'policy_contract': policy['contract_id'],
        'classification_contract': classification['contract_id'],
        'genuine_contract_id': contract['contract_id'],
        'schema_authenticity_id': schema['properties']['authenticity_schema_id']['const'],
        'genuine_required_fields': required_fields,
        'parity_reference_examples': [str(path.relative_to(ROOT)).replace('\\', '/') for path in PARITY_MANIFESTS],
        'checks': checks,
        'ok': all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    MD_OUT.write_text(
        '# M315-C001 Genuine Artifact Provenance Contract Summary\n\n'
        f"- Schema: `{summary['schema_path']}`\n"
        f"- Genuine contract: `{summary['genuine_contract_id']}`\n"
        f"- Required fields: `{', '.join(required_fields)}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding='utf-8',
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
