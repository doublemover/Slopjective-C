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
- `throws_callable`: `PASS`; replay `throws_callables`=`2`; negative fixtures=`1`
- `async_callable`: `PASS`; replay `async_callables`=`5`; negative fixtures=`1`
- `await_expression`: `PASS`; replay `await`=`3`; negative fixtures=`1`
- `actor_interface`: `PASS`; replay `actors`=`1`; negative fixtures=`1`
- `macro_attribute`: `PASS`; replay `macro_attrs`=`1`; negative fixtures=`1`
- `macro_package`: `PASS`; replay `macro_packages`=`1`; negative fixtures=`1`
- `macro_provenance`: `PASS`; replay `macro_provenance`=`1`; negative fixtures=`1`
- `property_behavior`: `PASS`; replay `property_behaviors`=`4`; negative fixtures=`1`
- `interop_attribute`: `PASS`; replay `interop_attrs`=`2`; negative fixtures=`6`

## Validation Commands
- `python scripts/build_objc3c_parser_draft_syntax_conformance.py --check`
- `python -m pytest tests/tooling/test_build_objc3c_parser_draft_syntax_conformance.py`
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`
