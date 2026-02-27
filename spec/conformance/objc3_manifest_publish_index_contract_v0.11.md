# objc3 Manifest Publish Index Contract v0.11

## 1. Contract Purpose

Define deterministic schema and ordering rules for publish lookup metadata used
by CI publication policy `MANPUB-POLICY-v0.11-B04`.

## 2. Index Paths

- Mutable pointer: `publish/index/current.json`
- Immutable snapshots: `publish/index/snapshots/<source_revision>_<run_id>.json`

## 3. Required Entry Fields

Each index entry must include all fields below:

- `manifest_kind`
- `manifest_name`
- `profile_id`
- `source_revision`
- `ci_workflow`
- `ci_run_id`
- `ci_run_attempt`
- `manifest_sha256`
- `signature_sha256`
- `provenance_sha256`
- `manifest_path`
- `signature_path`
- `provenance_path`
- `publish_record_path`
- `publish_policy_version`
- `published_at_utc`
- `retention_class`
- `retention_expiry_utc`
- `hold_state`

## 4. Deterministic Ordering Rule

Entries are sorted by tuple:

`(manifest_kind, manifest_name, source_revision, ci_run_id)`

Equivalent replay runs must produce byte-equivalent ordering for equal input.
Ordering drift emits `MANPUB-011`.

## 5. Validation Targets

- `reports/conformance/manifests/publish/index/current.json`
- `reports/conformance/manifests/publish/index/snapshots/main_20260223_7001.json`
- `reports/conformance/manifests/publish/verification/manifest_publish_verification_bundle_2026-02-23.md`
