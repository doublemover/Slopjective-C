# M267 Throws, Try, Do/Catch, Result, And Bridging Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part6-error-source-closure/m267-a001-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part6_error_source_closure`

Required truths:

- `throws` declaration modifiers are admitted as source-only frontend state on functions and Objective-C methods
- result-like carrier profiling remains emitted as deterministic frontend state
- `NSError` bridging profiling remains emitted as deterministic frontend state
- `try` is a reserved fail-closed parser keyword and reports `unsupported 'try' expression [O3P268]`
- `throw` is a reserved fail-closed parser keyword and reports `unsupported 'throw' statement [O3P267]`
- `do { ... } catch { ... }` is a reserved fail-closed parser surface and reports `unsupported 'do/catch' statement [O3P269]`

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m267-a001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_part6_error_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a001/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_try_expression_fail_closed_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a001/try --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_throw_statement_fail_closed_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a001/throw --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m267_do_catch_fail_closed_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m267/a001/do-catch --emit-prefix module --no-emit-ir --no-emit-object`

Positive probe interpretation:

- the positive source-only probe is satisfied by a written `module.manifest.json` carrying `frontend.pipeline.semantic_surface.objc_part6_error_source_closure`
- a trailing `runtime-aware import/module frontend closure not ready` runner status does not invalidate the Part 6 source-closure proof for `M267-A001`; that gate belongs to later import/runtime closure work

Expected positive summary facts:

- `function_throws_declaration_sites = 1`
- `method_throws_declaration_sites = 0`
- `result_like_sites = 7`
- `result_success_sites = 1`
- `result_failure_sites = 2`
- `result_branch_sites = 4`
- `result_payload_sites = 3`
- `ns_error_bridging_sites = 3`
- `ns_error_out_parameter_sites = 1`
- `ns_error_bridge_path_sites = 1`
- `try_keyword_sites = 0`
- `throw_keyword_sites = 0`
- `catch_keyword_sites = 0`

Expected fail-closed diagnostics:

- try row: `unsupported 'try' expression [O3P268]`
- throw row: `unsupported 'throw' statement [O3P267]`
- do/catch row: `unsupported 'do/catch' statement [O3P269]`
