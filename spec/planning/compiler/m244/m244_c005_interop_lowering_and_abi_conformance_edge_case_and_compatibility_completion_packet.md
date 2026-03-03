# M244-C005 Interop Lowering and ABI Conformance Edge-Case and Compatibility Completion Packet

Packet: `M244-C005`
Milestone: `M244`
Lane: `C`
Issue: `#6554`
Dependencies: `M244-C004`

## Purpose

Execute lane-C interop lowering and ABI conformance edge-case and
compatibility completion governance on top of C004 core-feature expansion assets so
downstream expansion and cross-lane conformance integration remain deterministic
and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_c005_expectations.md`
- Checker:
  `scripts/check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c005-interop-lowering-abi-conformance-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m244-c005-interop-lowering-abi-conformance-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m244-c005-lane-c-readiness`

## Dependency Anchors (M244-C004)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_expansion_c004_expectations.md`
- `spec/planning/compiler/m244/m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_packet.md`
- `scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`

## Gate Commands

- `python scripts/check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m244-c005-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C005/interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract_summary.json`

