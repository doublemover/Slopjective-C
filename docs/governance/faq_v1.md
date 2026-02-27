# Extension Governance FAQ v1 (`C-13`)

Each FAQ entry includes: `faq_id`, applicability, authoritative references, owner role, and verification date.

## FAQ-C13-01 Getting Started and Eligibility

- `faq_id`: `FAQ-C13-01-Q01`
- Question: Who can submit an extension proposal?
- Answer: A named proposal sponsor with accountable owners can submit. The intake packet must satisfy the `C-03` template contract before triage.
- Applicability: `author`
- References: `templates/experimental_extension_proposal.md`; `spec/planning/issue_152_extension_proposal_intake_template_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: If ownership is unclear, escalate through Lane C governance owners before intake submission.

## FAQ-C13-02 Proposal Template and Evidence Fields

- `faq_id`: `FAQ-C13-02-Q01`
- Question: What evidence domains are mandatory in the proposal template?
- Answer: Syntax, semantics, diagnostics, determinism, and security evidence are all required for intake readiness.
- Applicability: `author`
- References: `templates/experimental_extension_proposal.md`; `spec/planning/issue_152_extension_proposal_intake_template_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: If a required domain cannot be filled, file a defer request with owner/date and blocking reason.

## FAQ-C13-03 Lifecycle States and Promotion/Deprecation Paths

- `faq_id`: `FAQ-C13-03-Q01`
- Question: Which lifecycle states are used for extension governance?
- Answer: Extensions follow `LS-*` lifecycle states and `T-*` transition rules. Transition records must include traceable state and decision metadata.
- Applicability: `all`
- References: `spec/planning/issue_164_extension_lifecycle_states_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: State-transition conflicts require review-board clarification before publication.

## FAQ-C13-04 Test Obligations and Evidence Thresholds

- `faq_id`: `FAQ-C13-04-Q01`
- Question: What testing evidence is required for conformance claims?
- Answer: Claims must cite required test IDs, outcomes, environment fingerprints, immutable logs, and any approved waiver references.
- Applicability: `vendor`
- References: `templates/vendor_extension_conformance_claim.md`; `spec/planning/issue_175_extension_test_obligations_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: Hard-threshold failures are non-waiverable and block approval.

## FAQ-C13-05 Review Board Workflow and Decisions

- `faq_id`: `FAQ-C13-05-Q01`
- Question: How are final pilot and lifecycle decisions published?
- Answer: Decisions require quorum-compliant vote metadata, gate summaries, and publication outputs according to board SLA rules.
- Applicability: `reviewer`
- References: `spec/planning/issue_170_review_board_operating_model_package.md`; `spec/planning/issue_180_extension_pilots_workflow_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: Missing quorum or missing publication bundle blocks binding closure.

## FAQ-C13-06 Registry Publication and Update Behavior

- `faq_id`: `FAQ-C13-06-Q01`
- Question: When can a claim be published in the extension registry?
- Answer: Publication follows accepted decision linkage and schema-valid registry payloads. Deferred/held outcomes must include explicit owner/date rationale.
- Applicability: `vendor`
- References: `registries/experimental_extensions/index.schema.json`; `spec/planning/issue_176_extension_registry_format_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: Schema mismatches must be remediated before release publication.

## FAQ-C13-07 Security/Provenance Incident Handling

- `faq_id`: `FAQ-C13-07-Q01`
- Question: What happens if provenance evidence fails verification?
- Answer: The claim is held and incident handling is triggered; the affected extension cannot proceed until containment and revalidation complete.
- Applicability: `vendor`
- References: `templates/vendor_extension_conformance_claim.md`; `spec/planning/issue_174_security_provenance_incident_workflow_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: Route unresolved security incidents through security response owners and steering escalation.

## FAQ-C13-08 Ownership, Escalation, and Maintenance Cadence

- `faq_id`: `FAQ-C13-08-Q01`
- Question: Who maintains onboarding and FAQ documentation?
- Answer: `C-LEAD` owns updates with `D-BACKUP` as backup; response target is `SLA-2BD` for documentation drift and stale-reference issues.
- Applicability: `all`
- References: `docs/governance/extension_author_guide_v1.md`; `spec/planning/issue_159_future_work_ownership_matrix_package.md`; `spec/planning/issue_138_governance_charter_package.md`
- `last_verified_date`: `2026-02-23`
- `owner_role`: `C-LEAD`
- Escalation note: If SLA is missed twice consecutively, escalate via Lane C governance leadership.
