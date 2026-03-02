# M228 Object Emission and Link Path Reliability Modular Split/Scaffolding Expectations (D002)

Contract ID: `objc3c-object-emission-link-path-modular-split-scaffolding/m228-d002-v1`
Status: Accepted
Scope: lane-D modular split/scaffolding continuity for object emission route gating and fail-closed backend dispatch.

## Objective

Fail closed unless lane-D object emission/link-path modular split scaffolding
anchors remain explicit, deterministic, and traceable, including code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M228-D001`
- D001 prerequisite assets remain mandatory:
  - `docs/contracts/m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md`
  - `scripts/check_m228_d001_object_emission_link_path_reliability_contract.py`
  - `tests/tooling/test_check_m228_d001_object_emission_link_path_reliability_contract.py`
- D002 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m228/m228_d002_object_emission_link_path_modular_split_scaffolding_packet.md`
  - `scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py`

## Deterministic Invariants

1. `Objc3ToolchainRuntimeGaOperationsScaffold` remains the canonical modular
   split scaffold for lane-D route gating.
2. `BuildObjc3ToolchainRuntimeGaOperationsScaffold(...)` remains the only
   canonical D002 scaffold builder for backend selection/capability + artifact
   path readiness closure.
3. `frontend_anchor.cpp` wires
   `BuildObjc3ToolchainRuntimeGaOperationsScaffold(...)` and
   `IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(...)` and fail-closes
   before `RunIRCompile(...)` / `RunIRCompileLLVMDirect(...)` dispatch.
4. Architecture/spec anchors explicitly include M228 lane-D D002 modular split
   scaffold intent and deterministic metadata continuity wording.

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_toolchain_runtime_ga_operations_scaffold.h`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m228-d002-object-emission-link-path-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m228-d002-object-emission-link-path-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m228-d002-lane-d-readiness`.

## Validation

- `python scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m228-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m228/M228-D002/object_emission_link_path_modular_split_scaffolding_contract_summary.json`
