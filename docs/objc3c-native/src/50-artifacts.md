<!-- markdownlint-disable-file MD041 -->

## Artifacts and exit codes

For `.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (module/frontend/lowering/globals/functions)
  - `<prefix>.ll` (emitted IR)
  - `<prefix>.obj` (compiled from IR)

For non-`.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (libclang symbol rows)
  - `<prefix>.obj` (compiled Objective-C object)

Exit codes:

- `0`: success
- `1`: parse/semantic/diagnostic failure
- `2`: CLI usage / missing input / invalid arg / missing explicit clang path
- `3`: clang compile step failed

## M223 lowering/IR metadata envelope

Native `.objc3` IR emission now includes deterministic frontend-profile metadata in addition to lowering boundary replay data:

- Prologue comment:
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
- Named LLVM metadata payload:
  - `!objc3.frontend = !{!0}`
  - `!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}`

Operator replay check (from repo root):

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata --emit-prefix module
```

Then inspect:

- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.manifest.json`

Both artifacts should present aligned compatibility/migration profile information for deterministic replay triage.

## M213 lowering/runtime debug-info fidelity profile

Lowering/runtime debug-info fidelity is captured as a deterministic packet rooted under `tmp/` to preserve replay-stable source mapping evidence.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/debug-metadata-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `debug metadata markers` (required in debug metadata marker extracts):
  - `source_filename = "<module>.objc3"`
  - `"source":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `out << "source_filename = \"" << program_.module_name << ".objc3"\n\n";`
  - `manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and debug metadata marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, debug metadata marker, or source anchor is missing.

Debug-info fidelity capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json > tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/abi-ir-anchors.txt`
3. `rg -n "source_filename =|\"source\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.diagnostics.json > tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/debug-metadata-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m213_lowering_debug_fidelity_contract.py -q`

## M212 lowering/runtime code-action profile

Lowering/runtime evidence for the refactor/code-action engine is captured as a deterministic packet rooted under `tmp/` so rewrite application is replay-stable and auditable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `rewrite markers` (required in rewrite marker extracts):
  - `@@ rewrite_scope:module`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"source":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `<< ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and rewrite marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, rewrite marker, or source anchor is missing.

Code-action capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json > tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt`
3. `@("@@ rewrite_scope:module") | Set-Content tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "\"source\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m212_lowering_code_action_contract.py -q`

## M211 lowering/runtime LSP semantic profile

Lowering/runtime semantic token and symbol-navigation evidence is captured as a deterministic packet rooted under `tmp/` for replay-stable LSP contract validation.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `symbol-navigation markers` (required in marker extracts):
  - `@@ lsp_profile:semantic_tokens_navigation`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"semantic_surface":`
  - `"declared_globals":`
  - `"declared_functions":`
  - `"resolved_global_symbols":`
  - `"resolved_function_symbols":`
  - `"globals":`
  - `"functions":`
  - `"name":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `manifest << ",\"declared_functions\":" << manifest_functions.size()`
  - `manifest << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()`
  - `manifest << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `manifest << "  \"globals\": [\n";`
  - `manifest << "  \"functions\": [\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size() << ",\"param_types\":[";`
  - `<< ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and symbol-navigation marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, symbol-navigation marker, or source anchor is missing.

LSP semantic profile capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.ll tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json > tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/abi-ir-anchors.txt`
3. `@("@@ lsp_profile:semantic_tokens_navigation") | Set-Content tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt; rg -n "\"semantic_surface\":|\"declared_globals\":|\"declared_functions\":|\"resolved_global_symbols\":|\"resolved_function_symbols\":|\"globals\":|\"functions\":|\"name\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.diagnostics.json >> tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m211_lowering_lsp_contract.py -q`

## M206 lowering/runtime canonical optimization pipeline stage-1

