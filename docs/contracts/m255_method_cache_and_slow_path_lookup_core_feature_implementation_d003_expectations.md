# M255 Method Cache and Slow-Path Lookup Core Feature Implementation Expectations (D003)

Contract ID: `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`

`M255-D003` turns the deferred runtime slow path from `M255-D001` into a real
method-cache capability backed by emitted class/metaclass records, callable
implementation pointers, and deterministic cache snapshots.

## Required anchors

- `objc3_runtime_copy_method_cache_state_for_testing`
- `objc3_runtime_copy_method_cache_entry_for_testing`
- `known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key`
- `registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution`
- `normalized-receiver-plus-selector-stable-id-positive-and-negative-cache`
- `unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula`

## Behavioral requirements

1. Known-class receivers and class-self receivers must normalize onto one
   metaclass cache identity.
2. The first live lookup for a supported selector must walk emitted
   class/metaclass metadata and resolve a real emitted implementation pointer.
3. Repeat live dispatch must reuse a positive cache entry rather than walking
   the emitted metadata again.
4. Unsupported or unresolved selectors must populate a negative cache entry and
   continue to return the compatibility fallback result.
5. Repeat unresolved lookup must reuse the negative cache entry deterministically.
6. The emitted IR/object payload must expose the live method-cache / slow-path
   contract and real method-table implementation pointers.

## Proof surface

- The checker compiles
  `tests/tooling/fixtures/native/m255_d003_live_method_dispatch.objc3` through
  the native frontend.
- The probe links the emitted object with `artifacts/lib/objc3_runtime.lib` and
  proves:
  - instance dispatch resolves a live method body and then hits cache,
  - class-self dispatch resolves a live class method body,
  - known-class dispatch reuses the same metaclass cache key,
  - unresolved lookup populates and reuses a negative cache entry,
  - IR/object artifacts carry the live D003 metadata anchors.
