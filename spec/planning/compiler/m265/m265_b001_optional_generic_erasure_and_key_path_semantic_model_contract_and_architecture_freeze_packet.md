# M265-B001 - Optional, generic-erasure, and key-path semantic model

Issue: `#7246`

## Objective

Freeze the first sema-owned Part 3 type packet so later lowering/runtime work is anchored to one deterministic semantic boundary instead of parser-only claims.

## Dependencies

- `M265-A002`
- `M259-E002`
- `M263-E002`

## Required implementation

- publish `frontend.pipeline.semantic_surface.objc_part3_type_semantic_model`
- count optional binding sites, clause sites, guard sites, optional sends, nil-coalescing sites, typed key-path sites, generic-erasure semantic sites, and nullability semantic sites
- fail closed on:
  - optional bindings sourced from non-ObjC-reference-compatible expressions
  - optional sends whose receiver is not ObjC-reference-compatible
  - typed key-path roots that do not resolve to `self` or an in-scope identifier
- keep the packet deterministic without embedding new state into the central sema/handoff structs

## Dynamic proof shape

- source-only positive probe for optional bindings, optional sends, `??`, and `@keypath(self, ...)`
- source-only positive probe for generic-erasure and nullability semantic counts
- source-only sema-negative probes for:
  - invalid optional binding source
  - invalid optional send receiver
  - invalid typed key-path root

## Notes

- This issue is a semantic freeze, not runnable lowering.
- `?.` optional-member access remains fail closed after this issue.
- The packet is intentionally published from frontend artifacts via a standalone builder rather than by growing `Objc3SemanticIntegrationSurface` or `Objc3SemanticTypeMetadataHandoff`.
