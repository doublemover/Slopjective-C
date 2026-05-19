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
- C-only name surface: `native/objc3c/src/libobjc3c_frontend/c_api.h`
- C-only owned result surface: `native/objc3c/src/libobjc3c_frontend/public/c_api_owned_result.h`

`objc3c_frontend.h` exposes the canonical C ABI with an opaque frontend context type.

## Header Ownership

- `objc3c_frontend_version.h`: export macros, version values, and ABI gates
- `objc3c_frontend_context.h`: opaque context lifecycle
- `objc3c_frontend_options.h`: borrowed compile inputs, paths, and emit options
- `objc3c_frontend_result.h`: result storage, status values, and result-owned strings
- `objc3c_frontend_diagnostic.h`: stage summary and severity metadata
- `objc3c_frontend_artifact.h`: artifact kind selectors
- `objc3c_frontend_string.h`: owned string and borrowed view lifetime rules
- `objc3c_frontend_error.h`: context error copy semantics
- `c_api.h`: C-only names over the same ABI and ownership rules

Headers in `native/objc3c/src/libobjc3c_frontend/` that are not listed in the
public surface are internal C++ owner headers. They may populate public structs
or publish artifacts, but they are not package-facing C ABI headers.

## Ownership Rules

- owned C result handles are allocated by `objc3c_frontend_c_compile_*_owned()`
  and released by `objc3c_frontend_c_owned_result_destroy()`
- compile result storage adapters remain caller-provided and zero-initialized
  before first use
- result-owned strings are released by result destruction, not by string release
- accessor-returned result strings/views are borrowed until result destruction
- standalone owned strings are released by the matching string release function
- stage summaries are value snapshots; detailed diagnostics are read through the
  diagnostics artifact path when produced
- undefined artifact selectors return no path

## Stability

- exported symbols, enums, and struct layouts in `objc3c_frontend*.h` are the ABI boundary
- zero-initialize option and result structs before use

## ABI Version Gate

- version macros live in `objc3c_frontend_version.h`
- use `objc3c_frontend_is_exact_abi_version(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints
- `objc3c_frontend_version().abi_version` must match `objc3c_frontend_abi_version()`
- mismatched ABI versions are unsupported; callers must use the current header
  and library pair rather than relying on adapter layers or retired aliases
