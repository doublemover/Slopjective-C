<!-- markdownlint-disable-file MD041 -->

# Objc3c Native Frontend (Current Surface)

The native frontend accepts two input classes:

- `.objc3`: native lexer, parser, sema, lowering, IR emission, and object build
- non-`.objc3`: Objective-C parse/diagnostics and object build through the
  Objective-C path

The non-`.objc3` path is a driver/frontend integration path. It does not create
an alternate Objective-C 3.0 source mode or old-surface support path.

## CLI Usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [-fobjc-version=<N>] [--objc3-language-version <N>] [--objc3-ir-object-backend <clang|llvm-direct>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

Defaults:

- output dir: `tmp/artifacts/compilation/objc3c-native`
- emit prefix: `module`
- clang: `clang`
- llc: `llc`
- language version: `3`
- runtime dispatch symbol: `objc3_runtime_dispatch_i32`

## C API Runner

```text
objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [--summary-out <path>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]
```

The native build publishes `artifacts/bin/objc3c-frontend-c-api-runner.exe`.
