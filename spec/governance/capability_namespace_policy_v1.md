# Capability Namespace Policy v1 (`C-02`)

_Normative governance artifact - 2026-02-23._

This policy defines canonical naming, prefix ownership, and collision behavior
for extension identifiers used in derive and macro governance.

This document is aligned to:

- `spec/planning/issue_146_capability_namespace_policy_package.md`
- `spec/governance/MACRO_DERIVE_EXTENSION_GOVERNANCE.md`
- `spec/governance/macro_derive_extension_charter_v1.md`

## 1. Policy Scope

This policy applies to extension identifiers (`extension_id`) used in:

1. extension proposals and review records,
2. vendor conformance claims,
3. registry entries and lifecycle transition records,
4. feature-test token derivation for extension enablement and diagnostics.

Out of scope:

- capability identifier namespaces not used as extension IDs,
- schema mechanics unrelated to extension ID governance.

## 2. Canonical Namespace Classes

`extension_id` MUST match one of the classes below.

| Class ID | Namespace class | Canonical format | Regex |
| --- | --- | --- | --- |
| `NS-01` | Public portable | `objc3.meta.<feature_id>` | `^objc3\.meta\.[a-z0-9]+(?:_[a-z0-9]+)*$` |
| `NS-02` | Vendor | `vendor.<vendor_id>.meta.<feature_id>` | `^vendor\.[a-z0-9]+(?:_[a-z0-9]+)*\.meta\.[a-z0-9]+(?:_[a-z0-9]+)*$` |
| `NS-03` | Private | `private.<owner_id>.meta.<feature_id>` | `^private\.[a-z0-9]+(?:_[a-z0-9]+)*\.meta\.[a-z0-9]+(?:_[a-z0-9]+)*$` |

Global identifier rules:

1. lowercase ASCII only,
2. empty segments are forbidden,
3. `meta` is a fixed segment for extension IDs,
4. semantic version is stored separately in `extension_version`,
5. published IDs are immutable and never reused.

## 3. Reserved Tokens and Prefix Ownership

Reserved top-level tokens: `objc3`, `vendor`, and `private`.

Reserved segment values that MUST NOT be used as owner or vendor IDs:

- `objc3`
- `vendor`
- `private`
- `meta`

Reserved prefix ownership table:

| Prefix | Ownership class | Allocation authority | Allowed use | Forbidden use |
| --- | --- | --- | --- | --- |
| `objc3.meta.` | Public extension namespace | Steering owners + review board | Ratified public lifecycle records and portable claims. | Vendor-only or private canonical IDs. |
| `vendor.` | Vendor extension namespace | Owning vendor with governance collision checks | Vendor extension surfaces and non-portable claims. | Portable canonical claim without promotion. |
| `private.` | Implementation-private namespace | Toolchain owner | Internal experiments and internal manifests. | Portable claims and public canonical registry rows. |
| `objc3.cap.` | Capability namespace (non-extension) | Capability governance owners | Capability IDs only. | Any `extension_id` field. |
| `objc3.gfn.` | Mangling-policy namespace (non-extension) | Mangling-policy governance owners | Mangling policy artifacts only. | Any `extension_id` field. |

## 4. Lifecycle Namespace Constraints

Namespace class and lifecycle state MUST be coherent.

| Lifecycle state | Allowed namespace classes | Forbidden namespace classes |
| --- | --- | --- |
| `experimental` | `NS-02`, `NS-03` | `NS-01` |
| `provisional` | `NS-02` canonical, optional future public pre-allocation note | `NS-01` canonical assignment |
| `stable` | `NS-01` canonical, optional vendor alias with migration note | `NS-03` canonical |
| `deprecated` | Previously published canonical namespace only | Namespace reassignment or ID reuse |
| `retired` | Tombstone-only retention of published ID | ID reuse or deletion of audit record |

## 5. Collision Detection Contract

Every new or changed extension ID MUST pass all checks below.

| Check ID | Check |
| --- | --- |
| `NS-CHECK-01` | Syntax and class validation against Section 2 regex rules. |
| `NS-CHECK-02` | Prefix ownership validation against Section 3. |
| `NS-CHECK-03` | Exact-string uniqueness check against ratified and pending IDs. |
| `NS-CHECK-04` | Duplicate-ID rejection within a single payload set. |
| `NS-CHECK-05` | Feature-test token collision check after deterministic normalization to uppercase underscore form. |
| `NS-CHECK-06` | Near-collision advisory check for confusingly similar normalized tokens. |

