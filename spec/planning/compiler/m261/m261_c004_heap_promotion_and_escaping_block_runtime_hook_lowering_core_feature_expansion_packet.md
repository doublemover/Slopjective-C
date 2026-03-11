# M261-C004 Heap-Promotion And Escaping-Block Runtime Hook Lowering Core Feature Expansion Packet

Packet: `M261-C004`
Milestone: `M261`
Wave: `W53`
Lane: `C`
Issue: `#7188`
Contract ID: `objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1`
Dependencies: `M261-C003`, `M261-B003`

## Objective

Expand runnable block lowering so readonly scalar block values can escape
through call arguments or return values by lowering private runtime
heap-promotion and invoke hooks.

## Canonical Scope

- contract id `objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1`
- active model `native-lowering-emits-runtime-block-promotion-and-invoke-hooks-for-readonly-scalar-escaping-block-values`
- deferred model `byref-forwarding-owned-capture-escape-lifetimes-and-runtime-managed-copy-dispose-remain-deferred-until-m261-d002-and-m261-d003`
- execution evidence model `native-compile-link-run-proves-returned-and-argument-passed-readonly-scalar-block-values-through-runtime-promotion-hooks`
- preserved lower-layer contracts:
  - `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
  - `objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1`
  - `m168-block-storage-escape-lowering-v1`
  - `m169-block-copy-dispose-lowering-v1`
  - `m261-block-runtime-semantic-rules-v1`
  - `m261-block-escape-runtime-hook-lowering-v1`

## Acceptance Criteria

- Implement escaping readonly-scalar block promotion and invoke lowering as a
  real compiler/runtime capability.
- Prove call-argument escape lowering over a canonical positive fixture with
  exit `14`.
- Prove return-value escape lowering over a canonical positive fixture with
  exit `0`.
- Keep byref-forwarded and owned-object escaping blocks fail-closed with
  `O3L300`.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Call-argument promotion proof:
   - fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
   - expected run exit `14`
   - IR must carry
     `@objc3_runtime_promote_block_i32`,
     `@objc3_runtime_invoke_block_i32`, and
     `; executable_block_escape_runtime_hook_lowering = ...`
2. Return-value promotion proof:
   - fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_return_positive.objc3`
   - expected run exit `0`
   - IR must carry `@objc3_runtime_promote_block_i32`
3. Byref fail-closed proof:
   - fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_byref_negative.objc3`
   - expected compile failure with `O3L300`
4. Owned-capture fail-closed proof:
   - fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_owned_capture_negative.objc3`
   - expected compile failure with `O3L300`

## Non-Goals

- public block-object ABI freeze
- runtime-managed block copy/dispose allocation semantics
- byref-forwarded escaping blocks
- owned-object escaping block lifetimes

## Validation Commands

- `python scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py -q`
- `npm run check:objc3c:m261-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m261/M261-C004/escaping_block_runtime_hook_lowering_summary.json`
- `M261-D001` is the explicit next issue after this implementation lands.
