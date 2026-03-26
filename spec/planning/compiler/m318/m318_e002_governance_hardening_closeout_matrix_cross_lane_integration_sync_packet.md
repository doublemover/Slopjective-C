## Intent

Publish the final M318 closeout matrix and prove the governance-hardening milestone is ready to close.

## Scope

- Add the M318 closeout matrix definition.
- Prove it against the gate summary, live governance reports, and GitHub milestone state.
- Hand off to the next cleanup-first milestone after closeout.

## Contract

- Consumes:
  - `objc3c-governance-sustainable-progress-gate/m318-e001-v1`
  - `objc3c-governance-long-term-reporting-contract/m318-d001-v1`
  - `objc3c-governance-new-work-proposal-tooling/m318-c003-v1`
- Produces:
  - `objc3c-governance-hardening-closeout-matrix/m318-e002-v1`

## Validation plan

- Static checker validates the closeout matrix definition.
- Checker re-runs the E001 gate and inspects live GitHub milestone state.
- Lane-E readiness runs checker and pytest.

## Closeout evidence

- Closeout matrix definition:
  - `spec/planning/compiler/m318/m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync_matrix.json`
- Closeout summary:
  - `tmp/reports/m318/M318-E002/governance_hardening_closeout_matrix_summary.json`

## Next issue

Next issue: `M316-A001`.
