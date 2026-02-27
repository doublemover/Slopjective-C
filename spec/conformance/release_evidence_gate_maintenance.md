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
  - `reports/conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json`

It also verifies that a freshly generated evidence index references all required
artifact payloads.

## Local Runbook

```sh
python scripts/check_release_evidence.py
```

Or:

```sh
npm run check:release-evidence
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

1. add/adjust schema and sample payload files,
2. update `REQUIRED_SCHEMA_DATA_PAIRS` in
   `scripts/check_release_evidence.py`,
3. rerun local gate and ensure CI workflow passes,
4. update this maintenance doc and related conformance docs in the same batch.
