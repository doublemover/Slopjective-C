# Parser

Parser behavior is described through capability rows and executable tests.
Unsupported grammar remains rejected or reserved until the capability matrix
marks the behavior implemented.

Current v1 parser truth admits single-payload `throws(E)` as an implemented
bounded typed-throws effect and admits canonical `Optional<T>` as a semantic
type-signature carrier with a checked absent/present lowering contract and a
bounded packed i32/bool/id-handle runtime ABI plus full-width i64 runtime helper
ABI and `Optional<i64>` language call/return lowering through the wide
`{has_value,i64}` carrier, while staying fail-closed for nested/generic
value-optional payload lowering, property/ivar storage,
unchecked unwrap, nullability bridges, implicit nil, nil-to-scalar coercion,
throws/result conversion, expression-position `match`, and `=>` match arms. For
#8233, empty, multi, malformed, and non-type parenthesized `throws(...)` payload
shapes are parser-owned `O3P182` rejections and are never erased into bare
untyped `throws`; exactly one type payload is preserved through interface
contracts and hidden error-out ABI lowering, with direct-call, runtime-dispatch,
catch/bridge, and `try?` runtime paths covered by the bounded public row.
For #8234, canonical `Optional<T>` has first-class type identity plus a stable
`has_value`/`payload` contract in source and textual-interface records. The
owned contract now distinguishes bounded packed runtime ABI support for i32,
bool, and id handles plus full-width i64 direct language ABI from the
still-reserved broad value-optional runtime surface.
Lowercase `optional<T>` remains `O3C004` removed spelling rather than an alias,
and neither spelling enables unchecked unwrap, implicit nil absence,
nil-to-scalar, throws/result, nullable-pointer conversion, nested/generic payload
runtime lowering, property/ivar storage, or broad runtime ABI claims. The
parser-owned diagnostic symbols are
`kObjc3ParserDiagnosticReservedTypedThrowsCode`,
`kObjc3ParserDiagnosticReservedValueOptionalCode`, and
`kObjc3ParserDiagnosticRemovedOptionalAliasCode` in the language-evolution
reserved diagnostic contract; parser and source-closure summaries may publish
those identifiers as fail-closed anchors, not support claims.
Standalone textual-interface import also treats these rows as hard contracts:
`throws(E)` metadata may import `typed` only with one preserved payload, a
typed error-out ABI claim, and no untyped erasure; `none` and bare `untyped`
records keep zero typed payload arity. Typed catch compatibility records allow
only exact payload catches and the explicit `id<Error>` bridge catch; mismatched
typed catches and unsupported foreign carriers fail closed. `Optional<T>`
metadata imports the
semantic carrier, stable layout identity, explicit absent/present construction
contract, checked unwrap/binding diagnostic contract, and fail-closed
conversion/runtime flags. The checked-in v1 runtime ABI covers packed `i32`,
`bool`, and `id` object-handle payload carriers only; layout drift, unchecked
unwrap, implicit bridge, unsupported payload widening, broad runtime support
claims, property/ivar storage, and nullable-pointer erasure fail closed.
Statement-form
`match (expr) { case pattern where condition: { ... } default: { ... } }` is
the only guarded-pattern spelling admitted by the parser; `where` remains
contextual and is not a global keyword. Malformed guards, expression arms,
type-test patterns, and expression-position `match` stay targeted diagnostics;
type-test `case is Type:` patterns are parser-owned `O3P158` rejections rather
than a semantic/runtime claim.
For #8235, the parser admits only the Objective-C 3 generic free-function
spelling `fn name<T>(...)`; Objective-C method type-parameter clauses,
C/Objective-C style generic function declarations, and declaration-scoped
`@reify_generics` markers remain reserved parser-owned diagnostics. The parser
does not infer runtime reification from admitted generic free functions; only
the semantic erased-default metadata contract may record those callables.
For #8237,
language profile names are not grammar aliases: strict and strict-concurrency
selection fails before compilation unless the profile validator reports
durable release/runtime evidence. `strict-system` is likewise target-only
release evidence, not a native frontend language profile.

The #8207 umbrella language-evolution contract is source-owned at
`tests/tooling/fixtures/native/language_evolution_umbrella_contract.json`.
It binds the implemented bounded typed-throws surface, the reserved
value-optional broadening/profile surfaces, and the bounded generic and
statement-form guarded-match surface to checked-in fixtures only. Temp reports,
generated projections, Objective-C 2
compatibility paths, and alias spellings are not parser evidence.

Parser support claims are owned by:

- `native/objc3c/src/parse/` for parser implementation,
- `tests/native/parser/positive/canonical_module_main.objc3` for canonical
  parser syntax evidence,
- `tests/tooling/test_objc3c_parser_extraction.py` for extraction-level parser
  evidence,
- `tests/conformance/diagnostics/manifest.json` when a source form is a
  rejected or reserved diagnostic surface.

Parser docs must not advertise action aliases or direct helper invocations. Any
public replay command listed for parser evidence must be an npm bridge action,
currently `npm run objc3c -- test-behavior-matrix`.
