# M265-D003 Expectations

- Contract id: `objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1`
- Cross-module imports preserve the live optional/key-path runtime packets published by provider modules.
- The provider `module.runtime-import-surface.json` must carry both `objc_part3_optional_keypath_lowering_contract` and `objc_part3_optional_keypath_runtime_helper_contract`.
- The consumer manifest must aggregate those imported facts into `frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules` and `frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration`.
- The checker must compile separate provider and consumer modules, link a cross-module probe, and prove that typed key-path runtime metadata survives module boundaries.
