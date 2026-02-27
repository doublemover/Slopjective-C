# Objective-C 3 Conformance Evidence Bundle Schema (Issue #122)

Scope: `v0.11-B01` (`#122`).
Schema file: `schemas/objc3-conformance-evidence-bundle-v1.schema.json`.
Sample payload: `reports/conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json`.

## Required top-level keys

- `bundle_type` (constant: `objc3-conformance-evidence-bundle`)
- `bundle_schema` (constant: `objc3-conformance-evidence-bundle-v1`)
- `bundle_version` (SemVer)
- `issue_ref` (constant: `v0.11-B01/#122`)
- `generated_at` (UTC RFC3339 timestamp ending with `Z`)
- `producer` (`name`, `version`, `build_id`)
- `spec_baseline` (`language_line`, `release_train`, checklist ref, minimum suite version)
- `profile_claim` (profile, strictness, concurrency mode, claim status, optional feature sets, capability IDs)
- `manifests` (must contain both `abi-manifest` and `runtime-manifest` records)
- `test_evidence` (runner metadata, summary, and suite evidence)
- `known_deviations` (structured deviation records)
- `invariants` (`deterministic_ordering`, `normalized_paths`, `digest_algorithm`)

## Key enums and validation rules

- `profile_claim.profile`: `core-v1`, `strict-v1`, `strict-concurrency-v1`, `strict-system-v1`.
- `profile_claim.status`: `conformant`, `conformant-with-deviations`, `non-conformant`.
- `test_evidence.suites[].suite_id` must cover all five required buckets:
  `parser`, `semantic`, `lowering_abi`, `module_roundtrip`, `diagnostics`.
- `known_deviations[].status`: `accepted-temporary`, `accepted-permanent`, `open`.
- `known_deviations[].severity`: `low`, `medium`, `high`, `critical`.
- All object layers reject unknown keys (`additionalProperties: false`).
- All digest fields are normalized as `sha256:<64 lowercase hex>`.
- Cross-field rules enforced by schema:
  - `conformant` => zero known deviations and summary result `pass`.
  - `conformant-with-deviations` => at least one known deviation and summary result `pass-with-known-deviations`.
  - `non-conformant` => summary result `fail`.

## Validation command

```powershell
python -c "import json, pathlib, jsonschema; s=json.loads(pathlib.Path('schemas/objc3-conformance-evidence-bundle-v1.schema.json').read_text(encoding='utf-8-sig')); d=json.loads(pathlib.Path('reports/conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json').read_text(encoding='utf-8-sig')); jsonschema.Draft202012Validator(s).validate(d); print('valid: objc3 conformance evidence bundle')"
```
