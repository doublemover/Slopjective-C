# M261-C003 Byref-Cell Copy-Helper And Dispose-Helper Lowering Core Feature Implementation Packet

Packet: `M261-C003`
Milestone: `M261`
Wave: `W53`
Lane: `C`
Issue: `#7187`
Contract ID: `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
Dependencies: `M261-C002`, `M261-B003`

## Objective

Implement the first truthful runnable lane-C block lowering path for local nonescaping byref mutation and ownership-sensitive captures by emitting stack byref cells plus copy/dispose helper bodies.

## Canonical Scope

- contract id `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
- active model `native-lowering-emits-stack-byref-cells-and-copy-dispose-helper-bodies-for-nonescaping-block-captures`
- deferred model `heap-promotion-and-runtime-managed-block-copy-dispose-lifecycle-remain-deferred-until-m261-c004-and-m261-d002`
- execution evidence model `native-compile-link-run-proves-byref-mutation-and-owned-capture-helper-lowering-through-emitted-block-helper-bodies`
- preserved lower-layer contracts:
  - `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
  - `objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1`
  - `m168-block-storage-escape-lowering-v1`
  - `m169-block-copy-dispose-lowering-v1`
  - `m261-block-byref-helper-lowering-v1`

## Acceptance Criteria

- Implement byref-cell, copy-helper, and dispose-helper lowering as a real compiler/runtime capability rather than a manifest-only summary.
- Ensure the runnable local block path now supports:
  - readonly scalar captures
  - byref mutation through emitted stack byref cells
  - owned object captures through emitted helper bodies
  - weak/unowned captures through helper-elided pointer-capture storage.
- Prove compile/link/run over the canonical runtime fixtures with expected exits `15`, `14`, `11`, and `9`.
- Keep escaping heap-promotion/runtime hook work deferred to `M261-C004`.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Baseline local invoke proof:
   - fixture `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3`
   - expected run exit `15`.
2. Byref/runtime helper proof:
   - fixture `tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3`
   - expected run exit `14`
   - IR must carry `@__objc3_block_copy_helper_*` and `@__objc3_block_dispose_helper_*`
   - manifest lowering surfaces must publish `byref_layout_symbolized_sites >= 1`.
3. Owned capture helper proof:
   - fixture `tests/tooling/fixtures/native/m261_owned_object_capture_runtime_positive.objc3`
   - expected run exit `11`
   - IR must carry helper calls to `@objc3_runtime_retain_i32` and `@objc3_runtime_release_i32`.
4. Non-owning capture proof:
   - fixture `tests/tooling/fixtures/native/m261_nonowning_object_capture_runtime_positive.objc3`
   - expected run exit `9`
   - manifest must prove copy/dispose helpers remain elided.

## Non-Goals

- escaping block heap-promotion/runtime hook lowering
- first-class escaping block values
- runtime-managed block allocation/copy semantics outside the local nonescaping slice

## Validation Commands

- `python scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py -q`
- `npm run check:objc3c:m261-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C003/byref_cell_copy_dispose_helper_lowering_summary.json`
- `M261-C004` is the explicit next issue after this implementation lands.
