# M260 Reference Counting Weak Table And Autoreleasepool Implementation Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-memory-management-implementation/m260-d002-v1`

Scope: `M260-D002` upgrades the frozen `M260-D001` helper boundary into live runtime-backed refcount, weak-side-table, and `@autoreleasepool` behavior for the supported native object baseline.

## Required outcomes

1. Native semantic analysis no longer fail-closes `@autoreleasepool` in Objective-C 3 native mode.
2. Lowered IR emits private autoreleasepool push/pop helper calls and publishes an explicit D002 runtime-memory-management implementation boundary.
3. The runtime implements:
   - live instance retain/release counting
   - weak-slot bookkeeping and zeroing on final release
   - autoreleasepool scope push/pop with deterministic drain behavior
4. The stable public runtime header remains unchanged; the helper/runtime state surface stays private to `objc3_runtime_bootstrap_internal.h`.
5. Validation evidence lands at `tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`.

## Canonical proof artifacts

- `tests/tooling/fixtures/native/m260_d002_reference_counting_weak_autoreleasepool_positive.objc3`
- `tests/tooling/runtime/m260_d002_reference_counting_weak_autoreleasepool_probe.cpp`
- `scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py`
- `scripts/run_m260_d002_lane_d_readiness.py`
- `tests/tooling/test_check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py`

## Truthful boundary

- This issue makes `@autoreleasepool` runnable for the supported runtime-backed object baseline.
- The runtime memory-management surface still remains private/lowered; no public ownership or autoreleasepool API widening lands here.
- `M260-E001` is the next issue.
