# M274-C003 Packet: Metadata And Interface Preservation Across FFI Boundaries - Core Feature Expansion

- Issue: `#7369`
- Milestone: `M274`
- Lane: `C`
- Contract ID: `objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part11_ffi_metadata_and_interface_preservation`
- Dependencies: `M274-A003`, `M274-C002`

## Summary

This packet closes the remaining Part 11 lane-C replay gap by publishing one explicit runtime-import-surface and IR-visible preservation packet above the local `M274-C002` lowering contract.

## Preservation surface

- provider manifests preserve local Part 11 callable and annotation counts
- provider runtime import surfaces preserve those counts for separate compilation
- consumer manifests import and replay the provider counts deterministically
- emitted IR carries one explicit `objc_part11_ffi_metadata_and_interface_preservation` anchor

## Deferred boundary

- runnable host-language bridge execution remains later lane-D work
- this packet proves preservation and replay, not cross-language runtime invocation
