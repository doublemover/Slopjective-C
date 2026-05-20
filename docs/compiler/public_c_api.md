# Public C API

The public C API is a compiler integration surface. It does not create language
support without a matching capability matrix row.

Current runtime public C API ownership lives in:

- `native/objc3c/src/runtime/public/objc3_runtime_api.h`
- `native/objc3c/src/runtime/public/objc3_runtime_registration.h`
- `native/objc3c/src/runtime/public/objc3_runtime_result.h`
- `native/objc3c/src/runtime/public/objc3_runtime_dispatch_result.h`
- `native/objc3c/src/runtime/public/objc3_runtime_result_contract.h`

The exported runtime surface is:

- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32_checked`
- `objc3_runtime_dispatch_i32_from_class_checked`
- `objc3_runtime_dispatch_typed_checked`
- `objc3_runtime_dispatch_typed_from_class_checked`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_dispatch_i32_from_class`
- `objc3_runtime_dispatch_typed_value`
- `objc3_runtime_dispatch_typed_value_from_class`
- `objc3_runtime_copy_registration_state_for_testing`
- `objc3_runtime_reset_for_testing`

`objc3_runtime_dispatch_i32_checked` returns
`objc3_runtime_dispatch_i32_result`, including `abi_version`, `result_size`, a
status code, value, diagnostic code, diagnostic message, result contract,
diagnostic owner model, and fail-closed owner model.
`objc3_runtime_dispatch_typed_checked` returns
`objc3_runtime_dispatch_typed_result`, which carries `abi_version`,
`result_size`, the same result ownership fields plus a return kind and exactly
one populated typed payload field for successful non-void dispatch.
`objc3_runtime_dispatch_i32` remains the narrow plain i32 lowering entrypoint
for supported live sends; typed source lowering uses
`objc3_runtime_dispatch_typed_value` or
`objc3_runtime_dispatch_typed_value_from_class` when semantic analysis knows a
non-i32 return shape, including `super` lookup that must start at an explicit
class. These entrypoints do not create a second dispatch mode or bypass the
checked result ownership contract.

`objc3_runtime_registration_state_snapshot` is also ABI guarded with
`abi_version` and `snapshot_size` fields. Snapshot strings are runtime-owned
borrowed pointers, and callers must treat the struct shape as versioned public
runtime ABI.

Current frontend public C API ownership lives in:

- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_version.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_context.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_options.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_result.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_diagnostic.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_artifact.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_string.h`
- `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_error.h`
- `native/objc3c/src/libobjc3c_frontend/c_api.h`

Other headers in `native/objc3c/src/libobjc3c_frontend/` are internal C++
owner headers unless this document lists them above. They may consume or
populate public structs, but they do not define package-facing C ABI.

The frontend result contract is:

- callers own `objc3c_frontend_compile_result_t` records and zero-initialize them
  before first use,
- C-only callers may instead use opaque result handles returned by
  `objc3c_frontend_c_compile_*_owned()`,
- compile entrypoints populate result-owned strings,
- `objc3c_frontend_result_destroy()` and
  `objc3c_frontend_c_result_destroy()` release result-owned strings and clear
  caller-provided compile results,
- `objc3c_frontend_c_owned_result_destroy()` releases opaque result handles and
  their payload strings,
- borrowed result strings returned by accessors remain valid only until result
  destruction,
- standalone owned strings are released only through the matching string
  release function,
- stage summaries are value snapshots, while detailed diagnostics are read
  through the result-owned diagnostics artifact path,
- artifact accessors return `NULL` for undefined artifact kinds or missing
  outputs.

The frontend C API runner treats those ownership rules as executable
publication gates rather than presentation hints:

- runner compile sessions keep an opaque owned result handle and release
  result-owned payload strings only through
  `objc3c_frontend_c_owned_result_destroy()` in the session guard,
- runner option strings and paths are borrowed only for the compile call; the
  runner snapshots result-owned strings before destroying the context/result,
- result-owned error strings must be absent on successful compiles and present
  with non-empty text on failures,
- result-owned artifact paths are never synthesized by the runner; required
  diagnostics, manifest, IR, and object paths must be returned by the C API
  according to compile status and requested output options,
- output summaries publish the release/free contract for the result, standalone
  strings, error message, and each artifact path explicitly.

Do not infer language support from the existence of a C entrypoint. Public docs
must route claims through `docs/support/capability_matrix.md` and
`docs/support/evidence_map.md`.
