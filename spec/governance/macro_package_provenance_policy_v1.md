# Macro Package Provenance Policy v1 (`C-06`)

_Normative governance artifact - 2026-02-23._

This policy defines lockfile integrity, signed metadata requirements, and
reproducibility controls for derive and macro package consumption.

This document is aligned to:

- `spec/planning/issue_168_macro_package_provenance_policy_package.md`
- `spec/governance/extension_lifecycle_v1.md`
- `spec/governance/macro_security_incident_playbook_v1.md`

## 1. Policy Outcomes and Invariants

Required outcomes:

1. package identity is verified by signed metadata and pinned digests,
2. lockfiles are deterministic and replayable,
3. integrity mismatches fail closed in CI and release paths,
4. critical failures produce deterministic incident triggers.

Non-negotiable invariants:

- unsigned metadata is invalid in enforced modes,
- digest pinning is mandatory after lockfile creation,
- implicit lockfile mutation is forbidden in CI and release modes,
- mismatch handling severity and disposition are deterministic.

## 2. Domain Model and Trust Boundaries

### 2.1 Canonical entities

| Entity | Identity key |
| --- | --- |
| `macro_package` | `(package_name, package_version, source_locator)` |
| `package_metadata` | `metadata_digest` |
| `lockfile_entry` | `(package_name, resolved_version, source_digest_set)` |
| `provenance_record` | `provenance_digest` |
| `trust_root_set` | `trust_root_set_version` |

### 2.2 Trust boundaries

| Boundary ID | Boundary | Mandatory checks |
| --- | --- | --- |
| `BND-1` | Source retrieval -> resolver | Signature verification and digest pin checks. |
| `BND-2` | Lockfile materialization -> build orchestrator | Schema compatibility, canonical ordering, and no implicit mutation check. |
| `BND-3` | Build invocation -> policy gate | Graph equivalence and trust-root version check. |
| `BND-4` | Artifact publication -> release evidence store | Provenance completeness and reproducibility gate checks. |

## 3. Signed Metadata Requirements

### 3.1 Mandatory metadata fields

| Field | Requirement |
| --- | --- |
| `meta.package_name` | Required. |
| `meta.package_version` | Required semver, exact version. |
| `meta.source_locator` | Required canonical source locator. |
| `meta.source_revision` | Required for VCS sources. |
| `meta.capability_ids` | Required non-empty list. |
| `meta.payload_digest_sha256` | Required. |
| `meta.manifest_digest_sha256` | Required. |
| `meta.build_recipe_digest_sha256` | Required when recipe content exists. |
| `meta.created_at_utc` | Required RFC3339 timestamp. |
| `meta.signer_id` | Required signer identity. |
| `meta.signing_key_id` | Required key identity. |
| `meta.signature_algorithm` | Required approved algorithm identifier. |
| `meta.signature` | Required signature payload. |

### 3.2 Signature policy

| Rule ID | Rule |
| --- | --- |
| `SIG-01` | Signature MUST verify against active `trust_root_set`. |
| `SIG-02` | Signature algorithm MUST be in approved policy set. |
| `SIG-03` | Expired or revoked keys are hard failures except explicit emergency workflow. |
| `SIG-04` | Verification outcomes MUST be emitted in provenance evidence. |

### 3.3 Multi-signature policy by lifecycle context

| Lifecycle context | Requirement |
| --- | --- |
| `stable` package consumption | Two independent trusted signatures or one trusted signature with explicit delegated-signing policy. |
| `experimental` package consumption | One trusted signature allowed, but hash pinning and provenance requirements still mandatory. |

### 3.4 Revocation and trust-root rotation

1. trust-root updates are versioned and effective-date tagged,
2. revoked keys invalidate new resolutions immediately,
3. lockfile update tooling must flag entries signed by revoked keys,
4. emergency rollover exceptions must be time-boxed and audited.

## 4. Hash Pinning Policy

### 4.1 Required digests

Each lockfile entry MUST pin:

1. `payload_digest_sha256`,
2. `metadata_digest_sha256`,
3. provenance subject digest (`sha256`) when available.

Dual-hash migration with `sha512` is optional but recommended.

### 4.2 Pinning invariants

| Rule ID | Rule |
| --- | --- |
| `PIN-01` | Digest values are exact and immutable for a resolved version. |
| `PIN-02` | Wildcards, prefix matches, and range-like digest notation are forbidden. |
| `PIN-03` | Digest derivation must be canonical and platform-stable. |
| `PIN-04` | Any digest mismatch enters `MPROV-*` failure handling. |

