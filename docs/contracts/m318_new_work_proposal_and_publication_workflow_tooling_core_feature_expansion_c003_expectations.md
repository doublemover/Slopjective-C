# M318-C003 Expectations

Contract ID: `objc3c-governance-new-work-proposal-tooling/m318-c003-v1`

## Required outcome

- Provision the future tooling surface promised by `M318-C001`:
  - `tmp/planning/m318_governance/new_work_proposal_template.json`
  - `tmp/github-publish/m318_governance/publish_new_work_proposal.py`
- Make the tooling consume the existing planning-hygiene, issue-body, and exception-process contracts instead of inventing a parallel issue-publication path.
- Make render-only publication the default mode.
- Emit deterministic preview artifacts under `tmp/reports/m318/governance/new_work_proposal/`.

## Required proposal checks

- Proposal inputs must carry:
  - `issue_code`
  - `title_core`
  - `kind`
  - `milestone_focus_summary`
  - `why_it_matters`
  - `acceptance_criteria`
  - `primary_implementation_surfaces`
  - `dependencies`
  - `validation_posture`
  - `budget_impact`
  - `label_names`
- `validation_posture` must come from the `M317-C001` contract.
- `budget_impact` must come from the `M318-B002` planning-hygiene policy.
- `requires_exception_record` proposals must cite an active exception record from the governance registry.

## Required rendered outputs

- Issue title in the established `[Mxxx][Lane-X][Xnnn] ... - Kind` shape.
- Issue body sections required by `M317-C001`.
- Publication payload JSON suitable for later GitHub use.
- A summary JSON that records:
  - consumed contract ids
  - render mode
  - label set
  - milestone target
  - exception-record resolution
  - output paths

## Boundaries

- Do not create live GitHub issues as part of normal validation.
- Do not add another generator outside the predeclared `tmp/planning/` and `tmp/github-publish/` paths.
- Do not reintroduce universal `Required deliverables` boilerplate into generated issue bodies.
