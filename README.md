# Slopjective-C

Slopjective-C is the native Objective-C 3.0 compiler, runtime,
standard-library, package, showcase, and validation workspace. It takes checked
`.objc3` programs through a native compiler pipeline, publishes compiler
artifacts, links against an Objective-C 3 runtime surface, and keeps public
claims tied to replayable `npm run objc3c -- <action>` commands.

Objective-C with a spine: modules, typed functions, strict dispatch contracts,
runtime metadata, package manifests, public conformance rows, and a standard
library that is validated as code.

## The Hook

Objective-C 3.0 in this repo starts with familiar message syntax and adds
capabilities Objective-C 2.0 did not put behind one native compiler contract:

- module-scoped compilation,
- typed free functions beside Objective-C classes,
- typed method signatures and typed semantic flow,
- async/executor annotations,
- protocol and property metadata surfaced through the runtime,
- source string literals lowered into runtime-owned text handles,
- checked package, interop, ABI, and conformance artifacts.

Here is a checked-in program from `showcase/signalMesh/main.objc3`:

```objc
module SignalMesh;

extern fn task_runtime_cancelled_value() -> i32;

fn bridgeStatus(seed: i32) {
  return seed + 4;
}

async fn dispatchStatus(seed: i32) -> i32 __attribute__((objc_executor(named("com.example.signalmesh")))) {
  if (task_runtime_cancelled_value()) {
    return 0;
  }
  return bridgeStatus(seed);
}

@interface SignalMeshBox
- (i32) loadStatus async __attribute__((objc_executor(main)));
- (i32) currentStatus;
+ (i32) sharedStatus;
@end

@implementation SignalMeshBox
- (i32) loadStatus async __attribute__((objc_executor(main))) {
  return await dispatchStatus(5);
}

- (i32) currentStatus {
  return 9;
}

+ (i32) sharedStatus {
  return 4;
}
@end

fn foldStatus(seed: i32) {
  let state = seed + [SignalMeshBox sharedStatus];
  if (state > 10) {
    return state;
  }
  return 10;
}

fn main() {
  let box = [[SignalMeshBox alloc] init];
  return foldStatus([box currentStatus]);
}
```

Build the native toolchain and compile it:

```powershell
npm ci
python -m pip install --upgrade pytest jsonschema
npm run objc3c -- build-native-binaries
npm run objc3c -- compile-objc3c showcase/signalMesh/main.objc3 --out-dir tmp/readme-signalMesh --emit-prefix module
```

The native build publishes the compiler, C API runner, and runtime archive under
`artifacts/`. The compile command writes diagnostics, manifest data, LLVM IR,
object-output records, and frontend metadata under the selected output root.

## Current State

The checked capability matrix currently contains 93 rows:

- 73 implemented rows,
- 8 internal implementation-owner rows,
- 9 reserved rows,
- 3 rejected rows.

The implemented public surface is broad enough to work through actual language,
runtime, tooling, package, and release paths while still being strict about what
is not claimed. The matrix lives at
[`docs/support/capability_matrix.json`](docs/support/capability_matrix.json);
the reader-facing version is
[`docs/support/capability_matrix.md`](docs/support/capability_matrix.md).

## What Works Now

### Compiler

- Canonical `.objc3` module parsing.
- Typed semantic flow for current scalar, function, expression, and control-flow
  surfaces.
- Module-level `fn`, `pure fn`, `let`, and `extern fn` declarations.
- Typed Objective-C interface, implementation, method, property, category, and
  protocol declarations.
- Structured parser/sema diagnostics with recovery and fix-it records where the
  language surface defines them.
- Strict runtime-dispatch lowering.
- LLVM IR module emission and native object emission.
- Semantic-preserving optimization pipeline governance.

### Runtime

- Strict dispatch error behavior.
- Class realization, method tables, categories, protocols, properties, ivars,
  selector metadata, and registration replay.
- Public reflection APIs for supported class, protocol, property, selector, and
  storage metadata.
- Runtime debug trace payloads for structured inspection, async task lanes, and
  error-unwind lanes.
- Ownership hooks, block copy/dispose/invoke records, byref forwarding, error
  bridge cleanup, task continuation lifecycle, and actor mailbox isolation.

