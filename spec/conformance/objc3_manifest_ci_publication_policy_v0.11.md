# objc3 Manifest CI Publication Policy v0.11

Policy ID: `MANPUB-POLICY-v0.11-B04`

## 1. Scope

This policy governs CI publication of objc3 runtime and ABI manifests for merged
builds and defines immutable naming, signing/provenance checks, and retention
assignment requirements.

## 2. Immutable Naming Contract

Immutable paths are derived from manifest digest and must never be overwritten:

- `manifests/<manifest_name>/sha256/<manifest_sha256>/manifest.json`
- `manifests/<manifest_name>/sha256/<manifest_sha256>/manifest.sig`
- `manifests/<manifest_name>/sha256/<manifest_sha256>/provenance.intoto.jsonl`
- `manifests/<manifest_name>/sha256/<manifest_sha256>/publish_record.json`

Re-publish with identical bytes is idempotent. Collisions with differing bytes
must emit `MANPUB-004` and fail publication.

## 3. Signing and Provenance Gates

Publication is allowed only after:

- detached signature generation and verification succeeds (`MANPUB-005` on fail),
- provenance includes required claims and matching subject digests
  (`MANPUB-006` on fail),
- source context is approved protected-branch merge (`MANPUB-010` on fail).

## 4. Retention and Lookup Metadata

Published index entries must contain:

- immutable object references and digest fields,
- `retention_class`, `retention_expiry_utc`, and `hold_state`,
- stable ordering key `(manifest_kind, manifest_name, source_revision, ci_run_id)`.

Missing or invalid retention assignment emits `MANPUB-009`.

## 5. Required Publication Artifacts

Required artifacts for each publish unit:

- immutable manifest,
- detached signature,
- provenance attestation,
- publish record (`reports/conformance/manifests/publish/manifest_publish_record_v0.11.json`),
- lookup index current pointer and immutable snapshot.

## 6. Verification References

Verification evidence bundle:

- `reports/conformance/manifests/publish/verification/manifest_publish_verification_bundle_2026-02-23.md`
