# Objc3c Native Frontend (Current Surface)

The native frontend supports two input modes:

- `.objc3`: native lexer, parser, sema, lowering, IR emission, and object build
- non-`.objc3`: Objective-C parse/diagnostics and object build through the Objective-C path

## CLI Usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [-fobjc-version=<N>] [--objc3-language-version <N>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

Defaults:

- output dir: `tmp/artifacts/compilation/objc3c-native`
- emit prefix: `module`
- clang: `clang`
- llc: `llc`
- language version: `3`
- runtime dispatch symbol: `objc3_msgsend_i32`

## C API Runner

```text
objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [--summary-out <path>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]
```

The native build publishes `artifacts/bin/objc3c-frontend-c-api-runner.exe`.
## Supported Grammar (Current)

The live `.objc3` frontend currently supports:

- module declarations
- global `let`
- functions and parameters
- statement blocks
- `if`, `while`, `do-while`, `for`, `switch`
- `break`, `continue`, `return`
- scalar expressions and conditional expressions
- arithmetic, logical, relational, bitwise, and shift operators
- function calls and bracket message sends
- selected Objective-C container and type forms admitted by the current parser

The grammar documentation here describes the admitted live surface only.
Historical milestone-by-milestone parser expansion notes are archived.
## Semantic Surface (Current)

The live frontend currently enforces:

- deterministic parser and semantic diagnostics
- lexical scope and symbol resolution
- scalar type compatibility across the admitted surface
- control-flow legality for loops, switches, and returns
- fail-closed handling for unsupported or incomplete language slices

## Lowering Surface (Current)

The live lowering path currently covers:

- scalar values and control flow
- function calls and admitted message-send lowering
- manifest generation
- LLVM IR emission
- object emission through the configured backend

## Runtime Boundary (Current)

The live compiler/runtime boundary is centered on emitted metadata plus the native runtime library under `native/objc3c/src/runtime` and `artifacts/lib/objc3_runtime.lib`.
## Diagnostics

The live frontend writes deterministic diagnostics in two forms:

- `<prefix>.diagnostics.txt`
- `<prefix>.diagnostics.json`

Diagnostics are always emitted, including on failure.
The current native path is intentionally fail closed when it encounters unsupported constructs outside the admitted runnable surface.
## Artifacts and Exit Codes

For `.objc3` input:

- Always writes diagnostics text and JSON.
- On success writes:
  - manifest JSON
  - LLVM IR (`.ll`)
  - object file (`.obj`)

For non-`.objc3` input:

- Always writes diagnostics text and JSON.
- On success writes:
  - manifest JSON
  - compiled Objective-C object

Exit codes:

- `0`: success
- `1`: parse, semantic, or diagnostic failure
- `2`: CLI usage or invalid invocation
- `3`: toolchain compile step failure

## Build Artifacts

The live native build publishes:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`
## Live Validation Commands

From repo root:

```powershell
npm run test:objc3c
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:full
npm run test:ci
npm run proof:objc3c
python scripts/ci/check_task_hygiene.py
python scripts/check_objc3c_dependency_boundaries.py --strict
```

## What The Live Test Surface Covers

- recovery and deterministic compile behavior
- runnable execution smoke coverage
- replay proof coverage
- fixture-matrix execution
- negative fixture expectations
- dependency-boundary enforcement
- compact task-hygiene enforcement
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
