# Packet: `M260-B003`

Issue: `#7172`

Milestone: `M260`

Lane: `B`

Dependencies: `M260-B002`

## Goal

Expand the fail-closed semantic model for autoreleasepool and destruction-order
behavior so ownership-sensitive runtime-backed object storage edges become
deterministic diagnostics rather than a generic unsupported-feature bucket.

## In scope

- retain the generic `@autoreleasepool` native-mode rejection
- add an additional deterministic destruction-order diagnostic when
  autoreleasepool scopes coexist with owned runtime-backed object or synthesized
  property storage
- one positive proof fixture and one negative proof fixture
- one focused checker over the positive IR closeout summary plus negative
  diagnostics artifacts

## Validation

- `python scripts/check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py -q`
- `python scripts/run_m260_b003_lane_b_readiness.py`

## Non-goals

- No live autoreleasepool lowering or runtime support.
- No destruction-order runtime remains.
- No ARC runtime hook emission.

## Handoff

- Next issue: `M260-C001`
- Evidence path: `tmp/reports/m260/M260-B003/autoreleasepool_destruction_order_semantics_summary.json`
