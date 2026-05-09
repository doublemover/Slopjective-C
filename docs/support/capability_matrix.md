# Objective-C 3.0 Capability Matrix

This is the public support matrix. Product docs should link here for support
truth instead of carrying local planning or archived cross-reference claims.
Rows list public replay commands only when they go through `npm run objc3c --`.
Helper tests can be evidence without becoming direct workflow commands.

| Capability                         | State       | Support claim                                      | Evidence                                                                                                       |
| ---------------------------------- | ----------- | -------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- |
| Canonical parser syntax            | implemented | `objc3c.behavior.parser.canonical-syntax`          | `tests/native/parser/positive/canonical_module_main.objc3`; `tests/tooling/test_objc3c_parser_extraction.py`   |
| Typed semantic flow                | implemented | `objc3c.behavior.sema.typed-flow`                  | `tests/native/sema/types/typed_i32_bool_flow.objc3`                                                            |
| Canonical semantic rejection       | rejected    |                                                    | `tests/conformance/diagnostics/manifest.json`; `tests/tooling/test_objc3c_parser_contract_sema_integration.py` |
| Strict runtime dispatch lowering   | implemented | `objc3c.behavior.lowering.strict-runtime-dispatch` | `tests/native/lowering/errors/runtime_dispatch_requires_link_strict_error.objc3`                               |
| IR module emission                 | implemented | `objc3c.behavior.ir.module-emission`               | `tests/native/ir/module/basic_i32_return_main.objc3`                                                           |
| Runtime dispatch strict diagnostic | implemented | `objc3c.behavior.runtime.strict-dispatch-error`    | `tests/native/runtime/dispatch/message_send_runtime_dispatch.objc3`                                            |
| Runnable smoke path                | implemented | `objc3c.behavior.e2e.runnable-smoke`               | `tests/native/e2e/smoke/basic_i32_return_main.objc3`                                                           |
| Async and actor runtime closure    | reserved    |                                                    | `docs/spec/concurrency_reserved.md`; `tests/conformance/diagnostics/manifest.json`                             |
| JSON and schema registry helpers   | internal    |                                                    | `docs/support/capability_matrix.schema.json`; `tests/tooling/test_objc3c_shared_json_schema.py`                |

State meanings:

- `implemented`: the named behavior has docs and executable evidence for its owner phase.
- `rejected`: parser or semantic analysis emits a canonical diagnostic.
- `reserved`: syntax or concept is unavailable and documented as unavailable.
- `internal`: implementation helper, report shape, or workflow contract that is not public Objective-C 3.0 behavior.

Command rule:

- Capability docs may advertise `npm run objc3c -- <action>` commands only.
- Implementation-helper invocations, retired aliases, and success-without-evidence dispatch paths are not support claims.
