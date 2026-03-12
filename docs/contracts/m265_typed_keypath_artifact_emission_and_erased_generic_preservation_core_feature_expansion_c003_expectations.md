# M265-C003 Expectations

Contract lineage: `objc3c-part3-optional-keypath-lowering/m265-c001-v1`

Scope: Prove that the validated single-component typed key-path subset now lowers into retained runtime descriptor artifacts and stable nonzero handles while preserving erased-generic replay evidence through the emitted manifest, IR, object file, and linked executable.

Required anchors:
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `docs/objc3c-native/src/20-grammar.md`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `package.json`

Required truths:
- typed key-path lowering truth no longer claims the validated single-component subset is deferred
- the lowering packet reports `live_typed_keypath_artifact_sites = 1`, `deferred_typed_keypath_sites = 0`, and `ready_for_typed_keypath_artifact_emission = true`
- emitted IR publishes a retained `objc3.runtime.keypath_descriptors` section, per-descriptor globals, and an aggregate discovery root
- typed key-path expression lowering returns a stable nonzero descriptor handle for the validated subset
- erased-generic replay evidence remains visible through the generic metadata ABI packet and the typed-keypath IR comment
- the linked positive executable proves the emitted key-path handle is materially usable by returning `7`

Required probes:
- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-c003-readiness`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m265_typed_keypath_artifact_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/c003/positive --emit-prefix module`
- inspect `tmp/artifacts/compilation/objc3c-native/m265/c003/positive/module.manifest.json`
- inspect `tmp/artifacts/compilation/objc3c-native/m265/c003/positive/module.ll`
- inspect `tmp/artifacts/compilation/objc3c-native/m265/c003/positive/module.obj` with `llvm-readobj --sections`
- link `module.obj` into `module.exe` using `module.runtime-registration-manifest.json` and `module.runtime-metadata-linker-options.rsp`
- execute the linked positive binary and require exit code `7`

Expected live facts:
- the typed key-path lowering packet publishes:
  - `typed_keypath_literal_sites = 1`
  - `typed_keypath_class_root_sites = 1`
  - `live_typed_keypath_artifact_sites = 1`
  - `deferred_typed_keypath_sites = 0`
  - `ready_for_typed_keypath_artifact_emission = true`
- the generic metadata ABI packet publishes:
  - `generic_metadata_abi_sites = 1`
  - `generic_suffix_sites = 1`
  - `protocol_composition_sites = 1`
  - `object_pointer_type_sites = 1`
  - `deterministic_handoff = true`
- the IR carries:
  - `typed_keypath_artifact_emission = contract=objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1`
  - `generic_metadata_abi_lowering =`
  - `@__objc3_keypath_desc_0000`
  - `@__objc3_sec_keypath_descriptors`
- the object file contains `objc3.runtime.keypath_descriptors`
- the linked executable exits `7`

Validation:
- `python scripts/check_m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation.py`
- `python scripts/check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py`
- `python scripts/check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py -q`
- `python scripts/run_m265_c003_lane_c_readiness.py`

Evidence:
- `tmp/reports/m265/M265-C003/typed_keypath_artifact_emission_summary.json`
