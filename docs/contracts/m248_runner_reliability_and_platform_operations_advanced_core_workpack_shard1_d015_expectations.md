# Runner Reliability and Platform Operations Advanced Core Workpack (Shard 1) Expectations (M248-D015)

Contract ID: `objc3c-runner-reliability-platform-operations-advanced-core-workpack-shard1/m248-d015-v1`
Status: Accepted
Scope: lane-D advanced core workpack (shard 1) for runner reliability/platform operations closure.

## Objective

Extend D014 release-candidate replay closure with an explicit advanced-core
consistency/readiness gate so docs/runbook synchronization is promoted into a
hardened sign-off boundary before parse/lowering readiness can report ready.

## Deterministic Invariants

1. Parse/lowering readiness exposes explicit advanced-core helpers:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(...)`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes advanced-core
   consistency/readiness from D014 replay closure and docs/runbook
   synchronization evidence.
3. Advanced-core evidence is persisted on readiness surfaces:
   - `toolchain_runtime_ga_operations_advanced_core_consistent`
   - `toolchain_runtime_ga_operations_advanced_core_ready`
   - `toolchain_runtime_ga_operations_advanced_core_key`
4. Advanced-core key evidence is folded into integration-closeout and
   performance/quality guardrail replay keys.
5. Manifest parse/lowering readiness exports advanced-core booleans and key.
6. Failure reasons remain explicit when advanced-core consistency/readiness
   drifts.
7. D014 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m248_d015_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m248_d015_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_d015_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m248-d015-lane-d-readiness`

## Evidence Path

- `tmp/reports/m248/M248-D015/runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract_summary.json`
