# Objective-C 3.0 Capability Matrix

This is the public support matrix. Product docs link here for support truth
instead of carrying local planning or archived cross-reference claims. Rows list
public replay commands only when they go through `npm run objc3c -- <action>`.
Helper tests and source files can be evidence without becoming public workflow
commands.

Machine-readable truth is split deliberately:

- matrix data: `docs/support/capability_matrix.json`
- matrix schema: `schemas/objc3c-capability-matrix-v1.schema.json`
- local schema mirror: `docs/support/capability_matrix.schema.json`
- evidence rows: `docs/support/evidence_map.json`
- evidence schema: `schemas/objc3c-capability-evidence-map-v1.schema.json`

Command and evidence truth is hard-cut to the current surfaces:

- `package.json` exposes one public bridge: `objc3c`.
- `npm run objc3c -- <action>` dispatches into the
  `scripts.objc3c_workflow` module.
- `scripts/objc3c_workflow/action_catalog.py` owns action names, validation
  tiers, pass-through behavior, backend descriptions, and guarantee owners.
  `registry_views.py`, `action_integrity.py`, `request_dispatch.py`, and
  `path_bootstrap.py` own read-only registry access, handler integrity,
  parsed-request dispatch, and direct-entrypoint import roots.
  There is no supported workflow-registry facade or retired public-script alias
  table.
- Runtime dispatch claims are owned by the strict runtime C API and result
  headers under `native/objc3c/src/runtime/public/`.
- Shared JSON/schema claims are owned by checked-in schema files and the native
  `objc3c_json` / artifact JSON modules, not by prose-only summaries.
- The hard-cutover matrix explicitly rejects shims, fallback paths, migration
  lanes, direct helper commands, and compatibility-mode claims as public support
  surfaces unless a row marks the behavior implemented with evidence.

| Capability                                  | State       | Support claim                                      | Evidence                                                                                                       |
| ------------------------------------------- | ----------- | -------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- |
| Canonical parser syntax                     | implemented | `objc3c.behavior.parser.canonical-syntax`          | `tests/native/parser/positive/canonical_module_main.objc3`; `tests/tooling/test_objc3c_parser_extraction.py`   |
| Typed semantic flow                         | implemented | `objc3c.behavior.sema.typed-flow`                  | `tests/native/sema/types/typed_i32_bool_flow.objc3`                                                            |
| Canonical semantic rejection                | rejected    |                                                    | `tests/conformance/diagnostics/manifest.json`; `tests/tooling/test_objc3c_parser_contract_sema_integration.py` |
| Strict runtime dispatch lowering            | implemented | `objc3c.behavior.lowering.strict-runtime-dispatch` | `tests/native/lowering/errors/runtime_dispatch_requires_link_strict_error.objc3`                               |
| IR module emission                          | implemented | `objc3c.behavior.ir.module-emission`               | `tests/native/ir/module/basic_i32_return_main.objc3`                                                           |
| Runtime dispatch strict diagnostic          | implemented | `objc3c.behavior.runtime.strict-dispatch-error`    | `tests/native/runtime/dispatch/message_send_runtime_dispatch.objc3`; `native/objc3c/src/runtime/errors/`       |
| Runnable smoke path                         | implemented | `objc3c.behavior.e2e.runnable-smoke`               | `tests/native/e2e/smoke/basic_i32_return_main.objc3`                                                           |
| Async and actor runtime closure             | reserved    |                                                    | `docs/spec/concurrency_reserved.md`; `tests/conformance/diagnostics/manifest.json`                             |
| Native compiler module decomposition        | internal    |                                                    | `native/objc3c/src/CMakeLists.txt`; compiler/runtime/pipeline/artifacts/IO owner modules under `native/objc3c/src/` |
| Public C runtime dispatch result surface    | internal    |                                                    | `native/objc3c/src/runtime/public/objc3_runtime_api.h`; `native/objc3c/src/runtime/public/objc3_runtime_result.h` |
| npm objc3c workflow bridge                  | internal    |                                                    | `package.json`; `scripts/objc3c_workflow/action_catalog.py`; `scripts/objc3c_workflow/registry_views.py`; `scripts/objc3c_workflow/action_integrity.py`; `scripts/objc3c_workflow/request_dispatch.py`; `docs/runbooks/objc3c_public_command_surface.md` |
| Shared JSON and schema registry helpers     | internal    |                                                    | `schemas/objc3c-capability-matrix-v1.schema.json`; `schemas/objc3c-capability-evidence-map-v1.schema.json`; `docs/support/evidence_map.json`; `native/objc3c/src/io/json/`; `native/objc3c/src/artifacts/json/` |

State meanings:

- `implemented`: the named behavior has docs and executable evidence for its owner phase.
- `rejected`: parser or semantic analysis emits a canonical diagnostic.
- `reserved`: syntax or concept is unavailable and documented as unavailable.
- `internal`: implementation helper, report shape, or workflow contract that is not public Objective-C 3.0 behavior.

Command rule:

- Capability docs may advertise `npm run objc3c -- <action>` commands only.
- Implementation-helper invocations, retired package-script aliases, registry
  facades, and success-without-evidence dispatch paths are not support claims.
- Direct `python`, `pwsh`, CMake, or native helper invocations may appear as
  evidence owners, but public docs must not present them as user-facing command
  surface.
- Compatibility shims, fallback paths, migration lanes, old modes, and
  prose-only capability claims are not alternate support states.
