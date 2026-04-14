# Parser Draft Syntax Conformance

- Contract: `objc3c.parser.draft-syntax-conformance.v1`
- Status: `PASS`
- Issue: `#8012`
- Positive fixture: `tests/tooling/fixtures/native/recovery/dispatch/parser_draft_syntax_surfaces.objc3`

## Checks
- `manifest_contract_matches`: `PASS`
- `all_required_surfaces_listed`: `PASS`
- `no_unknown_surfaces_listed`: `PASS`
- `positive_fixture_compiles`: `PASS`
- `positive_manifest_has_parser_surface`: `PASS`
- `positive_manifest_deterministic`: `PASS`
- `positive_manifest_total_covers_surface_counts`: `PASS`
- `parser_has_property_behavior_payload_guard`: `PASS`
- `readme_references_conformance_manifest`: `PASS`
- `no_tmp_source_truth`: `PASS`
- `all_surface_results_pass`: `PASS`

## Surface Coverage
- `block_literal`: `PASS`; replay `blocks`=`1`; negative fixtures=`1`
- `try_expression`: `PASS`; replay `try`=`2`; negative fixtures=`1`
- `throw_statement`: `PASS`; replay `throw`=`1`; negative fixtures=`1`
- `do_catch`: `PASS`; replay `do_catch`=`1`; negative fixtures=`1`
- `error_catch_clause`: `PASS`; replay `error_catch_clauses`=`2`; negative fixtures=`1`
- `error_bridge_payload`: `PASS`; replay `error_bridge_payloads`=`3`; negative fixtures=`1`
- `error_foreign_boundary`: `PASS`; replay `error_foreign_boundaries`=`1`; negative fixtures=`1`
- `error_nested_cleanup_marker`: `PASS`; replay `error_nested_cleanup_markers`=`1`; negative fixtures=`1`
- `throws_callable`: `PASS`; replay `throws_callables`=`2`; negative fixtures=`1`
- `async_callable`: `PASS`; replay `async_callables`=`7`; negative fixtures=`1`
- `await_expression`: `PASS`; replay `await`=`5`; negative fixtures=`1`
- `actor_interface`: `PASS`; replay `actors`=`1`; negative fixtures=`1`
- `macro_attribute`: `PASS`; replay `macro_attrs`=`1`; negative fixtures=`1`
- `macro_package`: `PASS`; replay `macro_packages`=`1`; negative fixtures=`1`
- `macro_provenance`: `PASS`; replay `macro_provenance`=`1`; negative fixtures=`1`
- `macro_cache_key`: `PASS`; replay `macro_cache_keys`=`1`; negative fixtures=`1`
- `macro_sandbox_policy`: `PASS`; replay `macro_sandbox_policies`=`1`; negative fixtures=`1`
- `property_behavior`: `PASS`; replay `property_behaviors`=`4`; negative fixtures=`1`
- `property_attribute_metadata`: `PASS`; replay `property_attrs`=`20`; negative fixtures=`1`
- `property_accessor_metadata`: `PASS`; replay `property_accessor_selectors`=`4`; negative fixtures=`1`
- `property_synthesis_metadata`: `PASS`; replay `property_synthesis_metadata`=`4`; negative fixtures=`1`
- `property_reflection_input`: `PASS`; replay `property_reflection_inputs`=`16`; negative fixtures=`1`
- `property_ownership_nullability`: `PASS`; replay `property_ownership_nullability`=`8`; negative fixtures=`1`
- `interop_attribute`: `PASS`; replay `interop_attrs`=`2`; negative fixtures=`6`
- `interop_import_module`: `PASS`; replay `interop_import_modules`=`1`; negative fixtures=`1`
- `interop_swift_annotation`: `PASS`; replay `interop_swift_annotations`=`2`; negative fixtures=`1`
- `interop_cxx_annotation`: `PASS`; replay `interop_cxx_annotations`=`1`; negative fixtures=`1`
- `interop_header_import`: `PASS`; replay `interop_header_imports`=`1`; negative fixtures=`1`
- `interop_error_bridge`: `PASS`; replay `interop_error_bridges`=`1`; negative fixtures=`1`

## Validation Commands
- `python scripts/build_objc3c_parser_draft_syntax_conformance.py --check`
- `python -m pytest tests/tooling/test_build_objc3c_parser_draft_syntax_conformance.py`
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`
