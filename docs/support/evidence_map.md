# Capability Evidence Map

Every implemented capability must point to executable evidence. Reserved
capabilities point to a reserved-status doc or a canonical diagnostic surface.

| Capability ID                         | Evidence kind | Path                                                            | Command                                                                          |
| ------------------------------------- | ------------- | --------------------------------------------------------------- | -------------------------------------------------------------------------------- |
| `compiler.parser.core-declarations`   | test          | `tests/tooling/test_objc3c_parser_extraction.py`                | `python -m pytest tests/tooling/test_objc3c_parser_extraction.py`                |
| `compiler.parser.core-declarations`   | source        | `native/objc3c/src/parse/objc3_parser.cpp`                      |                                                                                  |
| `compiler.sema.canonical-diagnostics` | diagnostic    | `tests/conformance/diagnostics/manifest.json`                   |                                                                                  |
| `compiler.sema.canonical-diagnostics` | test          | `tests/tooling/test_objc3c_parser_contract_sema_integration.py` | `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py` |
| `runtime.concurrency.async-actors`    | doc           | `docs/spec/concurrency_reserved.md`                             |                                                                                  |
| `runtime.concurrency.async-actors`    | diagnostic    | `tests/conformance/diagnostics/manifest.json`                   |                                                                                  |
| `tooling.json.schema-registry`        | schema        | `docs/support/capability_matrix.schema.json`                    |                                                                                  |
| `tooling.json.schema-registry`        | test          | `tests/tooling/test_objc3c_shared_json_schema.py`               | `python -m pytest tests/tooling/test_objc3c_shared_json_schema.py`               |
