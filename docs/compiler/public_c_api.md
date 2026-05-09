# Public C API

The public C API is a compiler integration surface. It does not create language
support without a matching capability matrix row.

Current public C API ownership lives in:

- `native/objc3c/src/runtime/public/objc3_runtime_api.h`
- `native/objc3c/src/runtime/public/objc3_runtime_result.h`

The exported runtime surface is:

- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32_checked`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_copy_registration_state_for_testing`
- `objc3_runtime_reset_for_testing`

`objc3_runtime_dispatch_i32_checked` returns
`objc3_runtime_dispatch_i32_result`, including a status code, value, diagnostic
code, and diagnostic message. `objc3_runtime_dispatch_i32` remains the narrow
plain i32 lowering entrypoint for supported live sends; it does not create a
fallback dispatch mode or bypass the checked result ownership contract.

Do not infer language support from the existence of a C entrypoint. Public docs
must route claims through `docs/support/capability_matrix.md` and
`docs/support/evidence_map.md`.
