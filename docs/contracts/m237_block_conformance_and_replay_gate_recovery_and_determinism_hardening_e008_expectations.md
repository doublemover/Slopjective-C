# M237 Block conformance and replay gate Recovery and Determinism Hardening Expectations (E001)

Contract ID: `objc3c-block-conformance-and-replay-gate-contract-architecture-freeze/m237-e001-v1`
Status: Accepted
Issue: `#5840`
Dependencies: `M237-A001`, `M237-B001`, `M237-C001`
Scope: M237 lane-E block conformance and replay gate recovery and determinism hardening bound to currently-closed early lane steps.

## Objective

Fail closed unless lane-E block conformance and replay gate dependency anchors
remain explicit, deterministic, and traceable against currently-closed early
lane steps before E002+ workpacks consume lane-E readiness.

## Prerequisite Dependency Matrix (Currently-Closed Early Lane Steps)

| Lane Task | Required Freeze State |
| --- | --- |
| `M237-A001` | Contract assets for A001 are required and must remain present/readable. |
| `M237-B001` | Contract assets for B001 are required and must remain present/readable. |
| `M237-C001` | Contract assets for C001 are required and must remain present/readable. |

## Scope Anchors

- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m237/m237_e008_block_conformance_and_replay_gate_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m237_e008_block_conformance_and_replay_gate_contract.py`
  - `tests/tooling/test_check_m237_e008_block_conformance_and_replay_gate_contract.py`
- Dependency anchors from currently-closed early lane steps:
  - `docs/contracts/m237_block_syntax_and_capture_declarations_contract_and_architecture_freeze_a001_expectations.md`
  - `docs/contracts/m237_block_semantic_capture_and_lifetime_rules_contract_and_architecture_freeze_b001_expectations.md`
  - `docs/contracts/m237_block_lowering_and_invoke_emission_contract_and_architecture_freeze_c001_expectations.md`

## Validation

- `python scripts/check_m237_e008_block_conformance_and_replay_gate_contract.py --summary-out tmp/reports/m237/M237-E008/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m237_e008_block_conformance_and_replay_gate_contract.py -q`

## Evidence Path

- `tmp/reports/m237/M237-E008/local_check_summary.json`









