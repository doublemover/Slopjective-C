# M314-B004 Planning Packet

## Summary

Implement the compatibility window for the legacy alias mass now that the public subset and unified runner are in place.

## Implementation shape

- Add one machine-readable compatibility registry to `package.json`.
- Classify legacy alias families as temporary, non-public surfaces.
- Freeze a no-new-growth policy over those families.
- Point later cleanup work at the exact owners that must retire or stop documenting them.

## Covered alias families

- milestone-local validation aliases
- historical release-replay aliases
- historical planning aliases
- historical developer probe aliases

## Non-goals

- Do not remove compatibility aliases yet.
- Do not rewrite all docs yet.
- Do not retire the prototype compiler path yet.

## Evidence

- `spec/planning/compiler/m314/m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json`

Next issue: `M314-B005`.
