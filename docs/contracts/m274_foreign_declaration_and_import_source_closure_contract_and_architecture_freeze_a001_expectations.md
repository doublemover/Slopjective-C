# M274 Foreign Declaration And Import Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1`

## Required outcomes

- the frontend admits parser-owned foreign callable markers via `__attribute__((objc_foreign))`
- the frontend admits parser-owned `extern fn` / `extern pure fn` foreign callable declarations
- the frontend admits parser-owned import-module annotations and imported module names
- the frontend admits parser-owned `objc_import_module(named("..."))` annotations on callable declarations
- the emitted frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_foreign_declaration_and_import_source_closure`
- this lane freezes source closure only and does not claim foreign ABI lowering, imported-module runtime loading, or interop runtime execution yet
