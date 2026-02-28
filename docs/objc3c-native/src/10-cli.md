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

