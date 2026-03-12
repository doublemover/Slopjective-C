# M267 Frontend NSError And Status Bridging Surface Completion Expectations (A002)

Contract ID: `objc3c-part6-error-bridge-markers/m267-a002-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part6_error_source_closure`

Required truths:

- canonical declaration markers are admitted on functions and Objective-C methods:
  - `__attribute__((objc_nserror))`
  - `__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))`
- the Part 6 frontend surface counts:
  - `objc_nserror_attribute_sites`
  - `objc_status_code_attribute_sites`
  - `status_code_success_clause_sites`
  - `status_code_error_type_clause_sites`
  - `status_code_mapping_clause_sites`
- malformed `objc_status_code(...)` payloads fail closed in the parser
- runtime `try` lowering, status-to-error execution, and bridge temporaries remain deferred to later `M267` issues

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m267-a002-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_error_bridge_marker_surface_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a002/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_status_code_attribute_missing_mapping_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a002/missing-mapping --emit-prefix module --no-emit-ir --no-emit-object`

Positive probe interpretation:

- the positive source-only probe is satisfied by a written `module.manifest.json` carrying `frontend.pipeline.semantic_surface.objc_part6_error_source_closure`
- a trailing `runtime-aware import/module frontend closure not ready` runner status does not invalidate the A002 proof; that gate belongs to later import/runtime closure work

Expected positive summary facts:

- `function_throws_declaration_sites = 0`
- `method_throws_declaration_sites = 0`
- `result_like_sites = 2`
- `result_success_sites = 0`
- `result_failure_sites = 1`
- `result_branch_sites = 1`
- `result_payload_sites = 1`
- `ns_error_bridging_sites = 5`
- `ns_error_out_parameter_sites = 2`
- `ns_error_bridge_path_sites = 1`
- `objc_nserror_attribute_sites = 1`
- `objc_status_code_attribute_sites = 1`
- `status_code_success_clause_sites = 1`
- `status_code_error_type_clause_sites = 1`
- `status_code_mapping_clause_sites = 1`
- `error_bridge_marker_source_supported = true`
- `deterministic_handoff = true`
- `ready_for_semantic_expansion = true`

Expected fail-closed diagnostic:

- missing-mapping row: `objc_status_code clause value must not be empty [O3P274]`
