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
  - `npm run objc3c -- build-public-command-surface`
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
- non-authoritative test surfaces by themselves
- sidecars that are not tied to a reproducible compile and probe path

## Current Corrective Gaps

- unresolved sends must publish typed strict dispatch errors through the checked
  runtime result path
- strict dispatch status/error coverage must remain live-probe-backed and must
  include success for nil receiver, resolved live methods, resolved builtins,
  and resolved property accessors, plus structured errors for unknown selectors,
  unknown receiver classes, missing class graph state, rejected return
  shapes, rejected argument layouts, malformed metadata, and category
  conflicts
- the hard-cutover runtime module tree under
  `native/objc3c/src/runtime/{public,state,selectors,images,classes,dispatch,storage,memory,blocks,errors,concurrency}/`
  is the current named runtime layout for module-tree claims; those claims cover
  the wired dispatch, selector, image, and class-graph helper paths only when
  the linked strict dispatch probes and gates publish matching status evidence
- synthesized accessor IR still carries transitional lowering residue in `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- native proof remains invalid unless the emitted object, manifest, and linked runtime probe all come from the same reproducible compile path
