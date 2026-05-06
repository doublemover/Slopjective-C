# Objective-C 3.0 Capability Matrix

This is the public support matrix. Product docs should link here for support
truth instead of carrying local roadmap, milestone, or archived-anchor claims.

| Capability                       | State       | Evidence                                                                                                       |
| -------------------------------- | ----------- | -------------------------------------------------------------------------------------------------------------- |
| Core declarations and statements | implemented | `tests/tooling/test_objc3c_parser_extraction.py`; `native/objc3c/src/parse/objc3_parser.cpp`                   |
| Canonical semantic rejection     | rejected    | `tests/conformance/diagnostics/manifest.json`; `tests/tooling/test_objc3c_parser_contract_sema_integration.py` |
| Async and actor runtime closure  | reserved    | `docs/spec/concurrency_reserved.md`; `tests/conformance/diagnostics/manifest.json`                             |
| JSON and schema registry helpers | internal    | `docs/support/capability_matrix.schema.json`; `tests/tooling/test_objc3c_shared_json_schema.py`                |

State meanings:

- `implemented`: parser, semantic analysis, lowering, runtime behavior, docs, and executable tests exist for the named behavior.
- `rejected`: parser or semantic analysis emits a canonical diagnostic.
- `reserved`: syntax or concept is unavailable and documented as unavailable.
- `internal`: implementation helper, report shape, or workflow contract that is not public Objective-C 3.0 behavior.
