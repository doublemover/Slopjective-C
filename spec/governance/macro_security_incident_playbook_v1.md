# Macro Security Incident Playbook v1 {#macro-security-incident-playbook-v1}

_Status: normative governance artifact for `C-07` and `V013-GOV-04` (v1.2, 2026-02-23)._

This playbook is the binding governance specification for security incidents
affecting macro/derive packages. It defines mandatory `SEV-*` classification,
`CER-*` response workflow, disclosure policy, communication templates,
`MPROV-*`/`IT-*` provenance linkage, and simulation obligations.

## 1. Purpose and Governance Scope

This playbook applies to all macro package compromise events in source,
signing, distribution, and consumption paths.

Normative requirements:

- every incident MUST be assigned an `INC-*` class and `SEV-*` severity before
  closure decisions are made,
- every `SEV-1` and `SEV-2` event MUST execute full `CER-C`, `CER-E`, and
  `CER-R` phases with explicit gate approvals,
- every external disclosure MUST include affected coordinates, mitigation
  actions, and next-update timing,
- every closure record MUST preserve provenance linkage (`MPROV-*`, `IT-*`,
  lockfile/provenance references) as audit evidence.

## 2. Severity Taxonomy

### 2.1 Incident class taxonomy (`INC-*`)

| Incident class | Definition | Typical trigger examples |
| --- | --- | --- |
| `INC-SIGN` | Signing compromise, key misuse, or fraudulent signer identity. | `MPROV-002`, `MPROV-003`, signer key theft signal |
| `INC-PAYLOAD` | Package payload tampering or malicious content insertion. | `MPROV-005`, mirror tamper evidence |
| `INC-SOURCE` | Unauthorized source substitution or repository takeover. | `MPROV-011`, source integrity alert |
| `INC-PROV` | Provenance evidence forgery, omission, or replay mismatch with security implications. | `MPROV-009`, `MPROV-010` |
| `INC-TRUST` | Trust-root mismatch, revocation propagation failure, or policy bypass attempt. | `MPROV-012`, stale trust bundle |
| `INC-COORD` | Coordinated multi-vendor event requiring synchronized containment and disclosure. | Shared ecosystem compromise |

### 2.2 Severity model (`SEV-*`)

| Severity | Definition | Initial response SLA |
| --- | --- | --- |
| `SEV-1` Critical | Active or highly probable exploitation with release-impacting or ecosystem-wide risk. | Acknowledge <= 15 minutes; containment start <= 30 minutes. |
| `SEV-2` High | Confirmed compromise with constrained but material impact. | Acknowledge <= 30 minutes; containment start <= 60 minutes. |
| `SEV-3` Medium | Suspected compromise or integrity break without confirmed active exploitation. | Acknowledge <= 4 hours; containment start <= 8 hours. |
| `SEV-4` Low | Security-relevant anomaly with low confidence/impact requiring audit trail. | Acknowledge <= 1 business day; triage complete <= 2 business days. |

### 2.3 Severity assignment algorithm

Severity MUST be selected using exploitability confidence, blast radius, and
artifact exposure.

Decision rules:

1. assign `SEV-1` when exploitability is `high` and exposure includes
   `released`,
2. assign `SEV-2` when compromise is confirmed with constrained scope or
   non-released exposure,
3. assign `SEV-3` when compromise is unconfirmed but integrity controls fail,
4. assign `SEV-4` when signal confidence and impact are low and existing hard
   blocks mitigate spread.

## 3. Containment, Eradication, and Recovery (CER) Workflow

### 3.1 CER phase contract

| Phase | Entry condition | Exit condition | Required outputs |
| --- | --- | --- | --- |
| `CER-C` Containment | Incident accepted with `SEV-*` assigned. | Further spread blocked and impact scope bounded. | denylist/quarantine state, freeze list, ownership assignments |
| `CER-E` Eradication | Containment controls are active and scope is understood. | Root cause removed and compromised artifacts replaced or invalidated. | remediation records, trust/signer update records, clean lineage evidence |
| `CER-R` Recovery | Eradication evidence approved. | Release and CI flow restored under verified-safe state. | recovery validation report, release-unblock decision |

