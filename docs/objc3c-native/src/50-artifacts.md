<!-- markdownlint-disable-file MD041 -->

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
