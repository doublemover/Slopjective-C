# M272 Metadata And Interface Preservation For Dynamism Controls Expectations (C003)

Issue: `#7341`
Contract ID: `objc3c-part9-dispatch-metadata-interface-preservation/m272-c003-v1`

## Required behavior

- Runtime metadata source records must preserve:
  - method-level effective direct-dispatch intent
  - method-level `objc_final` intent
  - class-level `objc_final` / `objc_sealed` intent
- Emitted `module.runtime-import-surface.json` artifacts must carry one dedicated Part 9 preservation packet at `frontend.pipeline.semantic_surface.objc_part9_dispatch_metadata_and_interface_preservation`.
- Imported runtime surfaces must reload the same Part 9 preservation packet and the widened class/method source-record fields.
- Emitted LLVM IR must publish one replay-stable Part 9 preservation summary above the `M272-C002` direct-call lowering slice.

## Deliberate bounds

- This issue does not widen the runnable dispatch boundary beyond `M272-C002`.
- This issue does not claim broad dynamic-receiver direct dispatch.
- This issue does not add a new public runtime ABI.

## Positive proof

- A provider fixture must emit `module.runtime-import-surface.json`, `module.manifest.json`, `module.ll`, and `module.obj`.
- A consumer fixture compiled with `--objc3-import-runtime-surface <provider/module.runtime-import-surface.json>` must emit the same artifact family and preserve imported Part 9 metadata truthfully.
- The provider and consumer artifacts must prove:
  - local direct/final/sealed Part 9 intent survives runtime metadata source-record serialization
  - imported Part 9 intent survives runtime-import-surface reload
  - the consumer replay packet counts imported modules and preserved records deterministically
  - LLVM IR publishes the preservation replay key and named metadata node
