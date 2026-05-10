# Objective-C 3.0 Capability Matrix

This is the public support matrix. Product docs link here for support truth
instead of carrying local planning or archived cross-reference claims. Rows list
public replay commands only when they go through `npm run objc3c -- <action>`.
Helper tests, source files, and `owner_modules` can be evidence boundaries
without becoming public workflow commands.

Machine-readable truth is split deliberately:

- matrix data: `docs/support/capability_matrix.json`
- matrix schema id: `objc3c-capability-matrix-v1`
- schema owner: `scripts/objc3c_shared/schema_registry.py`
- support directory contract: `docs/support/README.md`
- evidence rows: `docs/support/evidence_map.json`
- issue closeout evidence: `docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`
  and `docs/issues/hard_cutover_8132_8150_closeout/payloads.md`
- evidence schema id: `objc3c-capability-evidence-map-v1`
- schema examples: `docs/support/capability_schema_examples.md`
- projection policy: the `projection_policy` object in
  `docs/support/capability_matrix.json`
- support-claim contract: the `claim_contract` object in
  `docs/support/capability_matrix.json`

The matrix also carries `retired_surface_terms` for wording that may appear only
in negative examples, issue evidence, or source-hygiene rejection data.

Projection policy is part of the support contract: markdown files may explain
the matrix and evidence map, but they do not create support claims without a
matching row in the authoritative JSON data.

Issue evidence and payload tables linked from this matrix list local
implementation commits as source evidence only. They do not prove validation,
push state, GitHub issue edits, remote closure, or compatibility support.

Support-claim contract is separate from projection mechanics: only
`implemented` rows with `support_claims` in the `objc3c.behavior.*` namespace
may become public Objective-C 3.0 behavior claims. `rejected`, `reserved`, and
`internal` rows are negative, unavailable, schema, workflow, report, or owner
truth only. Retired terms, compatibility/fallback wording, registry facades,
direct helper commands, and generated reports cannot supply missing support
claims.

Command and evidence truth is hard-cut to the current surfaces:

- `package.json` exposes one public bridge: `objc3c`.
- `npm run objc3c -- <action>` dispatches into the
  `scripts.objc3c_workflow` module.
- `scripts/objc3c_workflow/action_catalog.py` owns action names, validation
  tiers, pass-through behavior, backend descriptions, and guarantee owners.
  `registry_views.py`, `action_handler_integrity.py`, `request_dispatch.py`,
  and `path_bootstrap.py` own read-only registry access, action-handler integrity,
  parsed-request dispatch, and module-bridge import roots.
  There is no supported workflow-registry facade or retired package-script alias
  table.
- Runtime dispatch claims are owned by the strict runtime C API, dispatch
  owner modules, diagnostics owner modules, and result headers under
  `native/objc3c/src/runtime/public/`.
- Shared JSON/schema claims are owned by shared-registry schema IDs, checked-in
  schema files, and the native `objc3c_json` split helpers / artifact JSON
  modules, not by prose-only summaries or support-directory schema copies.
- Runtime and object-model prose is not a support claim unless an implemented
  matrix row links executable evidence for the exact behavior.
- The hard-cutover matrix rejects retired adapters, alternate acceptance paths,
  retired-source lanes, direct helper commands, and retired mode labels as
  public support surfaces. Evidence-log completion is also not support evidence.
  Rows that change support state must use canonical feature names with evidence,
  not revive those labels.