### 3.2 Containment controls by severity

| Control | `SEV-1` | `SEV-2` | `SEV-3` | `SEV-4` |
| --- | --- | --- | --- | --- |
| Denylist compromised package coordinates | Immediate | Immediate | As triage confirms | Optional |
| Freeze lockfile updates touching impacted packages | Immediate | Immediate | Case-by-case | Optional |
| Block release publication gates | Immediate | If release surface is affected | If uncertainty remains | Not required |
| Revoke suspect signer or trust path | Immediate when signer/trust is implicated | Immediate when signer/trust is implicated | Pending evidence | Not required |
| Notify maintainers/vendors | <= 1 hour | <= 4 hours | <= 1 business day | <= 2 business days |

Containment is complete only when all conditions hold:

1. no new enforced-mode resolutions for compromised coordinates,
2. all affected CI/release jobs are deterministically blocked or safe,
3. scope and confidence level are recorded in the incident record.

### 3.3 Eradication requirements (`CER-E`)

Required eradication actions:

- revoke or rotate compromised signer/trust material and publish updates,
- remove compromised package versions from resolvable surfaces,
- publish remediated package lineage with signed metadata and digests,
- provide lockfile migration guidance for impacted consumers,
- verify no unresolved malicious payload remains in mirrors/caches.

Eradication is complete only when:

1. root cause hypothesis is evidence-backed,
2. compromised trust/material is removed or invalidated across scope,
3. replacement guidance is published for impacted users,
4. security owner approval is recorded.

### 3.4 Recovery requirements (`CER-R`)

Required recovery actions:

- execute controlled re-enable of CI/release gates,
- run reproducibility replay checks for remediated package sets,
- verify provenance completeness for remediated builds,
- publish recovery advisory with required user actions.

Recovery is complete only when:

1. no active containment-only override remains without owner and expiry,
2. release paths pass required policy checks with remediated artifacts,
3. IC and security owner approve closure candidacy.

### 3.5 CER gate approvals

| Gate | Decision | Required approvers |
| --- | --- | --- |
| `G-C1` | Containment declared effective | IC + tooling owner |
| `G-E1` | Eradication plan approved | IC + security owner + provenance owner |
| `G-E2` | Eradication complete | Security owner + governance delegate |
| `G-R1` | Recovery start approved | IC + release owner |
| `G-R2` | Incident closure approved | IC + security owner + program owner |

## 4. Disclosure and Notification Timeline Policy

### 4.1 Disclosure tracks

| Track | Use case | Default confidentiality |
| --- | --- | --- |
| `DT-PRIVATE` | Active investigation with uncertain impact or active exploit risk. | Internal and need-to-know vendor channels only. |
| `DT-COORD` | Confirmed issue requiring coordinated fix rollout. | Controlled multi-party disclosure with embargo date. |
| `DT-PUBLIC` | Immediate user action is required. | Public advisory once safe containment guidance exists. |

### 4.2 Timeline SLAs by severity

| Milestone | `SEV-1` | `SEV-2` | `SEV-3` | `SEV-4` |
| --- | --- | --- | --- | --- |
| Internal incident start notice | <= 30 minutes | <= 1 hour | <= 8 hours | <= 1 business day |
| Vendor/maintainer notification | <= 1 hour | <= 4 hours | <= 1 business day | <= 2 business days |
| Preliminary external statement (if user risk exists) | <= 4 hours | <= 1 business day | <= 3 business days | As needed |
| Full advisory with mitigation instructions | <= 24 hours | <= 2 business days | <= 5 business days | Optional |
| Post-incident report publication target | <= 5 business days from closure | <= 7 business days | <= 10 business days | <= 15 business days |

