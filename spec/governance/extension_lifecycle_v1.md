# Extension Lifecycle Policy v1 (`C-05`)

_Normative governance artifact - 2026-02-23._

This policy defines lifecycle states, allowed transitions, and minimum evidence
for promotion, rollback, deprecation, and retirement decisions.

This document is aligned to:

- `spec/planning/issue_164_extension_lifecycle_states_package.md`
- `spec/governance/extension_review_rubric_v1.md`
- `spec/governance/capability_namespace_policy_v1.md`

## 1. Canonical Lifecycle States

| State ID | State | Intent | Default enablement posture | Namespace posture |
| --- | --- | --- | --- | --- |
| `LS-1` | `experimental` | Early validation with controlled blast radius. | Off in conforming mode. | Vendor or private canonical IDs only. |
| `LS-2` | `provisional` | Cross-vendor convergence with frozen core semantics. | Off by default unless explicit board waiver. | Vendor canonical ID required. |
| `LS-3` | `stable` | Normative portable behavior eligible for conformance claims. | May be enabled by default according to profile policy. | Public `objc3.meta.*` canonical ID required. |
| `LS-4` | `deprecated` | Supported compatibility path pending retirement. | Same as stable unless release policy narrows defaults. | Existing published ID retained with no reassignment. |
| `LS-5` | `retired` | Tombstone state for removed active support. | Not enableable. | Tombstone-only record; ID never reused. |

## 2. Lifecycle Invariants

| Invariant ID | Rule |
| --- | --- |
| `LI-01` | Every extension has exactly one active lifecycle state at any time. |
| `LI-02` | Any state transition requires a board decision record with effective release and date. |
| `LI-03` | Transition is invalid until mandatory artifacts are merged and published. |
| `LI-04` | Namespace restrictions from `C-02` are enforceable in every state. |
| `LI-05` | `LS-5` is terminal. No transition may originate from `retired`. |

## 3. Allowed and Disallowed Transitions

### 3.1 Allowed transitions

| Transition ID | From | To | Class |
| --- | --- | --- | --- |
| `T-01` | `LS-1` | `LS-2` | Promotion |
| `T-02` | `LS-2` | `LS-3` | Promotion |
| `T-03` | `LS-3` | `LS-4` | Deprecation |
| `T-04` | `LS-4` | `LS-5` | Retirement |
| `T-05` | `LS-2` | `LS-1` | Rollback |
| `T-06` | `LS-4` | `LS-3` | Exceptional deprecation reversal |

### 3.2 Disallowed transitions

| Disallowed path | Reason |
| --- | --- |
| `LS-1 -> LS-3` | Skips required cross-vendor convergence stage. |
| `LS-3 -> LS-1` | Violates compatibility and release predictability constraints. |
| `LS-5 -> any` | `retired` is terminal; restoration requires a new identity. |
| `LS-4 -> LS-2` | Reversal path, if approved, returns directly to `LS-3` only. |

## 4. Gate Families

| Gate ID | Gate name | Blocking rule |
| --- | --- | --- |
| `G-01` | Intake completeness gate | Missing mandatory proposal fields blocks transition review. |
| `G-02` | Rubric threshold gate | `C-04` floor or hard-fail miss blocks promotion. |
| `G-03` | Implementation maturity gate | Missing implementation maturity or deterministic replay blocks transition. |
| `G-04` | Interop and test obligations gate | `C-09` mandatory-family or threshold miss blocks advancement. |
| `G-05` | Migration and ecosystem impact gate | Missing migration plan blocks deprecation or retirement. |
| `G-06` | Governance decision gate | Transition is non-binding until quorum vote record is published. |

## 5. Transition Gate Matrix

| Transition | Required gates | Minimum threshold summary |
| --- | --- | --- |
| `T-01` | `G-01`, `G-02`, `G-03`, `G-04`, `G-06` | Rubric outcome `ACCEPT` or stronger; no hard-fail; provisional test obligations pass. |
| `T-02` | `G-01`, `G-02`, `G-03`, `G-04`, `G-05`, `G-06` | Rubric outcome `ACCEPT-STRONG`; two independent implementations; stable obligations pass. |
| `T-03` | `G-03`, `G-05`, `G-06` | Replacement path plus deprecation diagnostics and timeline required. |
| `T-04` | `G-04`, `G-05`, `G-06` | Sunset window satisfied and migration impact accepted. |
| `T-05` | `G-03`, `G-04`, `G-06` | Rollback trigger documented with containment plan. |
| `T-06` | `G-02`, `G-03`, `G-04`, `G-05`, `G-06` | Evidence equivalent to `T-02` plus approved reversal rationale. |

## 6. Minimum Evidence by Lifecycle State

