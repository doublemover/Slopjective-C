# M261-C002 Executable Block Object And Invoke-Thunk Lowering Core Feature Implementation Packet

Packet: `M261-C002`
Milestone: `M261`
Wave: `W53`
Lane: `C`
Issue: `#7186`
Contract ID: `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
Dependencies: `M261-C001`, `M261-A002`, `M261-B002`

## Objective

Implement one real runnable lane-C block lowering slice: stack block object emission, internal invoke-thunk emission, and direct local invocation for readonly scalar captures.

## Canonical Scope

- contract id `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
- active model `native-lowering-emits-stack-block-objects-and-direct-local-invoke-thunks-for-readonly-scalar-captures`
- deferred model `byref-cells-copy-dispose-helpers-owned-object-captures-and-heap-promotion-stay-fail-closed-until-m261-c003`
- execution evidence model `native-compile-link-run-proves-local-block-invocation-through-emitted-block-storage-and-invoke-thunk`
- preserved lower-layer contracts:
  - `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
  - `m167-block-abi-invoke-trampoline-lowering-v1`
  - `m168-block-storage-escape-lowering-v1`
  - `m169-block-copy-dispose-lowering-v1`
  - `m261-block-object-invoke-thunk-lowering-v1`

## Acceptance Criteria

- Emit one stack block object allocation plus captured scalar slots for the supported slice.
- Emit one internal invoke thunk definition per runnable block literal.
- Lower local direct invocation through the stored thunk pointer instead of failing closed.
- Prove compile/link/run over a canonical positive fixture with expected exit `15`.
- Keep byref/helper/ownership-sensitive cases fail-closed and covered by deterministic negative probes.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Positive probe:
   - fixture `tests/tooling/fixtures/native/m261_executable_block_object_invoke_thunk_positive.objc3`
   - compile with `artifacts/bin/objc3c-native.exe`
   - link with:
     - emitted `module.obj`
     - emitted `module.runtime-registration-manifest.json`
     - emitted `module.runtime-metadata-linker-options.rsp`
     - runtime archive from the registration manifest
   - run `module.exe`
   - expected exit `15`.
2. Deferred byref probe:
   - fixture `tests/tooling/fixtures/native/m261_capture_legality_escape_invocation_positive.objc3`
   - expected native compile failure with `O3S221`.
3. Deferred helper/ownership probe:
   - fixture `tests/tooling/fixtures/native/m261_owned_object_capture_helper_positive.objc3`
   - expected native compile failure with `O3S221`.

## Non-Goals

- byref-cell runtime lowering
- copy-helper bodies
- dispose-helper bodies
- ownership-qualified object capture runtime lowering
- escaping heap-promoted blocks

## Validation Commands

- `python scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py -q`
- `npm run check:objc3c:m261-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C002/executable_block_object_invoke_thunk_lowering_summary.json`
- `M261-C003` is the explicit next issue after this implementation lands.
