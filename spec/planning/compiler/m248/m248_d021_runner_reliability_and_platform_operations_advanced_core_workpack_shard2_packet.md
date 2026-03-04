# M248-D021 Runner Reliability and Platform Operations Advanced Core Workpack (Shard 2) Packet

Packet: `M248-D021`
Milestone: `M248`
Lane: `D`
Dependencies: `M248-D020`
Issue: `#6856`

## Scope

Add advanced core shard-2 consistency/readiness closure to lane-D runner
reliability and platform operations after advanced performance sign-off, and
wire deterministic key-backed shard-2 evidence through parse/lowering readiness
surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract: `docs/contracts/m248_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_d021_expectations.md`
- Checker: `scripts/check_m248_d021_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_contract.py`
- Tooling tests: `tests/tooling/test_check_m248_d021_runner_reliability_and_platform_operations_advanced_core_workpack_shard2_contract.py`
- Core surfaces:
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m248/M248-D021/runner_reliability_and_platform_operations_advanced_core_workpack_shard2_contract_summary.json`

## Determinism Criteria

- D020 advanced performance closure remains required and cannot be bypassed.
- Advanced core shard-2 consistency/readiness are deterministic and key-backed.
- Integration-closeout and performance/quality guardrail keys carry advanced
  core shard-2 replay evidence.
- Failure reasons remain explicit when advanced core shard-2 closure drifts.
