# M265-C002 Expectations

Contract lineage: `objc3c-part3-optional-keypath-lowering/m265-c001-v1`

Scope: Prove that `?.` optional-member access now lowers as a real native capability by desugaring onto the existing optional-send/nil-short-circuit machinery, while keeping typed key-path execution deferred.

Required anchors:
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `package.json`

Required truths:
- contiguous `?.` tokenizes as one punctuator
- parser-owned optional-member access lowers by desugaring to the existing optional-send path
- the emitted lowering packet still uses the `M265-C001` contract lineage while its optional model now explicitly includes optional-member access
- optional-member access now lowers through the same nil-short-circuit path as bracketed optional sends
- non-ObjC-reference receivers still fail closed with deterministic diagnostics
- typed key-path literals remain deferred and fail closed on the native lowering path

Required probes:
- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-c002-readiness`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m265_optional_member_access_runtime_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/c002/positive --emit-prefix module`
- link `tmp/artifacts/compilation/objc3c-native/m265/c002/positive/module.obj` into `module.exe` using `module.runtime-registration-manifest.json` and `module.runtime-metadata-linker-options.rsp`
- execute the linked positive binary and require exit code `9`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_optional_member_access_non_objc_negative.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/c002/non-objc --emit-prefix module --no-emit-ir --no-emit-object`

Expected live facts:
- the source-closure packet publishes optional-member access as admitted frontend surface, not unsupported surface
- the lowering packet publishes `optional_model` with optional-member access included
- the positive manifest publishes `optional_send_sites = 4`, `live_optional_lowering_sites = 4`, and `single_evaluation_nil_short_circuit_sites = 4`
- the positive source-closure packet publishes `optional_member_access_sites = 3` or greater and `optional_member_access_fail_closed = false`
- the positive IR carries the Part 3 lowering marker and optional-send nil/merge blocks
- the positive object backend remains `llvm-direct`
- the negative fixture emits `O3S206`

Validation:
- `python scripts/check_m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation.py -q`
- `python scripts/run_m265_c002_lane_c_readiness.py`

Evidence:
- `tmp/reports/m265/M265-C002/optional_chaining_binding_and_coalescing_lowering_summary.json`
