# M248-D015 Runner Reliability and Platform Operations Advanced Core Workpack (Shard 1) Packet

Packet: `M248-D015`
Milestone: `M248`
Lane: `D`
Dependencies: `M248-D014`
Issue: `#6850`

## Scope

Add advanced-core consistency/readiness closure to lane-D runner reliability
and platform operations after release-candidate replay dry-run closure, and
wire deterministic key-backed sign-off evidence through parse/lowering
readiness surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract: `docs/contracts/m248_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_d015_expectations.md`
- Checker: `scripts/check_m248_d015_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract.py`
- Tooling tests: `tests/tooling/test_check_m248_d015_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract.py`
- Core surfaces:
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m248/M248-D015/runner_reliability_and_platform_operations_advanced_core_workpack_shard1_contract_summary.json`

## Determinism Criteria

- D014 release-candidate replay dry-run remains required and cannot be bypassed.
- Advanced-core consistency/readiness are deterministic and key-backed.
- Integration-closeout and performance/quality guardrail keys carry
  advanced-core replay evidence.
- Failure reasons remain explicit when advanced-core closure drifts.
