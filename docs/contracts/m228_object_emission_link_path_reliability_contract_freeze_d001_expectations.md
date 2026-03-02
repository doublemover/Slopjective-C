# M228 Object Emission and Link Path Reliability Contract and Architecture Freeze (D001)

Contract ID: `objc3c-object-emission-link-path-reliability-freeze/m228-d001-v1`
Status: Accepted
Scope: lane-D freeze anchors for object emission backend routing, llc/clang compile path reliability, and deterministic backend-output replay surfaces.

## Objective

Freeze object emission and link path reliability contracts so backend routing and
object artifact generation remain deterministic and fail-closed before lane-D
modular split work.

## Scope Anchors

- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/io/objc3_toolchain_runtime_ga_operations_scaffold.h`
- `native/objc3c/src/io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `native/objc3c/src/ARCHITECTURE.md`

## Deterministic Invariants

1. IO process API keeps explicit compile routes:
   - `RunObjectiveCCompile(...)`
   - `RunIRCompile(...)`
   - `RunIRCompileLLVMDirect(...)`
2. LLVM-direct object emission fail-closes with deterministic error payload when
   `llc` is missing or exits non-zero.
3. Toolchain/runtime scaffold stays deterministic for backend-route selection,
   object artifact readiness, and scaffold key synthesis via
   `BuildObjc3ToolchainRuntimeGaOperationsScaffold(...)`.
4. Toolchain/runtime core feature surface stays deterministic for backend output
   marker routing and core-feature readiness derivation.
5. Frontend C API route selection continues to wire clang/llvm-direct object
   compile paths through `frontend_anchor.cpp`.
6. Architecture/spec anchors explicitly include M228 lane-D D001 freeze intent.

## Validation

- `python scripts/check_m228_d001_object_emission_link_path_reliability_contract.py`
- `python -m pytest tests/tooling/test_check_m228_d001_object_emission_link_path_reliability_contract.py -q`
- `npm run check:objc3c:m228-d001-lane-d-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-D001/object_emission_link_path_reliability_contract_summary.json`
