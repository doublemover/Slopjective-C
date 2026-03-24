# M273-A002 Packet: Frontend Macro Package and Provenance Surface Completion - Core Feature Implementation

## Contract

- Contract ID: `objc3c-part10-macro-package-provenance-source-completion/m273-a002-v1`
- Frontend surface path: `frontend.pipeline.semantic_surface.objc_part10_macro_package_and_provenance_source_completion`

## Scope

- admit parser-owned `objc_macro_package(named("..."))` callable markers
- admit parser-owned `objc_macro_provenance(named("..."))` callable markers
- publish deterministic counts for macro marker, package marker, provenance marker, and expansion-visible macro source sites

## Non-goals

- macro expansion execution
- macro sandbox/runtime policy
- runtime package loader metadata
- lowering/runtime contracts for expanded declarations
