# M244 Interop Lowering and ABI Conformance Modular Split and Scaffolding Expectations (C002)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-modular-split-scaffolding/m244-c002-v1`
Status: Accepted
Dependencies: `M244-C001`
Scope: lane-C interop lowering/ABI modular split and scaffolding for deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute modular split/scaffolding for lane-C interop lowering and ABI
conformance on top of C001 freeze anchors before downstream runtime projection
and cross-lane conformance expansion advances.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6551` defines canonical lane-C modular split/scaffolding scope.
- `M244-C001` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m244/m244_c001_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
  - `tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py`

## Deterministic Invariants

1. lane-C modular split/scaffolding dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C001` before `M244-C002` evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c002-interop-lowering-abi-conformance-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m244-c002-interop-lowering-abi-conformance-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m244-c002-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c001-lane-c-readiness`
  - `check:objc3c:m244-c002-lane-c-readiness`

## Validation

- `python scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
- `python scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m244-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C002/interop_lowering_and_abi_conformance_modular_split_scaffolding_contract_summary.json`