### 4.3 Mandatory disclosure content

Every external statement MUST include:

- affected package coordinates and version ranges,
- impact statement and exploitation confidence,
- mitigation/remediation actions,
- trust/provenance update references when applicable,
- timeline commitment for the next update.

## 5. Communication Templates

### 5.1 `COMM-T1` internal incident-start advisory

```text
Subject: [INCIDENT START][{incident_id}][{severity}] Macro package security event

Summary:
- Incident ID: {incident_id}
- Class/Severity: {incident_class} / {severity}
- First observed (UTC): {first_observed_utc}
- Intake source: {source_signal}

Current scope:
- Affected package(s): {package_coordinates}
- Affected lockfile/provenance refs: {lockfile_or_provenance_refs}
- Current user/release impact: {impact_statement}

Immediate actions:
- Containment actions in progress: {containment_actions}
- Owner roster: {incident_commander}, {security_owner}, {tooling_owner}
- Next update ETA (UTC): {next_update_utc}
```

### 5.2 `COMM-T2` vendor maintainer coordination notice

```text
Subject: [COORDINATION REQUIRED][{incident_id}] Compromised macro package workflow action

We have identified a security incident affecting:
- Package coordinates: {package_coordinates}
- Suspected compromise type: {incident_class}
- Severity: {severity}

Required maintainer actions by {deadline_utc}:
1. Acknowledge receipt and assign response owner.
2. Freeze publication for listed coordinates.
3. Confirm signing key status and trust-chain integrity.
4. Provide remediation timeline and replacement version plan.

Evidence references:
- Trigger/mismatch IDs: {mismatch_codes}
- Provenance refs: {provenance_refs}
- Contact channel: {coordination_channel}
```

### 5.3 `COMM-T3` public advisory

```text
Title: Security Advisory - Compromised macro package {package_coordinates}

Summary:
{plain_language_summary}

Affected versions:
- {affected_version_range}

Risk:
- Severity: {severity}
- Exploitation status: {exploitation_status}

Required actions:
1. {action_1}
2. {action_2}
3. {action_3}

Verification:
- Safe replacement versions: {replacement_versions}
- Trust/provenance update reference: {policy_reference}

Next update:
- Expected by {next_update_utc}
```

### 5.4 `COMM-T4` incident closure and recovery notice

```text
Subject: [INCIDENT CLOSED][{incident_id}] Macro package security incident closeout

Closure summary:
- Root cause: {root_cause}
- Compromised surface removed: {eradication_summary}
- Recovery verification: {recovery_verification_summary}

Policy/gov updates:
- Trust-root/signing updates: {trust_updates}
- Lockfile/provenance guidance: {consumer_guidance}

Residual risk:
- {residual_risk_statement}

Postmortem:
- Report path: {postmortem_reference}
- Publication date (UTC): {postmortem_publish_utc}
```

## 6. Provenance Linkage (`C-06` to `C-07`)

### 6.1 Intake handoff contract (`MPROV-*` and `IT-*`)

Incident intake MUST preserve:

- `MPROV-*` mismatch code and raw diagnostics,
- package tuple `(name, version, source)` and source locator,
- lockfile digest and trust-root set version,
- provenance record digest and replay evidence references,
- enforcement mode (`local`, `ci`, `release`) and failing gate.

### 6.2 Trigger mapping

| Trigger ID | Upstream policy signal | Default incident class | Default severity floor | Required initial actions |
| --- | --- | --- | --- | --- |
| `IT-01` | `MPROV-002`, `MPROV-003`, `MPROV-005` | `INC-SIGN` or `INC-PAYLOAD` | `SEV-2` | Quarantine package, freeze release gate, notify security owner |
| `IT-02` | Repeated `MPROV-009` across independent runners | `INC-PROV` | `SEV-2` | Freeze candidate release, begin forensics, open coordinated track |
| `IT-03` | `MPROV-011`, `MPROV-012` | `INC-SOURCE` or `INC-TRUST` | `SEV-2` | Block source substitution path, verify trust-root sync, notify maintainers |
| `IT-04` | Any enforced-mode policy override | `INC-COORD` | `SEV-3` | Open audit incident, validate override legitimacy, enforce expiry |

