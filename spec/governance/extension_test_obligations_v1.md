# Extension Test Obligations Policy v1 (`C-09`)

_Normative governance artifact - 2026-02-23._

This policy defines mandatory interop and conformance test obligations required
before lifecycle promotions and for steady-state lifecycle compliance.

This document is aligned to:

- `spec/planning/issue_175_extension_test_obligations_package.md`
- `spec/governance/extension_lifecycle_v1.md`
- `reports/conformance/extension_test_catalog_skeleton_v011.md`

## 1. Scope

This policy governs:

1. lifecycle-state family obligations for `LS-1` through `LS-5`,
2. quantitative failure thresholds and promotion blockers,
3. waiver controls and non-waiverable conditions,
4. evidence completeness and freshness obligations,
5. review gate model for governance decision intake.

## 2. Requirement Levels and Failure Classes

### 2.1 Requirement levels

| Level | Meaning |
| --- | --- |
| `M` | Mandatory. Miss blocks promotion or state retention. |
| `C` | Conditionally mandatory when activation predicate is true. |
| `R` | Required for deprecation or retirement compliance checks. |
| `I` | Informational monitoring only. |
| `N` | Not applicable. |

### 2.2 Failure severity classes

| Class | Meaning | Default disposition |
| --- | --- | --- |
| `P0` | Critical safety, soundness, security, or interop break. | Immediate hold; promotion forbidden. |
| `P1` | High-impact conformance violation with portability risk. | Promotion forbidden pending remediation evidence. |
| `P2` | Moderate issue with bounded blast radius. | Waiver-governed conditional handling only where allowed. |
| `P3` | Low-impact issue. | Track and remediate within budget. |

## 3. Canonical Test Families

| Family ID | Family name | Purpose |
| --- | --- | --- |
| `F-01` | Syntax and parser conformance | Accepted and rejected syntax surface plus parse determinism. |
| `F-02` | Static semantics and type soundness | Soundness and invalid-program rejection. |
| `F-03` | Diagnostics and fix-it behavior | Deterministic diagnostic identity and fix-it output. |
| `F-04` | ABI and metadata interop | Cross-toolchain ABI and metadata compatibility. |
| `F-05` | Runtime behavioral interop | Runtime semantics across producer and consumer implementations. |
| `F-06` | Cross-module and package interop | Import and export behavior across dependency boundaries. |
| `F-07` | Security and isolation conformance | Sandbox policy, unsafe-surface rejection, and security posture. |
| `F-08` | Reproducibility and determinism replay | Stable outputs across repeated runs. |
| `F-09` | Migration and deprecation compatibility | Compatibility behavior for migration and deprecation paths. |
| `F-10` | Claim-to-test traceability | Mapping between claim scope and required test IDs. |

## 4. Lifecycle Obligation Matrix

| Lifecycle state | `F-01` | `F-02` | `F-03` | `F-04` | `F-05` | `F-06` | `F-07` | `F-08` | `F-09` | `F-10` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `LS-1` | `M` | `M` | `M` | `C` | `C` | `C` | `M` | `M` | `N` | `M` |
| `LS-2` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `C` | `M` |
| `LS-3` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `M` |
| `LS-4` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `M` | `R` | `M` |
| `LS-5` | `N` | `N` | `R` | `R` | `R` | `R` | `R` | `R` | `R` | `R` |

Conditional activation predicates:

1. `F-04` is mandatory when extension output crosses binary or metadata
   boundaries,
2. `F-05` is mandatory when runtime behavior is externally observable,
3. `F-06` is mandatory when behavior crosses module or package boundaries,
4. `F-09` is mandatory in `LS-2` when migration or fallback claims are used in
   promotion rationale.

## 5. Interop Coverage Minima

| Context | Minimum vendor coverage | Minimum platform coverage | Minimum profile coverage |
| --- | --- | --- | --- |
| `LS-1` steady state | One implementation plus one reference consumer harness when interop families are active. | One primary platform tuple. | Profiles claimed by proposal. |
| `T-01` (`LS-1 -> LS-2`) | Two independent implementations or one implementation plus one independent verifier harness. | At least two platform tuples. | All declared claim profiles. |
| `LS-2` steady state | Two independent implementations. | At least two platform tuples. | All declared profiles plus one strict profile where applicable. |
| `T-02` (`LS-2 -> LS-3`) | Two independent implementations with bidirectional producer and consumer execution. | At least three platform tuples including one non-primary environment. | All claimed profiles in strictest applicable mode. |
| `LS-3` steady state | Same as `T-02`. | Same as `T-02`. | Same as `T-02`. |

