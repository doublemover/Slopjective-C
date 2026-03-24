# M273 Macro Safety, Sandbox, and Determinism Semantics Expectations (B003)

Contract ID: `objc3c-part10-macro-safety-sandbox-determinism-semantics/m273-b003-v1`

## Required outcomes

- The frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part10_macro_safety_sandbox_and_determinism_semantics`.
- Macro admission is currently limited to pure, body-backed free functions.
- Macro callables must carry `objc_macro`, `objc_macro_package`, and `objc_macro_provenance` together.
- Sandbox-admitted macro packages currently live under `std.metaprogramming`.
- Provenance must be a lowercase `sha256:` digest token.
- The compiler rejects incomplete metadata, orphan metadata, sandbox-rejected packages, non-deterministic provenance, non-pure/prototype/async/throws macro callables, and macro-marked methods.
