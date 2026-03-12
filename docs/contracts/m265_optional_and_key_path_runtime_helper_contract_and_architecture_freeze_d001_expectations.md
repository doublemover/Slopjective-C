# M265-D001 Expectations

Contract ID: `objc3c-part3-optional-keypath-runtime-helper-contract/m265-d001-v1`

Scope: Freeze the truthful runtime/helper boundary for the current runnable Part 3 slice. Optional sends and optional-member access already execute through the public selector lookup and dispatch entrypoints; validated single-component typed key-path sites now publish retained descriptor handles and descriptor sections; full runtime key-path evaluation remains deferred to `M265-D002`.

Required anchors:
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/runtime/README.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `package.json`

Required truths:
- a canonical semantic-surface packet publishes the runtime/helper contract
- optional sends and optional-member access remain on `objc3_runtime_lookup_selector` plus `objc3_runtime_dispatch_i32`
- lowering still owns nil short-circuit semantics
- validated single-component typed key-path sites now publish runtime-facing stable descriptor handles plus retained `objc3.runtime.keypath_descriptors` payloads
- full runtime key-path evaluation helpers remain deferred and are not falsely claimed as landed
- unsupported key-path shapes and non-ObjC optional-member access remain compile-time fail-closed, not runtime fallback behavior

Required probes:
- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-d001-readiness`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m265_optional_member_access_runtime_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/d001/optional --emit-prefix module`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m265_typed_keypath_artifact_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/d001/keypath --emit-prefix module`
- inspect both manifests for `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_runtime_helper_contract`
- link both `module.obj` outputs with their `module.runtime-registration-manifest.json` and `module.runtime-metadata-linker-options.rsp`
- execute the optional binary and require exit code `9`
- execute the key-path binary and require exit code `7`

Expected live facts:
- the runtime/helper packet publishes:
  - `public_lookup_selector_symbol = objc3_runtime_lookup_selector`
  - `public_dispatch_i32_symbol = objc3_runtime_dispatch_i32`
  - `keypath_descriptor_section = objc3.runtime.keypath_descriptors`
  - `optional_send_runtime_ready = true`
  - `typed_keypath_descriptor_handles_ready = true`
  - `typed_keypath_runtime_execution_helper_landed = false`
  - `diagnostic_fallback_ready = true`
- the optional-send IR carries `part3_optional_keypath_runtime_helper_contract =`
- the typed-keypath IR carries the same runtime-helper comment and `@__objc3_sec_keypath_descriptors`
- both positive executables link against the native runtime library and run successfully

Validation:
- `python scripts/check_m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation.py`
- `python scripts/check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py`
- `python scripts/check_m265_d001_optional_and_key_path_runtime_helper_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m265_d001_optional_and_key_path_runtime_helper_contract_and_architecture_freeze.py -q`
- `python scripts/run_m265_d001_lane_d_readiness.py`

Evidence:
- `tmp/reports/m265/M265-D001/optional_keypath_runtime_helper_contract_summary.json`
