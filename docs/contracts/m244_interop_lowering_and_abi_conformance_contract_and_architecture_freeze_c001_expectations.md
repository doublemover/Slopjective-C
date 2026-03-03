# M244 Interop Lowering and ABI Conformance Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-freeze/m244-c001-v1`
Status: Accepted
Dependencies: none
Scope: lane-C interop lowering and ABI conformance contract/architecture freeze for deterministic anchor and dependency-token continuity.

## Objective

Freeze lane-C interop lowering and ABI conformance boundaries before downstream
runtime projection, cross-lane integration, and conformance-matrix expansion.
Deterministic anchors, dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Tokens

- Root dependency token: `none`
- Contract token: `M244-C001` is required across contract, packet, checker, and
  readiness command surfaces.

## Required Anchors

1. Contract/checker/test assets remain mandatory:
   - `spec/planning/compiler/m244/m244_c001_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
   - `tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
2. Architecture and spec anchors remain explicit:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
3. Build/readiness wiring remains explicit in `package.json`:
   - `check:objc3c:m244-c001-interop-lowering-abi-conformance-contract`
   - `test:tooling:m244-c001-interop-lowering-abi-conformance-contract`
   - `check:objc3c:m244-c001-lane-c-readiness`

## Interop ABI Surface References

- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `tests/conformance/lowering_abi/`

## Validation

- `python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py`
- `python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py -q`
- `npm run check:objc3c:m244-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C001/interop_lowering_and_abi_conformance_contract_summary.json`
