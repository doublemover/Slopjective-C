## Intent

Land the long-lived future-work proposal tooling promised by `M318-C001` so new roadmap work can be drafted and published through one governance-aware path instead of one-off scripts and issue-body edits.

## Scope

- Add the proposal template/schema at `tmp/planning/m318_governance/new_work_proposal_template.json`.
- Add the publication tool at `tmp/github-publish/m318_governance/publish_new_work_proposal.py`.
- Render a deterministic sample proposal preview into `tmp/reports/m318/governance/new_work_proposal/`.
- Wire the tooling paths into `package.json` governance metadata.

## Contract

- Consumes:
  - `objc3c-cleanup-planning-packet-issue-template-contract/m317-c001-v1`
  - `objc3c-governance-planning-hygiene-policy/m318-b002-v1`
  - `objc3c-governance-anti-noise-exception-process/m318-a002-v1`
  - `objc3c-governance-automation-contract/m318-c001-v1`
- Produces:
  - `objc3c-governance-new-work-proposal-tooling/m318-c003-v1`
- Render-only publication is the default mode.
- Future live publication remains optional and must use the same rendered payload and governance checks.

## Validation plan

- Static checker verifies the expectations, result metadata, package wiring, template schema, and publication tool outputs.
- Pytest exercises render-only generation from a sample proposal fixture.
- Lane-C readiness runs the checker and pytest only.

## Closeout evidence

- Template/schema:
  - `tmp/planning/m318_governance/new_work_proposal_template.json`
- Publication tool:
  - `tmp/github-publish/m318_governance/publish_new_work_proposal.py`
- Sample fixture:
  - `tests/tooling/fixtures/m318_governance/new_work_proposal_sample.json`
- Sample rendered outputs:
  - `tmp/reports/m318/governance/new_work_proposal/issue_body.md`
  - `tmp/reports/m318/governance/new_work_proposal/issue_payload.json`
  - `tmp/reports/m318/governance/new_work_proposal/publication_summary.json`

## Next issue

Next issue: `M318-D001`.
