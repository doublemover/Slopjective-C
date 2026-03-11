# M261 Heap-Promotion And Escaping-Block Runtime Hook Lowering Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1`
Issue: `#7188`
Status: Accepted
Dependencies: `M261-C003`, `M261-B003`
Scope: M261 lane-C heap-promotion and escaping-block runtime hook lowering core feature expansion.

## Objective

Fail closed unless lane-C widens escaping block lowering truthfully:
readonly scalar block values may escape through call arguments or return values
via private runtime promotion and invoke hooks, while pointer-managed escaping
captures remain deferred.

## Architecture and Spec Anchors

- `docs/objc3c-native/src/20-grammar.md` documents the active runnable
  escaping-block slice and its deferred cases.
- `docs/objc3c-native.md` republishes the generated active slice summary.
- `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md` carries the baseline
  status wording for `M261-C004`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` freezes the emitted IR/runtime hook
  surface for `M261-C004`.
- `native/objc3c/src/ARCHITECTURE.md` records the implementation boundary and
  next-issue handoff to `M261-D001`.

## Code Anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m261-c004-block-escape-runtime-hook-lowering`.
- `package.json` includes
  `test:tooling:m261-c004-block-escape-runtime-hook-lowering`.
- `package.json` includes `check:objc3c:m261-c004-lane-c-readiness`.

## Validation

- `python scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py -q`
- `npm run check:objc3c:m261-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C004/escaping_block_runtime_hook_lowering_summary.json`

## Next Issue

- `M261-D001`
