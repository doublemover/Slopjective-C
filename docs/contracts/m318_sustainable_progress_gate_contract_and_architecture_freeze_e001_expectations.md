# M318-E001 Expectations

Contract ID: `objc3c-governance-sustainable-progress-gate/m318-e001-v1`

## Required outcome

- Freeze one truthful pre-closeout gate over the live `M318` governance surfaces.
- Make the gate fail closed if governance alarms fire, expired exceptions exist, or the proposal tool drifts away from render-only default mode.
- Make the gate prove the milestone is down to `E001` and `E002` before closeout.

## Required gate inputs

- `spec/governance/objc3c_long_term_governance_reporting_contract.json`
- `tmp/reports/m318/governance/topology.json`
- `tmp/reports/m318/governance/exception_registry.json`
- `tmp/reports/m318/governance/new_work_proposal/publication_summary.json`
- live GitHub open-issue state for milestone `399`

## Required gate conditions

- topology report is green and alarm-free
- no expired exception records
- proposal publication default mode remains `render-only`
- milestone open issues are exactly `M318-E001` and `M318-E002` at gate time

## Boundaries

- Do not close the milestone from this issue.
- Do not widen the gate beyond already-landed M318 governance surfaces.