| State | Required evidence bundle |
| --- | --- |
| `LS-1` | Completed intake packet, baseline syntax and semantics evidence, diagnostics evidence, determinism evidence, security evidence, namespace validity proof. |
| `LS-2` | Transition dossier, rubric `ACCEPT+`, at least one production implementation, provisional test obligations pass, initial vendor claim linkage. |
| `LS-3` | Transition dossier, rubric `ACCEPT-STRONG`, two independent implementations, cross-vendor interop evidence, stable test obligations pass, published claim references. |
| `LS-4` | Deprecation dossier, migration and replacement guidance, diagnostics with fix-its, sunset timeline, compatibility risk statement. |
| `LS-5` | Retirement dossier, proof deprecation window elapsed, ecosystem impact review, tombstone publication record. |

Freshness and replay rules:

1. promotion evidence older than 90 days requires revalidation,
2. security evidence older than 30 days requires explicit freshness note,
3. non-reproducible artifacts are treated as missing evidence.

## 7. Rollback, Deprecation, and Retirement Triggers

### 7.1 Rollback triggers

| Trigger ID | Condition | Required response window |
| --- | --- | --- |
| `RB-01` | New critical security or soundness defect in `LS-2` or `LS-3`. | Hold in 24 hours, disposition in five business days. |
| `RB-02` | Determinism or reproducibility regression beyond accepted threshold. | Disposition in five business days. |
| `RB-03` | Mandatory test obligation failures across two consecutive runs without valid infrastructure exception. | Disposition in ten business days. |
| `RB-04` | Namespace collision or invalid claim invalidates portability semantics. | Disposition in five business days. |

### 7.2 Deprecation entry triggers (`T-03`)

| Trigger ID | Condition |
| --- | --- |
| `DP-01` | Valid replacement and low-risk migration path exists. |
| `DP-02` | Current shape has unfixable or disproportionate safety or maintenance risk. |
| `DP-03` | Low adoption with disproportionate maintenance burden and bounded consumer impact. |

### 7.3 Retirement eligibility triggers (`T-04`)

| Trigger ID | Condition |
| --- | --- |
| `RM-01` | Minimum deprecation window is satisfied. |
| `RM-02` | Migration path remains valid for affected users at vote time. |
| `RM-03` | No unresolved P0 or P1 blocker tied to retirement fallout. |

## 8. Governance Touchpoints

| Touchpoint ID | Lifecycle moment | Required inputs | Required outputs |
| --- | --- | --- | --- |
| `TB-01` | Intake triage | Intake packet and trace fields | Triage disposition with blockers and due dates. |
| `TB-02` | Promotion readiness (`T-01` or `T-02`) | Transition dossier and gate results | Vote record with disposition, conditions, effective release, and effective date. |
| `TB-03` | Deprecation readiness (`T-03`) | Deprecation dossier and migration evidence | Deprecation decision with sunset start and rollback conditions. |
| `TB-04` | Retirement readiness (`T-04`) | Retirement dossier and sunset compliance evidence | Retirement decision and tombstone publication authorization. |
| `TB-05` | Emergency rollback (`RB-*`) | Incident report and containment status | Temporary hold or rollback disposition with follow-up actions. |

## 9. Required Traceability Fields

Every transition record MUST include:

| Field | Requirement |
| --- | --- |
| `trace.transition_record_id` | Stable immutable transition identifier. |
| `trace.extension_id` | Canonical extension identity. |
| `trace.current_state` | Source lifecycle state (`LS-*`). |
| `trace.target_state` | Destination lifecycle state (`LS-*`). |
| `trace.transition_id` | One of `T-01` through `T-06`. |
| `trace.gate_results` | Pass or fail state for all mandatory gates. |
| `trace.rubric_review_id` | Required for promotions and `T-06`. |
| `trace.rubric_outcome` | Required for promotions and `T-06`. |
| `trace.vendor_claim_refs` | Required for `LS-2` and above. |
| `trace.test_obligation_set_id` | Required for `LS-2` and above. |
| `trace.test_result_refs` | Required for `LS-2` and above. |
| `trace.board_vote_record` | Required for all binding transitions. |
| `trace.effective_release` | Required for approved transitions. |
| `trace.effective_date` | Required for approved transitions. |
| `trace.rollback_conditions` | Required for promotion and deprecation transitions. |
| `trace.deprecation_window_start` | Required for `T-03` and `T-04`. |
| `trace.earliest_removal_date` | Required for `T-03` and `T-04`. |

## 10. Downstream Contract

| Downstream task | Required lifecycle output |
| --- | --- |
| `C-08` vendor claim template | Lifecycle enums, transition IDs, and traceability fields. |
| `C-09` test obligations | Gate-family bindings and state-specific evidence obligations. |
| `C-12` pilot workflow | Promotion and rollback or deprecation scenarios using `TB-*` and trace fields. |

No downstream artifact may redefine `LS-*`, `T-*`, or `G-*` identifiers.