### 6.3 Recovery and closure provenance gates

Recovery and closure MUST be blocked until:

1. remediated builds pass provenance completeness checks,
2. replay checks show no unresolved `MPROV-009` drift,
3. trust-root and signer state aligns with declared policy version,
4. closure advisory cites updated lockfile/provenance operator guidance.

## 7. Simulation and Readiness Obligations

### 7.1 Required simulation set (`SIM-C07-*`)

| Simulation ID | Scenario | Minimum cadence | Required success result |
| --- | --- | --- | --- |
| `SIM-C07-01` | Signing key compromise with forged metadata (`INC-SIGN`) | Quarterly | Correct severity (`>= SEV-2`), containment within SLA, communications emitted |
| `SIM-C07-02` | Payload tamper in mirrored package (`INC-PAYLOAD`) | Quarterly | Digest mismatch quarantines package and blocks release gates |
| `SIM-C07-03` | Source substitution plus stale trust roots (`INC-SOURCE`/`INC-TRUST`) | Semiannual | `IT-03` mapping executes and trust remediation plan is approved |
| `SIM-C07-04` | Coordinated multi-vendor incident (`INC-COORD`) | Semiannual | Coordinated disclosure schedule is met and vendor comms are complete |
| `SIM-C07-05` | Recovery regression drill with replay mismatch (`INC-PROV`) | Quarterly | Recovery gate remains blocked until replay checks pass |

### 7.2 Required simulation evidence artifacts

Each simulation run MUST capture:

- timeline trace with SLA deltas,
- CER phase entry/exit decisions and approvers,
- communication template outputs (`COMM-T1` through `COMM-T4`, as applicable),
- provenance linkage evidence (`MPROV-*`, `IT-*`, lockfile/provenance refs),
- corrective actions with owner and due date.

### 7.3 Readiness quality gates

Readiness is passing only when all conditions hold:

1. no failed mandatory simulation in the two most recent scheduled cycles,
2. critical corrective actions are closed or explicitly time-boxed with owner,
3. latest `SEV-1` and `SEV-2` drills meet disclosure SLAs,
4. no unresolved break exists in `C-06` to `C-07` trigger handoff coverage.

## 8. Evidence, Retention, and Audit Obligations

### 8.1 Required incident evidence bundle

Every incident record MUST include:

- intake artifacts and severity rationale,
- containment/eradication/recovery actions with timestamps,
- forensic data (hashes, signatures, provenance outputs),
- communication history (`COMM-T1` through `COMM-T4` as applicable),
- closure memo and residual-risk statement.

### 8.2 Retention and immutability policy

- `SEV-1` and `SEV-2` evidence MUST be retained for at least four release cycles,
- `SEV-3` and `SEV-4` evidence MUST be retained for at least two release cycles,
- incident evidence objects MUST be append-only and integrity-protected,
- postmortem links MUST remain stable and auditable.

### 8.3 Audit linkage requirements

Audit evidence MUST maintain bidirectional references to:

- `spec/planning/issue_174_macro_security_incident_playbook_package.md`
  Sections `14` through `18`,
- this playbook Sections `2` through `8`,
- all associated `E174-*`, `AC-174-*`, and `SPT-0366` through `SPT-0374`
  identifiers used for closeout.

## 9. C-07 Acceptance Mapping and Traceability

