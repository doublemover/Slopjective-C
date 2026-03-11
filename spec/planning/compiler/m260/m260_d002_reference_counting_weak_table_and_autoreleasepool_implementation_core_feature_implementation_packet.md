# M260-D002 Reference Counting Weak Table And Autoreleasepool Implementation Core Feature Implementation Packet

Packet: `M260-D002`
Issue: `#7176`
Milestone: `M260`
Lane: `D`

## Objective

Implement the truthful runtime memory-management baseline for runtime-backed objects:

- reference counting
- weak-table zeroing
- `@autoreleasepool` scope push/pop and drain behavior

## Dependencies

- `M260-D001`
- `M260-C002`
- `M257-D002`

## Canonical artifacts

- `tests/tooling/fixtures/native/m260_d002_reference_counting_weak_autoreleasepool_positive.objc3`
- `tests/tooling/runtime/m260_d002_reference_counting_weak_autoreleasepool_probe.cpp`
- `docs/contracts/m260_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation_d002_expectations.md`
- `scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py`
- `tests/tooling/test_check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py`
- `scripts/run_m260_d002_lane_d_readiness.py`

## Validation

- `python scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py -q`
- `python scripts/run_m260_d002_lane_d_readiness.py`

## Truthful boundary

- The public runtime header remains register/lookup/dispatch plus testing snapshots.
- Private runtime helpers now include autoreleasepool push/pop and memory-management state snapshots.
- The next issue is `M260-E001`.

## Evidence path

- `tmp/reports/m260/M260-D002/reference_counting_weak_autoreleasepool_summary.json`
