# M237 Qualified Type Lowering and ABI Representation Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-block-lowering-and-invoke-emission-contract/m237-c007-v1`
Status: Accepted
Dependencies: none
Scope: M237 lane-C qualified type lowering and ABI representation diagnostics hardening for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5892` defines canonical lane-C diagnostics hardening scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m237/m237_c007_block_lowering_and_invoke_emission_diagnostics_hardening_packet.md`
  - `scripts/check_m237_c007_block_lowering_and_invoke_emission_contract.py`
  - `tests/tooling/test_check_m237_c007_block_lowering_and_invoke_emission_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M237 lane-C C007
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m237-c007-block-lowering-and-invoke-emission-contract`.
- `package.json` includes
  `test:tooling:m237-c007-block-lowering-and-invoke-emission-contract`.
- `package.json` includes `check:objc3c:m237-c007-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m237_c007_block_lowering_and_invoke_emission_contract.py`
- `python -m pytest tests/tooling/test_check_m237_c007_block_lowering_and_invoke_emission_contract.py -q`
- `npm run check:objc3c:m237-c007-lane-c-readiness`

## Evidence Path

- `tmp/reports/m237/M237-C007/block_lowering_and_invoke_emission_contract_summary.json`