| Acceptance / task group | Playbook coverage | Package coverage | Audit output |
| --- | --- | --- | --- |
| `AC-174-01` through `AC-174-04` (`SEV` + `CER`) | Sections `2` and `3` | Package Sections `4`, `6`, `14` | `E174-02`, `E174-03` |
| `AC-174-05`, `AC-174-06` (disclosure + templates) | Sections `4` and `5` | Package Sections `7`, `8`, `14` | `E174-04` |
| `AC-174-07` (`MPROV`/`IT` linkage) | Section `6` | Package Sections `9`, `14` | `E174-05` |
| `AC-174-08` (simulation obligations) | Section `7` | Package Sections `12`, `14` | `E174-06` |
| `AC-174-09`, `AC-174-10` (done mapping + validation) | Sections `8`, `9` | Package Sections `13`, `18` | `E174-07`, `E174-08` |
| `SPT-0366` through `SPT-0374` closeout track | Sections `2` through `9` | Package Sections `16`, `17`, `19` | Deterministic checklist + evidence ledger |

## 10. V013 Tabletop Scenario Matrix (Deterministic Addendum)

### 10.1 Scenario matrix contract (`SMT-V013-*`)

The following scenario matrix is mandatory for quarterly tabletop execution and
policy drift checks:

| Scenario ID | Incident class | Primary trigger(s) | Severity floor | Required response tier | Mandatory outputs |
| --- | --- | --- | --- | --- | --- |
| `SMT-V013-01` | `INC-SIGN` | `MPROV-002`, `MPROV-003` | `SEV-1` | `RT-V013-T1` | Immediate denylist, signer revocation, `COMM-T1` and `COMM-T2` within SLA |
| `SMT-V013-02` | `INC-PAYLOAD` | `MPROV-005`, `IT-01` | `SEV-2` | `RT-V013-T2` | Quarantine package coordinates, lockfile freeze, release gate block |
| `SMT-V013-03` | `INC-SOURCE` + `INC-TRUST` | `MPROV-011`, `MPROV-012`, `IT-03` | `SEV-2` | `RT-V013-T2` | Source-path block, trust-root refresh, maintainer notification |
| `SMT-V013-04` | `INC-PROV` | `MPROV-009`, `IT-02` | `SEV-2` | `RT-V013-T2` | Recovery hold, replay forensics, updated guidance before unblock |
| `SMT-V013-05` | `INC-COORD` | Coordinated vendor signal + `IT-04` | `SEV-1` | `RT-V013-T1` | Coordinated disclosure plan, vendor owner roster, deterministic update cadence |

### 10.2 Scenario pass and failure rules

1. each scenario run MUST use the matrix-defined severity floor and response tier,
2. missing mandatory outputs produces an automatic scenario failure,
3. scenario records MUST include CER gate decisions and timestamped SLA deltas,
4. failed scenarios MUST produce at least one `FRL-V013-*` remediation row.

## 11. Response Tier Policy (`RT-V013-*`)

### 11.1 Tier definitions

| Tier ID | Bound severity | Initial SLA window | Mandatory controls | Required evidence |
| --- | --- | --- | --- | --- |
| `RT-V013-T1` | `SEV-1` | Acknowledge <= 15 minutes; containment <= 30 minutes | Release hard-stop, denylist, signer/trust revocation, coordinated vendor paging | `COMM-T1`, `COMM-T2`, `G-C1` approval record |
| `RT-V013-T2` | `SEV-2` | Acknowledge <= 30 minutes; containment <= 60 minutes | Quarantine, lockfile freeze, targeted trust updates, forensic owner assignment | CER timeline, remediation plan, disclosure track record |
| `RT-V013-T3` | `SEV-3` | Acknowledge <= 4 hours; containment <= 8 hours | Conditional release hold, replay checks, confidence updates | triage memo, containment status record |
| `RT-V013-T4` | `SEV-4` | Acknowledge <= 1 business day; triage <= 2 business days | Audit-only incident with escalation watch conditions | audit trail and closure rationale |

### 11.2 Tier transition and override rules

