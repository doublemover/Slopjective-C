# M318-E002 Expectations

Contract ID: `objc3c-governance-hardening-closeout-matrix/m318-e002-v1`

## Required outcome

- Publish the final M318 closeout matrix over the live governance surfaces and the `M318-E001` gate.
- Make the matrix prove that `M318` is down to the closeout issue itself before milestone closure.
- Hand off cleanly to the next cleanup milestone, `M316`.

## Required closeout inputs

- `spec/governance/objc3c_long_term_governance_reporting_contract.json`
- `spec/planning/compiler/m318/m318_e001_sustainable_progress_gate_contract_and_architecture_freeze_gate.json`
- `tmp/reports/m318/M318-E001/sustainable_progress_gate_summary.json`
- `tmp/reports/m318/governance/topology.json`
- `tmp/reports/m318/governance/new_work_proposal/publication_summary.json`
- live GitHub open-issue state for milestone `399`

## Required matrix conditions

- sustainable-progress gate is green
- governance topology remains green and alarm-free
- proposal publication remains render-only
- milestone open issues are exactly `M318-E002` before closeout

## Boundaries

- Do not skip the milestone-open-state proof.
- Do not widen the closeout matrix beyond M318 governance surfaces.
