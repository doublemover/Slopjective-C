# Compiler Architecture

The native compiler is split into explicit stage and support targets under
`native/objc3c/src/`. The top-level target graph currently routes through:

- `config`, `ast`, and `support` for shared compiler model and utilities,
- `diag`, `diagnostics`, and `lex` for diagnostics and source tokenization,
- `parse` and `sema` for canonical source acceptance and rejection,
- `lower` and `ir` for lowering contracts, runtime metadata policy, and LLVM IR
  emission,
- `io` and `artifacts` for shared JSON/schema handling and frontend artifacts,
- `pipeline` for frontend handoff, dispatch-surface classification, and
  phase-result ownership,
- `runtime` for `objc3_runtime` and the public runtime C API,
- `libobjc3c_frontend`, `driver`, `cli`, and `tools` for embeddable and command
  entrypoints.

Current capability truth lives in `docs/support/capability_matrix.md`; evidence
rows in `docs/support/evidence_map.md` point to the owner modules. The module
split itself is an internal capability row. It does not widen the public
Objective-C 3.0 language surface without matching behavior evidence.

The CMake ownership evidence is:

- `native/objc3c/src/CMakeLists.txt` for stage ordering,
- `native/objc3c/src/config/CMakeLists.txt`,
  `native/objc3c/src/ast/CMakeLists.txt`, and
  `native/objc3c/src/support/CMakeLists.txt` for shared compiler model and
  utility ownership,
- `native/objc3c/src/diag/CMakeLists.txt`,
  `native/objc3c/src/diagnostics/CMakeLists.txt`, and
  `native/objc3c/src/lex/CMakeLists.txt` for diagnostics and source-token
  ownership,
- `native/objc3c/src/parse/CMakeLists.txt` and
  `native/objc3c/src/sema/CMakeLists.txt` for canonical source acceptance and
  rejection ownership,
- `native/objc3c/src/lower/CMakeLists.txt` for lowering module ownership,
- `native/objc3c/src/ir/CMakeLists.txt` for IR module ownership,
- `native/objc3c/src/pipeline/CMakeLists.txt` for pipeline and dispatch surface
  classification,
- `native/objc3c/src/io/CMakeLists.txt` and
  `native/objc3c/src/artifacts/CMakeLists.txt` for shared JSON/schema
  ownership,
- `native/objc3c/src/runtime/CMakeLists.txt` for the strict runtime library and
  public C API headers,
- `native/objc3c/src/libobjc3c_frontend/CMakeLists.txt`,
  `native/objc3c/src/driver/CMakeLists.txt`,
  `native/objc3c/src/cli/CMakeLists.txt`, and
  `native/objc3c/src/tools/CMakeLists.txt` for frontend, driver, CLI, and tool
  entrypoint ownership.
