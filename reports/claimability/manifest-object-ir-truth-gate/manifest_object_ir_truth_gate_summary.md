# Manifest/Object/IR Truth Gate

- Issue: `#8018`
- Contract: `objc3c.manifest.object.ir.truth.gate.v1`
- Status: `PASS`
- Required artifacts: `12`
- Deterministic artifacts: `10`
- Object sections checked: `12`
- Object symbols checked: `13`
- Scratch output: `tmp/artifacts/objc3c-native/manifest-object-ir-truth-gate` (not source truth)

## Checks

- `positive_runs_compiled`: `true`
- `positive_artifacts_present`: `true`
- `deterministic_artifact_hashes`: `true`
- `positive_ir_tokens`: `true`
- `positive_manifest_keys`: `true`
- `positive_conformance_reports_ready`: `true`
- `object_backend_llvm_direct`: `true`
- `object_sections_present`: `true`
- `object_symbols_present`: `true`
- `negative_runs_rejected`: `true`
- `negative_no_manifest_ir_object`: `true`
- `negative_diagnostics_deterministic`: `true`
- `source_tokens`: `true`
- `conformance`: `true`
- `claim_gate_reports`: `true`
- `no_source_truth_under_tmp`: `true`

## Artifact Determinism

- `module.manifest.json`: `true`
- `module.ll`: `true`
- `module.obj`: `true`
- `module.runtime-registration-descriptor.json`: `true`
- `module.runtime-registration-manifest.json`: `true`
- `module.runtime-metadata.bin`: `true`
- `module.runtime-metadata-discovery.json`: `true`
- `module.runtime-metadata-linker-options.rsp`: `true`
- `module.objc3-conformance-report.json`: `true`
- `module.objc3-conformance-publication.json`: `true`

## Object Evidence

- section `objc3.runtime.image_info`: `true`
- section `objc3.runtime.class_descriptors`: `true`
- section `objc3.runtime.protocol_descriptors`: `true`
- section `objc3.runtime.category_descriptors`: `true`
- section `objc3.runtime.property_descriptors`: `true`
- section `objc3.runtime.ivar_descriptors`: `true`
- section `objc3.runtime.selector_pool`: `true`
- section `objc3.runtime.string_pool`: `true`
- section `objc3.runtime.discovery_root`: `true`
- section `objc3.runtime.linker_anchor`: `true`
- section `objc3.runtime.image_root`: `true`
- section `objc3.runtime.registration_descriptor`: `true`
- symbol `__objc3_image_info`: `true`
- symbol `__objc3_sec_class_descriptors`: `true`
- symbol `__objc3_sec_protocol_descriptors`: `true`
- symbol `__objc3_sec_category_descriptors`: `true`
- symbol `__objc3_sec_property_descriptors`: `true`
- symbol `__objc3_sec_ivar_descriptors`: `true`
- symbol `objc3_runtime_metadata_discovery_root_`: `true`
- symbol `objc3_runtime_metadata_link_anchor_`: `true`
- symbol `__objc3_runtime_registration_table_`: `true`
- symbol `__objc3_runtime_image_root_`: `true`
- symbol `__objc3_runtime_registration_descriptor_`: `true`
- symbol `objc3_runtime_stage_registration_table_for_bootstrap`: `true`
- symbol `objc3_runtime_register_image`: `true`