Normalization algorithm for `NS-CHECK-05`:

1. uppercase the full ID,
2. replace dots with underscores,
3. prefix with `__OBJC3_EXT_`,
4. suffix with `__`.

Example:

`vendor.acme.meta.fast_hash` -> `__OBJC3_EXT_VENDOR_ACME_META_FAST_HASH__`

## 6. Deterministic Failure Codes

| Failure code | Trigger | Required disposition |
| --- | --- | --- |
| `EXTNS-ID-SYNTAX` | Regex or token rule violation. | Block intake or publish action; no automatic rewrite. |
| `EXTNS-ID-PREFIX` | Prefix ownership mismatch. | Block action and require corrected namespace class. |
| `EXTNS-ID-COLLISION` | Exact identifier collision. | Reject action; require new identifier. |
| `EXTNS-MACRO-COLLISION` | Normalized feature-test token collision. | Reject publication until one identifier changes. |
| `EXTNS-ID-UNAVAILABLE` | Required ID not available in import context. | Deterministic incompatibility diagnostic. |
| `EXTNS-CLAIM-INVALID` | Namespace class inconsistent with claim scope or lifecycle state. | Claim validation fail in governance mode. |

## 7. Escalation Model

Escalations use the charter envelope from `C-01`.

| Level | Trigger | Primary owner | Response target |
| --- | --- | --- | --- |
| `E1` | Pre-ratification namespace conflict with clear local remediation path. | Lane owner | Recovery plan within two business days. |
| `E2` | Cross-vendor ownership dispute or unresolved near-collision across two review cycles. | Review-board chair -> steering owners | Tie-break session within five business days. |
| `E3` | Released collision that introduces security, soundness, or identity ambiguity risk. | Security owner + steering owner | Temporary hold within 24 hours and incident publication. |
| `E4` | Release-impacting unresolved namespace blocker. | Steering owners + release integration owner | Explicit go or no-go decision before closeout. |

## 8. Required Namespace Review Record Fields

Every namespace approval record MUST include:

| Field | Requirement |
| --- | --- |
| `namespace_review_id` | Stable immutable identifier. |
| `extension_id` | Candidate ID under review. |
| `namespace_class` | One of `NS-01`, `NS-02`, `NS-03`. |
| `lifecycle_state` | Candidate lifecycle state. |
| `check_results` | Results for `NS-CHECK-01` through `NS-CHECK-06`. |
| `normalized_token` | Deterministic token from Section 5. |
| `collision_set_refs` | Evidence references for uniqueness checks. |
| `review_disposition` | `accepted`, `deferred`, or `rejected`. |
| `review_owner` | Named owner role. |
| `review_date` | Absolute `YYYY-MM-DD`. |

## 9. Accepted and Rejected Examples

Accepted examples:

| ID | Why accepted |
| --- | --- |
| `objc3.meta.derive_codable` | Valid public namespace and segment syntax. |
| `vendor.acme.meta.fast_hash` | Valid vendor namespace and segment syntax. |
| `private.acme_ci.meta.ast_dump_debug` | Valid private namespace for internal use. |

Rejected examples:

| ID | Why rejected |
| --- | --- |
| `vendor.ACME.meta.fast_hash` | Uppercase token not allowed. |
| `vendor.acme.meta.fast-hash` | Hyphen not allowed. |
| `vendor.objc3.meta.fast_hash` | Reserved token used as vendor ID. |
| `objc3.meta.fast_hash.v1` | Version suffix in `extension_id` is forbidden. |
| `private.acme.meta.release_portable_hash` | Private namespace cannot back portable claim semantics. |

## 10. Downstream Contract

| Consumer | Required input from this policy |
| --- | --- |
| `C-08` vendor conformance claims | Namespace class validity and lifecycle compatibility checks. |
| `C-11` extension registry | Canonical ID class and collision-free publication contract. |
| `C-05` lifecycle policy | State-dependent namespace restrictions. |

Schema-level permissiveness MUST NOT override this policy. If a schema accepts an
identifier that violates Section 2 through Section 4, this policy remains
authoritative and the identifier is invalid.
