# Packet: `M260-A002`

Issue: `#7169`

Milestone: `M260`

Lane: `A`

Dependencies: `M260-A001`

## Goal

Promote the frozen runtime-backed ownership source surface into emitted
property/member metadata that runtime-facing artifacts can consume directly.

## In scope

- emitted property descriptor publication of:
  - `property_attribute_profile`
  - `ownership_lifetime_profile`
  - `ownership_runtime_hook_profile`
  - `accessor_ownership_profile`
- deterministic lowering summary for the new emitted ownership surface
- one focused proof fixture and checker over the emitted manifest/IR/object
  boundary

## Validation

- `python scripts/check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py -q`
- `python scripts/run_m260_a002_lane_a_readiness.py`

## Non-goals

- No live ARC retain/release/autorelease hook emission.
- No executable function/method ownership qualifiers.
- No `@autoreleasepool` runnable support.

## Handoff

- Next issue: `M260-B001`
- Evidence path: `tmp/reports/m260/M260-A002/runtime_backed_object_ownership_attribute_surface_summary.json`
