# M272 Runtime Fast-Path Integration Contract Expectations (D001)

Issue: `#7342`
Contract ID: `objc3c-part9-runtime-fast-path-integration-contract/m272-d001-v1`

## Required behavior

- Part 9 must freeze the current runtime dispatch boundary above the already-landed direct-call lowering slice.
- Direct `objc_direct` sends lowered by `M272-C002` must remain exact LLVM calls and therefore must not perturb the runtime method-cache state.
- `objc_dynamic` opt-out sends and unresolved selectors must continue to execute through:
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- Cross-module runtime link planning must continue to retain imported direct-surface artifact paths as part of the same frozen runtime contract.

## Deliberate bounds

- This issue does not widen the live Part 9 runtime fast path beyond the current method-cache / slow-path / fallback engine.
- This issue does not claim optimizer-led devirtualization or broad dynamic-receiver direct dispatch.
- The next issue remains `M272-D002`.

## Positive proof

- `tests/tooling/fixtures/native/m272_d001_runtime_fast_path_contract_positive.objc3` must compile into `module.ll`, `module.obj`, and `module.runtime-registration-manifest.json`.
- `tests/tooling/runtime/m272_d001_runtime_fast_path_contract_probe.cpp` must prove:
  - direct wrappers leave the runtime method-cache state unchanged
  - mixed direct-plus-dynamic execution creates one positive cache entry and reuses it on the second call
  - unresolved selectors take the deterministic cached fallback path
