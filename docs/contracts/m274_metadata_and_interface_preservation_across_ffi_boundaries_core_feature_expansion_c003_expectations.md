# M274 Metadata And Interface Preservation Across FFI Boundaries Core Feature Expansion Expectations (C003)

Issue: `#7369`
Contract ID: `objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1`
Canonical manifest surface: `frontend.pipeline.semantic_surface.objc_part11_ffi_metadata_and_interface_preservation`
Canonical import-artifact member: `objc_part11_ffi_metadata_and_interface_preservation`

## Required outcomes

- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_ffi_metadata_and_interface_preservation`
- the runtime import surface publishes `objc_part11_ffi_metadata_and_interface_preservation`
- provider counts for Part 11 foreign-call and annotation preservation survive consumer compilation through `--objc3-import-runtime-surface`
- the packet preserves dependency continuity from `M274-A003` and `M274-C002`

## Canonical packet expectations

The emitted packet must preserve these stable facts:

- local foreign callable count
- local metadata preservation count
- local interface annotation count
- imported module count
- imported foreign callable count
- imported metadata preservation count
- imported interface annotation count
- deterministic replay key and separate-compilation readiness

## Dependency continuity

The packet is expected to preserve these contract IDs:

- `objc3c-part11-foreign-call-and-lifetime-lowering/m274-c002-v1`
- `objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1`
