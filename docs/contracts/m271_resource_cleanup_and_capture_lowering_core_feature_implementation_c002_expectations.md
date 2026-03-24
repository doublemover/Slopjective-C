# M271 Resource Cleanup And Capture Lowering Expectations (C002)

Contract ID: `objc3c-part8-resource-cleanup-and-capture-lowering/m271-c002-v1`

## Required Surface

- Keep `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract` as the single Part 8 lowering surface.
- Do not mint a second Part 8 lowering manifest packet for `M271-C002`.
- Add explicit `M271-C002` code/spec anchors describing the live implementation boundary.

## Live Capability Boundary

- Native lowering must emit real cleanup/resource helper traffic for the supported non-promoting path.
- A positive fixture must compile through `objc3c-native.exe` and emit:
  - `module.ll`
  - `module.obj`
  - `module.manifest.json`
  - `module.runtime-metadata.bin`
- The emitted IR must prove:
  - cleanup/resource helper declarations are present
  - a block dispose helper is emitted
  - the block dispose helper performs cleanup-hook calls
  - lexical cleanup/resource unwind still emits the resource close path

## Fail-Closed Boundary

- Escaping promotion of move-based cleanup/resource captures must remain fail-closed.
- A negative fixture that forces block-handle promotion must stop with `O3L300` and the existing ownership-transfer diagnostic.

## Truth Constraints

- This issue implements the live supported lowering slice.
- This issue still does not claim:
  - runtime ownership transfer for escaping move captures
  - borrowed lifetime runtime interop
  - runnable retainable-family execution behavior
