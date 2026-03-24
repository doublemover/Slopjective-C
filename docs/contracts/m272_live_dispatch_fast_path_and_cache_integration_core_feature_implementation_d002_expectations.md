# M272 Live Dispatch Fast-Path And Cache Integration Expectations (D002)

Issue: `#7343`
Contract ID: `objc3c-part9-live-dispatch-fast-path-and-cache-integration/m272-d002-v1`

## Required behavior

- Part 9 must widen the live runtime so direct, final, and sealed dispatch intent affects the runtime cache before the first live send executes.
- Registration-time class-graph rebuild must preseed deterministic cache entries for eligible implementation-backed methods whose emitted metadata proves direct, final, or sealed dispatch intent.
- Direct `objc_direct` sends lowered by `M272-C002` must remain exact LLVM calls and therefore must not perturb live-dispatch counters even when matching runtime fast-path cache entries already exist.
- `objc_dynamic` opt-out sends on final or sealed owners must hit the preseeded runtime cache on the first live dispatch instead of forcing a slow-path lookup.
- Unresolved selectors must continue to take the deterministic cached fallback path.
- Cross-module runtime link planning must retain imported direct-surface artifact paths so the same runtime fast-path seeding model remains available after multi-image registration.

## Deliberate bounds

- This issue does not reopen the public runtime ABI.
- This issue does not claim optimizer-led devirtualization.
- This issue does not claim broad dynamic-receiver direct dispatch lowering beyond the already-landed IR lowering slice.
- The next issue remains `M272-E001`.

## Positive proof

- `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3` must compile into `module.ll`, `module.obj`, and `module.runtime-registration-manifest.json`.
- `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp` must prove:
  - baseline runtime state already contains preseeded fast-path cache entries
  - the seeded entry for `dynamicEscape` is resolved and marked as class-final/class-sealed fast-path state
  - direct wrappers still leave runtime dispatch counters unchanged
  - the first mixed direct-plus-dynamic execution uses the preseeded cache and avoids a slow-path lookup
  - unresolved selectors still create one deterministic fallback cache entry and reuse it on the second call
