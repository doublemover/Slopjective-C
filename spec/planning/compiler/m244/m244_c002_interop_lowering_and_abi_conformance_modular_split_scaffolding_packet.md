# M244-C002 Interop Lowering and ABI Conformance Modular Split and Scaffolding Packet

Packet: `M244-C002`
Milestone: `M244`
Lane: `C`
Issue: `#6551`
Dependencies: `M244-C001`

## Purpose

Execute lane-C interop lowering and ABI conformance modular split/scaffolding
governance on top of C001 freeze assets so downstream runtime projection and
cross-lane conformance expansion remain deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c002-interop-lowering-abi-conformance-modular-split-scaffolding-contract`
  - `test:tooling:m244-c002-interop-lowering-abi-conformance-modular-split-scaffolding-contract`
  - `check:objc3c:m244-c002-lane-c-readiness`

## Dependency Anchors (M244-C001)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md`
- `spec/planning/compiler/m244/m244_c001_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_packet.md`
- `scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
- `tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py`

## Gate Commands

- `python scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
- `python scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m244-c002-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/M244-C002/interop_lowering_and_abi_conformance_modular_split_scaffolding_contract_summary.json`
