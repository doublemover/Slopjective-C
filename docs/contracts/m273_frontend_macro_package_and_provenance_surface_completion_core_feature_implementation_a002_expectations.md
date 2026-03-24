# M273 Frontend Macro Package and Provenance Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part10-macro-package-provenance-source-completion/m273-a002-v1`

## Required outcomes

- the frontend admits parser-owned `objc_macro_package(named("..."))` callable markers
- the frontend admits parser-owned `objc_macro_provenance(named("..."))` callable markers
- the emitted manifest publishes `frontend.pipeline.semantic_surface.objc_part10_macro_package_and_provenance_source_completion`
- the summary counts macro marker, package marker, provenance marker, and expansion-visible macro source sites deterministically
- this tranche remains frontend/source-model only and does not claim macro expansion execution or runtime package loading