### Language Features

- Protocol-qualified existential value flow.
- Existential witness model evidence.
- Generic protocol-qualified arguments.
- Callable type parameters.
- Variance and specialization evidence.
- Ownership and memory-model contracts.
- Block escape and capture legality.
- Property behavior semantics.
- Derive inventory and macro provenance contracts.
- Deterministic macro safety and host-cache boundaries.

### Standard Library

Checked modules live under `stdlib/modules/`.

- `objc3.text`: runtime-backed UTF-8 record shape, byte span shape, source string
  literal text-handle lowering, byte/unit counts, concatenation, prefix helpers,
  and fail-closed status reporting.
- `objc3.collections`: concrete i32 arrays, slices, aggregation, mutable map
  insert/update, set iteration, and deterministic iterator guards.
- `objc3.concurrency`: task spawn, task-group cancellation, executor hop, and
  actor mailbox helper surfaces.

### Modules And Interop

- Public import lookup.
- Visibility, reexport, and rebuild contracts.
- Dependency-graph diagnostics.
- Stale cache input rejection.
- Missing module, import cycle, duplicate export, hidden declaration, and ABI
  mismatch diagnostics.
- Runtime import/package metadata, mixed-image replay, C header import/export
  metadata, Swift/C++ annotation metadata, and Objective-C 2 adjacent metadata
  preservation where the checked interop rows define it.

### Tooling And Product Workflow

- One public command bridge: `npm run objc3c -- <action>`.
- Formatter/LSP/workspace/artifact-inspector payloads.
- First-run product path checks.
- Stable public conformance suite manifest and replay package.
- Local package manager model, registry mirror, package lock, install receipt,
  update receipt, and clean distribution checks.
- ABI governance, release operation policy, release channel lifecycle, and
  platform/toolchain matrix checks.

### Platform

`windows-x64` is the supported host row. Linux, macOS, sanitizer variants, and
broader toolchain ranges are represented explicitly as rejected, reserved, or
internal rows until their own evidence exists.

## Quick Start

This repository is easiest to use on Windows with PowerShell 7.

Install prerequisites:

- PowerShell 7 (`pwsh`)
- Node.js and `npm`
- Python 3 with `pip`
- LLVM at `C:\Program Files\LLVM` or referenced by `LLVM_ROOT`

LLVM tools used by the native path:

- `clang++.exe`
- `llvm-lib.exe`
- `libclang.lib` or `clang.lib`
- LLVM headers under `include/`

The compile/link/run path also uses `llc.exe`.

From a fresh clone:

```powershell
git clone https://github.com/doublemover/Slopjective-C.git
cd .\Slopjective-C
npm ci
python -m pip install --upgrade pytest jsonschema
```

If LLVM is installed somewhere else:

```powershell
$env:LLVM_ROOT = 'D:\path\to\LLVM'
```

Build the compiler and runtime archive:

```powershell
npm run objc3c -- build-native-binaries
```

Compile a checked-in program:

```powershell
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/readme-hello --emit-prefix module
```

Run a bounded smoke path:

```powershell
npm run objc3c -- test-smoke
```

