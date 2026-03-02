# Toolchain/Runtime GA Operations Readiness Advanced Core Workpack (Shard 2) Expectations (M250-D021)

Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-advanced-core-workpack-shard2/m250-d021-v1`
Status: Accepted
Scope: lane-D advanced core workpack (shard 2) for toolchain/runtime GA readiness closure.

## Objective

Extend D020 advanced performance closure with explicit advanced core shard-2
consistency/readiness gates so performance sign-off is promoted into a hardened
core shard-2 boundary before parse/lowering readiness can report ready.

## Deterministic Invariants

1. Parse/lowering readiness exposes explicit advanced core shard-2 helpers:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(...)`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes advanced core
shard-2 consistency/readiness from D020 advanced performance closure and
integration-closeout key-shape evidence.
3. Advanced core shard-2 evidence is persisted on readiness surfaces:
   - `toolchain_runtime_ga_operations_advanced_core_shard2_consistent`
   - `toolchain_runtime_ga_operations_advanced_core_shard2_ready`
   - `toolchain_runtime_ga_operations_advanced_core_shard2_key`
4. Advanced core shard-2 key evidence is folded into integration-closeout and
   performance/quality guardrail replay keys.
5. Manifest parse/lowering readiness exports advanced core shard-2 booleans and
   key.
6. Failure reasons remain explicit when advanced core shard-2
   consistency/readiness drifts.
7. D020 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m250_d021_toolchain_runtime_ga_operations_readiness_advanced_core_workpack_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m250_d021_toolchain_runtime_ga_operations_readiness_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m250-d021-lane-d-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-D021/toolchain_runtime_ga_operations_readiness_advanced_core_workpack_shard2_contract_summary.json`