1. tier selection MUST be derived from assigned `SEV-*`,
2. tier override requires IC plus security owner approval and explicit rationale,
3. tier downgrade MUST NOT occur before `CER-C` completion criteria are met,
4. all tiers inherit incident logging, ownership, and evidence bundle obligations.

### 11.3 CER gates by tier

| Tier | Gate requirements |
| --- | --- |
| `RT-V013-T1` | `G-C1`, `G-E1`, `G-E2`, `G-R1`, and `G-R2` are all mandatory; no waiver allowed. |
| `RT-V013-T2` | `G-C1`, `G-E1`, `G-E2`, and `G-R2` are mandatory; `G-R1` may combine with `G-E2` when explicitly approved. |
| `RT-V013-T3` | `G-C1` and `G-R2` are mandatory; `G-E1`/`G-E2` required when compromise is confirmed. |
| `RT-V013-T4` | `G-C1` may be replaced by documented audit hold decision; `G-R2` required for closure. |

## 12. Follow-Up Remediation Ledger and V013 Acceptance Mapping

### 12.1 Remediation ledger contract (`FRL-V013-*`)

All tabletop-driven follow-up actions MUST be tracked in an append-only
remediation ledger where each row contains:

- `remediation_id`,
- `linked_scenario_id`,
- `owner`,
- `due_utc`,
- `status` (`open`, `in_progress`, `closed`, `waived`),
- `closure_evidence`.

### 12.2 Published remediation baseline

| Remediation ID | Linked scenario | Action | Owner | Due (UTC) | Status |
| --- | --- | --- | --- | --- | --- |
| `FRL-V013-01` | `SMT-V013-01` | Rotate signer keys and publish trust bundle pinning guidance. | security owner | `2026-03-02T18:00:00Z` | `in_progress` |
| `FRL-V013-02` | `SMT-V013-02` | Add deterministic mirror quarantine replay runbook for containment. | tooling owner | `2026-03-04T18:00:00Z` | `open` |
| `FRL-V013-03` | `SMT-V013-03` | Enforce trust-root freshness guardrail for release runners. | release owner | `2026-03-06T18:00:00Z` | `open` |
| `FRL-V013-04` | `SMT-V013-04` | Add replay mismatch stop condition to recovery checklist. | provenance owner | `2026-03-03T18:00:00Z` | `in_progress` |
| `FRL-V013-05` | `SMT-V013-05` | Publish coordinated vendor contact rota with fallback owners. | governance delegate | `2026-03-05T18:00:00Z` | `open` |

### 12.3 Acceptance mapping for `AC-V013-GOV-04`

| Acceptance row | Playbook evidence |
| --- | --- |
| `AC-V013-GOV-04-01` | Section `10.1` publishes deterministic scenario matrix rows `SMT-V013-01` through `SMT-V013-05`. |
| `AC-V013-GOV-04-02` | Section `11.1` defines deterministic response tiers `RT-V013-T1` through `RT-V013-T4` bound to `SEV-*`. |
| `AC-V013-GOV-04-03` | Sections `10` through `12` codify tabletop outputs as normative controls. |
| `AC-V013-GOV-04-04` | Sections `12.1` and `12.2` define and publish follow-up remediation ledger requirements and baseline rows. |
| `AC-V013-GOV-04-05` | Section `10.2` requires scenario execution outcomes and failure handling linkage to report artifacts. |
| `AC-V013-GOV-04-06` | Validation transcript requirement is satisfied in planning/report artifacts tied to this playbook addendum. |

### 12.4 Reseed metadata binding (`#790`, `BATCH-20260223-11S`)

The `V013-GOV-04` reseed execution MUST preserve the same issue metadata tuple
across planning, governance, and reporting artifacts.

| Field | Bound value |
| --- | --- |
| `seed_id` | `V013-GOV-04` |
| `wave_id` | `W1` |
| `issue_id` | `#790` |
| `batch_id` | `BATCH-20260223-11S` |
| `acceptance_gate_id` | `AC-V013-GOV-04` |
