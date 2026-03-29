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

## Human Versus Machine Surface

Reader-facing documentation should explain what these artifacts mean and when
they are trustworthy. Machine-facing documentation should carry exact file
inventories, runner mappings, and generated proof/report paths.

Use these live paths:

- human-facing implementation narrative:
  - `README.md`
  - `site/index.md`
  - `docs/objc3c-native.md`
- generated operator and machine-facing appendix:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/render_objc3c_public_command_surface.py`
- generated proof/report outputs:
  - `tmp/reports/`
  - `tmp/artifacts/`

Non-goals for human-facing docs:

- dumping `tmp/` artifact inventories inline,
- mirroring generated runbooks by hand,
- using raw report paths as the primary way to explain the toolchain.

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
