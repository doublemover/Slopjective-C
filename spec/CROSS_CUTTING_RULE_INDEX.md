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
- Current Part 5 defer and guard lowering note:
- `M266-C002` now lowers boolean-clause `guard` and lexical `defer` cleanup
  insertion natively.
- `M266-C003` now lowers literal/default/wildcard/binding statement-form
  `match` arms natively.
- result-case statement-form `match` remains fail-closed in native LLVM
  lowering until a runtime `Result` payload ABI lands.
- current fail-closed lowering probes terminate deterministically with
  `O3L300`.

Current Part 5 cleanup/unwind runtime note:

- `M266-D001` freezes the current toolchain/runtime boundary for runnable
  cleanup execution
- executable proofs consume the emitted linker-response sidecar plus the
  runtime support archive path
- runtime cleanup helpers remain private:
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
  - `objc3_runtime_copy_memory_management_state_for_testing`
- `M266-D002` widens native runnable proof across ordinary exit, guard-mediated
  early return, and nested-scope return unwind without widening a public
  cleanup/unwind runtime ABI

Current Part 5 execution-gate note:

- `M266-E001` freezes one integrated executable gate for the currently
  supported Part 5 control-flow slice
- the gate consumes the emitted manifest/IR/object triplet and the
  `M266-A002` / `M266-B003` / `M266-C003` / `M266-D002` proof chain rather
  than inventing a separate synthetic reporting path
- current gate coverage is limited to boolean-clause `guard`, supported
  exhaustive statement-form `match`, and lexical `defer` cleanup execution on
  ordinary exit and return unwind
- expression-form `match`, guarded patterns, type-test patterns, and a public
  cleanup/unwind runtime ABI remain deferred

Current Part 5 closeout-matrix note:

- `M266-E002` publishes the runnable closeout matrix for the exact Part 5 slice
  already frozen by `M266-E001`
- the closeout matrix consumes `M266-D002` cleanup/unwind executable evidence
  and the `M266-E001` integrated guard/match/defer probe
- the closeout matrix does not widen unsupported Part 5 forms or the private
  cleanup/unwind runtime boundary

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
