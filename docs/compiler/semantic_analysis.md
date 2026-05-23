# Semantic Analysis

Semantic support claims require canonical diagnostics or executable tests.
Rejected behavior is represented by diagnostics, not alternate acceptance paths.

Typed throws, value optionals, and match expressions are not semantic support
claims in the current slice. Statement-form guarded match patterns are admitted
only as `case pattern where bool_condition: { ... }`; the guard is checked after
pattern binding, must type-check as `bool`, and a guarded catch-all does not make
the match exhaustive by itself. Parser/source-closure records must keep match
expressions fail-closed until result typing, interface preservation, lowering,
and runtime semantics exist for that feature family.
For #8233 and #8234 specifically, source-closure and textual-interface import
records may publish only reserved markers: typed throws stays `none` or bare
`untyped` with zero typed payload arity, while value optionals stay
`reserved-rejected-before-sema` with no layout, nullable-pointer conversion,
nil-to-scalar coercion, or throws/result conversion.

Generic callable reification is similarly bounded. Semantic records may publish
deterministic erased-default signature replay keys for admitted
`fn name<T>(...)` generic free functions, and redeclarations with generic
signature drift reject instead of merging erased shapes. That metadata is not a
runtime reification claim and does not admit Objective-C method type-parameter
clauses, C/Objective-C style generic free functions, or `@reify_generics`.
The semantic handoff is claimable only when the deterministic flag is set, the
reification policy is `erased_default`, the mangling policy is
`objc3c.generic-callable.semantic-mangling.v1`, and the replay key matches the
source-order generic signature.

Strict and strict-concurrency profiles are profile-selection contracts rather
than semantic aliases. `core` may be claimed by public conformance publication;
strict and strict-concurrency remain fail-closed until their release/runtime
evidence rows are implemented. `strict-system` remains target-only release
evidence and rejects as a native frontend selection rather than widening
semantic support.

The #8207 umbrella contract lives in
`tests/tooling/fixtures/native/language_evolution_umbrella_contract.json` and
keeps semantic promotion bounded: typed throws, value optionals, runtime
generic reification, match expressions, and strict profiles cannot be widened
from source-only metadata, generated reports, or Objective-C 2 compatibility
paths. Statement-form guarded match remains the only admitted #8236 surface.

The semantic-analysis owner boundary is:

- `native/objc3c/src/sema/` owns semantic phase implementation,
- `tests/native/sema/types/typed_i32_bool_flow.objc3` owns the current typed
  flow support claim,
- `tests/tooling/test_objc3c_parser_contract_sema_integration.py` and
  `tests/conformance/diagnostics/manifest.json` own canonical rejection
  evidence.

Semantic analysis may feed lowering, runtime metadata, and artifact summaries,
but those summaries are not public support claims unless the capability matrix
links them to evidence. A diagnostic row is still a support boundary row; it says
the implementation rejects the source form, not that another source spelling is
accepted.
