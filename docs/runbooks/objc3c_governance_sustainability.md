# objc3c Governance And Sustainability

This runbook freezes the governance baseline.

Canonical checked-in governance inventory contract:

- `tests/tooling/fixtures/governance_sustainability/budget_inventory.json`

Replayable inventory generator:

- `npm run objc3c -- validate-governance-sustainability`

Governance focus:

- measure the live repo shape before setting or tightening budgets
- keep budget and anti-noise claims tied to existing enforcement anchors
- treat exceptions and drift as explicit recorded objects instead of tribal knowledge
- ground extension and support-impact governance in package, adoption, support,
  release, distribution, and security evidence that already exists

Sustainable progress policy:

- every governed surface must map to one canonical checked-in owner and one replayable measurement path
- budget increases are allowed only with a measured delta, a recorded reason, and a follow-on ratchet path
- new automation should extend existing runner, hygiene, and source-surface checks before adding new command names
- governance claims stay narrower than the evidence; unresolved drifts must appear in reports, waivers, or release-blocking status instead of being silently tolerated

Exception model:

- exceptions are checked-in waiver objects, not ad hoc comments
- each waiver must name:
  - governed surface
  - owner
  - reason
  - evidence paths
  - opened timestamp
  - expiry timestamp
  - current status
- expired waivers are release-blocking until removed or superseded
- budget increases without a corresponding checked-in policy update and measured evidence are invalid

Canonical checked-in policy contracts:

- `tests/tooling/fixtures/governance_sustainability/sustainable_progress_policy.json`
- `tests/tooling/fixtures/governance_sustainability/waiver_registry.json`

Replayable policy summary:

- `npm run objc3c -- validate-governance-sustainability`

Extension, RFC, and support-impact review policy:

- `tests/tooling/fixtures/governance_sustainability/extension_review_policy.json`
- `tests/tooling/fixtures/governance_sustainability/new_work_proposal_template.json`
- public validation: `npm run objc3c -- validate-governance-sustainability`
- publication: `npm run objc3c -- publish-governance-sustainability`

Extension review is allowed to advance only when the proposal names its
language surface, review class, support-impact classification, evidence
dependencies, package/release impact, adoption impact, and revert or
demotion path. Support-impacting changes must consume release operations,
long-horizon support, package ecosystem, adoption-legibility, and security
hardening evidence instead of standing on prose-only review.

Maintainer, contributor, and package stewardship semantics:

- `tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json`
- `npm run objc3c -- validate-governance-sustainability`

Stewardship review keeps normal contributors on `CONTRIBUTING.md`, maintainers
on `docs/runbooks/objc3c_maintainer_workflows.md`, and package-governance
changes on the package ecosystem contracts. Any governance change that widens
the public package bridge, public workflow actions, package metadata, or
support claims must run the maintainer review checks and either stay within
budget or carry a checked-in waiver.

Machine-owned governance schema surface:

- `tests/tooling/fixtures/governance_sustainability/schema_surface.json`
- `schemas/objc3c-governance-budget-summary-v1.schema.json`
- `schemas/objc3c-governance-anti-regression-summary-v1.schema.json`
- `schemas/objc3c-governance-sustainability-evidence-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`
- `npm run objc3c -- validate-governance-sustainability`

This runbook cites registry-backed schema files only; it must not duplicate
governance JSON Schema fragments or turn generated governance summaries into
support claims.

Machine-owned governance artifact contract:

- `tests/tooling/fixtures/governance_sustainability/artifact_contract.json`
- `npm run objc3c -- validate-governance-sustainability`
- canonical evidence artifact: `tmp/artifacts/governance-sustainability/governance-sustainability-evidence.json`
- canonical publication artifacts:
  - `tmp/artifacts/governance-sustainability/stewardship-publication.json`
  - `tmp/artifacts/governance-sustainability/extension-review-publication.json`

The checked-in contracts under `tests/tooling/fixtures/governance_sustainability/`
are the owner inputs. Files under `tmp/reports/` and `tmp/artifacts/` are
replayable outputs only and must never become implementation inputs.

Replayable governance enforcement:

- public package command: `npm run objc3c -- validate-governance-sustainability`
- public package command: `npm run objc3c -- publish-governance-sustainability`
- helper implementations are action-registry anchors, not
  public command surface
- canonical enforcement summary: `tmp/reports/governance-sustainability/budget-enforcement/governance_budget_enforcement_summary.json`
- canonical integration summary: `tmp/reports/governance-sustainability/integration/governance_sustainability_integration_summary.json`
- canonical evidence artifact: `tmp/artifacts/governance-sustainability/governance-sustainability-evidence.json`
- canonical publication summary: `tmp/reports/governance-sustainability/publication-summary.json`

Long-horizon anti-regression reporting:

- `npm run objc3c -- validate-governance-sustainability`
- canonical history artifact: `tmp/artifacts/governance-sustainability/anti-regression-history.json`
- canonical anti-regression summary: `tmp/reports/governance-sustainability/anti-regression/governance_anti_regression_summary.json`

Closeout gate:

- `npm run objc3c -- validate-governance-sustainability`
- canonical closeout summary: `tmp/reports/governance-sustainability/closeout-gate/governance_sustainability_closeout_gate.json`

Current governance entry surfaces:

- `npm run objc3c -- check-task-hygiene`
- `npm run objc3c -- check-repo-superclean-surface`
- `npm run objc3c -- check-documentation-surface`
- `npm run objc3c -- check-dependency-boundaries`
- `docs/runbooks/objc3c_maintainer_workflows.md`
- `docs/runbooks/objc3c_public_command_surface.md`
- package bridge: `npm run objc3c -- <action>`

Current budget surfaces measured by the governance inventory summary:

- public package bridge count and category mix
- public workflow action count
- checked-in runbook count
- checked-in schema count
- live `check_*.py` script count
- must-remain-absent repo roots and workflow patterns
- known live governance drifts that later issues must ratchet:
  - none for numeric milestone checkers, live Python bytecode, or live `__pycache__` directories

Explicit non-goals:

- replacing the package, release, security, support, or adoption evidence paths
- treating governance docs as a substitute for executable review evidence
- promising hosted community infrastructure or registry moderation services
- widening public workflow surface before the policy and schema issues land

Generated evidence:

- `tmp/reports/governance-sustainability/budget-inventory/governance_budget_inventory_summary.json`
- `tmp/reports/governance-sustainability/sustainable-progress-policy/governance_policy_summary.json`
- `tmp/reports/governance-sustainability/artifact-contract/governance_artifact_contract_summary.json`
- `tmp/reports/governance-sustainability/extension-review-workflow/governance_extension_review_workflow_summary.json`
- `tmp/reports/governance-sustainability/integration/governance_sustainability_integration_summary.json`
- `tmp/reports/governance-sustainability/evidence-summary.json`
- `tmp/reports/governance-sustainability/publication-summary.json`
