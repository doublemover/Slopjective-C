# M239 Qualified Type Lowering and ABI Representation Advanced Edge Compatibility Workpack (shard 2) Expectations (C022)

Contract ID: `objc3c-cfg-ssa-lowering-and-phi-construction-contract/m239-c022-v1`
Status: Accepted
Dependencies: none
Scope: M239 lane-C qualified type lowering and ABI representation advanced edge compatibility workpack (shard 2) for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#4989` defines canonical lane-C advanced edge compatibility workpack (shard 2) scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m239/m239_c022_cfg_ssa_lowering_and_phi_construction_advanced_edge_compatibility_workpack_shard_2_packet.md`
  - `scripts/check_m239_c022_cfg_ssa_lowering_and_phi_construction_contract.py`
  - `tests/tooling/test_check_m239_c022_cfg_ssa_lowering_and_phi_construction_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M239 lane-C C022
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m239-c022-cfg-ssa-lowering-and-phi-construction-contract`.
- `package.json` includes
  `test:tooling:m239-c022-cfg-ssa-lowering-and-phi-construction-contract`.
- `package.json` includes `check:objc3c:m239-c022-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m239_c022_cfg_ssa_lowering_and_phi_construction_contract.py`
- `python -m pytest tests/tooling/test_check_m239_c022_cfg_ssa_lowering_and_phi_construction_contract.py -q`
- `npm run check:objc3c:m239-c022-lane-c-readiness`

## Evidence Path

- `tmp/reports/m239/M239-C022/cfg_ssa_lowering_and_phi_construction_contract_summary.json`























