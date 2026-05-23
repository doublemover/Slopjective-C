# Parser

Parser behavior is described through capability rows and executable tests.
Unsupported grammar remains rejected or reserved until the capability matrix
marks the behavior implemented.

Current v1 parser truth is fail-closed for `throws(E)` typed throws payloads,
canonical `Optional<T>` value optionals, expression-position `match`, and `=>`
match arms. For #8233, empty, single, multi, and malformed parenthesized
`throws(...)` payload shapes are parser-owned `O3P182` rejections and are never
erased into bare untyped `throws`. For #8234, canonical `Optional<T>` remains
`O3P159` reserved, lowercase `optional<T>` remains `O3C004` removed spelling
rather than an alias, and neither spelling enables nil-to-scalar or
nullable-pointer conversion. Statement-form
`match (expr) { case pattern where condition: { ... } default: { ... } }` is
the only guarded-pattern spelling admitted by the parser; `where` remains
contextual and is not a global keyword. Malformed guards, expression arms,
type-test patterns, and expression-position `match` stay targeted diagnostics.
For #8235, the parser admits only the Objective-C 3 generic free-function
spelling `fn name<T>(...)`; Objective-C method type-parameter clauses,
C/Objective-C style generic function declarations, and declaration-scoped
`@reify_generics` markers remain reserved parser-owned diagnostics. For #8237,
language profile names are not grammar aliases: strict and strict-concurrency
selection fails before compilation unless the profile validator reports
durable release/runtime evidence.

The #8207 umbrella language-evolution contract is source-owned at
`tests/tooling/fixtures/native/language_evolution_umbrella_contract.json`.
It binds the reserved typed-throws/value-optional/generic-reification/profile
surfaces and the bounded statement-form guarded-match surface to checked-in
fixtures only. Temp reports, generated projections, Objective-C 2
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
