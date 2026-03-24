# M275-A002 Packet: Frontend migration and canonicalization surface completion - Core feature implementation

## Contract

- Contract ID: `objc3c-part12-migration-canonicalization-source-completion/m275-a002-v1`
- Dependency contract: `objc3c-part12-diagnostics-fixit-migrator-source-inventory/m275-a001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part12_migration_and_canonicalization_source_completion`

## Scope

- Publish the live lexer-owned migration-assist behavior for legacy `YES` / `NO` / `NULL` spellings.
- Preserve `M275-A001` as the single advanced-feature source inventory and extend it with a deterministic migration/canonicalization completion packet.
- Do not claim automated rewrite application yet.

## Acceptance

- Packet includes deterministic legacy-site counts and canonical rewrite candidate counts.
- Packet truthfully records compatibility mode and migration-assist enablement.
- Happy-path proof uses a legacy-compatibility + migration-assist frontend run.
- Next issue: `M275-B001`.
