# M266 Runtime Cleanup And Unwind Integration Core Feature Implementation Expectations (D002)

Issue: `#7266`
Contract ID: `objc3c-runtime-cleanup-unwind-integration/m266-d002-v1`

`M266-D002` widens the frozen `M266-D001` cleanup boundary into a live runnable execution matrix.

Required outcomes:

- native executable proof covers ordinary lexical exit with multiple deferred cleanups
- native executable proof covers guard-mediated early return with deferred cleanup execution
- native executable proof covers nested-scope return unwind with inner-to-outer cleanup ordering
- emitted registration/link artifacts continue to provide the runtime-support archive path plus linker-response payload needed to link runnable executables
- the runtime side remains intentionally private and continues to use the autoreleasepool helper cluster rather than widening a public cleanup/unwind ABI
- emitted artifacts publish the cleanup/unwind runtime link model deterministically
- `M266-E001` remains the next issue

Required artifacts:

- `docs/contracts/m266_runtime_cleanup_and_unwind_integration_core_feature_implementation_d002_expectations.md`
- `spec/planning/compiler/m266/m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation_packet.md`
- `tests/tooling/runtime/m266_d002_cleanup_unwind_runtime_probe.cpp`
- `python scripts/check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py`
- `python scripts/run_m266_d002_lane_d_readiness.py`
- `python -m pytest tests/tooling/test_check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py -q`
- `tmp/reports/m266/M266-D002/runtime_cleanup_and_unwind_integration_summary.json`

Targeted validation:

- `python scripts/build_objc3c_native_docs.py`
- `python scripts/check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py -q`
- `python scripts/run_m266_d002_lane_d_readiness.py`
