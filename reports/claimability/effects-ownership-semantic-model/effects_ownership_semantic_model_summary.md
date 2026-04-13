# Effects Ownership Semantic Model

- Contract: `objc3c.effects.ownership.semantic.model.closure.v1`
- Status: `PASS`
- Issue: `#8014`
- Positive fixture: `tests/tooling/fixtures/native/effects_ownership_semantic_model_positive.objc3`
- Negative fixture: `tests/tooling/fixtures/native/recovery/negative/negative_effects_ownership_async_throws.objc3`

## Checks
- `positive_fixture_compiles`: `PASS`
- `positive_manifest_emitted`: `PASS`
- `positive_llvm_ir_emitted`: `PASS`
- `manifest_has_effects_ownership_surface`: `PASS`
- `all_summary_fields_emitted`: `PASS`
- `positive_minimum_counts_observed`: `PASS`
- `positive_landed_flags_true`: `PASS`
- `positive_contract_violations_zero`: `PASS`
- `positive_ready_and_deterministic`: `PASS`
- `replay_key_covers_effects_axes`: `PASS`
- `negative_fixture_fails_closed`: `PASS`
- `negative_diagnostics_json_emitted`: `PASS`
- `negative_async_throws_diagnostic_observed`: `PASS`
- `semantic_manifest_indexes_eff_8014_01`: `PASS`
- `semantic_manifest_indexes_eff_8014_02`: `PASS`
- `semantic_readme_mentions_issue_8014`: `PASS`
- `semantic_readme_mentions_positive_fixture`: `PASS`
- `semantic_readme_mentions_negative_fixture`: `PASS`
- `positive_conformance_references_fixture`: `PASS`
- `negative_conformance_references_fixture`: `PASS`
- `negative_conformance_expects_o3s226_location`: `PASS`
- `stress_manifest_compiles_positive_fixture`: `PASS`
- `no_tmp_source_truth`: `PASS`
- `static_sources_thread_surface`: `PASS`

## Observed Positive Counts
- `arc_ownership_qualified_sites`: `2`
- `retain_insertion_sites`: `1`
- `release_insertion_sites`: `1`
- `weak_zeroing_sites`: `1`
- `autoreleasepool_scope_sites`: `2`
- `cleanup_order_exit_sites`: `2`
- `block_literal_sites`: `1`
- `stack_to_heap_promotion_sites`: `1`
- `byref_forwarding_cell_sites`: `1`
- `copy_helper_required_sites`: `1`
- `dispose_helper_required_sites`: `1`
- `captured_object_lifetime_sites`: `3`
- `throws_propagation_sites`: `2`
- `bridged_error_sites`: `8`
- `foreign_boundary_sites`: `7`
- `async_continuation_sites`: `19`
- `cancellation_propagation_sites`: `1`
- `reentrancy_policy_sites`: `1`
- `imported_actor_api_sites`: `1`

## Validation Commands
- `python scripts/build_objc3c_effects_ownership_semantic_model.py --check`
- `python -m pytest tests/tooling/test_build_objc3c_effects_ownership_semantic_model.py`
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`
