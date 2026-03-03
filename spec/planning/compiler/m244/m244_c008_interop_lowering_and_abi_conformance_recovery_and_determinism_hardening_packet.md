# M244-C008 Interop Lowering and ABI Conformance Recovery and Determinism Hardening Packet

Packet: `M244-C008`
Milestone: `M244`
Lane: `C`
Issue: `#6557`
Dependencies: `M244-C007`

## Purpose

Execute lane-C interop lowering and ABI conformance recovery and determinism hardening
governance on top of C007 diagnostics hardening assets so downstream expansion and
cross-lane conformance integration remain deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_c008_expectations.md`
- Checker:
  `scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c008-interop-lowering-abi-conformance-recovery-determinism-hardening-contract`
  - `test:tooling:m244-c008-interop-lowering-abi-conformance-recovery-determinism-hardening-contract`
  - `check:objc3c:m244-c008-lane-c-readiness`

## Dependency Anchors (M244-C007)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_diagnostics_hardening_c007_expectations.md`
- `spec/planning/compiler/m244/m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_packet.md`
- `scripts/check_m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_contract.py`

## Gate Commands

- `python scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`
- `python scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m244-c008-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C008/interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract_summary.json`

