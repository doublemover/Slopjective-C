# M275-A001 Packet: Diagnostics, Fix-It, and Migrator Source Surface Inventory - Contract and Architecture Freeze

- Issue: `#7374`
- Milestone: `M275`
- Lane: `A`
- Contract ID: `objc3c-part12-diagnostics-fixit-migrator-source-inventory/m275-a001-v1`
- Surface: `frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory`
- Dependencies: `M264-E002`, `M265-E002`, `M266-E002`, `M267-E002`, `M268-E002`, `M269-E002`, `M270-E002`, `M271-E002`, `M272-E002`, `M273-E002`, `M274-E002`
- Next issue: `M275-A002`

## Scope

Freeze one deterministic frontend inventory for advanced diagnostics, fix-its, migrators, and reportable feature surfaces across the advanced Objective-C 3 tranche.

## Truth boundary

- the packet aggregates Part 6 through Part 11 source closure/completion packets
- the packet reuses lexer migration-hint counters for legacy `yes` / `no` / `null` spellings when migration assist is enabled
- the packet inventories source sites only
- feature-specific fix-it synthesis, migration rewrites, conformance report emission, and release-evidence publication remain later `M275` work
