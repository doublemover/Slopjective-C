# Macro/Derive Extension Charter v1 (`C-01`)

_Normative governance artifact - 2026-02-23._

This charter defines the authority, decision model, and publication obligations
for cross-vendor derive and macro extension governance. It is the canonical
`C-01` artifact and governs the operation of `C-02`, `C-04`, `C-05`, `C-06`,
`C-09`, and `C-11`.

This document is aligned to:

- `spec/planning/issue_138_governance_charter_package.md`
- `spec/governance/MACRO_DERIVE_EXTENSION_GOVERNANCE.md`

## 1. Purpose and Scope

This charter is binding for:

1. governance role ownership and accountability,
2. lifecycle decision authority and vote thresholds,
3. required outputs for intake, transition, and publication events,
4. review cadence, escalation behavior, and records required for audit replay.

Out of scope:

- implementation details for specific extension features,
- schema-level encoding details for any single registry implementation.

## 2. Governance Roles and Accountability

| Role ID | Role | Core accountability | Decision rights |
| --- | --- | --- | --- |
| `GR-01` | Steering owners | Governance baseline ownership and tie-break authority. | Final ratification, emergency hold/release confirmation, exception approval. |
| `GR-02` | Cross-vendor review board | Technical review and decision recommendations. | Lifecycle transition approval or rejection under quorum rules. |
| `GR-03` | Spec editors | Normative text coherence and cross-spec consistency. | Publication block on unresolved normative conflict. |
| `GR-04` | Conformance/tooling owners | Evidence quality and reproducibility assurance. | Transition block when mandatory evidence is missing or non-reproducible. |
| `GR-05` | Security response owner | Supply-chain and security risk triage. | Emergency escalation trigger and temporary policy hold action. |

Minimum representation requirements:

1. review-board votes require at least three voting members from at least two
   independent vendor organizations,
2. at least one spec editor is required for ratification cycles,
3. at least one tooling owner is required for transition reviews.

## 3. Decision Classes and Thresholds

| Decision class | Decision ID | Required authority | Threshold |
| --- | --- | --- | --- |
| Charter ratification or charter update | `DC-01` | `GR-01` + `GR-02` | Quorum plus two-thirds yes, and no unresolved steering objection. |
| Lifecycle transition decision (`Experimental`, `Provisional`, `Stable`, `Deprecated`, `Retired`) | `DC-02` | `GR-02` | Quorum plus simple majority; blockers must be resolved or explicitly waived. |
| Process or timing waiver (non-behavioral) | `DC-03` | `GR-01` | Two steering approvals plus recorded rationale and expiry. |
| Emergency hold | `DC-04` | `GR-05` + one steering owner | Immediate temporary hold plus steering confirmation within 24 hours. |

No decision is binding until the required record in Section 8 is published.

## 4. Quorum and Conflict-of-Interest Rules

| Rule ID | Rule |
| --- | --- |
| `QR-01` | Formal quorum is met only when at least three voting members are present. |
| `QR-02` | At least two independent vendor organizations must be represented in voting members. |
| `QR-03` | A proposal author may participate in technical discussion but is non-voting for the final disposition on that proposal. |
| `QR-04` | Any decision made without quorum is non-binding and must be re-run in a valid session. |

## 5. Required Lifecycle Outputs

| Event ID | Lifecycle event | Required output | Required owner | Exit rule |
| --- | --- | --- | --- | --- |
| `LO-01` | Proposal intake | Completed proposal packet with required evidence domains. | Proposal sponsor + lane owner | Intake completeness gate passes. |
| `LO-02` | Intake triage | Triage record (`accept`, `defer`, `reject`) with due dates. | Review-board chair | Triage outcome is published and linked. |
| `LO-03` | Transition request | Transition dossier with rubric, test, implementation, and compatibility evidence. | Proposal sponsor + tooling owner | All mandatory gate inputs exist. |
| `LO-04` | Transition decision | Board decision record with vote tally, rationale, conditions, effective release, and effective date. | Review-board chair + spec editor | Decision record is published and immutable. |
| `LO-05` | Post-decision publication | Registry and consumer updates with release-note linkage. | Tooling owner + release owner | Artifacts are published and reproducible. |
| `LO-06` | Deprecation or retirement tracking | Sunset and migration status record. | Steering owners + release owner | Minimum deprecation window and migration obligations are satisfied. |

Any transition event MUST include:

1. normative text delta references,
2. deterministic test and reproducibility evidence,
3. migration or compatibility impact statement,
4. explicit unresolved-risk statement.

## 6. Cadence and Publication Windows

| Cadence ID | Meeting/output | Required participants | Target SLA |
| --- | --- | --- | --- |
| `CD-01` | Weekly intake triage | Review-board chair + lane owner | New proposals triaged within five business days. |
| `CD-02` | Biweekly formal board session | Voting board + spec editor + tooling owner | Decision or explicit defer rationale per in-review proposal. |
| `CD-03` | Monthly governance status snapshot | Steering owner + release owner + tooling owner | Snapshot published on first business day each month. |
| `CD-04` | Quarterly process audit | Steering owners + review board | Audit publication with open waivers and escalation trends. |

## 7. Escalation Model

Escalation levels are fixed and map to `issue_138` governance behavior.

| Level | Trigger | Owner | Required response |
| --- | --- | --- | --- |
| `E1` | Process delay with no immediate safety risk. | Lane owner | Recovery plan within two business days. |
| `E2` | Deadlocked decision or unresolved blocker after two sessions. | Review-board chair -> steering owners | Tie-break session scheduled within five business days. |
| `E3` | Security or soundness risk in active transition. | Security owner + steering owner | Temporary hold within 24 hours and incident plan publication. |
| `E4` | Release-impacting unresolved governance item. | Steering owners + release integration owner | Explicit go or no-go disposition before milestone closure. |

Escalation records MUST include owner, date, trigger, and closure status.

## 8. Required Decision Record Fields

Each binding decision record MUST include all fields below.

| Field | Requirement |
| --- | --- |
| `decision_id` | Stable immutable identifier. |
| `decision_class` | One of `DC-01` through `DC-04`. |
| `proposal_or_transition_ref` | Identifier for the governed item under review. |
| `quorum_attestation` | Explicit statement of `QR-01` through `QR-04` pass state. |
| `vote_tally` | Machine-readable tally including abstain and recusal counts. |
| `disposition` | `accepted`, `conditional_accept`, `deferred`, `rejected`, or `hold`. |
| `conditions` | Explicit owner and due date for any conditional disposition. |
| `effective_release` | Required for accepted lifecycle transitions. |
| `effective_date` | Required for accepted lifecycle transitions. |
| `artifact_refs` | Immutable links to required evidence and downstream publication artifacts. |

## 9. Downstream Governance Contract

This charter provides mandatory process constraints to downstream artifacts.

| Downstream task | Required charter output |
| --- | --- |
| `C-02` namespace policy | Prefix ownership, escalation envelope (`E1` to `E4`), and publication record requirements. |
| `C-04` review rubric | Deterministic tie-break and threshold authority envelope. |
| `C-05` lifecycle policy | Decision classes, quorum rules, and required publication obligations. |
| `C-06` provenance policy | Escalation ownership and incident handoff authority chain. |
| `C-09` test obligations | Board intake and transition publication rules. |
| `C-11` registry format | Decision-link publication and immutable record obligations. |

No downstream policy may weaken quorum, conflict, or publication constraints
defined in this charter without a `DC-01` charter update decision.
