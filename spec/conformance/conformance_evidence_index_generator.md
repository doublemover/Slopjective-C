# Conformance Evidence Index Generator (Issue #123)

This document defines usage and output expectations for:

- `scripts/generate_conformance_evidence_index.py`

The generator scans evidence artifacts under `reports/conformance/` and emits
a deterministic profile/release index JSON suitable for local runs and CI.

## Output Contract

The generated JSON always includes these top-level fields:

- `schema_id` (`objc3-conformance-evidence-index/v1`)
- `index_version` (integer)
- `release_label` (string or `null`)
- `generated_at` (RFC3339 UTC string or `null`)
- `input_root` (repository-relative path)
- `artifact_count` (integer)
- `profile_count` (integer)
- `release_count` (integer)
- `artifacts` (array)
- `profiles` (array)
- `releases` (array)

Each `artifacts[]` entry always includes:

- `artifact_path`
- `file_sha256`
- `size_bytes`
- `media_type`
- `profile_id`
- `release_id`
- `artifact_id`
- `manifest_kind`
- `schema_ref`
- `source_generated_at`
- `issue_ref`

The `profiles[]` and `releases[]` sections provide bidirectional grouping:

- profile -> release -> artifact paths
- release -> profile -> artifact paths

## Determinism Rules

The generator is deterministic for fixed inputs and flags:

- scan order is stable (sorted normalized repository-relative paths),
- artifact records are sorted by `profile_id`, `release_id`, then path,
- grouping arrays are emitted in sorted order,
- output path is automatically excluded from scan when it is under
  `--input-root`,
- no interactive prompts are used.

If `--generated-at` is not provided, `SOURCE_DATE_EPOCH` is used when set;
otherwise `generated_at` is emitted as `null`.

## CLI Usage

Show help:

```powershell
python scripts/generate_conformance_evidence_index.py --help
```

Generate the checked-in sample:

```powershell
python scripts/generate_conformance_evidence_index.py `
  --output reports/conformance/evidence-index.v0.11.sample.json `
  --release-label v0.11 `
  --generated-at 2026-02-23T00:00:00Z
```

CI-friendly run using `SOURCE_DATE_EPOCH`:

```powershell
$env:SOURCE_DATE_EPOCH = "1767139200"
python scripts/generate_conformance_evidence_index.py `
  --output reports/conformance/evidence-index.v0.11.json `
  --release-label v0.11
```

## Exit Behavior

- exits `0` on success,
- exits non-zero for invalid arguments or invalid scan configuration
  (for example, missing input root or no matched artifacts without
  `--allow-empty`).
