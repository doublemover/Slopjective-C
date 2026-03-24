# M275 Frontend Migration and Canonicalization Surface Completion - Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part12-migration-canonicalization-source-completion/m275-a002-v1`

## Required outcomes

- The frontend publishes `frontend.pipeline.semantic_surface.objc_part12_migration_and_canonicalization_source_completion`.
- The packet depends on `M275-A001` inventory truth rather than introducing a parallel source-inventory model.
- Legacy `YES` / `NO` / `NULL` spellings admitted under `--objc3-migration-assist` are reported as deterministic canonicalization, fix-it, and migrator candidate counts.
- The happy path is proven with a legacy-compatibility + migration-assist frontend run.
- Validation evidence lands under `tmp/reports/m275/M275-A002/`.
