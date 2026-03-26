# M314-B004 Planning Packet

## Summary

Retire the compatibility window for the legacy alias mass now that the public subset and unified runner are in place.

## Implementation shape

- Remove the legacy alias families from `package.json`.
- Publish one machine-readable retirement record for those removed families.
- Freeze a no-new-growth policy over those families.
- Point later cleanup work at the exact owners that must not reintroduce or re-document them.

## Covered alias families

- milestone-local validation aliases
- historical release-replay aliases
- historical planning aliases
- historical developer probe aliases

## Non-goals

- Do not reintroduce compatibility aliases.
- Do not rewrite all historical docs yet.
- Do not retire the prototype compiler path yet.

## Evidence

- `spec/planning/compiler/m314/m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json`

Next issue: `M314-B005`.
