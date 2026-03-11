# M261 Capture Legality Escape Classification And Invocation Typing Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-executable-block-capture-legality-escape-and-invocation/m261-b002-v1`

## Objective

Turn the frozen block semantic boundary into a real source-only semantic
capability by enforcing live capture legality, truthful escape classification,
and local block invocation typing without widening native runnable block
support.

## Required implementation

1. Add this expectations document, a packet, a deterministic checker, a pytest
   wrapper, and a direct lane-B readiness runner.
2. Add `M261-B002` anchors to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
3. The implementation must make these behaviors real in source-only sema:
   - undefined block captures fail with `O3S202`
   - local block invocation argument mismatches fail with `O3S206`
   - truthful mutable-capture, byref, escape, and copy/dispose helper counts
     stay deterministic in the lowering handoff
4. The implementation must not widen native runnable block support. Native emit
   must still fail closed with `O3S221`.
5. Add deterministic proof fixtures for:
   - a positive source-only local block invocation case
   - an undefined-capture rejection case
   - an invocation type-mismatch rejection case
6. `package.json` must wire:
   - `check:objc3c:m261-b002-capture-legality-escape-invocation`
   - `test:tooling:m261-b002-capture-legality-escape-invocation`
   - `check:objc3c:m261-b002-lane-b-readiness`
7. The contract must explicitly hand off to `M261-B003`.

## Canonical models

- Capture legality model:
  `source-only-sema-enforces-live-capture-resolution-and-mutability-classification-before-runnable-block-object-lowering`
- Escape classification model:
  `source-only-sema-classifies-byref-escape-and-copy-dispose-requirements-from-parser-owned-annotations-before-runnable-helper-lowering`
- Invocation typing model:
  `source-only-sema-types-local-block-invocations-as-callable-values-while-native-block-execution-remains-fail-closed`

## Non-goals

- No runnable block-object execution.
- No runnable heap promotion or helper emission.
- No byref cell lowering beyond source-only legality/classification.
- No change to the native fail-closed `O3S221` boundary.

## Evidence

- `tmp/reports/m261/M261-B002/capture_legality_escape_classification_and_invocation_typing_summary.json`
