# M228 Object Emission and Link Path Reliability Modular Split Scaffolding (D002)

Contract ID: `objc3c-object-emission-link-path-modular-split-scaffolding/m228-d002-v1`
Status: Accepted
Scope: lane-D modular split/scaffold continuity for object emission route gating and fail-closed backend dispatch.

## Objective

Promote D001 freeze anchors into a modular split scaffold boundary so object
emission route selection remains deterministic and fail-closed before clang or
llvm-direct backend dispatch.

## Scope Anchors

- `native/objc3c/src/io/objc3_toolchain_runtime_ga_operations_scaffold.h`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

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
   scaffold intent.
5. D001 freeze anchors remain mandatory prerequisites:
   - `docs/contracts/m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md`
   - `scripts/check_m228_d001_object_emission_link_path_reliability_contract.py`
   - `tests/tooling/test_check_m228_d001_object_emission_link_path_reliability_contract.py`

## Validation

- `python scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m228-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m228/M228-D002/object_emission_link_path_modular_split_scaffolding_contract_summary.json`
