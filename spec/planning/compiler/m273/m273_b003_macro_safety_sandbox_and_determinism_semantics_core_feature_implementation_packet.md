# M273-B003 Packet: Macro Safety, Sandbox, and Determinism Semantics - Core feature implementation

- Issue: `M273-B003`
- Lane: `B`
- Contract ID: `objc3c-part10-macro-safety-sandbox-determinism-semantics/m273-b003-v1`
- Dependency: `objc3c-part10-derive-expansion-inventory/m273-b002-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part10_macro_safety_sandbox_and_determinism_semantics`

## Scope

Implement semantic enforcement and policy surfaces for macro safety, sandboxing, and deterministic expansion.

## Truthful boundary

- Macro admission is currently limited to pure, body-backed free functions.
- Macro package/provenance metadata must be complete.
- Sandbox-admitted packages currently live under `std.metaprogramming`.
- Provenance must be a lowercase `sha256:` digest token.
- Runnable macro execution and runtime package loading remain deferred.