Run native execution smoke when `llc.exe` is available:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Program Files\LLVM\bin\llc.exe'
npm run objc3c -- test-execution-smoke
```

## Showcase

Read and compile the checked examples when you want the fastest feel for the
language:

- `showcase/auroraBoard/main.objc3`: modules, protocols, categories,
  properties, reflection-shaped metadata, and object-model declarations.
- `showcase/signalMesh/main.objc3`: typed status flow, executor annotations,
  async-shaped declarations, runtime messaging, and Objective-C message syntax.
- `showcase/patchKit/main.objc3`: derive annotations, macro provenance
  annotations, property-behavior syntax, and interop-shaped declarations.

Compile one:

```powershell
npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3 --out-dir tmp/readme-auroraBoard --emit-prefix module
```

Validate the showcase portfolio:

```powershell
npm run objc3c -- check-showcase-surface
npm run objc3c -- validate-showcase
```

Package-aware sample libraries and apps live under
`showcase/applicationFrameworkSamples/`:

- object runtime sample library,
- interop adapter sample library,
- stdlib text and collections CLI sample,
- async runtime application sample.

Validate them with:

```powershell
npm run objc3c -- validate-application-framework-samples
```

## Public Command Surface

Use this shape for normal work:

```powershell
npm run objc3c -- <action>
```

Common actions:

- build: `build-native-binaries`, `build-native-contracts`, `build-native-full`
- compile: `compile-objc3c`
- examples: `check-showcase-surface`, `validate-showcase`
- docs: `build-site`, `build-native-docs`, `build-public-command-surface`
- validation: `test-smoke`, `test-ci`, `test-runtime-acceptance-fast`
- conformance: `validate-conformance-corpus`, `check-conformance-minima`
- stdlib: `validate-stdlib-foundation`
- modules: `validate-module-interop-contracts`
- interop: `validate-interop-conformance`, `validate-runnable-interop`
- packages: `validate-package-manager-model`, `validate-package-mirror`,
  `validate-package-install-distribution`
- release: `validate-abi-governance`, `validate-release-operations`

The synchronized command reference is
[`docs/runbooks/objc3c_public_command_surface.md`](docs/runbooks/objc3c_public_command_surface.md).

## Exact Support Boundaries

The README is an orientation page. Exact claims come from the capability matrix,
evidence map, fixtures, schemas, native probes, and public replay commands.

Current explicit boundaries:

- Objective-C 2 source syntax is not accepted as Objective-C 3 source.
- Full Swift and C++ ABI import are reserved.
- Hosted registry and network dependency resolution are not part of the package
  manager row.
- Full IDE behavior, rename, semantic tokens, generalized code actions, and
  statement stepping are outside the checked tooling row.
- Unicode scalar iteration, normalization, formatting, interpolation,
  source-level text mutation, and Foundation text bridging are outside the
  current text row.
- Collection literals, dictionary literals, set literals, generic collection
  ABI, arbitrary-length owned array storage, syntax-level for-in integration,
  non-i32 hashing, map iteration, and set deletion are outside the current
  collections row.
- Linux, macOS, sanitizer variants, and broad LLVM version ranges need their own
  platform evidence before they become supported host rows.

## Repository Map

- `native/objc3c/`: compiler, parser, sema, lowering, IR, runtime, C API, and
  driver implementation.
- `stdlib/`: checked standard-library workspace and module contracts.
- `showcase/`: example portfolio and application-framework samples.
- `docs/tutorials/`: setup, walkthrough, comparison, and sample guides.
- `docs/support/`: capability matrix, evidence map, schema examples, and claim
  ownership.
- `docs/runbooks/`: operator and maintainer workflows.
- `docs/objc3c-native/src/`: native compiler/runtime architecture source.
- `schemas/`: artifact, report, package, runtime, and release schema contracts.
- `scripts/`: build, validation, packaging, report, and workflow tooling.
- `tests/`: native behavior, conformance, runtime, stress, and tooling coverage.
- `site/`: public overview source and generated page.
- `tmp/`, `artifacts/`: machine-owned build, package, compiler, and replay
  outputs.

## Reading Path

1. Compile `showcase/signalMesh/main.objc3`.
2. Read [`docs/tutorials/getting_started.md`](docs/tutorials/getting_started.md).
3. Read [`showcase/README.md`](showcase/README.md).
4. Check exact rows in
   [`docs/support/capability_matrix.md`](docs/support/capability_matrix.md).
5. Open [`docs/objc3c-native.md`](docs/objc3c-native.md) for compiler/runtime
   architecture.

## Development Notes

- Keep user-facing workflow commands on `npm run objc3c -- <action>`.
- Treat `native/objc3c/` as the compiler/runtime implementation root.
- Treat `stdlib/` as the checked standard-library root.
- Treat `showcase/` as the checked example source root.
- Treat `tmp/` and `artifacts/` as machine-owned output roots.
- Update generated checked-in docs through their owner actions.

Useful validation commands:

```powershell
npm run objc3c -- test-smoke
npm run objc3c -- check-task-hygiene
npm run objc3c -- check-markdown
```

## License

No repository-wide license file is currently present.