## 6. Threshold Metrics and Promotion Blocking

### 6.1 Metrics

| Metric | Definition |
| --- | --- |
| `required_pass_rate` | Percent of mandatory and active conditional tests that pass. |
| `blocker_count` | Count of unresolved `P0` and `P1` failures in mandatory families. |
| `flake_budget` | Maximum unstable outcomes across mandatory tests. |
| `rerun_consistency` | Required count of consecutive clean reruns. |
| `interop_completeness` | Coverage percentage for required interop matrix cells. |

### 6.2 State thresholds

| State | Required pass rate | Blocker limit | Flake budget | Rerun consistency | Interop completeness |
| --- | --- | --- | --- | --- | --- |
| `LS-1` | `>=95.0%` | No unresolved `P0`; up to one unresolved `P1` only with approved hold. | `<=3.0%` | At least one clean run. | `>=80%` for active interop families. |
| `LS-2` | `>=98.5%` | No unresolved `P0` or `P1`. | `<=1.0%` | At least two clean runs. | `100%` of required cells. |
| `LS-3` | `100%` mandatory pass and `>=99.5%` overall including informational runs. | No unresolved `P0` or `P1`. | `<=0.3%` | At least three clean runs. | `100%` of required cells. |
| `LS-4` | `100%` for `M` and `R` obligations. | No unresolved `P0` or `P1`. | `<=0.5%` | At least two clean runs. | `100%` of required cells. |
| `LS-5` | `100%` for `R` obligations. | No unresolved `P0` or `P1`. | `<=0.5%` | At least one clean run. | `100%` of required cells. |

### 6.3 Promotion thresholds

| Transition | Required threshold profile | Hard blockers |
| --- | --- | --- |
| `T-01` | Must satisfy all `LS-2` thresholds and complete all mandatory provisional families. | Any unresolved `P0` or `P1`, missing interop evidence, expired waiver, stale evidence bundle. |
| `T-02` | Must satisfy all `LS-3` thresholds including three clean reruns and bidirectional interop evidence. | Any unresolved `P0` or `P1`, waiver on safety or portability families, missing strict-profile evidence, missing claim traceability. |

Threshold breach behavior:

1. any hard blocker sets transition disposition to `HOLD`,
2. breaches across two consecutive review cycles force remediation-only status,
3. promotion re-entry requires full threshold-compliant rerun set.

## 7. Waiver Policy

### 7.1 Allowed waiver classes

| Waiver class | Allowed use | Maximum validity |
| --- | --- | --- |
| `W-01` Infra incident | Temporary CI or environment outage. | 14 days |
| `W-02` External dependency defect | Third-party regression with upstream reference and containment. | 30 days |
| `W-03` Platform exception | Non-primary platform defect with explicit scope boundary. | 30 days |
| `W-04` Evidence freshness exception | Short evidence-age extension before board session. | 7 days |

### 7.2 Non-waiverable conditions

Waivers are forbidden for:

1. unresolved `P0` or `P1` in `F-02`, `F-04`, `F-05`, or `F-07`,
2. missing interop evidence required by Section 5,
3. missing or contradictory `F-10` claim traceability,
4. missing provenance integrity evidence required by policy.

### 7.3 Waiver record minimum fields

| Field | Requirement |
| --- | --- |
| `waiver_id` | Stable `WV-YYYY-NNN` token. |
| `waiver_class` | `W-01` through `W-04`. |
| `affected_test_ids` | Explicit list of impacted tests. |
| `non_waiverable_check` | Explicit pass assertion. |
| `owner` | Named mitigation owner. |
| `due_date` | Absolute remediation due date. |
| `expiry_date` | Absolute waiver expiry date. |
| `approval_record` | Approver identities and decision reference. |

## 8. Evidence Requirements

### 8.1 Mandatory evidence bundle

| Evidence class | Requirement |
| --- | --- |
| Test result ledger | Test IDs, outcomes, timestamps, and severity classes. |
| Interop matrix record | Producer and consumer pair coverage with complete matrix cells. |
| Environment fingerprint | Toolchain versions, profile flags, platform tuple, dependency lock references. |
| Raw logs and summary | Stable IDs linking normalized summary to raw logs. |
| Claim linkage manifest | Complete mapping from claims to required test IDs. |
| Provenance and replay metadata | Source revision, invocation identity, artifact digests, replay evidence. |

### 8.2 Freshness limits

| Evidence type | Maximum age at review time |
| --- | --- |
| Promotion test results (`T-01`) | 21 days |
| Promotion test results (`T-02`) | 14 days |
| Security and isolation evidence (`F-07`) | 7 days |
| Interop matrix evidence | 14 days |
| Provenance replay evidence | 30 days |

