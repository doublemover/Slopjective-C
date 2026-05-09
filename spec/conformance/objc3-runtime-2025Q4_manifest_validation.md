# objc3-runtime-2025Q4 Manifest Validation (Issue #115)

This document defines the required fields and validation flow for the runtime artifact manifest used by `v0.11-A01`.

Schema file: `schemas/objc3-runtime-2025Q4.manifest.schema.json`.
Registry owner: `scripts/objc3c_shared/schema_registry.py`.

## Required fields

A valid manifest must include these top-level fields:

- `schema_id` (must be `objc3-runtime-artifact-manifest/v1`)
- `artifact_id` (must be `objc3-runtime-2025Q4`)
- `release_stamp` (must be `2025Q4`)
- `manifest_version` (must be integer `1`)
- `generated_at` (UTC RFC3339 timestamp ending with `Z`)
- `producer` (`name`, `version`, `build_id`)
- `target` (`runtime_family`, `os`, `arch`, `abi`)
- `normative_reference` (`id`, `canonical_ref`, `date_stamp`, `commit_or_artifact_hash`)
- `artifacts` (non-empty array of artifact objects)
- `invariants` (`deterministic_ordering`, `reproducible`, `source_date_epoch`)
- `capabilities` (non-empty, unique capability IDs)

## Key constraints

- Digest values must use `sha256:` plus 64 lowercase hex characters.
- `target.runtime_family` is fixed to `objc3`.
- `normative_reference.id` is fixed to `NR-OBJC-RUNTIME`.
- `deterministic_ordering` is required and fixed to `true`.
- Unknown keys are rejected at all object levels (`additionalProperties: false`).

## Validation command

```powershell
npm run objc3c -- check-release-evidence
```

If validation fails, the release-evidence gate reports the failing schema path
and rule.
