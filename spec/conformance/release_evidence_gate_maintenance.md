# Release Evidence Gate Maintenance (Issue #125)

This document describes maintenance and troubleshooting for the v0.11
release-evidence gate.

## Gate Components

- Validator script: `scripts/check_release_evidence.py`
- Index generator dependency: `scripts/generate_conformance_evidence_index.py`
- CI workflow: `.github/workflows/conformance-evidence-gate.yml`

## Validated Inputs

The gate requires these schema/data pairs:

- `schemas/objc3-runtime-2025Q4.manifest.schema.json`
  - `reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json`
- `schemas/objc3-abi-2025Q4.schema.json`
  - `reports/conformance/manifests/objc3-abi-2025Q4.example.json`
- `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
  - conformance evidence bundle example selected by the release gate

The schema paths above are registry-owned by
`scripts/objc3c_shared/schema_registry.py`. This maintenance document records
the release-evidence data pairs only; it must not duplicate JSON Schema
fragments or introduce alternate schema aliases.

It also verifies that a freshly generated evidence index references all required
artifact payloads.

## Local Runbook

```sh
npm run objc3c -- check-release-evidence
```

## Failure Diagnostics

The validator emits actionable errors with `release-evidence:` prefix:

- missing required file,
- invalid JSON parse,
- schema validation failure (path + message),
- index generation failure,
- generated index missing required keys,
- generated index missing required artifact references.

## Update Policy

When introducing new release-evidence artifacts:

1. add or adjust schema and sample payload files,
2. register any new checked-in schema in
   `scripts/objc3c_shared/schema_registry.py`,
3. update `REQUIRED_SCHEMA_DATA_PAIRS` in
   `scripts/check_release_evidence.py`,
4. rerun `npm run objc3c -- check-release-evidence` and ensure CI workflow passes,
5. update this maintenance doc and related conformance docs in the same batch.
