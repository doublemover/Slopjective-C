# M275 Legacy-to-Canonical Migration Semantics - Edge-case and Compatibility Completion Expectations (B003)

Contract ID: `objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1`

## Required outcomes

- The frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part12_legacy_canonical_migration_semantics`.
- The packet is wired to the live migration-assist semantic path instead of a placeholder claim.
- Canonical mode plus migration assist rejects legacy `YES` / `NO` / `NULL` with `O3S216`.
- Canonical literals remain accepted on the happy path.

## Dynamic proof

- Positive fixture:
  - canonical mode
  - migration assist enabled
  - no diagnostics
  - manifest packet reports deterministic handoff
- Negative fixture:
  - canonical mode
  - migration assist enabled
  - diagnostics file reports three `O3S216` entries for legacy `YES` / `NO` / `NULL`