### 4.3 Source and mirror substitution

Substitution is allowed only when payload and metadata digests remain identical
and signature checks still pass. Locator changes require reviewed lockfile
metadata updates.

## 5. Lockfile Contract

### 5.1 Required top-level fields

| Field | Requirement |
| --- | --- |
| `lockfile_version` | Required semantic version token. |
| `policy_version` | Required policy token. |
| `generated_at_utc` | Required RFC3339 timestamp. |
| `generator_id` | Required generator identity and version. |
| `graph_digest_sha256` | Required canonical graph digest. |
| `entries` | Required non-empty array in enforced mode. |
| `trust_root_set_version` | Required trust-root version token. |
| `lockfile_signature` | Required in enforced mode. |

### 5.2 Required per-entry fields

| Field | Requirement |
| --- | --- |
| `name` | Required package name. |
| `resolved_version` | Required exact version. |
| `source_locator` | Required immutable source reference. |
| `metadata_digest_sha256` | Required metadata digest pin. |
| `payload_digest_sha256` | Required payload digest pin. |
| `signature_fingerprint` | Required signer fingerprint. |
| `dependency_edges` | Required deterministically sorted references. |
| `lifecycle_state_hint` | Required lifecycle hint for governance checks. |

### 5.3 Determinism rules

1. entries are sorted lexicographically by `name`, `resolved_version`, and
   `source_locator`,
2. canonical serialization is fixed by `lockfile_version`,
3. non-semantic ordering drift is not allowed.

### 5.4 Update modes

| Update mode | Allowed mutations | Forbidden mutations |
| --- | --- | --- |
| `refresh` | Signature, timestamp, or trust-root metadata refresh with unchanged graph and digest set. | Version changes or digest changes. |
| `patch` | Controlled compatibility-window dependency updates with explicit diff. | Unbounded upgrade or silent transitive drift. |
| `minor` | Controlled additive updates preserving compatibility profile. | Breaking schema changes. |
| `major` | Explicit schema or policy migration with migration record. | Silent compatibility fallback. |

Governance rules:

1. digest-changing mutations require explicit review rationale,
2. CI and release MUST reject implicit regeneration,
3. lockfile merges are resolved by deterministic regeneration, not manual edits.

### 5.5 Version compatibility

| Rule ID | Rule |
| --- | --- |
| `LV-01` | Unknown major versions are hard failures. |
| `LV-02` | Additive same-major fields may be ignored only if policy marks them safe. |
| `LV-03` | Missing mandatory fields for declared version are hard failures. |

## 6. Provenance and Reproducibility Policy

### 6.1 Required provenance claims

Required claims for enforced runs:

1. source repository and revision identity,
2. lockfile digest and `lockfile_version`,
3. complete resolved package digest set,
4. signer verification outcomes and trust-root version,
5. build environment fingerprint,
6. invocation identity and timestamp,
7. output artifact digests bound to lockfile digest.

### 6.2 Reproducibility levels

| Level | Definition | Minimum evidence |
| --- | --- | --- |
| `RP-0` | No reproducibility claim. | Not valid for conformance or release paths. |
| `RP-1` | Same-environment replay reproducibility. | Two-run replay with matching output digests. |
| `RP-2` | Cross-runner reproducibility in same profile class. | Independent runner replay with matching outputs and compatible provenance claims. |
| `RP-3` | Release-grade reproducibility with archival evidence. | `RP-2` plus signed reproducibility report retained by policy. |

Minimum requirements:

- CI conformance requires at least `RP-1`,
- release candidate requires at least `RP-2`,
- release evidence targets `RP-3`.

### 6.3 Reproducibility gate conditions

1. lockfile consumption is read-only in enforced mode,
2. all resolved digests match lockfile pins,
3. replay outputs match baseline outputs for the same profile,
4. deviations enter `MPROV-*` handling.

## 7. Mismatch Taxonomy (`MPROV-*`)

