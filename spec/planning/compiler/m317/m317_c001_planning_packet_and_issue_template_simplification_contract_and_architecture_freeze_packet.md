# M317-C001 Packet: Planning packet and issue-template simplification contract - Contract and architecture freeze

## Intent

Freeze the simplified authoring contract for future roadmap issue bodies and planning packets before `M317-C002` changes generators or GitHub templates.

## Why this issue exists

The cleanup program has already corrected labels, blockers, overlap handling, and downstream dependency rewrites. The next structural problem is authoring shape:

- roadmap issue bodies are too verbose by default
- packet structure is more repetitive than informative
- validation scaffolding is implied even when a shared harness should be used instead
- seed generators currently optimize for repetition rather than disciplined variance

`M317-C001` freezes the replacement contract first so `M317-C002` can implement against stable rules.

## Contract

- Source of truth:
  - `docs/contracts/m317_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m317/m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_contract.json`
  - `spec/planning/compiler/m317/m317_c001_simplified_issue_body_template.md`
  - `spec/planning/compiler/m317/m317_c001_simplified_planning_packet_template.md`
- Verification:
  - `scripts/check_m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze.py`
  - `tests/tooling/test_check_m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze.py`
  - `scripts/run_m317_c001_lane_c_readiness.py`

## Contract focus

- Canonical required/optional issue-body sections
- Canonical required/optional planning-packet sections
- Validation-posture taxonomy and defaults
- Generator-input fields required for future seeding
- Prohibited defaults and duplication patterns

## Closeout evidence

- Machine-readable contract JSON
- Concrete simplified issue-body template
- Concrete simplified planning-packet template
- Checker summary proving the contract is internally consistent and points at the current implementation surfaces to be changed later

## Next issue

- Next issue: `M317-C002`.
