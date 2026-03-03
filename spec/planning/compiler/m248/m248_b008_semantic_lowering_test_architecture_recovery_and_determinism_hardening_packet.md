# M248-B008 Semantic/Lowering Test Architecture Recovery and Determinism Hardening Packet

Packet: `M248-B008`
Milestone: `M248`
Lane: `B`
Freeze date: `2026-03-03`
Issue: `#6808`
Dependencies: `M248-B007`

## Purpose

Freeze lane-B semantic/lowering test architecture recovery and determinism
hardening prerequisites for M248 so dependency continuity stays explicit,
deterministic, and fail-closed, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_semantic_lowering_test_architecture_recovery_and_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`
- Dependency anchors from `M248-B007`:
  - `docs/contracts/m248_semantic_lowering_test_architecture_diagnostics_hardening_b007_expectations.md`
  - `spec/planning/compiler/m248/m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_packet.md`
  - `scripts/check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract`
  - `test:tooling:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m248-b008-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m248-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m248/M248-B008/semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract_summary.json`
