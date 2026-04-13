# Cross-Module Semantic Contracts Diagnostics

- Contract: `objc3c.cross_module.semantic.contracts.diagnostics.closure.v1`
- Status: `PASS`
- Issue: `#8015`
- Positive fixture: `tests/tooling/fixtures/native/cross_module_semantic_contracts_diagnostics_positive.objc3`
- Negative fixture: `tests/tooling/fixtures/native/recovery/negative/negative_cross_module_semantic_contracts_duplicate_module.objc3`

## Checks
- `positive_fixture_compiles`: `PASS`
- `positive_manifest_emitted`: `PASS`
- `positive_llvm_ir_emitted`: `PASS`
- `manifest_has_cross_module_surface`: `PASS`
- `all_summary_fields_emitted`: `PASS`
- `positive_minimum_counts_observed`: `PASS`
- `positive_landed_flags_true`: `PASS`
- `positive_contract_violations_zero`: `PASS`
- `positive_ready_and_deterministic`: `PASS`
- `replay_key_covers_cross_module_axes`: `PASS`
- `negative_fixture_fails_closed`: `PASS`
- `negative_diagnostics_json_emitted`: `PASS`
- `negative_duplicate_module_diagnostic_observed`: `PASS`
- `semantic_manifest_indexes_xmod_8015_01`: `PASS`
- `semantic_manifest_indexes_xmod_8015_02`: `PASS`
- `semantic_readme_mentions_issue_8015`: `PASS`
- `semantic_readme_mentions_positive_fixture`: `PASS`
- `semantic_readme_mentions_negative_fixture`: `PASS`
- `positive_conformance_references_fixture`: `PASS`
- `negative_conformance_references_fixture`: `PASS`
- `negative_conformance_expects_o3s200_location`: `PASS`
- `stress_manifest_compiles_positive_fixture`: `PASS`
- `no_tmp_source_truth`: `PASS`
- `static_sources_thread_surface`: `PASS`

## Observed Positive Counts
- `module_import_graph_sites`: `4`
- `import_edge_candidate_sites`: `4`
- `namespace_segment_sites`: `4`
- `object_pointer_type_sites`: `7`
- `pointer_declarator_sites`: `4`
- `namespace_collision_shadowing_sites`: `4`
- `public_private_api_partition_sites`: `4`
- `incremental_module_cache_invalidation_sites`: `4`
- `cross_module_conformance_sites`: `4`
- `normalized_cross_module_sites`: `4`
- `interop_import_module_annotation_sites`: `1`
- `interop_imported_module_name_sites`: `1`

## Validation Commands
- `python scripts/build_objc3c_cross_module_semantic_contracts_diagnostics.py --check`
- `python -m pytest tests/tooling/test_build_objc3c_cross_module_semantic_contracts_diagnostics.py`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`