| Code | Trigger class | Default severity | Required disposition |
| --- | --- | --- | --- |
| `MPROV-001` | Missing signature | `high` | Hard fail and open triage ticket. |
| `MPROV-002` | Signature invalid | `critical` | Hard fail, quarantine package, immediate escalation. |
| `MPROV-003` | Signer revoked or expired | `critical` | Hard fail and revocation workflow. |
| `MPROV-004` | Metadata digest mismatch | `high` | Hard fail and lockfile investigation. |
| `MPROV-005` | Payload digest mismatch | `critical` | Hard fail, quarantine source or mirror, escalate. |
| `MPROV-006` | Lockfile schema incompatibility | `high` | Hard fail until migration. |
| `MPROV-007` | Lockfile mutation in enforced mode | `high` | Hard fail and policy violation record. |
| `MPROV-008` | Graph digest drift | `high` | Hard fail in CI and release paths. |
| `MPROV-009` | Reproducibility replay mismatch | `critical` | Hard fail and incident triage. |
| `MPROV-010` | Provenance claim gap | `high` | Hard fail until provenance repaired. |
| `MPROV-011` | Unauthorized source substitution | `high` | Hard fail and reviewed update required. |
| `MPROV-012` | Trust-root set mismatch | `high` | Hard fail until trust-root sync. |

## 8. Mode-Aware Posture

| Mode | Policy posture |
| --- | --- |
| Local exploratory mode | Hard diagnostics by default; optional unsafe override for non-release experiments with audit note. |
| CI conformance mode | All `MPROV-*` are fail-closed; no override. |
| Release mode | All `MPROV-*` are fail-closed; critical classes require incident handling. |

Operator response minimums:

1. mismatch records MUST include code, package identity, lockfile digest, and
   source revision,
2. critical mismatches MUST open incident ticket within 24 hours,
3. retried runs MUST retain original evidence references.

## 9. Enforcement Points (`EP-*`)

| Enforcement point | Required checks | Fail behavior |
| --- | --- | --- |
| `EP-1` Resolution | Lockfile compatibility, signature verification, trust-root validity. | Block resolution. |
| `EP-2` Fetch and materialization | Metadata and payload digest checks plus substitution rules. | Block use and quarantine suspect content. |
| `EP-3` Build graph realization | Graph equivalence and no implicit lockfile mutation. | Mark run noncompliant and fail enforced mode. |
| `EP-4` CI gate | Complete dependency and provenance policy checks. | Fail pipeline and block merge or release promotion. |
| `EP-5` Reproducibility gate | Replay output and provenance consistency. | Fail release readiness. |
| `EP-6` Release publication gate | Final trust and reproducibility checks for publish action. | Block publication until resolved. |

Enforcement order is fixed: `EP-1` -> `EP-2` -> `EP-3` -> `EP-4` -> `EP-5` -> `EP-6`.

## 10. Incident Tie-Ins (`IT-*` to `C-07`)

| Trigger ID | Source event | Required handoff payload |
| --- | --- | --- |
| `IT-01` | `MPROV-002`, `MPROV-003`, or `MPROV-005` | package identity, signer identity, lockfile digest, affected builds, containment action |
| `IT-02` | Repeated `MPROV-009` across independent runners | replay reports, environment fingerprints, suspect dependency set |
| `IT-03` | `MPROV-011` or `MPROV-012` | trust-root versions, source locator diff, review history |
| `IT-04` | Any override in enforced mode | override rationale, approver, expiry, affected artifacts |

Immediate containment actions:

1. quarantine suspect package tuple,
2. freeze lockfile updates touching suspect tuple,
3. mark affected artifacts as non-releaseable,
4. publish deterministic impact-scope summary.

## 11. Required Provenance Review Record Fields

| Field | Requirement |
| --- | --- |
| `provenance_review_id` | Stable immutable identifier. |
| `policy_version` | `macro_package_provenance_policy_v1`. |
| `lockfile_digest` | Digest of reviewed lockfile snapshot. |
| `trust_root_set_version` | Reviewed trust-root version. |
| `enforcement_results` | Pass or fail for `EP-1` through `EP-6`. |
| `mismatch_records` | Zero or more `MPROV-*` records with dispositions. |
| `incident_handoffs` | Any emitted `IT-*` payload references. |
| `review_owner` | Named reviewer role. |
| `review_date` | Absolute `YYYY-MM-DD`. |

## 12. Downstream Contract

| Consumer | Required output from this policy |
| --- | --- |
| `C-07` security incident playbook | Stable trigger and payload contract for `IT-01` through `IT-04`. |
| `C-11` registry publication controls | Provenance references and integrity checks for registry update publication. |
| `C-09` test obligations | Reproducibility evidence floor and fail-closed handling for integrity violations. |

Downstream consumers may add stricter checks but may not weaken `MPROV-*`,
`EP-*`, or `IT-*` behavior.
