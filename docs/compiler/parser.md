# Parser

Parser behavior is described through capability rows and executable tests.
Unsupported grammar remains rejected or reserved until the capability matrix
marks the behavior implemented.

Current v1 parser truth is fail-closed for `throws(E)` typed throws payloads,
canonical `Optional<T>` value optionals, expression-position `match`, `=>`
match arms, and `case ... where ...` guarded match patterns. Negative fixtures
for those spellings prove unavailable syntax is rejected; they do not claim the
features are implemented. Lowercase `optional<T>` remains a removed spelling
diagnostic even though canonical `Optional<T>` is still reserved as a value
optional type.

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
