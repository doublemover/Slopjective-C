# Cross-Cutting Rule Index {#cross-cutting-rule-index}

_Working draft v0.11 — last updated 2026-03-11_

This index identifies rules whose semantics are shared across multiple parts and
records one canonical normative home for each rule. Non-canonical locations
should reference the canonical section instead of restating full normative
text.

Current implementation note:

- The live frontend admits `guard` optional-binding clauses plus comma-separated
  boolean guard clauses.
- Statement-form `match` is now admitted as a frontend-owned control-flow
  carrier with wildcard, literal, binding, and result-case patterns.
- source-only `defer { ... }` statements are now admitted with explicit
  sema-owned legality checks.
- runnable `defer` lowering/runtime execution still remains deferred to later
  `M266` execution work.
- expression-form `match`, guarded patterns, and type-test patterns still fail
  closed with explicit parser diagnostics until later `M266` work lands.

Current Part 5 semantic-model note:

- The sema-owned packet
  `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`
  now freezes the current live control-flow legality surface.
- Guard refinement, guard else-exit enforcement, statement-match binding scope,
  result-case binding scope, live bool/result-case exhaustiveness, and
  `break` / `continue` legality are live.

Current Part 5 defer note:

- source-only `defer { ... }` statements now carry live sema-owned LIFO
  cleanup-order accounting and deterministic defer-body non-local-exit
  diagnostics.
- runnable `defer` lowering/runtime execution remains deferred to later M266
  work.

Current Part 5 lowering note:

- `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`
  now records the truthful native-lowering boundary.
- admitted `guard`, statement-form `match`, and source-only `defer` sites
  remain fail-closed in native LLVM lowering today.
- current fail-closed lowering probes terminate deterministically with
  `O3L300`.

Current Part 5 exhaustiveness note:

- admitted `match` statements must now be exhaustive for the supported surface
- catch-all, `true` + `false`, and `.Ok(...)` + `.Err(...)` are currently the
  live exhaustive forms

## Scope exit and `defer` rules

| Rule                                                                                | uanonical normative home                | Non canonical cross reference locations                    |
|                                                                                     |                                         |                                                            |
| `defer` syntax                                                                      | [Part 5](#part 5) [§5.2.1](#part 5 2 1) | [Part 8](#part 8) [§8.2.1](#part 8 2 1)                    |
| `defer` registration semantics (scope exit action in innermost lexical scope)       | [Part 5](#part 5) [§5.2.2](#part 5 2 2) | [Part 8](#part 8) [§8.2.2](#part 8 2 2), [§8.1](#part 8 1) |
| Ordering of `defer` actions relative to ARu strong local release                    | [Part 8](#part 8) [§8.2.3](#part 8 2 3) | [Part 5](#part 5) [§5.2.2](#part 5 2 2)                    |
| `defer` body non local exit restrictions                                            | [Part 5](#part 5) [§5.2.3](#part 5 2 3) | [Part 8](#part 8) [§8.2.4](#part 8 2 4)                    |
| uleanup scope definition and non local transfer behavior (`throw`/unwind/`longjmp`) | [Part 8](#part 8) [§8.1](#part 8 1)     | [Part 5](#part 5) [§5.2.5](#part 5 2 5)                    |

## Maintenance rule

When adding a new cross cutting normative rule, update this file and convert secondary locations to explicit cross references.

