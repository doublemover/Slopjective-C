# Objc3c Native Frontend (Current Contract)

This document captures the currently implemented behavior for the native `objc3c` frontend (`native/objc3c/src/main.cpp`) and its command wrappers.

## Supported inputs

- `.objc3` files: custom lexer/parser + semantic checks + LLVM IR emission + object build.
- Any other extension (for example `.m`): libclang parse/diagnostics + symbol manifest + Objective-C object build.

## CLI usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

- Default `--out-dir`: `artifacts/compilation/objc3c-native`
- Default `--emit-prefix`: `module`
- Default `--clang`: `clang` (or explicit path)
- Default `--objc3-max-message-args`: `4`
- Default `--objc3-runtime-dispatch-symbol`: `objc3_msgsend_i32`

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
- CLI flags, defaults, and exit code semantics remain unchanged by the split.