Stale evidence blocks promotion unless unexpired `W-04` waiver exists.

### 8.3 Completeness checks

Evidence is complete only if:

1. every mandatory and active conditional family has executed test IDs,
2. every required interop cell has outcome and immutable log reference,
3. every failure has severity classification and disposition,
4. every waiver-affected test has active waiver linkage,
5. no orphan claim or orphan test row exists in claim-to-test mapping.

## 9. Review Gates (`TG-*`)

| Gate ID | Gate name | Blocking condition |
| --- | --- | --- |
| `TG-01` | Catalog coverage gate | Missing mandatory family rows or missing conditional activation rows. |
| `TG-02` | Threshold gate | Any metric below required threshold. |
| `TG-03` | Interop completeness gate | Missing interop matrix cell or missing direction coverage. |
| `TG-04` | Evidence integrity gate | Stale or non-immutable evidence, or unresolved provenance gap. |
| `TG-05` | Waiver governance gate | Prohibited, expired, or improperly approved waiver. |
| `TG-06` | Claim traceability gate | Unmapped claim scope or unmapped required test rows. |
| `TG-07` | Decision readiness gate | Any required gate not in pass state. |

Transition bindings:

| Context | Mandatory gates |
| --- | --- |
| `LS-1` steady-state check | `TG-01`, `TG-02`, `TG-04`, `TG-06` |
| `T-01` | `TG-01`, `TG-02`, `TG-03`, `TG-04`, `TG-05`, `TG-06`, `TG-07` |
| `LS-2` steady-state check | `TG-01`, `TG-02`, `TG-03`, `TG-04`, `TG-05`, `TG-06` |
| `T-02` | `TG-01`, `TG-02`, `TG-03`, `TG-04`, `TG-05`, `TG-06`, `TG-07` |
| `LS-3` steady-state check | `TG-01`, `TG-02`, `TG-03`, `TG-04`, `TG-06` |
| `T-03` and `T-04` | `TG-01`, `TG-02`, `TG-03`, `TG-04`, `TG-06` plus `F-09` and `R` obligations |

`TG-07` cannot be waived for promotion decisions.

## 10. Companion Test Catalog Contract

Companion artifact path:

- `reports/conformance/extension_test_catalog_skeleton_v011.md`

Each catalog row MUST include:

| Field | Requirement |
| --- | --- |
| `test_id` | Stable ID referenced by claims and review records. |
| `family_id` | One of `F-01` through `F-10`. |
| `lifecycle_min_state` | Earliest lifecycle state where row is active. |
| `condition_expression` | Activation predicate for conditional families. |
| `severity_if_fail` | Default severity class. |
| `required_profiles` | Profiles where row must run. |
| `interop_dimensions` | Required producer and consumer dimensions when interop applies. |
| `evidence_artifact_type` | Required evidence classes for row. |
| `waiver_eligibility` | Allowed waiver classes or `none`. |
| `gate_bindings` | One or more gate IDs consuming row. |

Consistency rules:

1. `F-01` through `F-10` all have at least one row,
2. conditional families define explicit non-`always` predicate when required,
3. each row maps to at least one gate,
4. non-waiverable families align with Section 7.2.

## 11. Required Test Obligation Review Record Fields

| Field | Requirement |
| --- | --- |
| `obligation_review_id` | Stable immutable identifier. |
| `extension_id` | Reviewed extension identity. |
| `lifecycle_context` | Lifecycle state or transition under review. |
| `threshold_metrics` | Computed values from Section 6.1. |
| `family_status` | Status per family `F-01` through `F-10`. |
| `gate_status` | Status per gate `TG-01` through `TG-07`. |
| `waiver_refs` | Active waivers with expiry and approval references. |
| `evidence_refs` | Immutable references for mandatory evidence bundle. |
| `disposition` | `pass`, `conditional-pass`, `fail`, or `hold`. |
| `review_owner` | Named owner role. |
| `review_date` | Absolute `YYYY-MM-DD`. |

## 12. Downstream Contract

| Consumer | Required output from this policy |
| --- | --- |
| `C-05` lifecycle policy | Gate and threshold inputs for promotion and retention decisions. |
| `C-08` vendor claim template | Claim-to-test traceability contract (`F-10`, `TG-06`). |
| `C-11` registry publication | Evidence references that prove test readiness for lifecycle state and claim scope. |

No consumer may relax mandatory-family obligations, threshold floors, or
non-waiverable rules defined in this policy.
