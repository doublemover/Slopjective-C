# Extension Author and Vendor Onboarding Guide v1 (`C-13`)

## 1. Purpose

This guide defines the operational onboarding path for extension authors and vendor implementers from proposal intake to registry publication.

Primary references:
- `templates/experimental_extension_proposal.md` (`C-03` intake contract)
- `templates/vendor_extension_conformance_claim.md` (`C-08` declaration contract)
- `registries/experimental_extensions/index.schema.json` (`C-11` publication format)
- `spec/planning/issue_170_review_board_operating_model_package.md` (`C-10` decision publication model)

## 2. End-to-End Workflow (`ONB-01`..`ONB-09`)

| Stage | Owner | Required input | Required output |
| --- | --- | --- | --- |
| `ONB-01` Preflight and scope declaration | Proposal sponsor | Extension intent, target lifecycle state, named owners | Approved scope and dependency checklist (`C-03`, `C-05`, `C-09`, `C-10`, `C-11`) |
| `ONB-02` Proposal intake packet authoring | Sponsor + vendor lead | `templates/experimental_extension_proposal.md` | Intake-ready proposal with syntax/semantics/diagnostics/determinism/security evidence |
| `ONB-03` Intake validation and triage | Lane C owner + board delegate | Completed intake packet | Triage disposition: `ready`, `defer`, or `reject` with blockers |
| `ONB-04` Implementation and evidence maturation | Vendor implementation owner | Accepted intake and test obligations | Evidence bundle with test coverage, reproducibility, and risk notes |
| `ONB-05` Vendor conformance claim preparation | Vendor declaration owner | `templates/vendor_extension_conformance_claim.md` and test/provenance evidence | Review-ready claim packet with capability IDs, test IDs, provenance data |
| `ONB-06` Lifecycle transition review | Review board quorum | Transition dossier + gate outcomes | Published decision record with vote/disposition metadata |
| `ONB-07` Registry publication handoff | Tooling/release owner | Approved decision + claim packet | Registry update request or explicit defer/hold note |
| `ONB-08` Docs and FAQ publication update | `C-LEAD` | Decision + registry publication refs | Updated onboarding and FAQ docs |
| `ONB-09` Post-publication verification | `C-LEAD` + `D-BACKUP` | Published docs and link check outputs | Verification record with stale-link/taxonomy checks |

## 3. Artifact Checklist by Role

### 3.1 Proposal sponsors

1. Create proposal from `templates/experimental_extension_proposal.md`.
2. Fill all required metadata and evidence domains.
3. Include traceability fields for task/issue/dependency/test linkage.

### 3.2 Vendor declaration owners

1. Create claim from `templates/vendor_extension_conformance_claim.md`.
2. Map every claimed capability to implementation and negative-case evidence.
3. Provide immutable test-log and provenance digest references.

### 3.3 Tooling/release owners

1. Validate registry payload shape against `registries/experimental_extensions/index.schema.json`.
2. Link decision publication artifacts and claim references in release notes.
3. Record defer/hold rationale when publication is delayed.

## 4. Review and Decision Expectations

- Intake packets are blocked if required sections are missing or non-falsifiable.
- Claims are blocked when required tests are absent, stale, or lack immutable logs.
- Lifecycle transitions must include transition IDs, gate outcomes, and decision linkage.
- Publication artifacts must preserve deterministic references and explicit owner/date follow-ups.

## 5. Quality and Freshness Rules

- Keep onboarding/FAQ references aligned with current template and registry paths.
- Re-verify board cadence and escalation references at least every 30 days.
- Re-verify security/provenance guidance at least every 14 days.

## 6. Ownership and Escalation

| Responsibility | Primary owner | Backup owner | SLA |
| --- | --- | --- | --- |
| Onboarding guide accuracy | `C-LEAD` | `D-BACKUP` | `SLA-2BD` |
| FAQ freshness and taxonomy coverage | `C-LEAD` | `D-BACKUP` | `SLA-2BD` |
| Cross-link integrity | `C-LEAD` + tooling delegate | `D-BACKUP` | `SLA-2BD` |

Escalation path:
1. Open documentation issue with impacted stage/category IDs.
2. Assign primary and backup owners.
3. Publish remediation plan within 5 business days if SLA is missed.

## 7. Last Verified

- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- `backup_owner_role`: `D-BACKUP`
