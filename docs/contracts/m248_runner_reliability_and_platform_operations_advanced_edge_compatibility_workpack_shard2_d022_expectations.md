# Runner Reliability and Platform Operations Advanced Edge Compatibility Workpack (Shard 2) Expectations (M248-D022)

Contract ID: `objc3c-runner-reliability-platform-operations-advanced-edge-compatibility-workpack-shard2/m248-d022-v1`
Status: Accepted
Scope: lane-D advanced edge compatibility workpack (shard 2) for runner reliability/platform operations closure.

## Objective

Extend D021 advanced core shard-2 closure with explicit advanced edge
compatibility shard-2 consistency/readiness gates so advanced core shard-2
sign-off is promoted into a hardened edge compatibility shard-2 boundary before
parse/lowering readiness can report ready.

## Deterministic Invariants

1. Parse/lowering readiness exposes explicit advanced edge compatibility
   helpers:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(...)`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes advanced edge
compatibility shard-2 consistency/readiness from D021 advanced core shard-2
closure and integration-closeout key-shape evidence.
3. Advanced edge compatibility shard-2 evidence is persisted on readiness
surfaces:
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent`
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_ready`
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_key`
4. Advanced edge compatibility shard-2 key evidence is folded into
   integration-closeout and performance/quality guardrail replay keys.
5. Manifest parse/lowering readiness exports advanced edge compatibility
   booleans and key.
6. Failure reasons remain explicit when advanced edge compatibility shard-2
   consistency/readiness drifts.
7. D021 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m248_d022_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard2_contract.py`
- `python scripts/check_m248_d022_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard2_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_d022_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m248-d022-lane-d-readiness`

## Evidence Path

- `tmp/reports/m248/M248-D022/runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard2_contract_summary.json`
