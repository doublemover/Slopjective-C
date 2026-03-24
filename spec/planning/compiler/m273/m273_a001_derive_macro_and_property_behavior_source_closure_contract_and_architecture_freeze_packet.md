# M273-A001 Packet: Derive, Macro, and Property-Behavior Source Closure - Contract and Architecture Freeze

## Contract

- Contract ID: `objc3c-part10-metaprogramming-source-closure/m273-a001-v1`
- Frontend surface path: `frontend.pipeline.semantic_surface.objc_part10_derive_macro_property_behavior_source_closure`

## Scope

- admit parser-owned `objc_derive(named("..."))` markers on `@interface`
- admit parser-owned `objc_macro(named("..."))` markers on callable declarations
- admit `behavior=...` property markers in `@property(...)`
- publish deterministic frontend counts and replay key for those markers

## Non-goals

- macro expansion
- derive synthesis
- property runtime behavior or accessor synthesis
- metadata or ABI widening beyond the frontend semantic surface packet
