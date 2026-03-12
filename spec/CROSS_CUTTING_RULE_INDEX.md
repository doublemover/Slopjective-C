# Cross-Cutting Rule Index {#cross-cutting-rule-index}

_Working draft v0.11 — last updated 2026-02-27_

This index identifies rules whose semantics are shared across multiple parts and records one canonical normative home for each rule. Non-canonical locations should reference the canonical section instead of restating full normative text.

Current implementation note:

- The live frontend admits `guard` optional-binding clauses plus comma-separated
  boolean guard clauses.
- Statement-form `match` is now admitted as a frontend-owned control-flow
  carrier with wildcard, literal, binding, and result-case patterns.
- `defer`, expression-form `match`, guarded patterns, and type-test patterns
  still fail closed with explicit parser diagnostics until later `M266`
  execution work lands.

Current Part 5 semantic-model note:

- The sema-owned packet
  `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`
  now freezes the current live control-flow legality surface.
- Guard refinement, guard else-exit enforcement, statement-match binding scope,
  result-case binding scope, and `break` / `continue` legality are live.
- `defer` cleanup ordering and `match` exhaustiveness remain explicit deferred
  semantics rather than implied implementation claims.

## Scope-exit and `defer` rules

| Rule                                                                                | Canonical normative home                | Non-canonical cross-reference locations                    |
| ----------------------------------------------------------------------------------- | --------------------------------------- | ---------------------------------------------------------- |
| `defer` syntax                                                                      | [Part 5](#part-5) [§5.2.1](#part-5-2-1) | [Part 8](#part-8) [§8.2.1](#part-8-2-1)                    |
| `defer` registration semantics (scope-exit action in innermost lexical scope)       | [Part 5](#part-5) [§5.2.2](#part-5-2-2) | [Part 8](#part-8) [§8.2.2](#part-8-2-2), [§8.1](#part-8-1) |
| Ordering of `defer` actions relative to ARC strong-local release                    | [Part 8](#part-8) [§8.2.3](#part-8-2-3) | [Part 5](#part-5) [§5.2.2](#part-5-2-2)                    |
| `defer` body non-local exit restrictions                                            | [Part 5](#part-5) [§5.2.3](#part-5-2-3) | [Part 8](#part-8) [§8.2.4](#part-8-2-4)                    |
| Cleanup-scope definition and non-local transfer behavior (`throw`/unwind/`longjmp`) | [Part 8](#part-8) [§8.1](#part-8-1)     | [Part 5](#part-5) [§5.2.5](#part-5-2-5)                    |

## Maintenance rule

When adding a new cross-cutting normative rule, update this file and convert secondary locations to explicit cross-references.
