# Object Model IR Lowering Closure

- Contract: `objc3c.object-model-ir-lowering-closure.v1`
- Status: `PASS`
- Issue: `#8016`
- Positive fixture: `tests/tooling/fixtures/native/recovery/dispatch/parser_container_inherited_ivar_layout.objc3`
- Negative fixture: `tests/tooling/fixtures/native/recovery/negative/negative_parser_container_ivar_layout_cycle.objc3`
- Source truth avoids tmp: `True`

## Layout Offsets
- `baseCount`: `4`
- `enabled`: `0`
- `childFlag`: `16`
- `token`: `8`

## Checks
- `descriptor_model_expanded`: `PASS`
- `offset_global_entries`: `PASS`
- `layout_table_entries`: `PASS`
- `layout_owner_entries`: `PASS`
- `child_owner_table_size`: `PASS`
- `ir_bundle_fields`: `PASS`
- `ir_emitter_uses_sema_offsets`: `PASS`
- `runtime_consumes_layout_payload`: `PASS`
- `contract_strings`: `PASS`
- `stress_manifest`: `PASS`
- `conformance`: `PASS`

## Validation Commands
- `python scripts/build_objc3c_object_model_ir_lowering_closure.py --check`
- `python -m pytest tests/tooling/test_build_objc3c_object_model_ir_lowering_closure.py`
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`
