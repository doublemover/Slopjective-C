# M273 Derive, Macro, and Property-Behavior Source Closure Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part10-metaprogramming-source-closure/m273-a001-v1`

## Required outcomes

- the frontend admits parser-owned `objc_derive(named("..."))` container markers on `@interface`
- the frontend admits parser-owned `objc_macro(named("..."))` callable markers on functions and Objective-C methods
- the frontend admits `behavior=...` markers inside `@property(...)`
- the emitted manifest publishes `frontend.pipeline.semantic_surface.objc_part10_derive_macro_property_behavior_source_closure`
- the summary remains source-closure only and does not claim expansion, synthesis, or runtime behavior
