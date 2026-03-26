## Intent

Freeze the long-term reporting contract that later gates and closeout work will trust for governance budgets, waivers, and future-work proposal publication.

## Scope

- Add a single governance reporting contract under `spec/governance/`.
- Bind the contract to the already-landed M318 registry, guard, workflow, and proposal-tool surfaces.
- Prove the contract against live generated summaries from the guard and proposal tool.

## Contract

- Consumes:
  - `objc3c-governance-anti-noise-exception-process/m318-a002-v1`
  - `objc3c-governance-anti-noise-budget-policy/m318-b001-v1`
  - `objc3c-governance-planning-hygiene-policy/m318-b002-v1`
  - `objc3c-governance-review-and-regression-model/m318-b003-v1`
  - `objc3c-governance-automation-contract/m318-c001-v1`
  - `objc3c-governance-new-work-proposal-tooling/m318-c003-v1`
- Produces:
  - `objc3c-governance-long-term-reporting-contract/m318-d001-v1`

## Validation plan

- Static checker validates the new reporting contract and package wiring.
- The checker executes the governance guard stages and render-only proposal tool to prove the contract against live report outputs.
- Lane-D readiness runs the checker and pytest.

## Closeout evidence

- Reporting contract:
  - `spec/governance/objc3c_long_term_governance_reporting_contract.json`
- Live report outputs:
  - `tmp/reports/m318/governance/budget_snapshot.json`
  - `tmp/reports/m318/governance/exception_registry.json`
  - `tmp/reports/m318/governance/residue_proof.json`
  - `tmp/reports/m318/governance/topology.json`
  - `tmp/reports/m318/governance/new_work_proposal/publication_summary.json`

## Next issue

Next issue: `M318-E001`.
