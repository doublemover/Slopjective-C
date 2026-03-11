# M261 Byref-Cell Copy-Helper And Dispose-Helper Lowering Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
Status: Accepted
Issue: `#7187`
Scope: Expand runnable lane-C block lowering from the `M261-C002` readonly-scalar slice to emitted stack byref cells plus helper bodies for local nonescaping byref and ownership-sensitive block captures, while leaving true escaping heap-promotion to `M261-C004`.

## Objective

Land one truthful runnable lane-C capability: native lowering now emits stack byref-cell storage plus copy/dispose helper bodies so local block bindings with byref mutation, owned captures, and weak/unowned captures compile, link, and execute through the in-tree runtime.

## Required Invariants

1. `native/objc3c/src/ast/objc3_ast.h` remains the canonical declaration point for:
   - `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
   - active model `native-lowering-emits-stack-byref-cells-and-copy-dispose-helper-bodies-for-nonescaping-block-captures`
   - deferred model `heap-promotion-and-runtime-managed-block-copy-dispose-lifecycle-remain-deferred-until-m261-c004-and-m261-d002`
   - execution evidence model `native-compile-link-run-proves-byref-mutation-and-owned-capture-helper-lowering-through-emitted-block-helper-bodies`.
2. `native/objc3c/src/sema/objc3_semantic_passes.cpp` admits the runnable `M261-C003` slice without depending on post-sema counters at the unsupported-feature gate, and it must publish the live storage/helper fields required by lowering:
   - `block_storage_mutable_capture_count`
   - `block_storage_byref_slot_count`
   - `block_storage_requires_byref_cells`
   - `block_storage_byref_layout_symbol`
   - `block_runtime_copy_helper_required`
   - `block_runtime_dispose_helper_required`
   - `block_runtime_capture_ownership_profile`.
3. `native/objc3c/src/lower/objc3_lowering_contract.cpp` and `native/objc3c/src/ir/objc3_ir_emitter.cpp` now make the live runtime path truthful for the nonescaping slice:
   - pointer-capture storage is emitted when byref or object captures are present
   - stack byref layout symbols are emitted for byref captures
   - copy/dispose helper bodies are emitted for owned/byref paths
   - weak/unowned captures stay non-owning and helper-elided
   - true escaping heap-promotion/runtime hook lowering remains deferred.
4. Native IR carries the dedicated emitted summary line:
   - `; executable_block_byref_helper_lowering = ...`
5. Code/spec anchors remain explicit and deterministic in:
   - `docs/objc3c-native.md`
   - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `package.json`.

## Dynamic Coverage

1. Baseline runnable slice stays live:
   - compile/link/run over `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3` succeeds with exit `15`.
2. Byref lowering happy path succeeds:
   - compile/link/run over `tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3` succeeds with exit `14`
   - IR proves emitted `@__objc3_block_copy_helper_*` and `@__objc3_block_dispose_helper_*`
   - manifest lowering surfaces prove byref layout symbols are published deterministically.
3. Owned object-capture helper lowering happy path succeeds:
   - compile/link/run over `tests/tooling/fixtures/native/m261_owned_object_capture_runtime_positive.objc3` succeeds with exit `11`
   - IR proves helper bodies call `@objc3_runtime_retain_i32` and `@objc3_runtime_release_i32`.
4. Weak/unowned capture happy path succeeds without ownership helpers:
   - compile/link/run over `tests/tooling/fixtures/native/m261_nonowning_object_capture_runtime_positive.objc3` succeeds with exit `9`
   - manifest lowering surfaces prove copy/dispose helpers stay elided.
5. Every positive runtime probe must also prove:
   - compile exit `0`
   - `module.object-backend.txt` is `llvm-direct`
   - `module.obj` exists and is non-empty
   - `module.runtime-registration-manifest.json` exists
   - `module.runtime-metadata-linker-options.rsp` exists.

## Non-Goals And Fail-Closed Rules

- `M261-C003` does not implement escaping block heap-promotion/runtime hooks.
- `M261-C003` does not make escaping block values first-class returnable or passable runtime objects.
- `M261-C003` does not implement runtime-managed block allocation/copy semantics outside the local nonescaping slice.
- Those deferred cases remain the responsibility of `M261-C004` and later lane-D work.

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m261-c003-byref-copy-dispose-helper-lowering`.
- `package.json` includes `test:tooling:m261-c003-byref-copy-dispose-helper-lowering`.
- `package.json` includes `check:objc3c:m261-c003-lane-c-readiness`.

## Validation

- `python scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py -q`
- `npm run check:objc3c:m261-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C003/byref_cell_copy_dispose_helper_lowering_summary.json`
