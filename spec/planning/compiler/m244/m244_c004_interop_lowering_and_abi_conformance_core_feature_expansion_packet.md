# M244-C004 Interop Lowering and ABI Conformance Core Feature Expansion Packet

Packet: `M244-C004`
Milestone: `M244`
Lane: `C`
Issue: `#6553`
Dependencies: `M244-C003`

## Purpose

Execute lane-C interop lowering and ABI conformance core-feature
expansion governance on top of C003 core-feature implementation assets so
downstream expansion and cross-lane conformance integration remain
deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c004-interop-lowering-abi-conformance-core-feature-expansion-contract`
  - `test:tooling:m244-c004-interop-lowering-abi-conformance-core-feature-expansion-contract`
  - `check:objc3c:m244-c004-lane-c-readiness`

## Dependency Anchors (M244-C003)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_implementation_c003_expectations.md`
- `spec/planning/compiler/m244/m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_packet.md`
- `scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
- `tests/tooling/test_check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`

## Gate Commands

- `python scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
- `python scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m244-c004-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C004/interop_lowering_and_abi_conformance_core_feature_expansion_contract_summary.json`

