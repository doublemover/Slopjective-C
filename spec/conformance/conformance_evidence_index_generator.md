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

## Public Workflow Usage

The release-evidence gate owns index generation through the canonical npm
bridge:

```powershell
npm run objc3c -- check-release-evidence
```

The implementation helper is `scripts/generate_conformance_evidence_index.py`.
Its stable input/output contract remains:

- output: the release-specific conformance evidence index selected by the
  release tooling,
- release label: `v0.11`,
- generated timestamp: explicit RFC3339 UTC value or `SOURCE_DATE_EPOCH`.

The public helper remains a stable script entrypoint, while the implementation
is split by owner responsibility under `scripts/conformance_evidence_index/`:

- `cli.py`: argument parsing, repository path resolution, output writing.
- `manifest.py`: manifest/profile/release inference and strict
  `generated_at` handling.
- `builder.py`: artifact record construction plus profile/release index and
  replay envelope rendering.
- `paths.py`: deterministic repository path, glob, media type, hashing, and
  JSON-loading helpers.
- `timestamps.py`: RFC3339 and `SOURCE_DATE_EPOCH` timestamp canonicalization.
- `model.py`: typed artifact record payload shape.

## Exit Behavior

- exits `0` on success,
- exits non-zero for invalid arguments or invalid scan configuration
  (for example, missing input root or no matched artifacts without
  `--allow-empty`).
