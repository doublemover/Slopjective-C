# M267 Error Runtime And Bridge-Helper Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-part6-error-runtime-and-bridge-helper-api/m267-d001-v1`

Goal:
Freeze the first truthful private runtime helper ABI used by the runnable Part 6 lowering.

Required behavior:

1. The runtime helper ABI remains private to `objc3_runtime_bootstrap_internal.h` and does not widen `objc3_runtime.h`.
2. The helper cluster must include:
   - `objc3_runtime_store_thrown_error_i32`
   - `objc3_runtime_load_thrown_error_i32`
   - `objc3_runtime_bridge_status_error_i32`
   - `objc3_runtime_bridge_nserror_error_i32`
   - `objc3_runtime_catch_matches_error_i32`
3. The runtime must publish one private testing snapshot:
   - `objc3_runtime_copy_error_bridge_state_for_testing`
4. Emitted IR must carry:
   - `; part6_error_runtime_bridge_helper = ...`
   - `!objc3.objc_part6_error_runtime_bridge_helper = !{!89}`
5. The positive fixture must compile and show helper declarations plus helper call sites in emitted IR.
6. The runtime probe `tests/tooling/runtime/m267_d001_error_runtime_bridge_helper_probe.cpp` must prove the helper entrypoints execute and the testing snapshot records the traffic deterministically.

Evidence path:

- `tmp/reports/m267/M267-D001/error_runtime_bridge_helper_contract_summary.json`
