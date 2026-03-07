# M237-E007 Block conformance and replay gate Diagnostics Hardening Packet

Packet: `M237-E007`
Milestone: `M237`
Lane: `E`
Issue: `#5840`
Freeze date: `2026-03-05`
Dependencies: `M237-A001`, `M237-B001`, `M237-C001`

## Purpose

Freeze lane-E block conformance and replay gate contract prerequisites for
M237 so lane-E readiness stays deterministic and fail-closed against
currently-closed early lane steps.

## Scope Anchors

- Contract:
  `docs/contracts/m237_block_conformance_and_replay_gate_diagnostics_hardening_e007_expectations.md`
- Checker:
  `scripts/check_m237_e007_block_conformance_and_replay_gate_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m237_e007_block_conformance_and_replay_gate_contract.py`
- Dependency anchors from currently-closed early lane steps:
  - `docs/contracts/m237_block_syntax_and_capture_declarations_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m237/m237_a001_block_syntax_and_capture_declarations_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m237_a001_block_syntax_and_capture_declarations_contract.py`
  - `tests/tooling/test_check_m237_a001_block_syntax_and_capture_declarations_contract.py`
  - `docs/contracts/m237_block_semantic_capture_and_lifetime_rules_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m237/m237_b001_block_semantic_capture_and_lifetime_rules_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m237_b001_block_semantic_capture_and_lifetime_rules_contract.py`
  - `tests/tooling/test_check_m237_b001_block_semantic_capture_and_lifetime_rules_contract.py`
  - `docs/contracts/m237_block_lowering_and_invoke_emission_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m237/m237_c001_block_lowering_and_invoke_emission_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m237_c001_block_lowering_and_invoke_emission_contract.py`
  - `tests/tooling/test_check_m237_c001_block_lowering_and_invoke_emission_contract.py`

## Gate Commands

- `python scripts/check_m237_e007_block_conformance_and_replay_gate_contract.py --summary-out tmp/reports/m237/M237-E007/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m237_e007_block_conformance_and_replay_gate_contract.py -q`

## Evidence Output

- `tmp/reports/m237/M237-E007/local_check_summary.json`








