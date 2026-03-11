# M261-A002 Block Literal Signature Capture Inventory And Invoke Surface Source Modeling Core Feature Implementation Packet

Issue: `#7180`  
Packet: `M261-A002`  
Milestone: `M261`  
Lane: `A`

## Scope

Upgrade the truthful `M261-A001` block source closure into a source-only
frontend capability that models:

- typed and untyped parameter signatures,
- capture inventory with explicit storage class truth,
- invoke-surface symbols and replay keys.

Runnable block lowering remains out of scope here.

## Required code anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/checker assets

- `docs/contracts/m261_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_a002_expectations.md`
- `scripts/check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
- `tests/tooling/test_check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
- `tests/tooling/fixtures/native/m261_block_source_model_completion_positive.objc3`

## Acceptance

- block literal signature, capture inventory, and invoke-surface source
  modeling land as a real compiler capability.
- source-only frontend runs admit the positive fixture and emit the completed
  source-model manifest surface.
- native emit paths still fail closed with `O3S221`.
- evidence lands under `tmp/reports/m261/M261-A002/`.

## Next handoff

`M261-B001` is the explicit next issue after this implementation lands.
