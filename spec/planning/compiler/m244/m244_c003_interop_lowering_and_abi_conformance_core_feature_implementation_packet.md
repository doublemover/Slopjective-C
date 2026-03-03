# M244-C003 Interop Lowering and ABI Conformance Core Feature Implementation Packet

Packet: `M244-C003`
Milestone: `M244`
Lane: `C`
Issue: `#6552`
Dependencies: `M244-C002`

## Purpose

Execute lane-C interop lowering and ABI conformance core-feature
implementation governance on top of C002 modular split/scaffolding assets so
downstream expansion and cross-lane conformance integration remain
deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_implementation_c003_expectations.md`
- Checker:
  `scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c003-interop-lowering-abi-conformance-core-feature-implementation-contract`
  - `test:tooling:m244-c003-interop-lowering-abi-conformance-core-feature-implementation-contract`
  - `check:objc3c:m244-c003-lane-c-readiness`

## Dependency Anchors (M244-C002)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_modular_split_scaffolding_c002_expectations.md`
- `spec/planning/compiler/m244/m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_packet.md`
- `scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`

## Gate Commands

- `python scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
- `python scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m244-c003-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C003/interop_lowering_and_abi_conformance_core_feature_implementation_contract_summary.json`