| Capability                                  | State       | Support claim                                      | Evidence                                                                                                       |
| ------------------------------------------- | ----------- | -------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- |
| Canonical parser syntax                     | implemented | `objc3c.behavior.parser.canonical-syntax`          | `tests/native/parser/positive/canonical_module_main.objc3`; `tests/tooling/test_objc3c_parser_extraction.py`; `native/objc3c/src/parse/objc3_parser_core.cpp` |
| Typed semantic flow                         | implemented | `objc3c.behavior.sema.typed-flow`                  | `tests/native/sema/types/typed_i32_bool_flow.objc3`; `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`; `native/objc3c/src/sema/objc3_semantic_type_relations.cpp` |
| Canonical semantic rejection                | rejected    |                                                    | `tests/conformance/diagnostics/manifest.json`; `tests/tooling/test_objc3c_parser_contract_sema_integration.py`; `native/objc3c/src/parse/objc3_parser_rejection_diagnostics.cpp` |
| Strict runtime dispatch lowering            | implemented | `objc3c.behavior.lowering.strict-runtime-dispatch` | `tests/native/lowering/errors/runtime_dispatch_requires_link_strict_error.objc3`; `native/objc3c/src/lower/contracts/runtime_dispatch_boundary_contracts.cpp`; `native/objc3c/src/pipeline/dispatch_surface_classification.cpp` |
| IR module emission                          | implemented | `objc3c.behavior.ir.module-emission`               | `tests/native/ir/module/basic_i32_return_main.objc3`; `native/objc3c/src/ir/objc3_ir_module_emission_surface.cpp`; `native/objc3c/src/ir/objc3_ir_deterministic_publication.cpp` |
| Runtime dispatch strict diagnostic          | implemented | `objc3c.behavior.runtime.strict-dispatch-error`    | `tests/native/runtime/dispatch/message_send_runtime_dispatch.objc3`; `native/objc3c/src/runtime/dispatch/dispatch_api.cpp`; `native/objc3c/src/runtime/public/objc3_runtime_dispatch_diagnostics.cpp` |
| Runnable smoke path                         | implemented | `objc3c.behavior.e2e.runnable-smoke`               | `tests/native/e2e/smoke/basic_i32_return_main.objc3`                                                           |
| Async and actor runtime closure             | reserved    |                                                    | `docs/spec/concurrency_reserved.md`; `tests/conformance/diagnostics/manifest.json`                             |
| Full object-model runtime realization       | reserved    |                                                    | `docs/support/hard_cutover_capability_truth.md`; `spec/MODULE_METADATA_AND_ABI_TABLES.md`; `docs/runbooks/objc3c_object_model_closure.md` |
| Advanced runtime-backed language closure    | reserved    |                                                    | `docs/support/hard_cutover_capability_truth.md`; `spec/PART_6_ERRORS_RESULTS_THROWS.md`; `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md`; `spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md` |
| Native compiler module decomposition        | internal    |                                                    | root and stage `CMakeLists.txt` files under `native/objc3c/src/`; `native/objc3c/src/lower/metadata/runtime_metadata_layout_policy.cpp`; `native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp`; `native/objc3c/src/runtime/classes/class_graph.cpp` |
| Public C runtime dispatch result surface    | internal    |                                                    | `native/objc3c/src/runtime/public/objc3_runtime_api.h`; `native/objc3c/src/runtime/public/objc3_runtime_result.h`; `native/objc3c/src/runtime/public/objc3_runtime_result_materialization_contract.h`; `native/objc3c/src/runtime/images/registration.cpp`; `native/objc3c/src/runtime/dispatch/typed_dispatch_result.cpp` |
| npm objc3c workflow bridge                  | internal    |                                                    | `package.json`; `scripts/objc3c_workflow/action_catalog.py`; `scripts/objc3c_workflow/registry_views.py`; `scripts/objc3c_workflow/action_handler_integrity.py`; `scripts/objc3c_workflow/request_dispatch.py`; `scripts/objc3c_workflow/path_bootstrap.py`; `docs/runbooks/objc3c_public_command_surface.md` |
| Shared JSON and schema registry helpers     | internal    |                                                    | `schemas/objc3c-capability-matrix-v1.schema.json`; `schemas/objc3c-capability-evidence-map-v1.schema.json`; `schemas/objc3-conformance-evidence-bundle-v1.schema.json`; `schemas/objc3c-adoption-legibility-evidence-v1.schema.json`; `schemas/objc3c-package-lock-v1.schema.json`; `schemas/objc3c-platform-support-matrix-v1.schema.json`; `schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json`; `schemas/objc3c-developer-tooling-editor-surface-v1.schema.json`; `schemas/objc3c-application-architecture-evidence-summary-v1.schema.json`; `schemas/objc3c-artifact-authenticity-v1.schema.json`; `schemas/source-hygiene-hard-cutover-report-v1.schema.json`; `scripts/objc3c_shared/schema_registry.py`; `docs/support/README.md`; `docs/support/evidence_map.json`; `native/objc3c/src/io/json/json_parser.cpp`; `native/objc3c/src/io/json/json_writer.cpp`; `native/objc3c/src/io/json/json_schema.cpp`; `native/objc3c/src/io/json/json_schema_errors.cpp`; `native/objc3c/src/io/json/json_schema_validation.cpp`; `native/objc3c/src/io/json/json_equivalence.cpp`; `native/objc3c/src/io/json/json_pointer.cpp`; `native/objc3c/src/io/json/json_schema_type.cpp`; `native/objc3c/src/artifacts/json/artifact_schema_registry.cpp`; `native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp`; `native/objc3c/src/artifacts/json/artifact_json_publication_contract.cpp` |
| Hard-cutover capability truth boundary      | internal    |                                                    | `docs/support/hard_cutover_capability_truth.md`; `docs/support/README.md`; `docs/issues/hard_cutover_8132_8150_evidence.md`; `docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`; `docs/issues/hard_cutover_8132_8150_closeout/payloads.md` |

State meanings:

- `implemented`: the named behavior has docs and executable evidence for its owner phase.
- `rejected`: parser or semantic analysis emits a canonical diagnostic.
- `reserved`: syntax or concept is unavailable and documented as unavailable.
- `internal`: implementation helper, report shape, or workflow contract that is not public Objective-C 3.0 behavior.
- `owner_modules`: internal source or schema boundaries that explain who owns a
  row without creating another public support claim.

Command rule:

- Capability docs may advertise `npm run objc3c -- <action>` commands only.
- Implementation-helper invocations, retired command surfaces, registry facades,
  and success-without-evidence dispatch paths are not support claims.
- Direct `python`, `pwsh`, CMake, or native helper invocations may appear as
  evidence owners, but public docs must not present them as user-facing command
  surface.
- Retired adapters, alternate acceptance paths, retired-source lanes, old modes, and
  prose-only capability claims are not alternate support states and must not be
  renamed into supported paths.
- Runtime completion, full object-model behavior, or advanced language closure
  must stay `reserved` until the matrix and evidence map carry exact
  implemented rows.

Schema examples live in `docs/support/capability_schema_examples.md`.
