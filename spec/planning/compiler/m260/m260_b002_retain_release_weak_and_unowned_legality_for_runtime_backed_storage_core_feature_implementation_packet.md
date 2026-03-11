# Packet: `M260-B002`

Issue: `#7171`

Milestone: `M260`

Lane: `B`

Dependencies: `M260-B001`, `M260-A002`

## Goal

Implement live semantic legality checks for runtime-backed object-property
ownership storage so emitted ownership metadata can no longer be sourced from
contradictory qualifier/modifier pairs.

## In scope

- explicit qualifier/modifier legality for:
  - `__weak`
  - `__unsafe_unretained`
  - `__strong`
- deterministic semantic diagnostics for conflicting runtime-backed object
  property storage forms
- one positive proof fixture and two negative proof fixtures:
  - `m260_runtime_backed_storage_ownership_legality_positive.objc3`
  - `m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3`
  - `m260_runtime_backed_storage_ownership_unowned_mismatch_negative.objc3`
- one focused checker over the live manifest/IR happy path plus negative
  compile diagnostics

## Validation

- `python scripts/check_m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation.py -q`
- `python scripts/run_m260_b002_lane_b_readiness.py`

## Non-goals

- No live ARC retain/release/autorelease execution hooks.
- No executable function/method ownership qualifiers.
- No runnable autoreleasepool or destruction-order semantics.

## Handoff

- Next issue: `M260-B003`
- Evidence path: `tmp/reports/m260/M260-B002/runtime_backed_storage_ownership_legality_summary.json`
