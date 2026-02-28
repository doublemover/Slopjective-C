# Objc3c Native Frontend (Current Contract)

This document captures the currently implemented behavior for the native `objc3c` frontend (`native/objc3c/src/main.cpp`) and its command wrappers.

## Supported inputs

- `.objc3` files: custom lexer/parser + semantic checks + LLVM IR emission + object build.
- Any other extension (for example `.m`): libclang parse/diagnostics + symbol manifest + Objective-C object build.

## CLI usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [-fobjc-version=<N>] [--objc3-language-version <N>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

- Default `--out-dir`: `tmp/artifacts/compilation/objc3c-native`
- Default `--emit-prefix`: `module`
- Default `--clang`: `clang` (or explicit path)
- Default `--llc`: `llc` (or explicit path)
- Default language version: `3` (`-fobjc-version=<N>` or `--objc3-language-version <N>`)
- Default `--objc3-compat-mode`: `canonical`
- Default `--objc3-migration-assist`: `off`
- Default `--objc3-ir-object-backend`: `llvm-direct`
- Default `--objc3-max-message-args`: `4`
- Default `--objc3-runtime-dispatch-symbol`: `objc3_msgsend_i32`
- Native frontend language-version support is currently fail-closed to Objective-C `3` only; other version values return usage error exit `2`.

## C API parity runner usage (M142-E001)

```text
objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [--summary-out <path>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]
```

- Binary path produced by native build scripts: `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- Default `--clang`: `clang` (or explicit path)
- Default `--llc`: `llc` (or explicit path)
- Default `--objc3-compat-mode`: `canonical`
- Default `--objc3-migration-assist`: `off`
- Default `--objc3-ir-object-backend`: `clang`
- Default summary output when `--summary-out` is omitted: `<out-dir>/<emit-prefix>.c_api_summary.json`
- Runner summary captures deterministic compatibility controls (`ir_object_backend`, `compatibility_mode`, `migration_assist`) and output paths (`diagnostics`, `manifest`, `ir`, `object`) for parity replay.
- For CLI/C API parity harness runs, use CLI backend override `--objc3-ir-object-backend clang` so both paths produce objects through the same compile backend.

## Artifact tmp-path governance (M143-D001)

- Source-mode parity workflows are tmp-governed by default:
  - `--work-dir` defaults to `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work`.
  - Derived outputs are rooted under `<work-dir>/<work_key>/{library,cli}`.
- `--work-key` is deterministic by default (derived from source + emit controls) and can be pinned explicitly for reproducible path contracts.
- Non-tmp source-mode work directories are rejected unless `--allow-non-tmp-work-dir` is set.

## LLVM capability discovery and backend routing (M144-E001)

- `--llvm-capabilities-summary <path>` points CLI routing to a deterministic capability summary packet.
- `--objc3-route-backend-from-capabilities` derives `--objc3-ir-object-backend` from summary capabilities (uses `llvm-direct` only when `llc --filetype=obj` is available).
- Capability probes are captured with `npm run check:objc3c:llvm-capabilities` into `tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`.

## Driver shell split boundaries (M136-E001)

- Driver source wiring order is deterministic:
  - `src/driver/objc3_cli_options.cpp`
  - `src/driver/objc3_driver_shell.cpp`
  - `src/driver/objc3_frontend_options.cpp`
  - `src/driver/objc3_objc3_path.cpp`
  - `src/driver/objc3_objectivec_path.cpp`
  - `src/driver/objc3_compilation_driver.cpp`
- `objc3_driver_shell` owns shell-only responsibilities:
  - classify input kind (`.objc3` vs non-`.objc3`),
  - validate required tool paths (`clang` / `llc`) and input file presence.
- `objc3_frontend_options` owns CLI-to-frontend option mapping for `.objc3` lowering controls.
- `objc3_objc3_path` defines extracted `.objc3` path execution helpers during shell split rollout.
- `objc3_compilation_driver` owns top-level shell dispatch/orchestration and routes non-`.objc3` inputs to Objective-C path execution.
- `objc3_objectivec_path` owns Objective-C translation-unit parse, diagnostics normalization, symbol-manifest emission, and object compilation.
- CLI split boundaries remain stable; compatibility controls (`-fobjc-version`, `--objc3-language-version`,
  `--objc3-compat-mode`, `--objc3-migration-assist`) are parsed at the driver shell layer and validated fail-closed.

