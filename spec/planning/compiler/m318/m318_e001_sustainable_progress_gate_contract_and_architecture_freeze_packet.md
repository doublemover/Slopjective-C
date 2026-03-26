## Intent

Freeze the sustainable-progress gate that must pass before the M318 closeout matrix is published.

## Scope

- Add a gate contract over the live governance reports and milestone open-issue state.
- Prove the gate against current summaries and GitHub milestone state.
- Keep closeout itself deferred to `M318-E002`.

## Contract

- Consumes:
  - `objc3c-governance-long-term-reporting-contract/m318-d001-v1`
  - `objc3c-governance-new-work-proposal-tooling/m318-c003-v1`
  - `objc3c-governance-budget-enforcement-implementation/m318-c002-v1`
- Produces:
  - `objc3c-governance-sustainable-progress-gate/m318-e001-v1`

## Validation plan

- Static checker validates the gate contract.
- Checker re-runs topology and proposal-tool render-only output as needed and inspects live GitHub milestone state.
- Lane-E readiness runs checker and pytest.

## Closeout evidence

- Gate contract:
  - `spec/planning/compiler/m318/m318_e001_sustainable_progress_gate_contract_and_architecture_freeze_gate.json`
- Gate summary:
  - `tmp/reports/m318/M318-E001/sustainable_progress_gate_summary.json`

## Next issue

Next issue: `M318-E002`.
