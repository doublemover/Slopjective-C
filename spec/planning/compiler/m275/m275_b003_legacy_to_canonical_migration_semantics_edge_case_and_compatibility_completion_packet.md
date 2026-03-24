# M275-B003 Packet: Legacy-to-canonical migration semantics - Edge-case and compatibility completion

## Scope

- Close the remaining migration compatibility gap with one deterministic Part 12 packet over the live canonical-mode migration-assist semantic path.

## Surface

- `frontend.pipeline.semantic_surface.objc_part12_legacy_canonical_migration_semantics`

## Dependencies

- `M275-B002` feature-specific fix-it synthesis
- live compatibility/strictness semantic boundary

## Required proof

- canonical happy path emits the packet with zero current-run legacy literal sites
- canonical negative path fails closed with `O3S216`
- the packet preserves the current fix-it family count and compatibility-mode semantics truthfully

## Next issue

- `M275-C001` machine-readable conformance and report contract - Contract and architecture freeze
