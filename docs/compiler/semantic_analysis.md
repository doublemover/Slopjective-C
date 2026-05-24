# Semantic Analysis

Semantic support claims require canonical diagnostics or executable tests.
Rejected behavior is represented by diagnostics, not alternate acceptance paths.

Typed throws is now a source/interface semantic surface with single-payload
hidden error-out ABI lowering, exact typed catch records, policy-backed
`id<Error>` bridge catches, incompatible-catch rejection, and unsupported
foreign-carrier fail-closed records. Value optionals now have semantic type identity
for canonical `Optional<T>` type signatures,
plus a sema-owned absent/present lowering contract and checked unwrap/binding
failure diagnostics plus bounded packed runtime ABI support for scalar payload
forms and `id` object-handle payloads. Nested/generic payload runtime lowering,
property/ivar storage, unchecked unwrap, nullability bridges, implicit nil
absence, nil-to-scalar coercion, and throws/result conversion remain unclaimed.
Match expressions are bounded to
their current evidence-backed surface.
Statement-form guarded match patterns are admitted only as `case pattern where
bool_condition: { ... }`; the guard is checked after
pattern binding, must type-check as `bool`, and a guarded catch-all does not make
the match exhaustive by itself. Parser/source-closure records must keep match
expressions fail-closed until result typing, interface preservation, lowering,
and runtime semantics exist for that feature family.
For #8233 and #8234 specifically, source-closure and textual-interface import
records may publish typed throws as `typed` only with one preserved payload,
`typed-error-out-abi`, and `runtime_execution_claimed=true`; erased or
drifted typed payload metadata fails closed. The semantic handoff now treats the
typed payload as callable effect identity: protocol conformance and duplicate
requirement compatibility compare `throws:typed:<payload>` exactly, and bare
`throws` is not compatible with `throws(E)`. For do/catch, exact typed payload
catches are accepted, the explicit `id<Error>` bridge catch is accepted, and
mismatched typed catches or unsupported foreign carriers fail closed rather than
being widened into untyped matches. Value optionals are modeled as
`Optional<T>` semantic carriers with stable `has_value` plus `payload` layout
identity, explicit absent/present construction records, checked
presence/narrowing records, checked unwrap diagnostics, and textual-interface
roundtrip. They still cannot be widened into implicit nil absence,
nullable-pointer conversion, nil-to-scalar coercion, throws/result conversion,
unchecked unwrap, unsupported payload ABI lowering, broad runtime constructor
support, property/ivar storage, or nullable-pointer erasure. The executable v1
path is the checked packed `Optional<i32>`, `Optional<bool>`, and
`Optional<id>` runtime ABI.

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
than semantic aliases. `core`, `strict`, and `strict-concurrency` may be
claimed by public conformance publication when the native profile validation,
strict diagnostics, strict-concurrency actor/sendability/task/scheduler checks,
and release-candidate replay evidence agree. `strict-system` remains target-only
release evidence and rejects as a native frontend selection rather than widening
semantic support.

The #8207 umbrella contract lives in
`tests/tooling/fixtures/native/language_evolution_umbrella_contract.json` and
keeps semantic promotion bounded: typed throws cannot widen past source/interface
metadata plus hidden error-out lowering and catch/bridge policy into multi-payload,
unsupported foreign-carrier, or erased public runtime behavior,
and value optionals cannot widen past their bounded packed scalar/id-handle ABI plus
checked lowering contract into broad nested/generic/property/ivar/nullability/nil
or unchecked runtime support. Runtime generic reification, type-test match
patterns, and strict-system profile support cannot be widened from source
metadata, generated reports, or Objective-C 2 compatibility paths.
Statement-form guarded match and bounded expression-form match are the admitted
#8236 surfaces.

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
