# Toolchain/Runtime GA Operations Readiness Modular Split Scaffolding Expectations (M250-D002)

Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding/m250-d002-v1`
Status: Accepted
Scope: Toolchain/runtime modular split scaffold continuity across `native/objc3c/src/io/*` and `native/objc3c/src/driver/*`.

## Objective

Freeze the D002 modular split/scaffolding boundary so toolchain/runtime GA operations readiness remains deterministic and fail-closed between backend selection, backend capability availability, and IR/object artifact compile routing.

## Deterministic Invariants

1. `Objc3ToolchainRuntimeGaOperationsScaffold` remains the canonical modular split scaffold surface:
   - `clang_backend_selected`
   - `llvm_direct_backend_selected`
   - `clang_path_configured`
   - `llc_path_configured`
   - `llvm_direct_backend_enabled`
   - `ir_artifact_ready`
   - `object_artifact_ready`
   - `compile_route_ready`
   - `modular_split_ready`
2. `BuildObjc3ToolchainRuntimeGaOperationsScaffold(...)` remains the only canonical closure-builder for D002 toolchain/runtime modular split continuity.
3. `RunObjc3LanguagePath(...)` wires `BuildObjc3ToolchainRuntimeGaOperationsScaffold(...)` and fail-closed readiness validation before backend dispatch (`RunIRCompile(...)` / `RunIRCompileLLVMDirect(...)`).
4. `native/objc3c/src/ARCHITECTURE.md` remains authoritative for the M250 lane-D D002 modular split scaffold anchor.
5. D001 freeze anchors remain mandatory prerequisites:
   - `docs/contracts/m250_toolchain_runtime_ga_operations_readiness_contract_freeze_d001_expectations.md`
   - `scripts/check_m250_d001_toolchain_runtime_ga_operations_readiness_contract.py`
   - `tests/tooling/test_check_m250_d001_toolchain_runtime_ga_operations_readiness_contract.py`
   - `spec/planning/compiler/m250/m250_d001_toolchain_runtime_ga_operations_readiness_contract_freeze_packet.md`

## Validation

- `python scripts/check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m250-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m250/M250-D002/toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract_summary.json`
