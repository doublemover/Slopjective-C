# M318-D001 Expectations

Contract ID: `objc3c-governance-long-term-reporting-contract/m318-d001-v1`

## Required outcome

- Freeze one long-lived governance and waiver-reporting contract over the already-live M318 surfaces:
  - exception registry source of truth
  - governance guard stage summaries
  - governance workflow path
  - new-work proposal publication summaries
- Make the contract explicit about which report paths are authoritative and which statuses are blocking.

## Required reporting surfaces

- Source of truth:
  - `spec/governance/objc3c_anti_noise_exception_registry.json`
- Live report paths:
  - `tmp/reports/m318/governance/budget_snapshot.json`
  - `tmp/reports/m318/governance/exception_registry.json`
  - `tmp/reports/m318/governance/residue_proof.json`
  - `tmp/reports/m318/governance/topology.json`
  - `tmp/reports/m318/governance/new_work_proposal/publication_summary.json`

## Required policy semantics

- `expired` exception records are blocking.
- `active` exception records must remain attributable and reviewable.
- Render-only proposal publication remains the default governance mode.
- The long-term reporting contract must point future gates at a single durable reporting topology instead of milestone-local ad hoc summaries.

## Boundaries

- Do not add a second governance guard.
- Do not move the exception registry source of truth out of `spec/governance/`.
- Do not make live publication the default mode for proposal tooling.
