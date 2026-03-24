# M272-D002 Packet: Live Dispatch Fast-Path And Cache Integration - Core Feature Implementation

Packet: `M272-D002`
Issue: `#7343`
Dependencies: `M272-D001`, `M272-C003`
Next issue: `M272-E001`

## Objective

Widen the truthful Part 9 live runtime so emitted direct/final/sealed dispatch intent pre-seeds deterministic cache entries and drives first-call cache-hit behavior on the native runtime path.

## Implementation requirements

1. Consume the emitted method-entry and class-record dispatch-intent metadata in the private runtime.
2. Rebuild the live method cache after registration with deterministic preseeded entries for safe implementation-backed direct/final/sealed methods.
3. Preserve direct-call IR behavior from `M272-C002`: exact LLVM direct calls still bypass runtime dispatch even when a matching seeded cache entry exists.
4. Preserve unresolved-selector fallback behavior and cache reuse.
5. Expose deterministic private runtime snapshot state for seeded fast-path entries, fast-path hits, and last fast-path reason.
6. Add one runnable fixture plus one runtime probe that prove baseline seeding, first-call cache hits for the dynamic opt-out path, and deterministic fallback continuity.
7. Add deterministic checker, pytest, package scripts, and lane-D readiness coverage.
8. Land stable evidence under `tmp/reports/m272/M272-D002/`.

## Truth constraints

- Do not change the public runtime ABI.
- Do not claim optimizer-led devirtualization.
- Do not claim broad dynamic-receiver direct dispatch lowering beyond the already-landed IR lowering slice.
- Preserve imported direct-surface artifact paths as the cross-module provenance source for this runtime widening.
