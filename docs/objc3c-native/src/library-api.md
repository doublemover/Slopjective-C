# libobjc3c_frontend Library API

This document describes the live embedding surface exposed by `native/objc3c/src/libobjc3c_frontend/api.h`.

## Public Surface

- primary header: `native/objc3c/src/libobjc3c_frontend/api.h`
- version header: `native/objc3c/src/libobjc3c_frontend/version.h`
- optional C shim header: `native/objc3c/src/libobjc3c_frontend/c_api.h`

`api.h` exposes a C ABI with an opaque frontend context type.

## Stability

- exported symbols, enums, and struct layouts in `api.h` are the ABI boundary
- append-only growth for public structs
- zero-initialize option and result structs before use

## Compatibility and Versioning

- version macros live in `version.h`
- use `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints
- `objc3c_frontend_version().abi_version` must match `objc3c_frontend_abi_version()`
