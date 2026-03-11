# M261-B002 Capture Legality Escape Classification And Invocation Typing Core Feature Implementation Packet

Issue: `#7183`  
Packet: `M261-B002`  
Milestone: `M261`  
Lane: `B`

## Scope

Implement real source-only semantic enforcement for block capture legality,
truthful escape classification, and local block invocation typing while keeping
the native block-runtime boundary fail closed.

## Required code anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`

## Required docs/checker assets

- `docs/contracts/m261_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation_b002_expectations.md`
- `scripts/check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py`
- `tests/tooling/test_check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py`
- `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_positive.objc3`
- `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_missing_capture.objc3`
- `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_bad_call.objc3`

## Acceptance

- source-only frontend runs reject undefined captures with `O3S202`.
- source-only frontend runs reject invocation type mismatches with `O3S206`.
- truthful block escape/copy-dispose lowering handoff remains deterministic.
- native emit still fails closed with `O3S221`.
- evidence lands under `tmp/reports/m261/M261-B002/`.

## Next handoff

`M261-B003` is the explicit next issue after this implementation lands.
