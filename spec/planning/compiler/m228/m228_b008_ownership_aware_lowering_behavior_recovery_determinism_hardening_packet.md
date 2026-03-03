# M228-B008 Ownership-Aware Lowering Behavior Recovery and Determinism Hardening Packet

Packet: `M228-B008`
Milestone: `M228`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M228-B007`

## Purpose

Freeze lane-B recovery/determinism hardening closure for ownership-aware
lowering behavior so diagnostics hardening continuity and parse recovery
determinism evidence remain deterministic and fail-closed before LLVM IR
emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ownership_aware_lowering_behavior_recovery_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
- Dependency anchors (`M228-B007`):
  - `docs/contracts/m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md`
  - `scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract`
  - `test:tooling:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract`
  - `check:objc3c:m228-b008-lane-b-readiness`
- Ownership-aware lowering recovery/determinism integration:
  - `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- `python scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m228-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m228/M228-B008/ownership_aware_lowering_behavior_recovery_determinism_hardening_contract_summary.json`
