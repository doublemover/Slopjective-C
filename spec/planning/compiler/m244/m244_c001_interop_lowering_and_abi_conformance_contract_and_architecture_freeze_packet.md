# M244-C001 Interop Lowering and ABI Conformance Contract and Architecture Freeze Packet

Packet: `M244-C001`
Milestone: `M244`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: none

## Purpose

Freeze lane-C interop lowering and ABI conformance contract/architecture
prerequisites so downstream runtime projection and cross-lane conformance
packets inherit a deterministic and fail-closed foundation.
Deterministic anchors, dependency tokens, and fail-closed behavior remain mandatory scope controls.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md`
- Checker:
  `scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c001-interop-lowering-abi-conformance-contract`
  - `test:tooling:m244-c001-interop-lowering-abi-conformance-contract`
  - `check:objc3c:m244-c001-lane-c-readiness`

## Dependency Tokens

- `none` (root lane-C freeze)
- `M244-C001` token continuity is required across docs, script/test paths, and
  readiness command keys.

## Gate Commands

- `python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
- `python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py -q`
- `npm run check:objc3c:m244-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C001/interop_lowering_and_abi_conformance_contract_summary.json`
