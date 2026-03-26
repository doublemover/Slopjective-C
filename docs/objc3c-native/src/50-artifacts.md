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

## Native Output Truth

Treat these as authoritative only when they come from a real compiler invocation:

- emitted LLVM IR
- emitted object files
- manifests that point at those emitted outputs
- executable probes that link against `artifacts/lib/objc3_runtime.lib`

Do not treat these as authoritative proof:

- hand-written `.ll` files
- compatibility shims by themselves
- sidecars that are not tied to a reproducible compile and probe path

## Current Corrective Gaps

- unresolved sends still have one deterministic arithmetic fallback path in `native/objc3c/src/runtime/objc3_runtime.cpp`
- synthesized accessor IR still carries transitional lowering residue in `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- native proof remains invalid unless the emitted object, manifest, and linked runtime probe all come from the same reproducible compile path
