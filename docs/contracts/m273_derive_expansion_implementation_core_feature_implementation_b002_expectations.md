# M273 Derive Expansion Implementation Expectations (B002)

Contract ID: `objc3c-part10-derive-expansion-inventory/m273-b002-v1`

## Required outcomes

- The frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part10_derive_expansion_inventory`.
- Supported derive requests normalize deterministically into selector inventory rows.
- Supported derives currently include `Equality`, `Equatable` as an alias for `Equality`, `Hash`, and `DebugDescription`.
- Unsupported derive names fail closed with `O3S317`.
- Derive usage on category interfaces fails closed with `O3S318`.
- Selector collisions against derived selectors fail closed with `O3S319`.
- Runtime-backed derived method body emission remains deferred.