Lowering/runtime canonical optimization stage-1 evidence is captured as deterministic packet artifacts rooted under `tmp/` so replay checks stay stable across optimizer reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/canonical-optimization-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `canonical optimization stage-1 markers` (required in source-anchor extracts):
  - `runtime_dispatch_call_emitted_ = false;`
  - `runtime_dispatch_call_emitted_ = true;`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `FunctionMayHaveGlobalSideEffects`
  - `call_may_have_global_side_effects`
  - `global_proofs_invalidated`
  - `semantic_surface`
  - `function_signature_surface`
  - `scalar_return_i32`
  - `scalar_return_bool`
  - `scalar_return_void`
  - `scalar_param_i32`
  - `scalar_param_bool`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);`
  - `if (call_may_have_global_side_effects) {`
  - `ctx.global_proofs_invalidated = true;`
  - `manifest_functions.reserve(program.functions.size())`
  - `std::unordered_set<std::string> manifest_function_names`
  - `if (manifest_function_names.insert(fn.name).second)`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `<< ",\"declared_functions\":" << manifest_functions.size()`
  - `<< ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()`
  - `<< ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `<< ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32`
  - `<< ",\"scalar_return_bool\":" << scalar_return_bool`
  - `<< ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32`
  - `<< ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and canonical optimization source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, canonical optimization marker, or source anchor is missing.

Canonical optimization stage-1 capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1 --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.ll tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.manifest.json > tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/abi-ir-anchors.txt`
3. `rg -n "runtime_dispatch_call_emitted_|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|FunctionMayHaveGlobalSideEffects|call_may_have_global_side_effects|global_proofs_invalidated|manifest_functions\.reserve\(program\.functions\.size\(\)\)|manifest_function_names|function_signature_surface|scalar_return_i32|scalar_return_bool|scalar_return_void|scalar_param_i32|scalar_param_bool|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/canonical-optimization-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m206_lowering_canonical_optimization_contract.py -q`

## M204 lowering/runtime macro diagnostics and provenance

Lowering/runtime macro diagnostics and provenance evidence is captured as deterministic packet artifacts rooted under `tmp/` so pragma diagnostics metadata and lowering replay boundaries remain auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `macro diagnostics/provenance markers` (required in source-anchor extracts):
  - `MakeDiag(...)`
  - `error:<line>:<column>: <message> [<code>]`
  - `ConsumeLanguageVersionPragmas(diagnostics)`
  - `ConsumeLanguageVersionPragmaDirective(...)`
  - `O3L005`
  - `O3L006`
  - `O3L007`
  - `O3L008`
  - `first_line`
  - `first_column`
  - `last_line`
  - `last_column`
  - `ParseDiagSortKey(...)`
  - `"severity"`
  - `"line"`
  - `"column"`
  - `"code"`
  - `"message"`
  - `"raw"`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";`
  - `ConsumeLanguageVersionPragmas(diagnostics);`
  - `ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));`
  - `diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));`
  - `language_version_pragma_contract_.first_line = line;`
  - `language_version_pragma_contract_.first_column = column;`
  - `language_version_pragma_contract_.last_line = line;`
  - `language_version_pragma_contract_.last_column = column;`
  - `result.language_version_pragma_contract.first_line = pragma_contract.first_line;`
  - `result.language_version_pragma_contract.first_column = pragma_contract.first_column;`
  - `result.language_version_pragma_contract.last_line = pragma_contract.last_line;`
  - `result.language_version_pragma_contract.last_column = pragma_contract.last_column;`
  - `manifest << "    \"language_version_pragma_contract\":{\"seen\":"`
  - `<< ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line`
  - `<< ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column`
  - `<< ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line`
  - `<< ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";`
  - `const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
  - `<< EscapeJsonString(key.message) << "\",\"raw\":\"" << EscapeJsonString(diagnostics[i]) << "\"}";`
  - `WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and macro diagnostics/provenance source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, macro diagnostics/provenance marker, or source anchor is missing.

Macro diagnostics/provenance capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt`
3. `rg -n "MakeDiag\(|error:|ConsumeLanguageVersionPragmas\(diagnostics\)|ConsumeLanguageVersionPragmaDirective\(|O3L005|O3L006|O3L007|O3L008|first_line|first_column|last_line|last_column|ParseDiagSortKey\(|\"severity\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/io/objc3_diagnostics_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m204_lowering_macro_diagnostics_contract.py -q`

## M203 lowering/runtime compile-time evaluation engine

Lowering/runtime compile-time evaluation engine evidence is captured as deterministic packet artifacts rooted under `tmp/` so constant-evaluation lowering and runtime dispatch fast-path replay remains auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/compile-time-eval-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `compile-time evaluation markers` (required in source-anchor extracts):
  - `TryGetCompileTimeI32ExprInContext`
  - `IsCompileTimeNilReceiverExprInContext`
  - `IsCompileTimeKnownNonNilExprInContext`
  - `has_assigned_const_value`
  - `has_assigned_nil_value`
  - `has_clause_const_value`
  - `has_let_const_value`
  - `const_value_ptrs`
  - `nil_bound_ptrs`
  - `nonzero_bound_ptrs`
  - `global_proofs_invalidated`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `const bool has_assigned_const_value =`
  - `op == "=" && value_expr != nullptr && TryGetCompileTimeI32ExprInContext(value_expr, ctx, assigned_const_value);`
  - `const bool has_assigned_nil_value = op == "=" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);`
  - `ctx.const_value_ptrs.erase(ptr);`
  - `const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);`
  - `const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);`
  - `bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {`
  - `bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {`
  - `if (expr->op == "&&" || expr->op == "||") {`
  - `if (expr->op == "<<" || expr->op == ">>") {`
  - `bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {`
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `ctx.global_proofs_invalidated = true;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and compile-time-evaluation source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, compile-time evaluation marker, or source anchor is missing.

Compile-time evaluation engine capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.ll tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.manifest.json > tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/abi-ir-anchors.txt`
3. `rg -n "TryGetCompileTimeI32ExprInContext|IsCompileTimeNilReceiverExprInContext|IsCompileTimeKnownNonNilExprInContext|has_assigned_const_value|has_assigned_nil_value|has_clause_const_value|has_let_const_value|const_value_ptrs|nil_bound_ptrs|nonzero_bound_ptrs|global_proofs_invalidated|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/compile-time-eval-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m203_lowering_compile_time_eval_contract.py -q`

## M202 lowering/runtime derive/synthesis pipeline

Lowering/runtime derive/synthesis pipeline evidence is captured as deterministic packet artifacts rooted under `tmp/` so semantic integration/type-metadata derivation and runtime-facing manifest synthesis remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/`
  - `tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/derive-synthesis-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `derive/synthesis markers` (required in source-anchor extracts):
  - `BuildSemanticIntegrationSurface(...)`
  - `BuildSemanticTypeMetadataHandoff(...)`
  - `IsDeterministicSemanticTypeMetadataHandoff(...)`
  - `global_names_lexicographic`
  - `functions_lexicographic`
  - `deterministic_type_metadata_handoff`
  - `type_metadata_global_entries`
  - `type_metadata_function_entries`
  - `semantic_surface`
  - `resolved_global_symbols`
  - `resolved_function_symbols`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,`
  - `Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {`
  - `bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {`
  - `result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);`
  - `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`
  - `result.deterministic_type_metadata_handoff =`
  - `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`
  - `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`
  - `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`
  - `result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;`
  - `<< ",\"deterministic_type_metadata_handoff\":"`
  - `<< (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")`
  - `<< ",\"type_metadata_global_entries\":"`
  - `<< pipeline_result.sema_parity_surface.type_metadata_global_entries`
  - `<< ",\"type_metadata_function_entries\":"`
  - `<< pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\n";`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `<< ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()`
  - `<< ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and derive/synthesis source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, derive/synthesis marker, or source anchor is missing.

Derive/synthesis pipeline capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.ll tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.manifest.json > tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/abi-ir-anchors.txt`
3. `rg -n "BuildSemanticIntegrationSurface|BuildSemanticTypeMetadataHandoff|IsDeterministicSemanticTypeMetadataHandoff|global_names_lexicographic|functions_lexicographic|deterministic_type_metadata_handoff|type_metadata_global_entries|type_metadata_function_entries|semantic_surface|resolved_global_symbols|resolved_function_symbols|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering|declare i32 @" native/objc3c/src/sema/objc3_semantic_passes.cpp native/objc3c/src/sema/objc3_sema_pass_manager.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp > tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/derive-synthesis-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m202_lowering_derive_synthesis_contract.py -q`

## M193 lowering/runtime SIMD/vector type lowering

Lowering/runtime SIMD/vector type-lowering evidence is captured as deterministic packet artifacts rooted under `tmp/` so vector ABI replay material, LLVM IR boundary comments, and emitted manifest contract surfaces stay replay-stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/`
  - `tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/simd-vector-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; simd_vector_lowering = i32x2=<2 x i32>;i32x4=<4 x i32>;i32x8=<8 x i32>;i32x16=<16 x i32>;boolx2=<2 x i1>;boolx4=<4 x i1>;boolx8=<8 x i1>;boolx16=<16 x i1>;lane_contract=2,4,8,16`
  - `; simd_vector_function_signatures = <N>`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
  - `"lowering_vector_abi":{"replay_key":"i32x2=<2 x i32>;i32x4=<4 x i32>;i32x8=<8 x i32>;i32x16=<16 x i32>;boolx2=<2 x i1>;boolx4=<4 x i1>;boolx8=<8 x i1>;boolx16=<16 x i1>;lane_contract=2,4,8,16","lane_contract":"2,4,8,16","vector_signature_functions":<N>}`
  - `"vector_signature_surface":{"vector_signature_functions":<N>,"vector_return_signatures":<N>,"vector_param_signatures":<N>,"vector_i32_signatures":<N>,"vector_bool_signatures":<N>,"lane2":<N>,"lane4":<N>,"lane8":<N>,"lane16":<N>}`
- `SIMD/vector architecture+isolation anchors` (required in source-anchor extracts):
  - `kObjc3SimdVectorLaneContract = "2,4,8,16"`
  - `kObjc3SimdVectorBaseI32 = "i32"`
  - `kObjc3SimdVectorBaseBool = "bool"`
  - `IsSupportedObjc3SimdVectorLaneCount(...)`
  - `TryBuildObjc3SimdVectorLLVMType(...)`
  - `Objc3SimdVectorTypeLoweringReplayKey()`
  - `CountVectorSignatureFunctions(...)`
  - `simd_vector_lowering =`
  - `simd_vector_function_signatures =`
  - `"vector_signature_surface"`
  - `"lowering_vector_abi"`
  - `"lane_contract":"2,4,8,16"`
- `source anchors`:
  - `inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";`
  - `inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";`
  - `inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";`
  - `bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {`
  - `bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type) {`
  - `std::string Objc3SimdVectorTypeLoweringReplayKey() {`
  - `static std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {`
  - `out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\n";`
  - `out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";`
  - `manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":" << vector_signature_functions`
  - `manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()`
  - `<< "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and SIMD/vector source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, SIMD/vector marker, or source anchor is missing.

SIMD/vector type-lowering capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering --emit-prefix module`
2. `rg -n "lowering_ir_boundary|simd_vector_lowering|simd_vector_function_signatures|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"|\"lowering_vector_abi\"|\"vector_signature_surface\"" tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.ll tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.manifest.json > tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/abi-ir-anchors.txt`
3. `rg -n "kObjc3SimdVectorLaneContract|kObjc3SimdVectorBaseI32|kObjc3SimdVectorBaseBool|IsSupportedObjc3SimdVectorLaneCount|TryBuildObjc3SimdVectorLLVMType|Objc3SimdVectorTypeLoweringReplayKey|CountVectorSignatureFunctions|simd_vector_lowering|simd_vector_function_signatures|vector_signature_surface|lowering_vector_abi|lane_contract" native/objc3c/src/lower/objc3_lowering_contract.h native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/simd-vector-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m193_lowering_simd_vector_lowering_contract.py -q`

## M194 lowering/runtime atomics and memory-order mapping

Lowering/runtime atomics memory-order mapping evidence is captured as deterministic packet artifacts rooted under `tmp/` so language atomic-order normalization and LLVM memory-order lowering mappings remain replay-stable and auditable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/`
  - `tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/atomic-memory-order-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `atomic memory-order architecture/isolation anchors` (required in source-anchor extracts):
  - `kObjc3AtomicMemoryOrderRelaxed = "relaxed"`
  - `kObjc3AtomicMemoryOrderAcquire = "acquire"`
  - `kObjc3AtomicMemoryOrderRelease = "release"`
  - `kObjc3AtomicMemoryOrderAcqRel = "acq_rel"`
  - `kObjc3AtomicMemoryOrderSeqCst = "seq_cst"`
  - `enum class Objc3AtomicMemoryOrder`
  - `TryParseObjc3AtomicMemoryOrder(...)`
  - `Objc3AtomicMemoryOrderToLLVMOrdering(...)`
  - `Objc3AtomicMemoryOrderMappingReplayKey()`
  - `acquire_release`
  - `monotonic`
  - `acquire`
  - `release`
  - `acq_rel`
  - `seq_cst`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `source anchors`:
  - `inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";`
  - `inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";`
  - `inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";`
  - `inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";`
  - `inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";`
  - `enum class Objc3AtomicMemoryOrder : std::uint8_t {`
  - `bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {`
  - `if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {`
  - `const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {`
  - `return "monotonic";`
  - `return "acquire";`
  - `return "release";`
  - `return "acq_rel";`
  - `return "seq_cst";`
  - `std::string Objc3AtomicMemoryOrderMappingReplayKey() {`
  - `std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + "=" +`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and atomic memory-order source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, atomic memory-order marker, or source anchor is missing.

Atomics memory-order capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.ll tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.manifest.json > tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/abi-ir-anchors.txt`
3. `rg -n "kObjc3AtomicMemoryOrderRelaxed|kObjc3AtomicMemoryOrderAcquire|kObjc3AtomicMemoryOrderRelease|kObjc3AtomicMemoryOrderAcqRel|kObjc3AtomicMemoryOrderSeqCst|enum class Objc3AtomicMemoryOrder|TryParseObjc3AtomicMemoryOrder|Objc3AtomicMemoryOrderToLLVMOrdering|Objc3AtomicMemoryOrderMappingReplayKey|acquire_release|monotonic|acq_rel|seq_cst|Objc3LoweringIRBoundaryReplayKey\\(|declare i32 @|\\\"lowering\\\":{\\\"runtime_dispatch_symbol\\\":\\\"" native/objc3c/src/lower/objc3_lowering_contract.h native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/atomic-memory-order-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m194_lowering_atomics_memory_order_contract.py -q`

## M195 lowering/runtime system-extension conformance and policy

Lowering/runtime system-extension conformance/policy evidence is captured as deterministic packet artifacts rooted under `tmp/` so policy validation guards, lowering boundary serialization, and runtime dispatch surfaces remain replay-stable and isolated across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/`
  - `tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/system-extension-policy-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `system-extension conformance/policy architecture+isolation anchors` (required in source-anchor extracts):
  - `ValidateSupportedLanguageVersion(...)`
  - `ValidateSupportedCompatibilityMode(...)`
  - `TryNormalizeObjc3LoweringContract(...)`
  - `kRuntimeDispatchDefaultArgs = 4`
  - `kRuntimeDispatchMaxArgs = 16`
  - `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32"`
  - `output_dir = "tmp/artifacts/compilation/objc3c-native"`
  - `frontend_options.lowering.max_message_send_args = options.max_message_send_args;`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `source anchors`:
  - `static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {`
  - `static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {`
  - `if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {`
  - `inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;`
  - `inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;`
  - `inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`
  - `std::string output_dir = "tmp/artifacts/compilation/objc3c-native";`
  - `bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,`
  - `error = "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +`
  - `std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and system-extension policy source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, system-extension policy marker, or source anchor is missing.

System-extension conformance/policy capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.ll tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.manifest.json > tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/abi-ir-anchors.txt`
3. `rg -n "ValidateSupportedLanguageVersion|ValidateSupportedCompatibilityMode|TryNormalizeObjc3LoweringContract|kRuntimeDispatchDefaultArgs = 4|kRuntimeDispatchMaxArgs = 16|kRuntimeDispatchDefaultSymbol = \\\"objc3_msgsend_i32\\\"|output_dir = \\\"tmp/artifacts/compilation/objc3c-native\\\"|frontend_options\\.lowering\\.max_message_send_args = options\\.max_message_send_args;|frontend_options\\.lowering\\.runtime_dispatch_symbol = options\\.runtime_dispatch_symbol;|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol=|declare i32 @|\\\"lowering\\\":{\\\"runtime_dispatch_symbol\\\":\\\"" native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp native/objc3c/src/pipeline/frontend_pipeline_contract.h native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/system-extension-policy-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m195_lowering_system_extension_policy_contract.py -q`

## M196 lowering/runtime C interop headers and ABI alignment

Lowering/runtime C interop header and ABI-alignment evidence is captured as deterministic packet artifacts rooted under `tmp/` so C shim header contracts, embedding ABI compatibility guards, and lowering/runtime dispatch boundaries remain replay-stable and isolated across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/`
  - `tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/c-interop-header-abi-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `C interop headers + ABI alignment architecture/isolation anchors` (required in source-anchor extracts):
  - `Optional C ABI shim for non-C++ embedding environments.`
  - `#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u`
  - `static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");`
  - `static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,`
  - `return objc3c_frontend_is_abi_compatible(requested_abi_version);`
  - `Public embedding ABI contract:`
  - `Reserved struct fields are for forward ABI growth and should be zero-initialized by callers.`
  - `ABI evolution policy for exposed structs/enums is additive; existing fields and values remain stable.`
  - `#define OBJC3C_FRONTEND_ABI_VERSION 1u`
  - `#define OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `compile_options.runtime_dispatch_symbol = runtime_symbol;`
  - `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `source anchors`:
  - `/* Optional C ABI shim for non-C++ embedding environments. */`
  - `#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u`
  - `static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");`
  - `static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,`
  - `return objc3c_frontend_is_abi_compatible(requested_abi_version);`
  - `* Public embedding ABI contract:`
  - `* - Reserved struct fields are for forward ABI growth and should be zero-initialized by callers.`
  - `* - ABI evolution policy for exposed structs/enums is additive; existing fields and values remain stable.`
  - `#define OBJC3C_FRONTEND_ABI_VERSION 1u`
  - `#define OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `compile_options.runtime_dispatch_symbol = runtime_symbol;`
  - `inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`
  - `Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and C interop header ABI-alignment source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, C interop header ABI-alignment marker, or source anchor is missing.

C interop header ABI-alignment capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.ll tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.manifest.json > tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/abi-ir-anchors.txt`
3. `rg -n "Optional C ABI shim for non-C\\+\\+ embedding environments\\.|OBJC3C_FRONTEND_C_API_ABI_VERSION|c_compile_options_t, objc3c_frontend_compile_options_t|objc3c_frontend_is_abi_compatible\\(requested_abi_version\\)|Public embedding ABI contract|Reserved struct fields are for forward ABI growth|ABI evolution policy for exposed structs/enums is additive|OBJC3C_FRONTEND_ABI_VERSION|OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION|frontend_options\\.lowering\\.runtime_dispatch_symbol = options\\.runtime_dispatch_symbol;|compile_options\\.runtime_dispatch_symbol = runtime_symbol;|kRuntimeDispatchDefaultSymbol = \\\"objc3_msgsend_i32\\\";|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol=|declare i32 @|\\\"lowering\\\":{\\\"runtime_dispatch_symbol\\\":\\\"" native/objc3c/src/libobjc3c_frontend/c_api.h native/objc3c/src/libobjc3c_frontend/c_api.cpp native/objc3c/src/libobjc3c_frontend/api.h native/objc3c/src/libobjc3c_frontend/version.h native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp native/objc3c/src/pipeline/frontend_pipeline_contract.h native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/c-interop-header-abi-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m196_lowering_c_interop_headers_abi_contract.py -q`

## M197 lowering/runtime C++ interop shim strategy

Lowering/runtime C++ interop shim strategy evidence is captured as deterministic packet artifacts rooted under `tmp/` so the C++ frontend surface, C ABI shim surface, and runtime dispatch shim remain replay-stable and isolated across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/`
  - `tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/cpp-interop-shim-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `C++ interop shim architecture/isolation anchors` (required in source-anchor extracts):
  - `Optional C ABI shim for non-C++ embedding environments.`
  - `OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u`
  - `return objc3c_frontend_compile_file(context, options, result);`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `compile_options.runtime_dispatch_symbol = runtime_symbol;`
  - `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {`
  - `static const int64_t kModulus = 2147483629LL;`
- `source anchors`:
  - `/* Optional C ABI shim for non-C++ embedding environments. */`
  - `static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");`
  - `return objc3c_frontend_compile_file(context, options, result);`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `compile_options.runtime_dispatch_symbol = runtime_symbol;`
  - `inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`
  - `Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {`
  - `static const int64_t kModulus = 2147483629LL;`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and C++ interop shim source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, C++ interop shim marker, or source anchor is missing.

C++ interop shim strategy capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.ll tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.manifest.json > tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/abi-ir-anchors.txt`
3. `rg -n "Optional C ABI shim for non-C\\+\\+ embedding environments\\.|OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u|return objc3c_frontend_compile_file\\(context, options, result\\);|frontend_options\\.lowering\\.runtime_dispatch_symbol = options\\.runtime_dispatch_symbol;|compile_options\\.runtime_dispatch_symbol = runtime_symbol;|kRuntimeDispatchDefaultSymbol = \\\"objc3_msgsend_i32\\\";|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol=|declare i32 @|\\\"lowering\\\":{\\\"runtime_dispatch_symbol\\\":\\\"|int objc3_msgsend_i32\\(|static const int64_t kModulus = 2147483629LL;" native/objc3c/src/libobjc3c_frontend/c_api.h native/objc3c/src/libobjc3c_frontend/c_api.cpp native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp native/objc3c/src/pipeline/frontend_pipeline_contract.h native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp tests/tooling/runtime/objc3_msgsend_i32_shim.c > tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/cpp-interop-shim-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m197_lowering_cpp_interop_shim_contract.py -q`

## M198 lowering/runtime swift metadata bridge

Lowering/runtime Swift metadata-bridge evidence is captured as deterministic packet artifacts rooted under `tmp/` so frontend-to-sema type metadata handoff and lowering/runtime boundary replay remain isolated and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/`
  - `tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/swift-metadata-bridge-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `swift metadata bridge architecture/isolation markers` (required in source-anchor extracts):
  - `BuildSemanticTypeMetadataHandoff(...)`
  - `param_has_invalid_type_suffix`
  - `deterministic_type_metadata_handoff`
  - `type_metadata_global_entries`
  - `type_metadata_function_entries`
  - `Objc3IRFrontendMetadata`
  - `frontend_profile`
  - `!objc3.frontend`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));`
  - `metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;`
  - `metadata.param_has_invalid_type_suffix.size() == metadata.arity;`
  - `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`
  - `result.deterministic_type_metadata_handoff =`
  - `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`
  - `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`
  - `<< ",\"deterministic_type_metadata_handoff\":"`
  - `<< ",\"type_metadata_global_entries\":"`
  - `<< ",\"type_metadata_function_entries\":"`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
  - `Objc3IRFrontendMetadata ir_frontend_metadata;`
  - `if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {`
  - `out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)`
  - `out << "!objc3.frontend = !{!0}\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and Swift metadata bridge source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, Swift metadata bridge marker, or source anchor is missing.

Swift metadata bridge capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.ll tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.manifest.json > tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/abi-ir-anchors.txt`
3. `rg -n "BuildSemanticTypeMetadataHandoff|param_has_invalid_type_suffix|deterministic_type_metadata_handoff|type_metadata_global_entries|type_metadata_function_entries|Objc3IRFrontendMetadata|EmitObjc3IRText|frontend_profile|!objc3.frontend|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/sema/objc3_semantic_passes.cpp native/objc3c/src/sema/objc3_sema_pass_manager.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/swift-metadata-bridge-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m198_lowering_swift_metadata_bridge_contract.py -q`

## M199 lowering/runtime foreign type import diagnostics

Lowering/runtime foreign-type import diagnostics evidence is captured as deterministic packet artifacts rooted under `tmp/` so Objective-C import diagnostics remain stage-isolated while lowering/runtime boundary replay metadata stays stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/`
  - `tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/foreign-type-import-diagnostics-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `foreign type import diagnostic-isolation markers` (required in source-anchor extracts):
  - `FormatDiagnostic(...)`
  - `NormalizeDiagnostics(...)`
  - `WriteDiagnosticsArtifacts(...)`
  - `FlattenStageDiagnostics(...)`
  - `ParseDiagSortKey(...)`
  - `"severity"`
  - `"line"`
  - `"column"`
  - `"code"`
  - `"message"`
  - `"raw"`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `std::string FormatDiagnostic(CXDiagnostic diagnostic) {`
  - `out << severity_text << ":" << line << ":" << column << ": " << ToString(clang_getDiagnosticSpelling(diagnostic));`
  - `diagnostics.push_back(FormatDiagnostic(diagnostic));`
  - `NormalizeDiagnostics(diagnostics);`
  - `WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);`
  - `DiagSortKey ParseDiagSortKey(const std::string &diag) {`
  - `std::stable_sort(rows.begin(), rows.end(), [](const DiagSortKey &a, const DiagSortKey &b) {`
  - `if (diagnostics.empty() || diagnostics.back() != row.raw) {`
  - `const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);`
  - `const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - foreign-type import diagnostics remain stable after canonical normalization and JSON re-render (`severity/line/column/code/message/raw`) across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, foreign-type diagnostic-isolation marker, or source anchor is missing.

Foreign-type import diagnostics capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.ll tmp/artifacts/compilation/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/module.manifest.json > tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/abi-ir-anchors.txt`
3. `rg -n "FormatDiagnostic\\(|NormalizeDiagnostics\\(|WriteDiagnosticsArtifacts\\(|FlattenStageDiagnostics\\(|ParseDiagSortKey\\(|\\\"severity\\\":|\\\"line\\\":|\\\"column\\\":|\\\"code\\\":|\\\"message\\\":|\\\"raw\\\":|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/driver/objc3_objectivec_path.cpp native/objc3c/src/diag/objc3_diag_utils.cpp native/objc3c/src/io/objc3_diagnostics_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m199/lowering-runtime-foreign-type-import-diagnostics/foreign-type-import-diagnostics-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m199_lowering_foreign_type_diagnostics_contract.py -q`

## M200 lowering/runtime interop integration suite and packaging

Lowering/runtime interop integration-suite and packaging evidence is captured as deterministic packet artifacts rooted under `tmp/` so runtime-dispatch contract transport stays isolated and replay-stable across CLI/C-API/frontend/lowering boundaries.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/`
  - `tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/interop-packaging-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `interop integration/packaging architecture markers` (required in source-anchor extracts):
  - `options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;`
  - `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`
  - `compile_options.runtime_dispatch_symbol = runtime_symbol;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and interop-packaging source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, or interop integration/packaging marker is missing.

Interop integration-suite/packaging capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.ll tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.manifest.json > tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/abi-ir-anchors.txt`
3. `rg -n "options\.lowering\.runtime_dispatch_symbol = cli_options\.runtime_dispatch_symbol;|frontend_options\.lowering\.runtime_dispatch_symbol = options\.runtime_dispatch_symbol;|compile_options\.runtime_dispatch_symbol = runtime_symbol;|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol=|lowering_ir_boundary =|declare i32 @|\\\"lowering\\\":{\\\"runtime_dispatch_symbol\\\":\\\"|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/driver/objc3_frontend_options.cpp native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/interop-packaging-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m200_lowering_interop_packaging_contract.py -q`

## M201 lowering/runtime macro expansion architecture and isolation

Lowering/runtime macro-expansion architecture and isolation evidence is captured as deterministic packet artifacts rooted under `tmp/` so migration-hint transport and pragma-contract boundaries remain replay-stable through lowering/runtime metadata emission.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/`
  - `tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/macro-expansion-isolation-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `macro-expansion isolation markers` (required in source-anchor extracts):
  - `migration_hints_.legacy_yes_count`
  - `migration_hints_.legacy_no_count`
  - `migration_hints_.legacy_null_count`
  - `language_version_pragma_contract_.directive_count`
  - `result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;`
  - `result.language_version_pragma_contract.seen = pragma_contract.seen;`
  - `sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;`
  - `append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");`
  - `append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");`
  - `append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");`
  - `"migration_hints":{"legacy_yes":`
  - `"language_version_pragma_contract":{"seen":`
  - `ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;`
  - `ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;`
  - `ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `++migration_hints_.legacy_yes_count;`
  - `++migration_hints_.legacy_no_count;`
  - `++migration_hints_.legacy_null_count;`
  - `++language_version_pragma_contract_.directive_count;`
  - `result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;`
  - `result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;`
  - `result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;`
  - `result.language_version_pragma_contract.seen = pragma_contract.seen;`
  - `sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;`
  - `sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;`
  - `sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;`
  - `append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");`
  - `append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");`
  - `append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");`
  - `manifest << "    \"migration_hints\":{\"legacy_yes\":" << pipeline_result.migration_hints.legacy_yes_count`
  - `manifest << "    \"language_version_pragma_contract\":{\"seen\":"`
  - `ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;`
  - `ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;`
  - `ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;`
  - `out << "; frontend_profile = language_version="`
  - `out << "!objc3.frontend = !{!0}\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and macro-expansion isolation source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, macro-expansion marker, or source anchor is missing.

Macro-expansion architecture/isolation capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/module.ll tmp/artifacts/compilation/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/module.manifest.json > tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/abi-ir-anchors.txt`
3. `rg -n "migration_hints_.legacy_yes_count|migration_hints_.legacy_no_count|migration_hints_.legacy_null_count|language_version_pragma_contract_.directive_count|result.migration_hints.legacy_yes_count|result.migration_hints.legacy_no_count|result.migration_hints.legacy_null_count|result.language_version_pragma_contract.seen|sema_input.migration_hints.legacy_yes_count|sema_input.migration_hints.legacy_no_count|sema_input.migration_hints.legacy_null_count|append_for_literal\\(input.migration_hints.legacy_yes_count|append_for_literal\\(input.migration_hints.legacy_no_count|append_for_literal\\(input.migration_hints.legacy_null_count|migration_hints|language_version_pragma_contract|ir_frontend_metadata\\.migration_legacy_yes|ir_frontend_metadata\\.migration_legacy_no|ir_frontend_metadata\\.migration_legacy_null|frontend_profile|!objc3\\.frontend|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/sema/objc3_sema_pass_manager.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m201/lowering-runtime-macro-expansion-isolation/macro-expansion-isolation-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m201_lowering_macro_expansion_contract.py -q`

## M205 lowering/runtime macro security policy enforcement

Lowering/runtime macro-security policy enforcement evidence is captured as deterministic packet artifacts rooted under `tmp/` so pragma-policy diagnostics and lowering replay boundaries remain auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `macro-security markers` (required in source-anchor extracts):
  - `ConsumeLanguageVersionPragmas(diagnostics)`
  - `ConsumeLanguageVersionPragmaDirective(...)`
  - `LanguageVersionPragmaPlacement::kNonLeading`
  - `O3L005`
  - `O3L006`
  - `O3L007`
  - `O3L008`
  - `frontend.language_version_pragma_contract`
  - `directive_count`
  - `duplicate`
  - `non_leading`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `ConsumeLanguageVersionPragmas(diagnostics);`
  - `ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))`
  - `if (placement == LanguageVersionPragmaPlacement::kNonLeading) {`
  - `diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));`
  - `result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;`
  - `result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;`
  - `result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;`
  - `manifest << "    \"language_version_pragma_contract\":{\"seen\":"`
  - `<< ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count`
  - `<< ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")`
  - `<< ",\"non_leading\":"`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and macro-security source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, macro-security marker, or source anchor is missing.

Macro-security capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt`
3. `rg -n "ConsumeLanguageVersionPragmas\(diagnostics\)|ConsumeLanguageVersionPragmaDirective\(|LanguageVersionPragmaPlacement::kNonLeading|O3L005|O3L006|O3L007|O3L008|language_version_pragma_contract|directive_count|duplicate|non_leading|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m205_lowering_macro_security_contract.py -q`

## M207 lowering/runtime dispatch-specific optimization passes

Lowering/runtime dispatch-specific optimization pass evidence is captured as deterministic packet artifacts rooted under `tmp/` so nil-elision, non-nil fast-path routing, and usage-driven runtime-dispatch declaration emission remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `dispatch optimization markers` (required in source-anchor extracts):
  - `runtime_dispatch_call_emitted_ = false;`
  - `runtime_dispatch_call_emitted_ = true;`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `msg_nil_`
  - `msg_dispatch_`
  - `phi i32 [0, %`
  - `FunctionMayHaveGlobalSideEffects`
  - `call_may_have_global_side_effects`
  - `global_proofs_invalidated`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `if (runtime_dispatch_call_emitted_) {`
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `const std::string nil_label = NewLabel(ctx, "msg_nil_");`
  - `const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");`
  - `ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +`
  - `const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);`
  - `if (call_may_have_global_side_effects) {`
  - `ctx.global_proofs_invalidated = true;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and dispatch optimization source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, dispatch optimization marker, or source anchor is missing.

Dispatch optimization capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt`
3. `rg -n "runtime_dispatch_call_emitted_|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|msg_nil_|msg_dispatch_|phi i32 \[0, %|FunctionMayHaveGlobalSideEffects|call_may_have_global_side_effects|global_proofs_invalidated|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m207_lowering_dispatch_optimizations_contract.py -q`

## M208 lowering/runtime whole-module optimization controls

Lowering/runtime whole-module optimization (WMO) controls are captured as deterministic packet artifacts rooted under `tmp/` so module-shape and runtime-dispatch surfaces remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.manifest.json`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `whole-module control markers` (required in source-anchor extracts):
  - `max_message_send_args`
  - `semantic_surface`
  - `declared_functions`
  - `resolved_function_symbols`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `manifest_functions.reserve(program.functions.size())`
  - `std::unordered_set<std::string> manifest_function_names`
  - `if (manifest_function_names.insert(fn.name).second)`
  - `manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `<< ",\"declared_functions\":" << manifest_functions.size()`
  - `<< ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {`
  - `error = "invalid lowering contract max_message_send_args: "`
  - `boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;`
  - `boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;`
  - `if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {`
  - `lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");`
  - `call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "`
- `closure criteria`:
  - rerunning identical source + lowering/runtime options preserves byte-identical `module.ll` and `module.manifest.json`.
  - ABI/IR anchors and WMO control source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, whole-module control marker, or source anchor is missing.

WMO control capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.ll tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.manifest.json > tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/abi-ir-anchors.txt`
3. `rg -n "manifest_functions\\.reserve\\(program\\.functions\\.size\\(\\)\\)|manifest_function_names|max_message_send_args|semantic_surface|declared_functions|resolved_function_symbols|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp > tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m208_lowering_wmo_contract.py -q`

## M209 lowering/runtime profile-guided optimization hooks

Lowering/runtime LLVM profile-guided optimization (PGO) hook evidence is captured as deterministic packet artifacts rooted under `tmp/` so profile surfaces remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.manifest.json`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/pgo-hook-source-anchors.txt`
- `PGO hook ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `source anchors`:
  - `Objc3IRFrontendMetadata ir_frontend_metadata;`
  - `ir_frontend_metadata.language_version = options.language_version;`
  - `ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);`
  - `ir_frontend_metadata.migration_assist = options.migration_assist;`
  - `ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;`
  - `ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;`
  - `ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)`
  - `out << "!objc3.frontend = !{!0}\n";`
  - `out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\""`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning identical source + lowering/runtime options preserves byte-identical `module.ll` and `module.manifest.json`.
  - PGO hook ABI/IR anchors and source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, or source anchor is missing.

PGO hook capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|!0 = !{|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.ll tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.manifest.json > tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/abi-ir-anchors.txt`
3. `rg -n "Objc3IRFrontendMetadata ir_frontend_metadata;|ir_frontend_metadata\\.language_version = options\\.language_version;|ir_frontend_metadata\\.compatibility_mode = CompatibilityModeName\\(options\\.compatibility_mode\\);|ir_frontend_metadata\\.migration_assist = options\\.migration_assist;|ir_frontend_metadata\\.migration_legacy_yes = pipeline_result\\.migration_hints\\.legacy_yes_count;|ir_frontend_metadata\\.migration_legacy_no = pipeline_result\\.migration_hints\\.legacy_no_count;|ir_frontend_metadata\\.migration_legacy_null = pipeline_result\\.migration_hints\\.legacy_null_count;|Objc3LoweringIRBoundaryReplayKey\\(|invalid lowering contract runtime_dispatch_symbol|runtime_dispatch_symbol=|runtime_dispatch_arg_slots=|selector_global_ordering=lexicographic" native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/pgo-hook-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m209_lowering_pgo_contract.py -q`

## M210 lowering/runtime performance budgets and regression gates

Lowering/LLVM/runtime perf regression evidence is captured as a deterministic packet rooted under `tmp/` so throughput budget and cache-proof gates fail closed.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/`
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json`
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `perf regression gate markers` (required in gate extracts):
  - `tmp/artifacts/objc3c-native/perf-budget`
  - `summary.json`
  - `defaultMaxElapsedMs`
  - `defaultPerFixtureBudgetMs`
  - `cache_hit=(true|false)`
  - `dispatch_fixture_count`
  - `max_elapsed_ms`
  - `total_elapsed_ms`
  - `budget_breached`
  - `cache_proof`
  - `status`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `$perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"`
  - `$summaryPath = Join-Path $runDir "summary.json"`
  - `$defaultMaxElapsedMs = 4000`
  - `$defaultPerFixtureBudgetMs = 150`
  - `$matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\s*$")`
  - `throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"`
  - `dispatch_fixture_count = $dispatchFixtureCount`
- `closure criteria`:
  - rerunning the same source + lowering options must preserve byte-identical `module.ll` and `module.manifest.json` plus stable perf-budget summary gate markers.
  - perf-budget packets remain fail-closed when `status != "PASS"`, `budget_breached == true`, or cache-proof gates drift.
  - closure remains open if any required packet artifact, ABI/IR anchor, perf regression gate marker, or source anchor is missing.

Performance-budget capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression --emit-prefix module`
2. `npm run test:objc3c:perf-budget`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt`
4. `rg -n "tmp/artifacts/objc3c-native/perf-budget|summary.json|defaultMaxElapsedMs|defaultPerFixtureBudgetMs|cache_hit=|dispatch_fixture_count|max_elapsed_ms|total_elapsed_ms|budget_breached|cache_proof|status" scripts/check_objc3c_native_perf_budget.ps1 tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt`
5. `python -m pytest tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py -q`

## M214 lowering/runtime daemonized compiler profile

Lowering/runtime daemon/watch mode evidence is captured as deterministic packet artifacts under `tmp/` for incremental replay validation.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.manifest.json`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in daemonized packet artifacts):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `incremental replay markers` (required in daemon/watch evidence extracts):
  - `@@ cycle:cycle-001`
  - `@@ cycle:cycle-002`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `incremental_cycle_id`
  - `run1_sha256`
  - `run2_sha256`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - cycle-001 and cycle-002 captures from identical source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - ABI/IR anchor extracts and incremental replay marker extracts remain stable across daemon/watch reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, incremental replay marker, or source anchor is missing.

Daemonized capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001 --emit-prefix module`
2. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002 --emit-prefix module`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.ll tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.manifest.json tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.ll tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.manifest.json > tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/abi-ir-anchors.txt`
4. `@("@@ cycle:cycle-001", "@@ cycle:cycle-002") | Set-Content tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt; rg -n "\"incremental_cycle_id\":|\"run1_sha256\":|\"run2_sha256\":" tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json >> tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m214_lowering_daemonized_contract.py -q`

## M215 lowering/runtime SDK packaging profile

Lowering/runtime SDK packaging evidence is captured as a deterministic packet for IDE-facing toolchains under `tmp/`.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.obj`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt`
- `ABI/IR anchors` (persist verbatim in each SDK packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `IDE-consumable artifact markers` (required in SDK packet marker extracts):
  - `"schema_version": "1.0.0"`
  - `"diagnostics": [`
  - `"severity":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
  - `"module":`
  - `"frontend":`
  - `"lowering":`
  - `"globals":`
  - `"functions":`
  - `"runtime_dispatch_symbol":`
  - `"runtime_dispatch_arg_slots":`
  - `clang`
  - `llvm-direct`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());`
  - `WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest_json);`
  - `const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, `module.diagnostics.json`, and `module.object-backend.txt`.
  - ABI/IR anchor extracts and IDE-consumable marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, source anchor, or IDE-consumable marker is missing.

SDK packaging capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt`
3. `rg -n "\"schema_version\":|\"diagnostics\":|\"severity\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":|\"module\":|\"frontend\":|\"lowering\":|\"globals\":|\"functions\":|\"runtime_dispatch_symbol\":|\"runtime_dispatch_arg_slots\":|clang|llvm-direct" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py -q`

## M216 lowering/runtime conformance suite profile

Lowering/runtime conformance suite evidence is captured as deterministic packet artifacts under `tmp/`.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.manifest.json`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/conformance-matrix-markers.txt`
- `ABI/IR anchors` (persist verbatim in the packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `conformance-matrix markers` (required in matrix summary evidence):
  - `suite.status`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `spec_section_map`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `Require-Range "CRPT-" 1 6`
  - `Require-Range "CAN-" 1 7`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - conformance matrix marker rows and ABI/IR anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, source anchor, or conformance-matrix marker is missing.

Conformance suite capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite --emit-prefix module`
2. `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.ll tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.manifest.json > tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/abi-ir-anchors.txt`
4. `rg -n "\"suite\":|\"status\":|\"matrix\":|\"total_cases\":|\"failed_cases\":|\"spec_section_map\"" tmp/artifacts/conformance-suite/<target>/summary.json > tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/conformance-matrix-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m216_lowering_conformance_contract.py -q`

## M217 lowering/runtime differential parity profile

Lowering/runtime differential parity is captured as a deterministic packet versus baseline toolchains under `tmp/`.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/`
- `packet toolchain roots`:
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/`
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-clang/`
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-llvm-direct/`
- `packet artifacts` (required in each toolchain root):
  - `module.ll`
  - `module.manifest.json`
- `diff marker reports`:
  - `tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/ir-diff-markers.txt`
  - `tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/manifest-diff-markers.txt`
- `ABI/IR anchors` (persist verbatim in native and baseline packets):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `differential source markers` (source anchors to include in parity packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `diff markers` (required deterministic parity rows):
  - `@@ anchor:lowering_ir_boundary`
  - `@@ anchor:frontend_profile`
  - `@@ anchor:objc3.frontend`
  - `@@ anchor:runtime_dispatch_declare`
  - `@@ anchor:manifest.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical packet artifacts inside each toolchain root.
  - native and baseline toolchains may differ in non-anchor payloads, but ABI/IR anchors and diff marker rows must remain stable across reruns.
  - closure remains open if any required toolchain packet artifact, ABI/IR anchor, differential source marker, or diff marker report is missing.

Differential parity capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native --emit-prefix module`
2. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-clang --emit-prefix module --cli-ir-object-backend clang`
3. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-llvm-direct --emit-prefix module --cli-ir-object-backend llvm-direct`
4. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/module.ll > tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/ir-diff-markers.txt`
5. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/module.manifest.json > tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/manifest-diff-markers.txt`
6. `python -m pytest tests/tooling/test_objc3c_m217_lowering_differential_contract.py -q`

## M218 lowering/runtime RC provenance profile

Release-candidate lowering/runtime provenance is captured as a deterministic packet rooted under `tmp/`.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json`
  - `tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
  - `tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt`
- `ABI/IR anchors` (persist verbatim in each RC packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `attestation markers` (contract markers to include in RC packet attestation notes):
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `RC provenance closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - replay and attestation marker reports stay stable across reruns (no added/removed markers).
  - closure remains open if any required packet artifact, ABI/IR anchor, replay marker, or attestation marker is missing.

RC provenance capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json >> tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
4. `rg -n "Objc3LoweringIRBoundaryReplayKey|invalid lowering contract runtime_dispatch_symbol|runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py -q`

## M219 lowering/runtime cross-platform parity profile

Cross-platform lowering/runtime parity evidence is captured as deterministic packet artifacts under `tmp/` across windows/linux/macos.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/`
- `platform packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/windows/`
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/linux/`
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/macos/`
- `packet artifacts` (required for each platform root):
  - `module.ll`
  - `module.manifest.json`
- `replay marker reports`:
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/windows-replay-markers.txt`
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/linux-replay-markers.txt`
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/macos-replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in each platform packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors that must be present in packet notes across windows/linux/macos):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `cross-platform parity closure criteria`:
  - rerunning the same source + lowering options on each platform produces byte-identical `module.ll` and `module.manifest.json` within that platform.
  - replay marker token sets must match across windows/linux/macos (ordering may differ only by tool output line-number prefixes).
  - closure remains open if any required platform packet artifact, ABI/IR anchor, or replay marker is missing.

Cross-platform capture commands (run per platform worker):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform> --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>/module.ll > tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>-replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>/module.manifest.json >> tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>-replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m219_lowering_cross_platform_contract.py -q`

## M220 lowering/runtime public-beta triage profile

Public-beta lowering/runtime triage must ship as deterministic packet evidence rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json`
  - `tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in each beta triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `patch-loop closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - replay markers stay stable across reruns (no added/removed lines, no reordered anchors).
  - closure remains open if any ABI/IR anchor or replay marker is missing.

Public-beta triage capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll > tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json >> tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py -q`

## M221 lowering/runtime GA blocker burn-down profile

GA-blocker burn-down evidence for lowering/runtime should be captured as a deterministic packet rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json`
  - `tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `GA blocker closure signal`:
  - identical source + lowering options produce byte-identical packet artifacts and stable replay markers.

Burn-down capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll > tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json >> tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py -q`

## M224 lowering/LLVM IR/runtime ABI release readiness

GA readiness evidence for native `.objc3` lowering remains deterministic and fail-closed:

- IR replay markers must remain present and aligned:
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
- Manifest lowering packet must mirror the runtime boundary contract:
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- Runtime ABI declaration remains symbol-aligned with the lowering contract when dispatch calls are emitted:
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
- Boundary normalization remains fail-closed for invalid symbols:
  - `invalid lowering contract runtime_dispatch_symbol`
- Determinism expectation for GA:
  - identical source + lowering options produce byte-identical `module.ll` and `module.manifest.json`.

Operator evidence sequence:

1. Generate artifacts in a deterministic tmp root:

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness --emit-prefix module
```

2. Validate marker alignment in:
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.manifest.json`
3. Run contract guard:
  - `python -m pytest tests/tooling/test_objc3c_m224_lowering_release_contract.py -q`

## M225 lowering/runtime roadmap seeding profile

Post-1.0 backlog seeding for lowering/runtime 1.1/1.2 should record deterministic artifact evidence plus source-anchored ABI/IR signals:

- `1.1 artifact evidence capture`:
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.manifest.json`
  - extract and persist:
    - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
    - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
    - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
    - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `1.2 ABI/IR signal extraction`:
  - replay-key source marker: `Objc3LoweringIRBoundaryReplayKey(...)`
  - IR ABI declaration marker: `declare i32 @` + `runtime_dispatch_symbol`
  - lowering normalization marker: `invalid lowering contract runtime_dispatch_symbol`

Roadmap-seeding commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding --emit-prefix module`
2. `python -m pytest tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py -q`

## Recovery fixture layout (`tests/tooling/fixtures/native/recovery`)

Current recovery fixtures are partitioned as:

```text
tests/tooling/fixtures/native/recovery/
  positive/
    bool_param_flow_relational.objc3
    comparison_logic.objc3
    function_return_annotation_bool.objc3
    function_return_annotation_bool.objc3-ir.expect.txt
    hello.objc3
    main_bool_entrypoint.objc3
    main_bool_entrypoint.objc3-ir.expect.txt
    lowering_dispatch/
      msgsend_lookup_basic.m
      msgsend_lookup_basic.dispatch-ir.expect.txt
      msgsend_lookup_two_args.m
      msgsend_lookup_two_args.dispatch-ir.expect.txt
    message_send_basic.objc3
    message_send_bool_compatible.objc3
    message_send_four_args.objc3
    message_send_keywords.objc3
    relational_logical_bool_literals.objc3
    return_paths_ok.objc3
    typed_bool_param_i32_expr_call.objc3
    typed_bool_param_i32_expr_call.objc3-ir.expect.txt
    typed_bool_return_literal_edges.objc3
    typed_bool_return_literal_edges.objc3-ir.expect.txt
    typed_i32_bool.objc3
    typed_i32_bool.objc3-ir.expect.txt
    typed_i32_return_from_bool_expr.objc3
    typed_i32_return_from_bool_expr.objc3-ir.expect.txt
  negative/
    neg_bool_fn_value.objc3
    neg_i32_param_from_bool_call.objc3
    neg_param_nullable.objc3
    neg_param_type_generic.objc3
    neg_rel_arg_mismatch.objc3
    neg_typed_bool_return_from_i32_call.objc3
    negative_arity_mismatch.objc3
    negative_message_arg_function.objc3
    negative_message_missing_keyword_colon.objc3
    negative_message_receiver_function.objc3
    negative_message_too_many_args.objc3
    negative_message_unterminated.objc3
    negative_missing_return.objc3
    negative_return_annotation_type_mismatch.objc3
    negative_return_annotation_unsupported_type.objc3
    negative_type_mismatch.objc3
    negative_undefined_symbol.objc3
```

`scripts/check_objc3c_native_recovery_contract.ps1` discovers fixtures recursively under `positive/` and `negative/` and enforces contract behavior per class (compile success/object+manifest artifacts for positive fixtures, deterministic failure diagnostics for negative fixtures).

`scripts/run_objc3c_lowering_regression_suite.ps1` also supports per-fixture dispatch IR assertions for Objective-C fixtures:

- Place `<fixture>.dispatch-ir.expect.txt` next to the `.m` file.
- One unique non-comment token per line (`#` and blank lines ignored; duplicate tokens fail validation).
- Optional dispatch fixture roots are discovered in deterministic order when present:
  - `tests/tooling/fixtures/native/recovery/positive/lowering_dispatch`
  - `tests/tooling/fixtures/native/dispatch/positive`
- The suite emits replay artifacts `module.dispatch.ll` for run1/run2 under the case directory and requires:
  - clang emit success for both runs,
  - deterministic `module.dispatch.ll` bytes across replay,
  - every token to appear in both emitted IR files.
- For `.objc3` fixtures, place `<fixture>.objc3-ir.expect.txt` next to the source file to assert deterministic native-lowered IR markers (`module.ll`) across replay runs.

## M26 Lane-E execution smoke harness contract (`scripts/check_objc3c_native_execution_smoke.ps1`)

Execution smoke validates a native `.objc3` compile-link-run path and fails closed on the first contract breach.

- Harness preconditions:
  - Native compiler executable must exist at `artifacts/bin/objc3c-native.exe` (or be buildable via `scripts/build_objc3c_native.ps1`).
  - Runtime shim source must exist at `tests/tooling/runtime/objc3_msgsend_i32_shim.c`.
  - `clang` must be invokable (override with `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`).
- Positive fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/positive` in deterministic sorted order.
  - Requires `<fixture>.exitcode.txt` sidecar with one integer expected process exit code.
  - Compiles via `objc3c-native.exe` and requires `module.obj`.
  - When positive `.meta.json` explicitly sets `execution.requires_runtime_shim`, harness verifies `module.ll` parity:
    - `true`: lowered IR must include both runtime dispatch declaration and call for the configured symbol.
    - `false`: lowered IR must include neither runtime dispatch declaration nor call for the configured symbol.
  - Links with `clang <module.obj> tests/tooling/runtime/objc3_msgsend_i32_shim.c -o module.exe`.
  - Runs `module.exe` and requires `run_exit == expected_exit`.
- Negative fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/negative` in deterministic sorted order.
  - Requires `<fixture>.meta.json` sidecar.
  - Sidecar contract currently consumed by the harness:
    - `fixture`: filename that must match the fixture.
    - `expect_failure.stage`: `compile`, `link`, or `run`.
    - `expect_failure.required_diagnostic_tokens`: required stage-diagnostic tokens (case-sensitive matching).
    - `execution.requires_runtime_shim`: informational flag documenting whether a successful execution path would require the shim.
    - `execution.runtime_dispatch_symbol`: optional symbol name (defaults to `objc3_msgsend_i32`).
  - Stage behavior:
    - `compile`: requires non-zero compile exit and expected tokens in compile diagnostics.
    - `link`: requires compile success + non-zero link exit + expected tokens in link diagnostics.
    - `run`: requires compile/link success + non-zero run exit + expected tokens in run diagnostics.
- Output contract:
  - Per-run artifacts: `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/`.
  - Per-case logs: compile/link/run logs and compile out-dir artifacts.
  - Summary JSON: `summary.json` with `status`, `total`, `passed`, `failed`, and per-case result rows.

## Runtime shim contract (`objc3_msgsend_i32`)

- `.objc3` message-send lowering emits direct calls with configurable slot count:
  - `declare i32 @objc3_msgsend_i32(i32, ptr, i32, ..., i32)`
  - Slot count equals `--objc3-max-message-args` (default `4`).
- Native execution smoke expects a C-ABI compatible runtime shim symbol named exactly `objc3_msgsend_i32`, equivalent to:

```c
int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3);
```

- Call-shape contract:
  - `receiver` is lowered as `i32`.
  - `selector` is a pointer to a lowered selector literal.
  - Dispatch argument slots are passed as `i32`; unused slots are zero-filled by lowering.
  - When lowered receiver value is `0` (`nil`), native IR short-circuits message send result to `0`.
  - Link-time runtime-dispatch symbol resolution still applies because emitted IR keeps a non-nil dispatch branch.
  - For direct syntactic `nil` receiver sends that lower to constant `0`, runtime-dispatch declaration/call emission is omitted.
  - For immutable local bindings initialized to `nil`, runtime-dispatch declaration/call emission is omitted when no other non-elided sends are present.
  - For flow-sensitive nil-bound send sites prior to reassignment, runtime-dispatch call emission is omitted.
  - Explicit `= nil` reassignment regains nil-bound elision eligibility for subsequent identifier receiver sends.
  - Nil-provenance elision eligibility propagates through `let`/`for let` alias bindings when their initializer expression is nil-proven.
  - Conditional receiver expressions are nil-elided when condition evaluation is compile-time known and selects a nil-proven branch.
  - Immutable global identifiers with nil-proven initializers are nil-elided under the same dispatch omission contract.
  - Mutable global identifiers are excluded from global nil elision and continue to lower through runtime nil-check branch scaffolding.
  - Runtime-dispatch declaration emission is now usage-driven: it is emitted only when lowering emits at least one non-elided runtime-dispatch call.
  - Compile-time non-nil receivers lower through a direct runtime-dispatch call path without `msg_nil/msg_dispatch` branch scaffolding.
  - Current non-nil fast-path eligibility includes non-zero integer literals, `YES`, unary constant-expression receivers, short-circuit logical constant-expression receivers, compile-time non-zero global constant identifiers, and flow-sensitive local bindings proven compile-time non-zero at the send site.
  - Global identifier proofing is mutation-gated: only receiver globals with no detected write forms in function bodies (`=`, compound assignment, `++`/`--`, and `for` init/step assignment/update clauses) are eligible for compile-time non-zero fast-path lowering.
  - Call boundaries invalidate global receiver proof state only when the callee is effect-summary impure; pure-call boundaries retain global nil/non-zero proofs and preserve post-call fast-path/elision eligibility.
  - `pure fn` annotations are supported for prototypes and definitions: pure-annotated external prototypes are treated as side-effect-free call boundaries, while impure pure-annotated definitions fail during semantic validation with deterministic diagnostic `O3S215` (`pure contract violation: function '<name>' declared 'pure' has side effects (cause: <cause-token>; cause-site:<line>:<column>; detail:<detail-token>@<line>:<column>)`) emitted at the definition location.
  - Deterministic pure-contract cause tokens currently include `global-write`, `message-send`, `impure-callee:<name>`, and `unannotated-extern-call:<name>`.
  - For call-derived causes (`impure-callee:*`, `unannotated-extern-call:*`), `cause-site` coordinates bind to the callee identifier token location.
  - `detail` carries the propagated leaf impurity token/site so transitive call-derived failures expose the underlying root cause deterministically.
  - Compile-time non-nil proofing uses local constant-expression tracking for literals, identifiers bound to compile-time constants, conditionals, unary canonical forms, and supported binary operators.
  - Logical `&&`/`||` constant evaluation follows short-circuit semantics so skipped branches are not required to be compile-time evaluable for receiver non-nil proofing.
  - Flow-sensitive non-zero proofs can be regained after explicit `=` writes to compile-time non-zero values and are invalidated by subsequent writes that are not compile-time non-zero proofs.
  - Numeric zero receiver sends keep runtime-dispatch declaration/call emission.
  - Reassignments to non-nil or runtime-unknown values keep runtime-dispatch declaration/call emission.
- Determinism assumption for smoke:
  - For identical inputs, shim behavior should be deterministic and stable.
  - Harness assertions are based on process exit code equality (positive fixtures) and deterministic link diagnostics token matching (negative fixtures).

## Execution fixture layout (`tests/tooling/fixtures/native/execution`)

Execution smoke fixture roots and sidecars:

```text
tests/tooling/fixtures/native/execution/
  positive/
    <name>.objc3
    <name>.exitcode.txt
    [optional] <name>.meta.json
  negative/
    <name>.objc3
    <name>.meta.json
```

Metadata conventions consumed by the current harness:

- Positive sidecar (`<name>.exitcode.txt`):
  - Single integer exit code (parsed as `int`).
- Optional positive sidecar (`<name>.meta.json`):
  - `fixture`: must match fixture filename.
  - `execution.native_compile_args`: optional string array appended to native compiler invocation.
  - `execution.requires_runtime_shim` (optional): when present, execution smoke enforces `module.ll` dispatch declaration/call parity for the configured dispatch symbol.
  - `execution.runtime_dispatch_symbol` (optional): runtime dispatch symbol for parity checks (defaults to `objc3_msgsend_i32`).
- Negative sidecar (`<name>.meta.json`):
  - `expect_failure.stage`: `compile`, `link`, or `run`.
  - `expect_failure.required_diagnostic_tokens`: deterministic stage-diagnostic token list.
  - `execution.requires_runtime_shim`: informational flag for runtime dependency.
  - `execution.runtime_dispatch_symbol` (optional): expected runtime symbol.

Current repository state on 2026-02-27:

- `tests/tooling/fixtures/native/execution/positive` includes baseline, assignment, do-while, and for-loop smoke fixtures with `.exitcode.txt` sidecars.
- `tests/tooling/fixtures/native/execution/negative` includes compile/link-stage fixtures with `.meta.json` sidecars.

## Build and compile commands

From repo root:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3
```

Native driver option for `.objc3` frontend behavior:
- `--objc3-max-message-args <0-16>`
  - Default: `4`
  - Tightens the semantic arg-cap check for bracketed message-send expressions.
- `--objc3-runtime-dispatch-symbol <symbol>`
  - Default: `objc3_msgsend_i32`
  - Rebinds the runtime dispatch call target used by message-send lowering (`[A-Za-z_.$][A-Za-z0-9_.$]*`).

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3
```

## Driver shell split validation commands (M136-E001)

Use one `.objc3` input and one non-`.objc3` Objective-C input to validate both shell branches:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objc3 --emit-prefix module_objc3
npm run compile:objc3c -- tests/tooling/fixtures/native/recovery/positive/lowering_dispatch/msgsend_lookup_basic.m --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objectivec --emit-prefix module_objc
```

Expected success surface:

- `.objc3` compile writes diagnostics + manifest + `module_objc3.ll` + `module_objc3.obj`.
- Objective-C compile writes diagnostics + manifest + `module_objc.obj`.
- Both invocations exit `0` on success.

## Lexer extraction contract artifacts (M137-E001)

`scripts/check_objc3c_lexer_extraction_token_contract.ps1` emits deterministic validation artifacts under:

- `tmp/artifacts/objc3c-native/lexer-extraction-token-contract/<run_id>/`
  - `summary.json` (contract id, pass/fail counters, check rows, and per-case replay evidence)
  - run logs and replay output directories for positive/negative lexer fixtures

Commands:

```powershell
npm run test:objc3c:lexer-extraction-token-contract
npm run test:objc3c:lexer-parity
npm run check:compiler-closeout:m137
```

## Parser/AST extraction validation artifacts (M138-E001)

`npm run test:objc3c:parser-replay-proof` writes deterministic replay-proof outputs under:

- `tmp/artifacts/objc3c-native/parser-replay-proof/<proof_run_id>/summary.json`

Parser/AST extraction surface validation commands:

```powershell
npm run test:objc3c:parser-ast-extraction
npm run check:compiler-closeout:m138
```

`npm run check:compiler-closeout:m138` fail-closes on parser + AST builder + docs/CI/release wiring drift via:

- `python scripts/check_m138_parser_ast_contract.py`
- `python -m pytest tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py -q`

## Sema pass-manager + diagnostics bus validation artifacts (M139-E001)

Sema extraction and diagnostics-bus validation commands:

```powershell
npm run test:objc3c:sema-pass-manager-diagnostics-bus
npm run check:compiler-closeout:m139
```

`npm run test:objc3c:sema-pass-manager-diagnostics-bus` writes deterministic pass-manager diagnostics-bus proof artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Deterministic run-id behavior:

- Default run id is fixed to `m143-sema-type-system-default`.
- Optional override remains `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID` (validated token syntax).

`npm run check:compiler-closeout:m139` fail-closes on sema pass-manager and diagnostics-bus wiring drift via:

- `python scripts/check_m139_sema_pass_manager_contract.py`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`
- `python -m pytest tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_pure_contract_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py -q`

## Frontend library boundary validation artifacts (M140-E001)

M140 extraction and boundary closeout commands:

```powershell
npm run test:objc3c:m140-boundary-contract
npm run check:compiler-closeout:m140
```

`npm run check:compiler-closeout:m140` fail-closes on frontend library boundary drift via:

- `python scripts/check_m140_frontend_library_boundary_contract.py`
- `python -m pytest tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py tests/tooling/test_objc3c_m140_boundary_contract.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## CMake targetization and linkage topology validation artifacts (M141-E001)

M141 target-topology validation commands:

```powershell
npm run test:objc3c:m141-target-topology
npm run check:compiler-closeout:m141
```

`npm run check:compiler-closeout:m141` fail-closes on targetization/linkage-topology drift via:

- `python scripts/check_m141_cmake_target_topology_contract.py`
- `python -m pytest tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_cmake_target_topology.py tests/tooling/test_objc3c_process_io_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## Frontend lowering parity harness artifacts (M142-E001)

Parity harness replay commands:

```powershell
npm run check:objc3c:library-cli-parity:source
npm run check:compiler-closeout:m142
```

`npm run check:objc3c:library-cli-parity:source` writes deterministic parity outputs under:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/cli/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/summary.json`

Where `<work_key>` is deterministic (derived from source + emit/lowering/runtime controls when not passed explicitly), effective replay roots are:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/cli/`

Expected compared artifacts per side (`emit_prefix=module` default):

- `module.diagnostics.json`
- `module.manifest.json`
- `module.ll`
- `module.obj`

Object backend note for harness replay:

- CLI emits backend provenance sidecar `module.object-backend.txt` (`clang` or `llvm-direct`).
- M142 parity dimensions exclude `module.object-backend.txt`; it is a provenance note, not a compared artifact payload.
- Source-mode parity command pins `--cli-ir-object-backend clang` so CLI and C API object outputs are backend-aligned.

Lane-C lowering/LLVM IR/runtime-ABI parity anchors (`M142-C001`):

- Native IR prologue emits deterministic ABI replay markers:
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; runtime_dispatch_decl = declare i32 @<symbol>(i32, ptr[, i32...])`
  - `; simd_vector_lowering = <canonical replay key>`
- Runtime dispatch declaration emission reuses the same replay key shape as the prologue marker.
- Manifest parity still records lowering/runtime ABI controls under:
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
  - `"lowering_vector_abi":{"replay_key":"<canonical replay key>"}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m142_lowering_cli_c_api_parity_contract.py -q`

`npm run check:compiler-closeout:m142` fail-closes on parity harness source/docs/package drift via:

- `python scripts/check_m142_frontend_lowering_parity_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_frontend_lowering_parity_contract.py tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py -q`

## Artifact tmp-path governance artifacts (M143-D001)

Tmp-governed parity governance commands:

```powershell
npm run check:objc3c:library-cli-parity:source:m143
npm run check:compiler-closeout:m143
```

`npm run check:objc3c:library-cli-parity:source:m143` writes replay-governed outputs under:

- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json`

Governance contract notes:

- Work roots are deterministic via `--work-key` (or default-derived deterministic key when omitted).
- Source mode fail-closes when stale `<emit-prefix>` outputs are detected in target work roots.
- Source mode fail-closes when expected generated parity artifacts are missing after command execution.
- Tmp-path policy is default-enforced; non-tmp work roots require explicit opt-in.
- Lane-C lowering/runtime artifact roots remain under `tmp/artifacts/objc3c-native/`:
  - `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`
- Lane-C deterministic default run ids:
  - `m143-lane-c-lowering-regression-default` (`OBJC3C_NATIVE_LOWERING_RUN_ID`)
  - `m143-lane-c-typed-abi-default` (`OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID`)
  - `m143-lane-c-lowering-replay-proof-default` (`OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID`)

`npm run check:compiler-closeout:m143` fail-closes on tmp-governance source/docs/package drift via:

- `python scripts/check_m143_artifact_tmp_governance_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py tests/tooling/test_check_m143_artifact_tmp_governance_contract.py -q`

## LLVM capability discovery artifacts (M144-E001)

Capability discovery and routing validation commands:

```powershell
npm run check:objc3c:llvm-capabilities
npm run check:objc3c:library-cli-parity:source:m144
npm run check:compiler-closeout:m144
```

Capability probe summary output:

- `tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`

Capability-routed source-mode parity output:

- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json`

`npm run check:compiler-closeout:m144` fail-closes on capability discovery source/docs/package drift via:

- `python scripts/check_m144_llvm_capability_discovery_contract.py`
- `python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py tests/tooling/test_check_m144_llvm_capability_discovery_contract.py -q`

## Direct LLVM object-emission lane-B matrix artifacts (M145-B001)

Lane-B sema/type-system direct-LLVM matrix validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run check:compiler-closeout:m145
```

`npm run test:objc3c:m145-direct-llvm-matrix` writes matrix artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct_forced_missing_llc/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/negative_backend_matrix/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Fail-closed matrix markers captured in `summary.json` checks:

- `runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes`
- `runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker`
- `runtime.negative.matrix.backend.exit_codes.<fixture>`

## Direct LLVM object-emission lane-D validation artifacts (M145-D001)

Lane-D conformance/perf validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run check:compiler-closeout:m145
```

Lane-D artifact roots:

- Conformance aggregation output:
  - `tmp/artifacts/conformance_suite/summary.json`
- Perf-budget summary output:
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
- Coverage sources tied to lane-D fixture:
  - `tests/conformance/COVERAGE_MAP.md`
  - `tests/conformance/lowering_abi/manifest.json`
  - `tests/conformance/lowering_abi/M145-D001.json`

## Interface/implementation lowering artifact contract (M146-C001)

M146-C hardens lowering/runtime ABI artifact publication for `@interface` + `@implementation` parser/sema metadata.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m146/lowering-interface-implementation-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m146/lowering-interface-implementation-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m146/lowering-interface-implementation-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m146/lowering-interface-implementation-contract/interface-implementation-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_interface_implementation_handoff`
- `frontend.pipeline.sema_pass_manager.type_metadata_interface_entries`
- `frontend.pipeline.sema_pass_manager.type_metadata_implementation_entries`
- `frontend.pipeline.semantic_surface.declared_interfaces`
- `frontend.pipeline.semantic_surface.declared_implementations`
- `frontend.pipeline.semantic_surface.resolved_interface_symbols`
- `frontend.pipeline.semantic_surface.resolved_implementation_symbols`
- `frontend.pipeline.semantic_surface.objc_interface_implementation_surface`
- top-level `"interfaces"` and `"implementations"` arrays sourced from sema metadata handoff.

IR publication markers:

- `; frontend_objc_interface_implementation_profile = declared_interfaces=<N>, declared_implementations=<N>, resolved_interface_symbols=<N>, resolved_implementation_symbols=<N>, interface_method_symbols=<N>, implementation_method_symbols=<N>, linked_implementation_symbols=<N>, deterministic_interface_implementation_handoff=<bool>`
- `!objc3.objc_interface_implementation = !{!1}`
- `!1 = !{i64 <declared_interfaces>, i64 <declared_implementations>, i64 <resolved_interface_symbols>, i64 <resolved_implementation_symbols>, i64 <interface_method_symbols>, i64 <implementation_method_symbols>, i64 <linked_implementation_symbols>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py -q`

## Protocol/category lowering artifact contract (M147-C001)

M147-C extends lowering/runtime ABI artifact publication for `@protocol` + `@category` metadata envelopes while preserving deterministic lane-C replay behavior.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m147/lowering-protocol-category-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m147/lowering-protocol-category-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m147/lowering-protocol-category-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m147/lowering-protocol-category-contract/protocol-category-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_protocol_category_handoff`
- `frontend.pipeline.sema_pass_manager.type_metadata_protocol_entries`
- `frontend.pipeline.sema_pass_manager.type_metadata_category_entries`
- `frontend.pipeline.semantic_surface.declared_protocols`
- `frontend.pipeline.semantic_surface.declared_categories`
- `frontend.pipeline.semantic_surface.resolved_protocol_symbols`
- `frontend.pipeline.semantic_surface.resolved_category_symbols`
- `frontend.pipeline.semantic_surface.objc_protocol_category_surface`
- top-level `"protocols"` and `"categories"` arrays carried by the lowering envelope.

IR publication markers:

- `; frontend_objc_protocol_category_profile = declared_protocols=<N>, declared_categories=<N>, resolved_protocol_symbols=<N>, resolved_category_symbols=<N>, protocol_method_symbols=<N>, category_method_symbols=<N>, linked_category_symbols=<N>, deterministic_protocol_category_handoff=<bool>`
- `!objc3.objc_protocol_category = !{!2}`
- `!2 = !{i64 <declared_protocols>, i64 <declared_categories>, i64 <resolved_protocol_symbols>, i64 <resolved_category_symbols>, i64 <protocol_method_symbols>, i64 <category_method_symbols>, i64 <linked_category_symbols>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py -q`

## Selector-normalization lowering artifact contract (M148-C001)

M148-C extends lowering/runtime ABI artifact publication with selector-normalized method declaration envelope markers.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m148/lowering-selector-normalization-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m148/lowering-selector-normalization-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m148/lowering-selector-normalization-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m148/lowering-selector-normalization-contract/selector-normalization-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_selector_normalization_handoff`
- `frontend.pipeline.sema_pass_manager.selector_method_declaration_entries`
- `frontend.pipeline.sema_pass_manager.selector_normalized_method_declarations`
- `frontend.pipeline.sema_pass_manager.selector_piece_entries`
- `frontend.pipeline.sema_pass_manager.selector_piece_parameter_links`
- `frontend.pipeline.semantic_surface.objc_selector_normalization_surface`

IR publication markers:

- `; frontend_objc_selector_normalization_profile = method_declaration_entries=<N>, normalized_method_declarations=<N>, selector_piece_entries=<N>, selector_piece_parameter_links=<N>, deterministic_selector_normalization_handoff=<bool>`
- `!objc3.objc_selector_normalization = !{!3}`
- `!3 = !{i64 <method_declaration_entries>, i64 <normalized_method_declarations>, i64 <selector_piece_entries>, i64 <selector_piece_parameter_links>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py -q`

## Property-attribute lowering artifact contract (M149-C001)

M149-C extends lowering/runtime ABI artifact publication with `@property` grammar envelopes covering attribute and accessor-modifier summaries.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m149/lowering-property-attribute-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m149/lowering-property-attribute-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m149/lowering-property-attribute-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m149/lowering-property-attribute-contract/property-attribute-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_property_attribute_handoff`
- `frontend.pipeline.sema_pass_manager.property_declaration_entries`
- `frontend.pipeline.sema_pass_manager.property_attribute_entries`
- `frontend.pipeline.sema_pass_manager.property_attribute_value_entries`
- `frontend.pipeline.sema_pass_manager.property_accessor_modifier_entries`
- `frontend.pipeline.sema_pass_manager.property_getter_selector_entries`
- `frontend.pipeline.sema_pass_manager.property_setter_selector_entries`
- `frontend.pipeline.semantic_surface.objc_property_attribute_surface`

IR publication markers:

- `; frontend_objc_property_attribute_profile = property_declaration_entries=<N>, property_attribute_entries=<N>, property_attribute_value_entries=<N>, property_accessor_modifier_entries=<N>, property_getter_selector_entries=<N>, property_setter_selector_entries=<N>, deterministic_property_attribute_handoff=<bool>`
- `!objc3.objc_property_attribute = !{!4}`
- `!4 = !{i64 <property_declaration_entries>, i64 <property_attribute_entries>, i64 <property_attribute_value_entries>, i64 <property_accessor_modifier_entries>, i64 <property_getter_selector_entries>, i64 <property_setter_selector_entries>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py -q`

## Object-pointer/nullability/generics lowering artifact contract (M150-C001)

M150-C extends lowering/runtime ABI artifact publication with object-pointer declarator, nullability suffix, and lightweight-generic parse envelope markers.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m150/lowering-object-pointer-nullability-generics-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m150/lowering-object-pointer-nullability-generics-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m150/lowering-object-pointer-nullability-generics-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m150/lowering-object-pointer-nullability-generics-contract/object-pointer-nullability-generics-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_object_pointer_nullability_generics_handoff`
- `frontend.pipeline.sema_pass_manager.object_pointer_type_spellings`
- `frontend.pipeline.sema_pass_manager.pointer_declarator_entries`
- `frontend.pipeline.sema_pass_manager.pointer_declarator_depth_total`
- `frontend.pipeline.sema_pass_manager.pointer_declarator_token_entries`
- `frontend.pipeline.sema_pass_manager.nullability_suffix_entries`
- `frontend.pipeline.sema_pass_manager.generic_suffix_entries`
- `frontend.pipeline.sema_pass_manager.terminated_generic_suffix_entries`
- `frontend.pipeline.sema_pass_manager.unterminated_generic_suffix_entries`
- `frontend.pipeline.semantic_surface.objc_object_pointer_nullability_generics_surface`

IR publication markers:

- `; frontend_objc_object_pointer_nullability_generics_profile = object_pointer_type_spellings=<N>, pointer_declarator_entries=<N>, pointer_declarator_depth_total=<N>, pointer_declarator_token_entries=<N>, nullability_suffix_entries=<N>, generic_suffix_entries=<N>, terminated_generic_suffix_entries=<N>, unterminated_generic_suffix_entries=<N>, deterministic_object_pointer_nullability_generics_handoff=<bool>`
- `!objc3.objc_object_pointer_nullability_generics = !{!5}`
- `!5 = !{i64 <object_pointer_type_spellings>, i64 <pointer_declarator_entries>, i64 <pointer_declarator_depth_total>, i64 <pointer_declarator_token_entries>, i64 <nullability_suffix_entries>, i64 <generic_suffix_entries>, i64 <terminated_generic_suffix_entries>, i64 <unterminated_generic_suffix_entries>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py -q`

## Symbol-graph/scope-resolution lowering artifact contract (M151-C001)

M151-C extends lowering/runtime ABI artifact publication with symbol-graph and scope-resolution handoff envelopes sourced from sema integration + type-metadata replay packets.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m151/lowering-symbol-graph-scope-resolution-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m151/lowering-symbol-graph-scope-resolution-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m151/lowering-symbol-graph-scope-resolution-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m151/lowering-symbol-graph-scope-resolution-contract/symbol-graph-scope-resolution-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.symbol_graph_global_symbol_nodes`
- `frontend.pipeline.sema_pass_manager.scope_resolution_scope_frames_total`
- `frontend.pipeline.sema_pass_manager.deterministic_symbol_graph_handoff`
- `frontend.pipeline.sema_pass_manager.deterministic_scope_resolution_handoff`
- `frontend.pipeline.sema_pass_manager.symbol_graph_scope_resolution_handoff_key`
- `frontend.pipeline.semantic_surface.objc_symbol_graph_scope_resolution_surface`

IR publication markers:

- `; frontend_objc_symbol_graph_scope_resolution_profile = global_symbol_nodes=<N>, function_symbol_nodes=<N>, interface_symbol_nodes=<N>, implementation_symbol_nodes=<N>, interface_property_symbol_nodes=<N>, implementation_property_symbol_nodes=<N>, interface_method_symbol_nodes=<N>, implementation_method_symbol_nodes=<N>, top_level_scope_symbols=<N>, nested_scope_symbols=<N>, scope_frames_total=<N>, implementation_interface_resolution_sites=<N>, implementation_interface_resolution_hits=<N>, implementation_interface_resolution_misses=<N>, method_resolution_sites=<N>, method_resolution_hits=<N>, method_resolution_misses=<N>, deterministic_symbol_graph_handoff=<bool>, deterministic_scope_resolution_handoff=<bool>, deterministic_symbol_graph_scope_resolution_handoff_key=<key>`
- `!objc3.objc_symbol_graph_scope_resolution = !{!6}`
- `!6 = !{i64 <global_symbol_nodes>, i64 <function_symbol_nodes>, i64 <interface_symbol_nodes>, i64 <implementation_symbol_nodes>, i64 <interface_property_symbol_nodes>, i64 <implementation_property_symbol_nodes>, i64 <interface_method_symbol_nodes>, i64 <implementation_method_symbol_nodes>, i64 <top_level_scope_symbols>, i64 <nested_scope_symbols>, i64 <scope_frames_total>, i64 <implementation_interface_resolution_sites>, i64 <implementation_interface_resolution_hits>, i64 <implementation_interface_resolution_misses>, i64 <method_resolution_sites>, i64 <method_resolution_hits>, i64 <method_resolution_misses>, i1 <deterministic_symbol_graph_handoff>, i1 <deterministic_scope_resolution_handoff>, !"<handoff_key>"}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py -q`

## Class/protocol/category linking lowering artifact contract (M152-C001)

M152-C extends lowering/runtime ABI artifact publication with an aggregate class/protocol/category linking packet that ties class interface/implementation linkage to protocol/category composition linkage counters.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m152/lowering-class-protocol-category-linking-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m152/lowering-class-protocol-category-linking-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m152/lowering-class-protocol-category-linking-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m152/lowering-class-protocol-category-linking-contract/class-protocol-category-linking-source-anchors.txt`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_class_protocol_category_linking_handoff`
- `frontend.pipeline.sema_pass_manager.class_protocol_category_declared_class_interfaces`
- `frontend.pipeline.sema_pass_manager.class_protocol_category_linked_class_method_symbols`
- `frontend.pipeline.sema_pass_manager.class_protocol_category_protocol_composition_sites`
- `frontend.pipeline.sema_pass_manager.class_protocol_category_invalid_protocol_composition_sites`
- `frontend.pipeline.semantic_surface.objc_class_protocol_category_linking_surface`

IR publication markers:

- `; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces=<N>, declared_class_implementations=<N>, resolved_class_interfaces=<N>, resolved_class_implementations=<N>, linked_class_method_symbols=<N>, linked_category_method_symbols=<N>, protocol_composition_sites=<N>, protocol_composition_symbols=<N>, category_composition_sites=<N>, category_composition_symbols=<N>, invalid_protocol_composition_sites=<N>, deterministic_class_protocol_category_linking_handoff=<bool>`
- `!objc3.objc_class_protocol_category_linking = !{!7}`
- `!7 = !{i64 <declared_class_interfaces>, i64 <declared_class_implementations>, i64 <resolved_class_interfaces>, i64 <resolved_class_implementations>, i64 <linked_class_method_symbols>, i64 <linked_category_method_symbols>, i64 <protocol_composition_sites>, i64 <protocol_composition_symbols>, i64 <category_composition_sites>, i64 <category_composition_symbols>, i64 <invalid_protocol_composition_sites>, i1 <deterministic_class_protocol_category_linking_handoff>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py -q`

## Method lookup/override/conflict lowering artifact contract (M153-C001)

M153-C extends lowering contract publication with a replay-stable packet for method lookup, superclass override
resolution, and override-conflict counters consumed from frontend sema summaries.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m153/lowering-method-lookup-override-conflict-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m153/lowering-method-lookup-override-conflict-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m153/lowering-method-lookup-override-conflict-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m153/lowering-method-lookup-override-conflict-contract/method-lookup-override-conflict-source-anchors.txt`

Lowering contract markers:

- `kObjc3MethodLookupOverrideConflictLaneContract`
- `Objc3MethodLookupOverrideConflictContract`
- `IsValidObjc3MethodLookupOverrideConflictContract(...)`
- `Objc3MethodLookupOverrideConflictReplayKey(...)`

Replay key publication markers:

- `method_lookup_sites=<N>`
- `method_lookup_hits=<N>`
- `method_lookup_misses=<N>`
- `override_lookup_sites=<N>`
- `override_lookup_hits=<N>`
- `override_lookup_misses=<N>`
- `override_conflicts=<N>`
- `unresolved_base_interfaces=<N>`
- `deterministic=<bool>`
- `lane_contract=m153-method-lookup-override-conflict-v1`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py -q`

## Property synthesis/ivar binding lowering artifact contract (M154-C001)

M154-C extends lowering contract publication with a replay-stable packet for property synthesis and ivar binding
semantics consumed from deterministic frontend sema summaries. Until dedicated ivar-binding counters land in sema
handoffs, lane-C derives the packet from deterministic property declaration cardinality using default ivar binding
semantics.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m154/lowering-property-synthesis-ivar-binding-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m154/lowering-property-synthesis-ivar-binding-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m154/lowering-property-synthesis-ivar-binding-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m154/lowering-property-synthesis-ivar-binding-contract/property-synthesis-ivar-binding-source-anchors.txt`

Lowering contract markers:

- `kObjc3PropertySynthesisIvarBindingLaneContract`
- `Objc3PropertySynthesisIvarBindingContract`
- `Objc3DefaultPropertySynthesisIvarBindingContract(...)`
- `IsValidObjc3PropertySynthesisIvarBindingContract(...)`
- `Objc3PropertySynthesisIvarBindingReplayKey(...)`

Replay key publication markers:

- `property_synthesis_sites=<N>`
- `property_synthesis_explicit_ivar_bindings=<N>`
- `property_synthesis_default_ivar_bindings=<N>`
- `ivar_binding_sites=<N>`
- `ivar_binding_resolved=<N>`
- `ivar_binding_missing=<N>`
- `ivar_binding_conflicts=<N>`
- `deterministic=<bool>`
- `lane_contract=m154-property-synthesis-ivar-binding-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_property_synthesis_ivar_binding_handoff`
- `frontend.pipeline.sema_pass_manager.property_synthesis_sites`
- `frontend.pipeline.sema_pass_manager.property_synthesis_explicit_ivar_bindings`
- `frontend.pipeline.sema_pass_manager.property_synthesis_default_ivar_bindings`
- `frontend.pipeline.sema_pass_manager.ivar_binding_sites`
- `frontend.pipeline.sema_pass_manager.ivar_binding_resolved`
- `frontend.pipeline.sema_pass_manager.ivar_binding_missing`
- `frontend.pipeline.sema_pass_manager.ivar_binding_conflicts`
- `frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`
- `frontend.pipeline.semantic_surface.objc_property_synthesis_ivar_binding_surface`
- `lowering_property_synthesis_ivar_binding.replay_key`
- `lowering_property_synthesis_ivar_binding.lane_contract`

IR publication marker:

- `; property_synthesis_ivar_binding_lowering = property_synthesis_sites=<N>;property_synthesis_explicit_ivar_bindings=<N>;property_synthesis_default_ivar_bindings=<N>;ivar_binding_sites=<N>;ivar_binding_resolved=<N>;ivar_binding_missing=<N>;ivar_binding_conflicts=<N>;deterministic=<bool>;lane_contract=m154-property-synthesis-ivar-binding-v1`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py -q`

## id/Class/SEL/object-pointer typecheck lowering artifact contract (M155-C001)

M155-C extends lowering contract publication with a replay-stable packet that tracks Objective-C `id`, `Class`,
`SEL`, and nominal object-pointer typecheck spellings across function/method/property type surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m155/lowering-id-class-sel-object-pointer-typecheck-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m155/lowering-id-class-sel-object-pointer-typecheck-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m155/lowering-id-class-sel-object-pointer-typecheck-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m155/lowering-id-class-sel-object-pointer-typecheck-contract/id-class-sel-object-pointer-typecheck-source-anchors.txt`

Lowering contract markers:

- `kObjc3IdClassSelObjectPointerTypecheckLaneContract`
- `Objc3IdClassSelObjectPointerTypecheckContract`
- `IsValidObjc3IdClassSelObjectPointerTypecheckContract(...)`
- `Objc3IdClassSelObjectPointerTypecheckReplayKey(...)`

Replay key publication markers:

- `id_typecheck_sites=<N>`
- `class_typecheck_sites=<N>`
- `sel_typecheck_sites=<N>`
- `object_pointer_typecheck_sites=<N>`
- `total_typecheck_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m155-id-class-sel-object-pointer-typecheck-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_id_class_sel_object_pointer_typecheck_handoff`
- `frontend.pipeline.sema_pass_manager.id_typecheck_sites`
- `frontend.pipeline.sema_pass_manager.class_typecheck_sites`
- `frontend.pipeline.sema_pass_manager.sel_typecheck_sites`
- `frontend.pipeline.sema_pass_manager.object_pointer_typecheck_sites`
- `frontend.pipeline.sema_pass_manager.id_class_sel_object_pointer_typecheck_sites_total`
- `frontend.pipeline.sema_pass_manager.lowering_id_class_sel_object_pointer_typecheck_replay_key`
- `frontend.pipeline.semantic_surface.objc_id_class_sel_object_pointer_typecheck_surface`
- `lowering_id_class_sel_object_pointer_typecheck.replay_key`
- `lowering_id_class_sel_object_pointer_typecheck.lane_contract`

IR publication markers:

- `; id_class_sel_object_pointer_typecheck_lowering = id_typecheck_sites=<N>;class_typecheck_sites=<N>;sel_typecheck_sites=<N>;object_pointer_typecheck_sites=<N>;total_typecheck_sites=<N>;deterministic=<bool>;lane_contract=m155-id-class-sel-object-pointer-typecheck-v1`
- `; frontend_objc_id_class_sel_object_pointer_typecheck_profile = id_typecheck_sites=<N>, class_typecheck_sites=<N>, sel_typecheck_sites=<N>, object_pointer_typecheck_sites=<N>, total_typecheck_sites=<N>, deterministic_id_class_sel_object_pointer_typecheck_handoff=<bool>`
- `!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}`
- `!8 = !{i64 <id_typecheck_sites>, i64 <class_typecheck_sites>, i64 <sel_typecheck_sites>, i64 <object_pointer_typecheck_sites>, i64 <total_typecheck_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py -q`

## Message-send selector-lowering artifact contract (M156-C001)

M156-C publishes a replay-stable lowering packet for Objective-C message-send expression forms and canonical selector
lowering metadata.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m156/lowering-message-send-selector-lowering-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m156/lowering-message-send-selector-lowering-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m156/lowering-message-send-selector-lowering-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m156/lowering-message-send-selector-lowering-contract/message-send-selector-lowering-source-anchors.txt`

Lowering contract markers:

- `kObjc3MessageSendSelectorLoweringLaneContract`
- `Objc3MessageSendSelectorLoweringContract`
- `IsValidObjc3MessageSendSelectorLoweringContract(...)`
- `Objc3MessageSendSelectorLoweringReplayKey(...)`

Replay key publication markers:

- `message_send_sites=<N>`
- `unary_selector_sites=<N>`
- `keyword_selector_sites=<N>`
- `selector_piece_sites=<N>`
- `argument_expression_sites=<N>`
- `receiver_expression_sites=<N>`
- `selector_literal_entries=<N>`
- `selector_literal_characters=<N>`
- `deterministic=<bool>`
- `lane_contract=m156-message-send-selector-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_message_send_selector_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_unary_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_keyword_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_selector_piece_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_argument_expression_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_receiver_sites`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_selector_literal_entries`
- `frontend.pipeline.sema_pass_manager.message_send_selector_lowering_selector_literal_characters`
- `frontend.pipeline.sema_pass_manager.lowering_message_send_selector_lowering_replay_key`
- `frontend.pipeline.semantic_surface.objc_message_send_selector_lowering_surface`
- `lowering_message_send_selector_lowering.replay_key`
- `lowering_message_send_selector_lowering.lane_contract`

IR publication markers:

- `; message_send_selector_lowering = message_send_sites=<N>;unary_selector_sites=<N>;keyword_selector_sites=<N>;selector_piece_sites=<N>;argument_expression_sites=<N>;receiver_expression_sites=<N>;selector_literal_entries=<N>;selector_literal_characters=<N>;deterministic=<bool>;lane_contract=m156-message-send-selector-lowering-v1`
- `; frontend_objc_message_send_selector_lowering_profile = message_send_sites=<N>, unary_selector_sites=<N>, keyword_selector_sites=<N>, selector_piece_sites=<N>, argument_expression_sites=<N>, receiver_expression_sites=<N>, selector_literal_entries=<N>, selector_literal_characters=<N>, deterministic_message_send_selector_lowering_handoff=<bool>`
- `!objc3.objc_message_send_selector_lowering = !{!9}`
- `!9 = !{i64 <message_send_sites>, i64 <unary_selector_sites>, i64 <keyword_selector_sites>, i64 <selector_piece_sites>, i64 <argument_expression_sites>, i64 <receiver_expression_sites>, i64 <selector_literal_entries>, i64 <selector_literal_characters>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m156_lowering_message_send_selector_lowering_contract.py -q`

## Dispatch ABI marshalling artifact contract (M157-C001)

M157-C publishes a replay-stable lowering packet for dispatch-path ABI argument/result marshalling.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m157/lowering-dispatch-abi-marshalling-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m157/lowering-dispatch-abi-marshalling-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m157/lowering-dispatch-abi-marshalling-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m157/lowering-dispatch-abi-marshalling-contract/dispatch-abi-marshalling-source-anchors.txt`

Lowering contract markers:

- `kObjc3DispatchAbiMarshallingLaneContract`
- `Objc3DispatchAbiMarshallingContract`
- `IsValidObjc3DispatchAbiMarshallingContract(...)`
- `Objc3DispatchAbiMarshallingReplayKey(...)`

Replay key publication markers:

- `message_send_sites=<N>`
- `receiver_slots_marshaled=<N>`
- `selector_slots_marshaled=<N>`
- `argument_value_slots_marshaled=<N>`
- `argument_padding_slots_marshaled=<N>`
- `argument_total_slots_marshaled=<N>`
- `total_marshaled_slots=<N>`
- `runtime_dispatch_arg_slots=<N>`
- `deterministic=<bool>`
- `lane_contract=m157-dispatch-abi-marshalling-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_dispatch_abi_marshalling_handoff`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_message_send_sites`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_receiver_slots_marshaled`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_selector_slots_marshaled`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_argument_value_slots_marshaled`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_argument_padding_slots_marshaled`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_argument_total_slots_marshaled`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_total_marshaled_slots`
- `frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_runtime_dispatch_arg_slots`
- `frontend.pipeline.sema_pass_manager.lowering_dispatch_abi_marshalling_replay_key`
- `frontend.pipeline.semantic_surface.objc_dispatch_abi_marshalling_surface`
- `lowering_dispatch_abi_marshalling.replay_key`
- `lowering_dispatch_abi_marshalling.lane_contract`

IR publication markers:

- `; dispatch_abi_marshalling_lowering = message_send_sites=<N>;receiver_slots_marshaled=<N>;selector_slots_marshaled=<N>;argument_value_slots_marshaled=<N>;argument_padding_slots_marshaled=<N>;argument_total_slots_marshaled=<N>;total_marshaled_slots=<N>;runtime_dispatch_arg_slots=<N>;deterministic=<bool>;lane_contract=m157-dispatch-abi-marshalling-v1`
- `; frontend_objc_dispatch_abi_marshalling_profile = message_send_sites=<N>, receiver_slots_marshaled=<N>, selector_slots_marshaled=<N>, argument_value_slots_marshaled=<N>, argument_padding_slots_marshaled=<N>, argument_total_slots_marshaled=<N>, total_marshaled_slots=<N>, runtime_dispatch_arg_slots=<N>, deterministic_dispatch_abi_marshalling_handoff=<bool>`
- `!objc3.objc_dispatch_abi_marshalling = !{!10}`
- `!10 = !{i64 <message_send_sites>, i64 <receiver_slots_marshaled>, i64 <selector_slots_marshaled>, i64 <argument_value_slots_marshaled>, i64 <argument_padding_slots_marshaled>, i64 <argument_total_slots_marshaled>, i64 <total_marshaled_slots>, i64 <runtime_dispatch_arg_slots>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m157_lowering_dispatch_abi_marshalling_contract.py -q`

## Nil-receiver semantics/foldability artifact contract (M158-C001)

M158-C publishes a replay-stable lowering packet for nil-receiver semantics and foldability.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m158/lowering-nil-receiver-semantics-foldability-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m158/lowering-nil-receiver-semantics-foldability-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m158/lowering-nil-receiver-semantics-foldability-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m158/lowering-nil-receiver-semantics-foldability-contract/nil-receiver-foldability-source-anchors.txt`

Lowering contract markers:

- `kObjc3NilReceiverSemanticsFoldabilityLaneContract`
- `Objc3NilReceiverSemanticsFoldabilityContract`
- `IsValidObjc3NilReceiverSemanticsFoldabilityContract(...)`
- `Objc3NilReceiverSemanticsFoldabilityReplayKey(...)`

Replay key publication markers:

- `message_send_sites=<N>`
- `receiver_nil_literal_sites=<N>`
- `nil_receiver_semantics_enabled_sites=<N>`
- `nil_receiver_foldable_sites=<N>`
- `nil_receiver_runtime_dispatch_required_sites=<N>`
- `non_nil_receiver_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m158-nil-receiver-semantics-foldability-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_nil_receiver_semantics_foldability_handoff`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_message_send_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_receiver_nil_literal_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_enabled_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_foldable_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_runtime_dispatch_required_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_non_nil_receiver_sites`
- `frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_nil_receiver_semantics_foldability_replay_key`
- `frontend.pipeline.semantic_surface.objc_nil_receiver_semantics_foldability_surface`
- `lowering_nil_receiver_semantics_foldability.replay_key`
- `lowering_nil_receiver_semantics_foldability.lane_contract`

IR publication markers:

- `; nil_receiver_semantics_foldability_lowering = message_send_sites=<N>;receiver_nil_literal_sites=<N>;nil_receiver_semantics_enabled_sites=<N>;nil_receiver_foldable_sites=<N>;nil_receiver_runtime_dispatch_required_sites=<N>;non_nil_receiver_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m158-nil-receiver-semantics-foldability-v1`
- `; frontend_objc_nil_receiver_semantics_foldability_profile = message_send_sites=<N>, receiver_nil_literal_sites=<N>, nil_receiver_semantics_enabled_sites=<N>, nil_receiver_foldable_sites=<N>, nil_receiver_runtime_dispatch_required_sites=<N>, non_nil_receiver_sites=<N>, contract_violation_sites=<N>, deterministic_nil_receiver_semantics_foldability_handoff=<bool>`
- `!objc3.objc_nil_receiver_semantics_foldability = !{!11}`
- `!11 = !{i64 <message_send_sites>, i64 <receiver_nil_literal_sites>, i64 <nil_receiver_semantics_enabled_sites>, i64 <nil_receiver_foldable_sites>, i64 <nil_receiver_runtime_dispatch_required_sites>, i64 <non_nil_receiver_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m158_lowering_nil_receiver_semantics_foldability_contract.py -q`

## Super-dispatch/method-family artifact contract (M159-C001)

M159-C publishes a replay-stable lowering packet for super-dispatch and method-family semantics.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m159/lowering-super-dispatch-method-family-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m159/lowering-super-dispatch-method-family-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m159/lowering-super-dispatch-method-family-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m159/lowering-super-dispatch-method-family-contract/super-dispatch-method-family-source-anchors.txt`

Lowering contract markers:

- `kObjc3SuperDispatchMethodFamilyLaneContract`
- `Objc3SuperDispatchMethodFamilyContract`
- `IsValidObjc3SuperDispatchMethodFamilyContract(...)`
- `Objc3SuperDispatchMethodFamilyReplayKey(...)`

Replay key publication markers:

- `message_send_sites=<N>`
- `receiver_super_identifier_sites=<N>`
- `super_dispatch_enabled_sites=<N>`
- `super_dispatch_requires_class_context_sites=<N>`
- `method_family_init_sites=<N>`
- `method_family_copy_sites=<N>`
- `method_family_mutable_copy_sites=<N>`
- `method_family_new_sites=<N>`
- `method_family_none_sites=<N>`
- `method_family_returns_retained_result_sites=<N>`
- `method_family_returns_related_result_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m159-super-dispatch-method-family-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_super_dispatch_method_family_handoff`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_message_send_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_receiver_super_identifier_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_enabled_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_requires_class_context_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_init_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_copy_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_mutable_copy_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_new_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_none_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_returns_retained_result_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_returns_related_result_sites`
- `frontend.pipeline.sema_pass_manager.super_dispatch_method_family_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_super_dispatch_method_family_replay_key`
- `frontend.pipeline.semantic_surface.objc_super_dispatch_method_family_surface`
- `lowering_super_dispatch_method_family.replay_key`
- `lowering_super_dispatch_method_family.lane_contract`

IR publication markers:

- `; super_dispatch_method_family_lowering = message_send_sites=<N>;receiver_super_identifier_sites=<N>;super_dispatch_enabled_sites=<N>;super_dispatch_requires_class_context_sites=<N>;method_family_init_sites=<N>;method_family_copy_sites=<N>;method_family_mutable_copy_sites=<N>;method_family_new_sites=<N>;method_family_none_sites=<N>;method_family_returns_retained_result_sites=<N>;method_family_returns_related_result_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m159-super-dispatch-method-family-v1`
- `; frontend_objc_super_dispatch_method_family_profile = message_send_sites=<N>, receiver_super_identifier_sites=<N>, super_dispatch_enabled_sites=<N>, super_dispatch_requires_class_context_sites=<N>, method_family_init_sites=<N>, method_family_copy_sites=<N>, method_family_mutable_copy_sites=<N>, method_family_new_sites=<N>, method_family_none_sites=<N>, method_family_returns_retained_result_sites=<N>, method_family_returns_related_result_sites=<N>, contract_violation_sites=<N>, deterministic_super_dispatch_method_family_handoff=<bool>`
- `!objc3.objc_super_dispatch_method_family = !{!12}`
- `!12 = !{i64 <message_send_sites>, i64 <receiver_super_identifier_sites>, i64 <super_dispatch_enabled_sites>, i64 <super_dispatch_requires_class_context_sites>, i64 <method_family_init_sites>, i64 <method_family_copy_sites>, i64 <method_family_mutable_copy_sites>, i64 <method_family_new_sites>, i64 <method_family_none_sites>, i64 <method_family_returns_retained_result_sites>, i64 <method_family_returns_related_result_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m159_lowering_super_dispatch_method_family_contract.py -q`

## Runtime-shim host-link artifact contract (M160-C001)

M160-C publishes replay-stable runtime-shim host-link invariants over message-send lowering.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m160/lowering-runtime-shim-host-link-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m160/lowering-runtime-shim-host-link-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m160/lowering-runtime-shim-host-link-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m160/lowering-runtime-shim-host-link-contract/runtime-shim-host-link-source-anchors.txt`

Lowering contract markers:

- `kObjc3RuntimeShimHostLinkLaneContract`
- `Objc3RuntimeShimHostLinkContract`
- `IsValidObjc3RuntimeShimHostLinkContract(...)`
- `Objc3RuntimeShimHostLinkReplayKey(...)`

Replay key publication markers:

- `message_send_sites=<N>`
- `runtime_shim_required_sites=<N>`
- `runtime_shim_elided_sites=<N>`
- `runtime_dispatch_arg_slots=<N>`
- `runtime_dispatch_declaration_parameter_count=<N>`
- `runtime_dispatch_symbol=<symbol>`
- `default_runtime_dispatch_symbol_binding=<bool>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m160-runtime-shim-host-link-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_runtime_shim_host_link_handoff`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_message_send_sites`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_required_runtime_shim_sites`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_elided_runtime_shim_sites`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_runtime_dispatch_arg_slots`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_runtime_dispatch_symbol`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_default_runtime_dispatch_symbol_binding`
- `frontend.pipeline.sema_pass_manager.runtime_shim_host_link_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_runtime_shim_host_link_replay_key`
- `frontend.pipeline.semantic_surface.objc_runtime_shim_host_link_surface`
- `lowering_runtime_shim_host_link.replay_key`
- `lowering_runtime_shim_host_link.lane_contract`

IR publication markers:

- `; runtime_shim_host_link_lowering = message_send_sites=<N>;runtime_shim_required_sites=<N>;runtime_shim_elided_sites=<N>;runtime_dispatch_arg_slots=<N>;runtime_dispatch_declaration_parameter_count=<N>;runtime_dispatch_symbol=<symbol>;default_runtime_dispatch_symbol_binding=<bool>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m160-runtime-shim-host-link-v1`
- `; frontend_objc_runtime_shim_host_link_profile = message_send_sites=<N>, runtime_shim_required_sites=<N>, runtime_shim_elided_sites=<N>, runtime_dispatch_arg_slots=<N>, runtime_dispatch_declaration_parameter_count=<N>, runtime_dispatch_symbol=<symbol>, default_runtime_dispatch_symbol_binding=<bool>, contract_violation_sites=<N>, deterministic_runtime_shim_host_link_handoff=<bool>`
- `!objc3.objc_runtime_shim_host_link = !{!13}`
- `!13 = !{i64 <message_send_sites>, i64 <runtime_shim_required_sites>, i64 <runtime_shim_elided_sites>, i64 <runtime_dispatch_arg_slots>, i64 <runtime_dispatch_declaration_parameter_count>, !"runtime_dispatch_symbol", i1 <default_runtime_dispatch_symbol_binding>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m160_lowering_runtime_shim_host_link_contract.py -q`

## Ownership-qualifier lowering artifact contract (M161-C001)

M161-C publishes replay-stable ownership-qualifier lowering invariants derived from sema type-annotation surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m161/lowering-ownership-qualifier-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m161/lowering-ownership-qualifier-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m161/lowering-ownership-qualifier-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m161/lowering-ownership-qualifier-contract/ownership-qualifier-source-anchors.txt`

Lowering contract markers:

- `kObjc3OwnershipQualifierLoweringLaneContract`
- `Objc3OwnershipQualifierLoweringContract`
- `IsValidObjc3OwnershipQualifierLoweringContract(...)`
- `Objc3OwnershipQualifierLoweringReplayKey(...)`

Replay key publication markers:

- `ownership_qualifier_sites=<N>`
- `invalid_ownership_qualifier_sites=<N>`
- `object_pointer_type_annotation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m161-ownership-qualifier-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_ownership_qualifier_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites`
- `frontend.pipeline.sema_pass_manager.ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites`
- `frontend.pipeline.sema_pass_manager.ownership_qualifier_lowering_type_annotation_object_pointer_type_sites`
- `frontend.pipeline.sema_pass_manager.lowering_ownership_qualifier_replay_key`
- `frontend.pipeline.semantic_surface.objc_ownership_qualifier_lowering_surface`
- `lowering_ownership_qualifier.replay_key`
- `lowering_ownership_qualifier.lane_contract`

IR publication markers:

- `; ownership_qualifier_lowering = ownership_qualifier_sites=<N>;invalid_ownership_qualifier_sites=<N>;object_pointer_type_annotation_sites=<N>;deterministic=<bool>;lane_contract=m161-ownership-qualifier-lowering-v1`
- `; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites=<N>, invalid_ownership_qualifier_sites=<N>, object_pointer_type_annotation_sites=<N>, deterministic_ownership_qualifier_lowering_handoff=<bool>`
- `!objc3.objc_ownership_qualifier_lowering = !{!14}`
- `!14 = !{i64 <ownership_qualifier_sites>, i64 <invalid_ownership_qualifier_sites>, i64 <object_pointer_type_annotation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m161_lowering_ownership_qualifier_contract.py -q`

## Retain-release operation lowering artifact contract (M162-C001)

M162-C publishes replay-stable retain/release operation lowering invariants derived from sema
ownership-operation surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m162/lowering-retain-release-operation-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m162/lowering-retain-release-operation-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m162/lowering-retain-release-operation-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m162/lowering-retain-release-operation-contract/retain-release-source-anchors.txt`

Lowering contract markers:

- `kObjc3RetainReleaseOperationLoweringLaneContract`
- `Objc3RetainReleaseOperationLoweringContract`
- `IsValidObjc3RetainReleaseOperationLoweringContract(...)`
- `Objc3RetainReleaseOperationLoweringReplayKey(...)`

Replay key publication markers:

- `ownership_qualified_sites=<N>`
- `retain_insertion_sites=<N>`
- `release_insertion_sites=<N>`
- `autorelease_insertion_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m162-retain-release-operation-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_retain_release_operation_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_ownership_qualified_sites`
- `frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_retain_insertion_sites`
- `frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_release_insertion_sites`
- `frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_autorelease_insertion_sites`
- `frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_retain_release_operation_replay_key`
- `frontend.pipeline.semantic_surface.objc_retain_release_operation_lowering_surface`
- `lowering_retain_release_operation.replay_key`
- `lowering_retain_release_operation.lane_contract`

IR publication markers:

- `; retain_release_operation_lowering = ownership_qualified_sites=<N>;retain_insertion_sites=<N>;release_insertion_sites=<N>;autorelease_insertion_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m162-retain-release-operation-lowering-v1`
- `; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites=<N>, retain_insertion_sites=<N>, release_insertion_sites=<N>, autorelease_insertion_sites=<N>, contract_violation_sites=<N>, deterministic_retain_release_operation_lowering_handoff=<bool>`
- `!objc3.objc_retain_release_operation_lowering = !{!15}`
- `!15 = !{i64 <ownership_qualified_sites>, i64 <retain_insertion_sites>, i64 <release_insertion_sites>, i64 <autorelease_insertion_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m162_lowering_retain_release_operation_contract.py -q`

## Autoreleasepool scope lowering artifact contract (M163-C001)

M163-C publishes replay-stable autoreleasepool scope/lifetime lowering invariants derived
from sema scope-summary surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m163/lowering-autoreleasepool-scope-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m163/lowering-autoreleasepool-scope-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m163/lowering-autoreleasepool-scope-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m163/lowering-autoreleasepool-scope-contract/autoreleasepool-scope-source-anchors.txt`

Lowering contract markers:

- `kObjc3AutoreleasePoolScopeLoweringLaneContract`
- `Objc3AutoreleasePoolScopeLoweringContract`
- `IsValidObjc3AutoreleasePoolScopeLoweringContract(...)`
- `Objc3AutoreleasePoolScopeLoweringReplayKey(...)`

Replay key publication markers:

- `scope_sites=<N>`
- `scope_symbolized_sites=<N>`
- `max_scope_depth=<N>`
- `scope_entry_transition_sites=<N>`
- `scope_exit_transition_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m163-autoreleasepool-scope-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_autoreleasepool_scope_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_scope_sites`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_scope_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_max_scope_depth`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_scope_entry_transition_sites`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_scope_exit_transition_sites`
- `frontend.pipeline.sema_pass_manager.autoreleasepool_scope_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_autoreleasepool_scope_replay_key`
- `frontend.pipeline.semantic_surface.objc_autoreleasepool_scope_lowering_surface`
- `lowering_autoreleasepool_scope.replay_key`
- `lowering_autoreleasepool_scope.lane_contract`

IR publication markers:

- `; autoreleasepool_scope_lowering = scope_sites=<N>;scope_symbolized_sites=<N>;max_scope_depth=<N>;scope_entry_transition_sites=<N>;scope_exit_transition_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m163-autoreleasepool-scope-lowering-v1`
- `; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites=<N>, scope_symbolized_sites=<N>, max_scope_depth=<N>, scope_entry_transition_sites=<N>, scope_exit_transition_sites=<N>, contract_violation_sites=<N>, deterministic_autoreleasepool_scope_lowering_handoff=<bool>`
- `!objc3.objc_autoreleasepool_scope_lowering = !{!16}`
- `!16 = !{i64 <scope_sites>, i64 <scope_symbolized_sites>, i64 <max_scope_depth>, i64 <scope_entry_transition_sites>, i64 <scope_exit_transition_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m163_lowering_autoreleasepool_scope_contract.py -q`

## Weak/unowned semantics lowering artifact contract (M164-C001)

M164-C publishes replay-stable weak/unowned ownership semantics lowering invariants derived
from sema weak-unowned summary surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m164/lowering-weak-unowned-semantics-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m164/lowering-weak-unowned-semantics-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m164/lowering-weak-unowned-semantics-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m164/lowering-weak-unowned-semantics-contract/weak-unowned-semantics-source-anchors.txt`

Lowering contract markers:

- `kObjc3WeakUnownedSemanticsLoweringLaneContract`
- `Objc3WeakUnownedSemanticsLoweringContract`
- `IsValidObjc3WeakUnownedSemanticsLoweringContract(...)`
- `Objc3WeakUnownedSemanticsLoweringReplayKey(...)`

Replay key publication markers:

- `ownership_candidate_sites=<N>`
- `weak_reference_sites=<N>`
- `unowned_reference_sites=<N>`
- `unowned_safe_reference_sites=<N>`
- `weak_unowned_conflict_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m164-weak-unowned-semantics-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_weak_unowned_semantics_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_ownership_candidate_sites`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_weak_reference_sites`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_unowned_reference_sites`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_unowned_safe_reference_sites`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_conflict_sites`
- `frontend.pipeline.sema_pass_manager.weak_unowned_semantics_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_weak_unowned_semantics_replay_key`
- `frontend.pipeline.semantic_surface.objc_weak_unowned_semantics_lowering_surface`
- `lowering_weak_unowned_semantics.replay_key`
- `lowering_weak_unowned_semantics.lane_contract`

IR publication markers:

- `; weak_unowned_semantics_lowering = ownership_candidate_sites=<N>;weak_reference_sites=<N>;unowned_reference_sites=<N>;unowned_safe_reference_sites=<N>;weak_unowned_conflict_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m164-weak-unowned-semantics-lowering-v1`
- `; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites=<N>, weak_reference_sites=<N>, unowned_reference_sites=<N>, unowned_safe_reference_sites=<N>, weak_unowned_conflict_sites=<N>, contract_violation_sites=<N>, deterministic_weak_unowned_semantics_lowering_handoff=<bool>`
- `!objc3.objc_weak_unowned_semantics_lowering = !{!17}`
- `!17 = !{i64 <ownership_candidate_sites>, i64 <weak_reference_sites>, i64 <unowned_reference_sites>, i64 <unowned_safe_reference_sites>, i64 <weak_unowned_conflict_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m164_lowering_weak_unowned_contract.py -q`

## ARC diagnostics/fix-it lowering artifact contract (M165-C001)

M165-C publishes replay-stable ARC diagnostics/fix-it lowering invariants derived
from sema ARC diagnostics/fix-it summary surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m165/lowering-arc-diagnostics-fixit-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m165/lowering-arc-diagnostics-fixit-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m165/lowering-arc-diagnostics-fixit-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m165/lowering-arc-diagnostics-fixit-contract/arc-diagnostics-fixit-source-anchors.txt`

Lowering contract markers:

- `kObjc3ArcDiagnosticsFixitLoweringLaneContract`
- `Objc3ArcDiagnosticsFixitLoweringContract`
- `IsValidObjc3ArcDiagnosticsFixitLoweringContract(...)`
- `Objc3ArcDiagnosticsFixitLoweringReplayKey(...)`

Replay key publication markers:

- `ownership_arc_diagnostic_candidate_sites=<N>`
- `ownership_arc_fixit_available_sites=<N>`
- `ownership_arc_profiled_sites=<N>`
- `ownership_arc_weak_unowned_conflict_diagnostic_sites=<N>`
- `ownership_arc_empty_fixit_hint_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m165-arc-diagnostics-fixit-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_arc_diagnostics_fixit_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites`
- `frontend.pipeline.sema_pass_manager.arc_diagnostics_fixit_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_arc_diagnostics_fixit_replay_key`
- `frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface`
- `lowering_arc_diagnostics_fixit.replay_key`
- `lowering_arc_diagnostics_fixit.lane_contract`

IR publication markers:

- `; arc_diagnostics_fixit_lowering = ownership_arc_diagnostic_candidate_sites=<N>;ownership_arc_fixit_available_sites=<N>;ownership_arc_profiled_sites=<N>;ownership_arc_weak_unowned_conflict_diagnostic_sites=<N>;ownership_arc_empty_fixit_hint_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m165-arc-diagnostics-fixit-lowering-v1`
- `; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites=<N>, ownership_arc_fixit_available_sites=<N>, ownership_arc_profiled_sites=<N>, ownership_arc_weak_unowned_conflict_diagnostic_sites=<N>, ownership_arc_empty_fixit_hint_sites=<N>, contract_violation_sites=<N>, deterministic_arc_diagnostics_fixit_lowering_handoff=<bool>`
- `!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}`
- `!18 = !{i64 <ownership_arc_diagnostic_candidate_sites>, i64 <ownership_arc_fixit_available_sites>, i64 <ownership_arc_profiled_sites>, i64 <ownership_arc_weak_unowned_conflict_diagnostic_sites>, i64 <ownership_arc_empty_fixit_hint_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m165_lowering_arc_diagnostics_fixit_contract.py -q`

## Block literal capture lowering artifact contract (M166-C001)

M166-C publishes replay-stable block literal capture lowering invariants derived
from sema block capture summary surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m166/lowering-block-literal-capture-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m166/lowering-block-literal-capture-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m166/lowering-block-literal-capture-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m166/lowering-block-literal-capture-contract/block-literal-capture-source-anchors.txt`

Lowering contract markers:

- `kObjc3BlockLiteralCaptureLoweringLaneContract`
- `Objc3BlockLiteralCaptureLoweringContract`
- `IsValidObjc3BlockLiteralCaptureLoweringContract(...)`
- `Objc3BlockLiteralCaptureLoweringReplayKey(...)`

Replay key publication markers:

- `block_literal_sites=<N>`
- `block_parameter_entries=<N>`
- `block_capture_entries=<N>`
- `block_body_statement_entries=<N>`
- `block_empty_capture_sites=<N>`
- `block_nondeterministic_capture_sites=<N>`
- `block_non_normalized_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m166-block-literal-capture-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_block_literal_capture_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_literal_sites`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_parameter_entries`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_capture_entries`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_body_statement_entries`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_empty_capture_sites`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_nondeterministic_capture_sites`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_block_non_normalized_sites`
- `frontend.pipeline.sema_pass_manager.block_literal_capture_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_block_literal_capture_replay_key`
- `frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface`
- `lowering_block_literal_capture.replay_key`
- `lowering_block_literal_capture.lane_contract`

IR publication markers:

- `; block_literal_capture_lowering = block_literal_sites=<N>;block_parameter_entries=<N>;block_capture_entries=<N>;block_body_statement_entries=<N>;block_empty_capture_sites=<N>;block_nondeterministic_capture_sites=<N>;block_non_normalized_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m166-block-literal-capture-lowering-v1`
- `; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites=<N>, block_parameter_entries=<N>, block_capture_entries=<N>, block_body_statement_entries=<N>, block_empty_capture_sites=<N>, block_nondeterministic_capture_sites=<N>, block_non_normalized_sites=<N>, contract_violation_sites=<N>, deterministic_block_literal_capture_lowering_handoff=<bool>`
- `!objc3.objc_block_literal_capture_lowering = !{!19}`
- `!19 = !{i64 <block_literal_sites>, i64 <block_parameter_entries>, i64 <block_capture_entries>, i64 <block_body_statement_entries>, i64 <block_empty_capture_sites>, i64 <block_nondeterministic_capture_sites>, i64 <block_non_normalized_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m166_lowering_block_literal_capture_contract.py -q`

## Block ABI invoke-trampoline lowering artifact contract (M167-C001)

M167-C publishes replay-stable block ABI invoke-trampoline lowering invariants
derived from M167 sema block ABI parity surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m167/lowering-block-abi-invoke-trampoline-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m167/lowering-block-abi-invoke-trampoline-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m167/lowering-block-abi-invoke-trampoline-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m167/lowering-block-abi-invoke-trampoline-contract/block-abi-invoke-trampoline-source-anchors.txt`

Lowering contract markers:

- `kObjc3BlockAbiInvokeTrampolineLoweringLaneContract`
- `Objc3BlockAbiInvokeTrampolineLoweringContract`
- `IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(...)`
- `Objc3BlockAbiInvokeTrampolineLoweringReplayKey(...)`

Replay key publication markers:

- `block_literal_sites=<N>`
- `invoke_argument_slots_total=<N>`
- `capture_word_count_total=<N>`
- `parameter_entries_total=<N>`
- `capture_entries_total=<N>`
- `body_statement_entries_total=<N>`
- `descriptor_symbolized_sites=<N>`
- `invoke_trampoline_symbolized_sites=<N>`
- `missing_invoke_trampoline_sites=<N>`
- `non_normalized_layout_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m167-block-abi-invoke-trampoline-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_block_abi_invoke_trampoline_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_sites`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_invoke_argument_slots`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_capture_word_count`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_parameter_entries`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_capture_entries`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_body_statement_entries`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_missing_invoke_sites`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites`
- `frontend.pipeline.sema_pass_manager.block_abi_invoke_trampoline_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_block_abi_invoke_trampoline_replay_key`
- `frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface`
- `lowering_block_abi_invoke_trampoline.replay_key`
- `lowering_block_abi_invoke_trampoline.lane_contract`

IR publication markers:

- `; block_abi_invoke_trampoline_lowering = block_literal_sites=<N>;invoke_argument_slots_total=<N>;capture_word_count_total=<N>;parameter_entries_total=<N>;capture_entries_total=<N>;body_statement_entries_total=<N>;descriptor_symbolized_sites=<N>;invoke_trampoline_symbolized_sites=<N>;missing_invoke_trampoline_sites=<N>;non_normalized_layout_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m167-block-abi-invoke-trampoline-lowering-v1`
- `; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites=<N>, invoke_argument_slots_total=<N>, capture_word_count_total=<N>, parameter_entries_total=<N>, capture_entries_total=<N>, body_statement_entries_total=<N>, descriptor_symbolized_sites=<N>, invoke_trampoline_symbolized_sites=<N>, missing_invoke_trampoline_sites=<N>, non_normalized_layout_sites=<N>, contract_violation_sites=<N>, deterministic_block_abi_invoke_trampoline_lowering_handoff=<bool>`
- `!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}`
- `!20 = !{i64 <block_literal_sites>, i64 <invoke_argument_slots_total>, i64 <capture_word_count_total>, i64 <parameter_entries_total>, i64 <capture_entries_total>, i64 <body_statement_entries_total>, i64 <descriptor_symbolized_sites>, i64 <invoke_trampoline_symbolized_sites>, i64 <missing_invoke_trampoline_sites>, i64 <non_normalized_layout_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m167_lowering_block_abi_invoke_trampoline_contract.py -q`

## Block storage escape lowering artifact contract (M168-C001)

M168-C publishes replay-stable block storage escape lowering invariants derived
from M168 sema `__block` storage/escape parity surfaces.

Deterministic lane-C artifact roots:

- `tmp/artifacts/compilation/objc3c-native/m168/lowering-block-storage-escape-contract/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m168/lowering-block-storage-escape-contract/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m168/lowering-block-storage-escape-contract/module.diagnostics.json`
- `tmp/reports/objc3c-native/m168/lowering-block-storage-escape-contract/block-storage-escape-source-anchors.txt`

Lowering contract markers:

- `kObjc3BlockStorageEscapeLoweringLaneContract`
- `Objc3BlockStorageEscapeLoweringContract`
- `IsValidObjc3BlockStorageEscapeLoweringContract(...)`
- `Objc3BlockStorageEscapeLoweringReplayKey(...)`

Replay key publication markers:

- `block_literal_sites=<N>`
- `mutable_capture_count_total=<N>`
- `byref_slot_count_total=<N>`
- `parameter_entries_total=<N>`
- `capture_entries_total=<N>`
- `body_statement_entries_total=<N>`
- `requires_byref_cells_sites=<N>`
- `escape_analysis_enabled_sites=<N>`
- `escape_to_heap_sites=<N>`
- `escape_profile_normalized_sites=<N>`
- `byref_layout_symbolized_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m168-block-storage-escape-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_block_storage_escape_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_mutable_capture_count`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_byref_slot_count`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_parameter_entries`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_capture_entries`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_body_statement_entries`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_requires_byref_cells_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_escape_analysis_enabled_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_escape_to_heap_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_escape_profile_normalized_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_byref_layout_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.block_storage_escape_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_block_storage_escape_replay_key`
- `frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface`
- `lowering_block_storage_escape.replay_key`
- `lowering_block_storage_escape.lane_contract`

IR publication markers:

- `; block_storage_escape_lowering = block_literal_sites=<N>;mutable_capture_count_total=<N>;byref_slot_count_total=<N>;parameter_entries_total=<N>;capture_entries_total=<N>;body_statement_entries_total=<N>;requires_byref_cells_sites=<N>;escape_analysis_enabled_sites=<N>;escape_to_heap_sites=<N>;escape_profile_normalized_sites=<N>;byref_layout_symbolized_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m168-block-storage-escape-lowering-v1`
- `; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites=<N>, mutable_capture_count_total=<N>, byref_slot_count_total=<N>, parameter_entries_total=<N>, capture_entries_total=<N>, body_statement_entries_total=<N>, requires_byref_cells_sites=<N>, escape_analysis_enabled_sites=<N>, escape_to_heap_sites=<N>, escape_profile_normalized_sites=<N>, byref_layout_symbolized_sites=<N>, contract_violation_sites=<N>, deterministic_block_storage_escape_lowering_handoff=<bool>`
- `!objc3.objc_block_storage_escape_lowering = !{!21}`
- `!21 = !{i64 <block_literal_sites>, i64 <mutable_capture_count_total>, i64 <byref_slot_count_total>, i64 <parameter_entries_total>, i64 <capture_entries_total>, i64 <body_statement_entries_total>, i64 <requires_byref_cells_sites>, i64 <escape_analysis_enabled_sites>, i64 <escape_to_heap_sites>, i64 <escape_profile_normalized_sites>, i64 <byref_layout_symbolized_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m168_lowering_block_storage_escape_contract.py -q`

## Block copy-dispose helper lowering artifact contract (M169-C001)

M169-C lowers sema-authored block copy/dispose helper semantics into deterministic
artifact packets and IR metadata publication surfaces.

Lowering contract markers:

- `kObjc3BlockCopyDisposeLoweringLaneContract`
- `Objc3BlockCopyDisposeLoweringContract`
- `IsValidObjc3BlockCopyDisposeLoweringContract(...)`
- `Objc3BlockCopyDisposeLoweringReplayKey(...)`

Replay key publication markers:

- `block_literal_sites=<N>`
- `mutable_capture_count_total=<N>`
- `byref_slot_count_total=<N>`
- `parameter_entries_total=<N>`
- `capture_entries_total=<N>`
- `body_statement_entries_total=<N>`
- `copy_helper_required_sites=<N>`
- `dispose_helper_required_sites=<N>`
- `profile_normalized_sites=<N>`
- `copy_helper_symbolized_sites=<N>`
- `dispose_helper_symbolized_sites=<N>`
- `contract_violation_sites=<N>`
- `deterministic=<bool>`
- `lane_contract=m169-block-copy-dispose-lowering-v1`

Published manifest contract keys:

- `frontend.pipeline.sema_pass_manager.deterministic_block_copy_dispose_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_mutable_capture_count`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_byref_slot_count`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_parameter_entries`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_capture_entries`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_body_statement_entries`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_copy_helper_required_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_dispose_helper_required_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_profile_normalized_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_copy_helper_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_dispose_helper_symbolized_sites`
- `frontend.pipeline.sema_pass_manager.block_copy_dispose_lowering_contract_violation_sites`
- `frontend.pipeline.sema_pass_manager.lowering_block_copy_dispose_replay_key`
- `frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface`
- `lowering_block_copy_dispose.replay_key`
- `lowering_block_copy_dispose.lane_contract`

IR publication markers:

- `; block_copy_dispose_lowering = block_literal_sites=<N>;mutable_capture_count_total=<N>;byref_slot_count_total=<N>;parameter_entries_total=<N>;capture_entries_total=<N>;body_statement_entries_total=<N>;copy_helper_required_sites=<N>;dispose_helper_required_sites=<N>;profile_normalized_sites=<N>;copy_helper_symbolized_sites=<N>;dispose_helper_symbolized_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m169-block-copy-dispose-lowering-v1`
- `; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites=<N>, mutable_capture_count_total=<N>, byref_slot_count_total=<N>, parameter_entries_total=<N>, capture_entries_total=<N>, body_statement_entries_total=<N>, copy_helper_required_sites=<N>, dispose_helper_required_sites=<N>, profile_normalized_sites=<N>, copy_helper_symbolized_sites=<N>, dispose_helper_symbolized_sites=<N>, contract_violation_sites=<N>, deterministic_block_copy_dispose_lowering_handoff=<bool>`
- `!objc3.objc_block_copy_dispose_lowering = !{!22}`
- `!22 = !{i64 <block_literal_sites>, i64 <mutable_capture_count_total>, i64 <byref_slot_count_total>, i64 <parameter_entries_total>, i64 <capture_entries_total>, i64 <body_statement_entries_total>, i64 <copy_helper_required_sites>, i64 <dispose_helper_required_sites>, i64 <profile_normalized_sites>, i64 <copy_helper_symbolized_sites>, i64 <dispose_helper_symbolized_sites>, i64 <contract_violation_sites>, i1 <deterministic>}`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m169_lowering_block_copy_dispose_contract.py -q`

## Block determinism/perf baseline lowering artifact contract (M170-C001)

M170-C lowers sema-authored block determinism/perf baseline summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M170-C lowering contract anchors:

- `kObjc3BlockDeterminismPerfBaselineLoweringLaneContract`
- `Objc3BlockDeterminismPerfBaselineLoweringContract`
- `IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(...)`
- `Objc3BlockDeterminismPerfBaselineLoweringReplayKey(...)`

Pipeline/manifest and IR markers:

- `frontend.pipeline.sema_pass_manager.deterministic_block_determinism_perf_baseline_handoff`
- `frontend.pipeline.sema_pass_manager.block_determinism_perf_baseline_sites_total`
- `frontend.pipeline.semantic_surface.objc_block_determinism_perf_baseline_lowering_surface`
- `lowering_block_determinism_perf_baseline.replay_key`
- `; block_determinism_perf_baseline_lowering = block_literal_sites=<N>...`
- `; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites=<N>...`
- `!objc3.objc_block_determinism_perf_baseline_lowering = !{!23}`

Recommended M170 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m170_lowering_block_determinism_perf_baseline_contract.py -q`

## Lightweight generics constraint lowering artifact contract (M171-C001)

M171-C lowers sema-authored lightweight generic constraint summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M171-C lowering contract anchors:

- `kObjc3LightweightGenericsConstraintLoweringLaneContract`
- `Objc3LightweightGenericsConstraintLoweringContract`
- `IsValidObjc3LightweightGenericsConstraintLoweringContract(...)`
- `Objc3LightweightGenericsConstraintLoweringReplayKey(...)`

Pipeline/manifest and IR markers:

- `frontend.pipeline.sema_pass_manager.deterministic_lightweight_generic_constraint_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.lightweight_generic_constraint_lowering_sites`
- `frontend.pipeline.semantic_surface.objc_lightweight_generic_constraint_lowering_surface`
- `lowering_lightweight_generic_constraint.replay_key`
- `; lightweight_generic_constraint_lowering = generic_constraint_sites=<N>...`
- `; frontend_objc_lightweight_generic_constraint_lowering_profile = generic_constraint_sites=<N>...`
- `!objc3.objc_lightweight_generic_constraint_lowering = !{!24}`

Recommended M171 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m171_lowering_lightweight_generics_constraints_contract.py -q`

## Nullability-flow warning-precision lowering artifact contract (M172-C001)

M172-C lowers sema-authored nullability-flow warning-precision summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M172-C lowering contract anchors:

- `kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract`
- `Objc3NullabilityFlowWarningPrecisionLoweringContract`
- `IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(...)`
- `Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(...)`

Pipeline/manifest and IR markers:

- `frontend.pipeline.sema_pass_manager.deterministic_nullability_flow_warning_precision_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.nullability_flow_warning_precision_lowering_sites`
- `frontend.pipeline.semantic_surface.objc_nullability_flow_warning_precision_lowering_surface`
- `lowering_nullability_flow_warning_precision.replay_key`
- `; nullability_flow_warning_precision_lowering = nullability_flow_sites=<N>...`
- `; frontend_objc_nullability_flow_warning_precision_lowering_profile = nullability_flow_sites=<N>...`
- `!objc3.objc_nullability_flow_warning_precision_lowering = !{!25}`

Recommended M172 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m172_lowering_nullability_flow_warning_precision_contract.py -q`

## Protocol-qualified object type lowering artifact contract (M173-C001)

M173-C lowers sema-authored protocol-qualified object type summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M173-C lowering contract anchors:

- `kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract`
- `Objc3ProtocolQualifiedObjectTypeLoweringContract`
- `IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(...)`
- `Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(...)`

Pipeline/manifest and IR markers:

- `frontend.pipeline.sema_pass_manager.deterministic_protocol_qualified_object_type_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.protocol_qualified_object_type_lowering_sites`
- `frontend.pipeline.semantic_surface.objc_protocol_qualified_object_type_lowering_surface`
- `lowering_protocol_qualified_object_type.replay_key`
- `; protocol_qualified_object_type_lowering = protocol_qualified_object_type_sites=<N>...`
- `; frontend_objc_protocol_qualified_object_type_lowering_profile = protocol_qualified_object_type_sites=<N>...`
- `!objc3.objc_protocol_qualified_object_type_lowering = !{!26}`

Recommended M173 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m173_lowering_protocol_qualified_object_type_contract.py -q`

## Variance/bridged-cast lowering artifact contract (M174-C001)

M174-C lowers sema-authored variance/bridged-cast summaries into deterministic
lowering replay metadata and IR side-channel annotations.

M174-C lowering contract anchors:

- `kObjc3VarianceBridgeCastLoweringLaneContract`
- `Objc3VarianceBridgeCastLoweringContract`
- `IsValidObjc3VarianceBridgeCastLoweringContract(...)`
- `Objc3VarianceBridgeCastLoweringReplayKey(...)`

Pipeline/manifest and IR markers:

- `frontend.pipeline.sema_pass_manager.deterministic_variance_bridge_cast_lowering_handoff`
- `frontend.pipeline.sema_pass_manager.variance_bridge_cast_lowering_sites`
- `frontend.pipeline.semantic_surface.objc_variance_bridge_cast_lowering_surface`
- `lowering_variance_bridge_cast.replay_key`
- `; variance_bridge_cast_lowering = variance_bridge_cast_sites=<N>...`
- `; frontend_objc_variance_bridge_cast_lowering_profile = variance_bridge_cast_sites=<N>...`
- `!objc3.objc_variance_bridge_cast_lowering = !{!27}`

Recommended M174 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m174_lowering_variance_bridge_cast_contract.py -q`

## Generic metadata emission and ABI checks lowering artifact contract (M175-C001)

M175-C lowers sema-authored generic metadata/ABI summaries into deterministic
lowering replay metadata and IR side-channel annotations.

M175-C lowering contract anchors:

- `kObjc3GenericMetadataAbiLoweringLaneContract`
- `Objc3GenericMetadataAbiLoweringContract`
- `IsValidObjc3GenericMetadataAbiLoweringContract(...)`
- `Objc3GenericMetadataAbiLoweringReplayKey(...)`
- `BuildGenericMetadataAbiLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_generic_metadata_abi_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_generic_metadata_abi_lowering_surface`
- `lowering_generic_metadata_abi.replay_key`
- `; generic_metadata_abi_lowering = generic_metadata_abi_sites=<N>...`
- `; frontend_objc_generic_metadata_abi_lowering_profile = generic_metadata_abi_sites=<N>...`
- `!objc3.objc_generic_metadata_abi_lowering = !{!28}`

Recommended M175 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m175_lowering_generic_metadata_abi_contract.py -q`

## Module map ingestion and import graph lowering artifact contract (M176-C001)

M176-C lowers sema-authored module-import-graph summaries into deterministic
lowering replay metadata and IR side-channel annotations.

M176-C lowering contract anchors:

- `kObjc3ModuleImportGraphLoweringLaneContract`
- `Objc3ModuleImportGraphLoweringContract`
- `IsValidObjc3ModuleImportGraphLoweringContract(...)`
- `Objc3ModuleImportGraphLoweringReplayKey(...)`
- `BuildModuleImportGraphLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_module_import_graph_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_module_import_graph_lowering_surface`
- `lowering_module_import_graph.replay_key`
- `; module_import_graph_lowering = module_import_graph_sites=<N>...`
- `; frontend_objc_module_import_graph_lowering_profile = module_import_graph_sites=<N>...`
- `!objc3.objc_module_import_graph_lowering = !{!29}`

Recommended M176 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m176_lowering_module_import_graph_contract.py -q`

## Namespace collision and shadowing diagnostics lowering artifact contract (M177-C001)

M177-C lowers sema-authored namespace-collision/shadowing summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M177-C lowering contract anchors:

- `kObjc3NamespaceCollisionShadowingLoweringLaneContract`
- `Objc3NamespaceCollisionShadowingLoweringContract`
- `IsValidObjc3NamespaceCollisionShadowingLoweringContract(...)`
- `Objc3NamespaceCollisionShadowingLoweringReplayKey(...)`
- `BuildNamespaceCollisionShadowingLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_namespace_collision_shadowing_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_namespace_collision_shadowing_lowering_surface`
- `lowering_namespace_collision_shadowing.replay_key`
- `; namespace_collision_shadowing_lowering = namespace_collision_shadowing_sites=<N>...`
- `; frontend_objc_namespace_collision_shadowing_lowering_profile = namespace_collision_shadowing_sites=<N>...`
- `!objc3.objc_namespace_collision_shadowing_lowering = !{!30}`

Recommended M177 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m177_lowering_namespace_collision_shadowing_contract.py -q`

## Public-private API partition semantics lowering artifact contract (M178-C001)

M178-C lowers sema-authored public/private API partition summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M178-C lowering contract anchors:

- `kObjc3PublicPrivateApiPartitionLoweringLaneContract`
- `Objc3PublicPrivateApiPartitionLoweringContract`
- `IsValidObjc3PublicPrivateApiPartitionLoweringContract(...)`
- `Objc3PublicPrivateApiPartitionLoweringReplayKey(...)`
- `BuildPublicPrivateApiPartitionLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_public_private_api_partition_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_public_private_api_partition_lowering_surface`
- `lowering_public_private_api_partition.replay_key`
- `; public_private_api_partition_lowering = public_private_api_partition_sites=<N>...`
- `; frontend_objc_public_private_api_partition_lowering_profile = public_private_api_partition_sites=<N>...`
- `!objc3.objc_public_private_api_partition_lowering = !{!31}`

Recommended M178 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m178_lowering_public_private_api_partition_contract.py -q`

## Incremental module cache and invalidation lowering artifact contract (M179-C001)

M179-C lowers sema-authored incremental module cache/invalidation summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M179-C lowering contract anchors:

- `kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract`
- `Objc3IncrementalModuleCacheInvalidationLoweringContract`
- `IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(...)`
- `Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(...)`
- `BuildIncrementalModuleCacheInvalidationLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_incremental_module_cache_invalidation_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_incremental_module_cache_invalidation_lowering_surface`
- `lowering_incremental_module_cache_invalidation.replay_key`
- `; incremental_module_cache_invalidation_lowering = incremental_module_cache_invalidation_sites=<N>...`
- `; frontend_objc_incremental_module_cache_invalidation_lowering_profile = incremental_module_cache_invalidation_sites=<N>...`
- `!objc3.objc_incremental_module_cache_invalidation_lowering = !{!32}`

Recommended M179 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m179_lowering_incremental_module_cache_contract.py -q`

## Cross-module conformance suite lowering artifact contract (M180-C001)

M180-C lowers sema-authored cross-module conformance summaries into
deterministic lowering replay metadata and IR side-channel annotations.

M180-C lowering contract anchors:

- `kObjc3CrossModuleConformanceLoweringLaneContract`
- `Objc3CrossModuleConformanceLoweringContract`
- `IsValidObjc3CrossModuleConformanceLoweringContract(...)`
- `Objc3CrossModuleConformanceLoweringReplayKey(...)`
- `BuildCrossModuleConformanceLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_cross_module_conformance_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_cross_module_conformance_lowering_surface`
- `lowering_cross_module_conformance.replay_key`
- `; cross_module_conformance_lowering = cross_module_conformance_sites=<N>...`
- `; frontend_objc_cross_module_conformance_lowering_profile = cross_module_conformance_sites=<N>...`
- `!objc3.objc_cross_module_conformance_lowering = !{!33}`

Recommended M180 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m180_lowering_cross_module_conformance_contract.py -q`

## Throws propagation lowering artifact contract (M181-C001)

M181-C lowers sema-authored throws propagation summaries into deterministic
lowering replay metadata and IR side-channel annotations.

M181-C lowering contract anchors:

- `kObjc3ThrowsPropagationLoweringLaneContract`
- `Objc3ThrowsPropagationLoweringContract`
- `IsValidObjc3ThrowsPropagationLoweringContract(...)`
- `Objc3ThrowsPropagationLoweringReplayKey(...)`
- `BuildThrowsPropagationLoweringContract(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_throws_propagation_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_throws_propagation_lowering_surface`
- `lowering_throws_propagation.replay_key`
- `; throws_propagation_lowering = throws_propagation_sites=<N>...`
- `; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites=<N>...`
- `!objc3.objc_throws_propagation_lowering = !{!34}`

Recommended M181 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m181_lowering_throws_propagation_contract.py -q`

## Result-like lowering artifact contract (M182-C001)

M182-C publishes deterministic lowering replay metadata for result-like
control-flow handoff.

M182-C lowering contract anchors:

- `kObjc3ResultLikeLoweringLaneContract`
- `Objc3ResultLikeLoweringContract`
- `IsValidObjc3ResultLikeLoweringContract(...)`
- `Objc3ResultLikeLoweringReplayKey(...)`

Deterministic handoff checks:

- `normalized_sites + branch_merge_sites == result_like_sites`
- each of `result_success_sites`, `result_failure_sites`, `result_branch_sites`,
  `result_payload_sites`, and `contract_violation_sites` is bounded by
  `result_like_sites`
- `deterministic_result_like_lowering_handoff` requires zero contract violations

IR replay publication marker:

- `result_like_lowering = result_like_sites=<N>;result_success_sites=<N>;result_failure_sites=<N>;result_branch_sites=<N>;result_payload_sites=<N>;normalized_sites=<N>;branch_merge_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m182-result-like-lowering-v1`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m182_lowering_result_like_contract.py -q`

## NSError bridging lowering artifact contract (M183-C001)

M183-C lowers sema-authored NSError bridging summaries into deterministic
lowering replay metadata and IR side-channel annotations.

M183-C lowering contract anchors:

- `kObjc3NSErrorBridgingLoweringLaneContract`
- `Objc3NSErrorBridgingLoweringContract`
- `IsValidObjc3NSErrorBridgingLoweringContract(...)`
- `Objc3NSErrorBridgingLoweringReplayKey(...)`
- `frontend.pipeline.sema_pass_manager.deterministic_ns_error_bridging_lowering_handoff`
- `frontend.pipeline.semantic_surface.objc_ns_error_bridging_lowering_surface`
- `lowering_ns_error_bridging.replay_key`
- `; ns_error_bridging_lowering = ns_error_bridging_sites=<N>;ns_error_parameter_sites=<N>;ns_error_out_parameter_sites=<N>;ns_error_bridge_path_sites=<N>;failable_call_sites=<N>;normalized_sites=<N>;bridge_boundary_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m183-ns-error-bridging-lowering-v1`
- `; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites=<N>...`
- `!objc3.objc_ns_error_bridging_lowering = !{!36}`

Recommended M183 lowering contract check:

- `python -m pytest tests/tooling/test_objc3c_m183_lowering_ns_error_bridging_contract.py -q`

## Task runtime interop and cancellation lowering artifact contract (M189-C001)

M189-C publishes deterministic lowering replay metadata for task runtime
interop and cancellation boundaries.

M189-C lowering contract anchors:

- `kObjc3TaskRuntimeInteropCancellationLoweringLaneContract`
- `Objc3TaskRuntimeInteropCancellationLoweringContract`
- `IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(...)`
- `Objc3TaskRuntimeInteropCancellationLoweringReplayKey(...)`
- `task_runtime_interop_cancellation_lowering = task_runtime_sites=<N>`
- `frontend_objc_task_runtime_interop_cancellation_lowering_profile`
- `!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}`

Deterministic handoff checks:

- `normalized_sites + guard_blocked_sites == task_runtime_sites`
- `runtime_resume_sites + runtime_cancel_sites <= task_runtime_sites`
- each of `task_runtime_interop_sites`, `cancellation_probe_sites`,
  `cancellation_handler_sites`, and `contract_violation_sites` is bounded by
  `task_runtime_sites`
- `deterministic_task_runtime_interop_cancellation_lowering_handoff` requires
  zero contract violations

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m189_lowering_task_runtime_interop_cancellation_contract.py -q`

## Concurrency replay-proof and race-guard lowering artifact contract (M190-C001)

M190-C publishes deterministic lowering replay metadata for concurrency
replay-proof scheduling and race-guard boundaries.

M190-C lowering contract anchors:

- `kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract`
- `Objc3ConcurrencyReplayRaceGuardLoweringContract`
- `IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(...)`
- `Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(...)`
- `concurrency_replay_race_guard_lowering = concurrency_replay_sites=<N>`
- `frontend_objc_concurrency_replay_race_guard_lowering_profile`
- `!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}`

Deterministic handoff checks:

- `deterministic_schedule_sites + guard_blocked_sites == concurrency_replay_sites`
- each of `replay_proof_sites`, `race_guard_sites`, `task_handoff_sites`,
  `actor_isolation_sites`, and `contract_violation_sites` is bounded by
  `concurrency_replay_sites`
- `deterministic_concurrency_replay_race_guard_lowering_handoff` requires zero
  contract violations

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m190_lowering_concurrency_replay_race_guard_contract.py -q`

## Unsafe and pointer-arithmetic extension gating lowering artifact contract (M191-C001)

M191-C publishes deterministic lowering replay metadata for unsafe low-level
extension gating and pointer-arithmetic boundaries.

M191-C lowering contract anchors:

- `kObjc3UnsafePointerExtensionLoweringLaneContract`
- `Objc3UnsafePointerExtensionLoweringContract`
- `IsValidObjc3UnsafePointerExtensionLoweringContract(...)`
- `Objc3UnsafePointerExtensionLoweringReplayKey(...)`
- `unsafe_pointer_extension_lowering = unsafe_pointer_extension_sites=<N>`
- `frontend_objc_unsafe_pointer_extension_lowering_profile`
- `!objc3.objc_unsafe_pointer_extension_lowering = !{!37}`

Deterministic handoff checks:

- `normalized_sites + gate_blocked_sites == unsafe_pointer_extension_sites`
- each of `unsafe_keyword_sites`, `pointer_arithmetic_sites`,
  `raw_pointer_type_sites`, `unsafe_operation_sites`, and
  `contract_violation_sites` is bounded by `unsafe_pointer_extension_sites`
- `deterministic_unsafe_pointer_extension_lowering_handoff` requires zero
  contract violations

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m191_lowering_unsafe_pointer_contract.py -q`

## Inline asm and intrinsic governance lowering artifact contract (M192-C001)

M192-C publishes deterministic lowering replay metadata for inline asm and
intrinsic governance boundaries.

M192-C lowering contract anchors:

- `kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract`
- `Objc3InlineAsmIntrinsicGovernanceLoweringContract`
- `IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(...)`
- `Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(...)`
- `inline_asm_intrinsic_governance_lowering = inline_asm_intrinsic_sites=<N>`
- `frontend_objc_inline_asm_intrinsic_governance_lowering_profile`
- `!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}`

Deterministic handoff checks:

- `normalized_sites + gate_blocked_sites == inline_asm_intrinsic_sites`
- each of `inline_asm_sites`, `intrinsic_sites`, `governed_intrinsic_sites`,
  `privileged_intrinsic_sites`, and `contract_violation_sites` is bounded by
  `inline_asm_intrinsic_sites`
- `deterministic_inline_asm_intrinsic_governance_lowering_handoff` requires
  zero contract violations

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m192_lowering_inline_asm_intrinsic_contract.py -q`

## Execution smoke commands (M26 lane-E)

```powershell
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

- Direct script equivalent path:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
```

- Optional toolchain override:
  - `OBJC3C_NATIVE_EXECUTION_CLANG_PATH=<clang executable>`

