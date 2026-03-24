# M273-B002 Packet: Derive Expansion Implementation - Core feature implementation

- Issue: `M273-B002`
- Lane: `B`
- Contract ID: `objc3c-part10-derive-expansion-inventory/m273-b002-v1`
- Dependency: `objc3c-part10-expansion-behavior-semantic-model/m273-b001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part10_derive_expansion_inventory`

## Scope

Implement derive expansion as a deterministic, tool-visible semantic capability.

## Truthful boundary

- Supported derives normalize into deterministic selector inventory rows.
- Unsupported derive names are rejected semantically.
- Category-interface derive usage is rejected semantically.
- Derived selector collisions are rejected semantically.
- Runnable derived method bodies remain deferred.
