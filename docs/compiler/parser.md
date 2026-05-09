# Parser

Parser behavior is described through capability rows and executable tests.
Unsupported grammar remains rejected or reserved until the capability matrix
marks the behavior implemented.

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
