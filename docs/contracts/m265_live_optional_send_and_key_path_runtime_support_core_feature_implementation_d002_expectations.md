# M265-D002 Expectations

Contract ID: `objc3c-part3-optional-keypath-runtime-live-support/m265-d002-v1`

Scope: Land the first truthful live runtime tranche for the executable Part 3 type surface. Optional sends and optional-member access remain on the existing public selector lookup and dispatch entrypoints; validated single-component typed key-path handles now feed a private runtime registry/testing-helper surface sourced from retained `objc3.runtime.keypath_descriptors` metadata. Full multi-component key-path evaluation remains deferred.

Required anchors:
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `native/objc3c/src/runtime/README.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

Required truths:
- optional sends and optional-member access still execute through `objc3_runtime_lookup_selector` plus `objc3_runtime_dispatch_i32`
- lowering still owns nil short-circuit semantics
- the registration table now carries `keypath_descriptor_root` with ABI version `2` and pointer-field count `12`
- runtime registration consumes retained typed key-path descriptors into a private key-path registry
- validated single-component typed key-path handles expose private testing helpers for:
  - registry-state snapshots
  - entry lookup by stable handle
  - component-count lookup
  - root-is-self lookup
- the semantic-surface packet at `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_runtime_helper_contract` now truthfully reports `typed_keypath_runtime_execution_helper_landed = true`
- full multi-component typed key-path evaluation remains deferred and is not falsely claimed as implemented

Required probes:
- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-d002-readiness`
- compile and link `tests/tooling/fixtures/native/m265_optional_member_access_runtime_positive.objc3`
- execute the optional binary and require exit code `9`
- compile and link `tests/tooling/fixtures/native/m265_typed_keypath_runtime_positive.objc3`
- execute the typed key-path binary and require exit code `11`
- compile `tests/tooling/fixtures/native/m265_typed_keypath_runtime_module.objc3`
- link and run `tests/tooling/runtime/m265_d002_keypath_runtime_probe.cpp` against the emitted object and runtime archive
- require the probe JSON to prove:
  - `keypath_table_entry_count = 1`
  - `image_backed_keypath_count = 1`
  - `ambiguous_keypath_handle_count = 0`
  - `last_materialized_handle = 1`
  - `entry_found = 1`
  - `entry_ambiguous = 0`
  - `entry_root_is_self = 0`
  - `entry_component_count = 1`
  - `entry_metadata_provider_count = 1`
  - `component_count_helper = 1`
  - `root_is_self_helper = 0`
  - `missing_found = 0`
  - `root_name = "Person"`
  - `component_path = "name"`
  - `profile_present = 1`
  - `generic_metadata_replay_key_present = 1`

Validation:
- `python scripts/check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py`
- `python scripts/check_m265_d002_live_optional_send_and_key_path_runtime_support_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m265_d002_live_optional_send_and_key_path_runtime_support_core_feature_implementation.py -q`
- `python scripts/run_m265_d002_lane_d_readiness.py`

Evidence:
- `tmp/reports/m265/M265-D002/live_optional_send_and_keypath_runtime_support_summary.json`
