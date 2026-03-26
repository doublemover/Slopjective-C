# M315-C003 Planning Packet

Issue: `#7803`  
Title: `Genuine replay artifact regeneration and provenance capture`

## Objective

Implement a truthful provenance bridge over the replay artifact surface by generating
one fresh replay artifact with a full authenticity envelope and classifying the older
committed replay corpus as legacy bridge material until it is regenerated.

## Implementation summary

This issue does not pretend the historical replay corpus is already genuine. Instead
it:
- compiles a fresh native fixture through the real frontend runner;
- captures a `generated_replay` envelope for the fresh output;
- emits a legacy bridge registry over the committed replay corpus with per-entry
  validation recipes derived from the existing validation tests;
- keeps the historical replay corpus explicitly bridge-only.

## Validation posture

The checker must:
- compile the positive fixture through `artifacts/bin/objc3c-frontend-c-api-runner.exe`;
- verify `module.ll`, `module.manifest.json`, and the generated authenticity envelope;
- scan the committed replay corpus and produce deterministic bridge counts;
- verify the bridge registry entries use the `legacy_generated_replay_bridge` class and
  reference existing validation tests;
- reject any attempt to count the legacy bridge corpus as genuine replay proof.

Next issue: `M315-C004`.
