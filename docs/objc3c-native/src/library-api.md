# libobjc3c_frontend Library API

This document describes the live embedding surface exposed by `native/objc3c/src/libobjc3c_frontend/objc3c_frontend.h`.

## Public Surface

- primary header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend.h`
- version header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_version.h`
- context header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_context.h`
- options header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_options.h`
- result header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_result.h`
- diagnostic header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_diagnostic.h`
- artifact header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_artifact.h`
- string header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_string.h`
- error header: `native/objc3c/src/libobjc3c_frontend/objc3c_frontend_error.h`
- optional C API header: `native/objc3c/src/libobjc3c_frontend/c_api.h`

`objc3c_frontend.h` exposes the canonical C ABI with an opaque frontend context type.

## Stability

- exported symbols, enums, and struct layouts in `objc3c_frontend*.h` are the ABI boundary
- append-only growth for public structs
- zero-initialize option and result structs before use

## Compatibility and Versioning

- version macros live in `objc3c_frontend_version.h`
- use `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints
- `objc3c_frontend_version().abi_version` must match `objc3c_frontend_abi_version()`
