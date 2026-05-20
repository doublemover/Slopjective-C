# Slopjective-C

Slopjective-C is a native Objective-C 3.0 compiler, runtime, standard-library,
showcase, and validation project. The compiler builds from this repository,
accepts checked-in `.objc3` sources, emits compiler artifacts, and validates the
current language surface through the public `npm run objc3c -- <action>` command
model.

## Start Here

Use this README as the root routing page. It points to the current language
surface, the public command bridge, the support matrix, and the generated
published site without making independent capability claims.

Canonical roots:

- `README.md`: top-level orientation and first-session routing.
- `CONTRIBUTING.md`: contributor boundaries and repo hygiene expectations.
- `docs/tutorials/`: guided setup, migration, and comparison tutorials.
- `showcase/README.md`: checked-in example portfolio.
- `docs/runbooks/objc3c_public_command_surface.md`: synchronized command
  reference.
- `docs/support/capability_matrix.json`: machine-readable support truth.

## Objective-C 3.0 In One File

This is a checked-in showcase program from `showcase/signalMesh/main.objc3`:

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

The current Objective-C 3.0 surface combines Objective-C message syntax with a
module-level compilation model, typed free functions, typed Objective-C method
signatures, semantic diagnostics, runtime dispatch contracts, and emitted
compiler artifacts. Objective-C 2.0 code does not have this module-and-typed-flow
model as a single native compiler surface.

Compile the example after building the native toolchain:

```powershell
npm ci
python -m pip install --upgrade pytest jsonschema
npm run objc3c -- build-native-binaries
npm run objc3c -- compile-objc3c showcase/signalMesh/main.objc3 --out-dir tmp/readme-signalMesh --emit-prefix module
```

The native build publishes:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

The compile step writes the selected output root with compiler-owned artifacts
such as diagnostics, manifest data, LLVM IR, and object-output records.

## What Objective-C 3.0 Does Here

The repository currently supports and validates these Objective-C 3.0 surfaces:

- canonical `.objc3` modules and parser syntax,
- typed semantic flow for current `i32`, `bool`, function, control-flow, and
  expression surfaces,
- typed Objective-C declarations beside module-level `fn`, `pure fn`, `let`, and
  external function declarations,
- strict runtime-dispatch lowering and diagnostics,
- LLVM IR module emission and native object emission through the native compiler,
- runtime metadata and native runtime archive publication,
- runnable smoke programs and checked-in showcase examples,
- a checked-in standard-library workspace under `stdlib/modules/`,
- schema-backed capability and evidence records for public support claims.

Exact public support is defined by
[`docs/support/capability_matrix.md`](docs/support/capability_matrix.md) and the
machine-readable matrix at
[`docs/support/capability_matrix.json`](docs/support/capability_matrix.json).
README prose is a routing guide; the matrix is the support contract.

## Showcase Programs

The fastest way to understand the language surface is to read and compile the
showcase sources:

- `showcase/auroraBoard/main.objc3`: modules, protocols, categories,
  properties, reflection-shaped metadata, and object-model declarations.
- `showcase/signalMesh/main.objc3`: typed status flow, executor annotations,
  async-shaped declarations, runtime messaging, and Objective-C message syntax.
- `showcase/patchKit/main.objc3`: derive annotations, macro provenance
  annotations, property-behavior syntax, and interop-shaped declarations.

Compile one example:

```powershell
npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3 --out-dir tmp/readme-auroraBoard --emit-prefix module
```

Verify the checked-in portfolio:

```powershell
npm run objc3c -- check-showcase-surface
npm run objc3c -- validate-showcase
```

Showcase validation writes machine-owned artifacts under `tmp/artifacts/showcase/`
and reports under `tmp/reports/showcase/`. The checked-in sources and workspace
contracts under `showcase/` remain the example inputs.

## Fresh Setup

This repository is easiest to use on Windows with PowerShell 7.

Minimum setup:

- PowerShell 7 (`pwsh`)
- Node.js and `npm`
- Python 3 with `pip`
- LLVM installed at `C:\Program Files\LLVM` or referenced by `LLVM_ROOT`

LLVM tools used by the native build:

- `clang++.exe`
- `llvm-lib.exe`
- `libclang.lib` or `clang.lib`
- LLVM headers under `include/`

The fuller compile/link/run path also uses `llc.exe`.

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

## First Working Session

Compile a checked-in program:

```powershell
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/readme-hello --emit-prefix module
```

Run the bounded smoke path:

```powershell
npm run objc3c -- test-smoke
```

Run execution smoke when `llc.exe` is available:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Program Files\LLVM\bin\llc.exe'
npm run objc3c -- test-execution-smoke
```

## Current Capability Evidence

The public capability matrix currently records executable evidence for:

| Capability                         | Evidence area                                                         |
| ---------------------------------- | --------------------------------------------------------------------- |
| Canonical parser syntax            | `tests/native/parser/positive/`, parser-core sources                  |
| Typed semantic flow                | `tests/native/sema/types/`, sema pass manager and type relations      |
| Strict runtime-dispatch lowering   | `tests/native/lowering/errors/`, lowering contracts                   |
| IR module emission                 | `tests/native/ir/module/`, IR publication sources                     |
| Runtime dispatch strict diagnostic | `tests/native/runtime/dispatch/`, runtime public dispatch diagnostics |
| Runnable smoke path                | `tests/native/e2e/smoke/`                                             |

The matrix also records internal owner surfaces for the native compiler module
tree, public C runtime result APIs, the workflow bridge, shared JSON/schema
helpers, and hard-cutover capability truth. Those internal rows explain
implementation ownership; public language behavior claims come from implemented
behavior rows.

## Public Command Surface

Use this command shape for normal work:

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
- packaging: `package-runnable-toolchain`

The synchronized command reference is
[`docs/runbooks/objc3c_public_command_surface.md`](docs/runbooks/objc3c_public_command_surface.md).

## Spec Structure

Specification and reader-facing docs are split by role:

- `spec/`: language, ABI, runtime, and metadata contracts.
- `docs/tutorials/`: task-oriented reader paths.
- `docs/runbooks/`: operator and maintainer workflows.
- `docs/support/`: support claims, evidence maps, schema examples, and claim
  responsibility.
- `site/`: generated public overview for the published site.

Support prose in these files must route back to the capability matrix and
evidence map. A spec chapter, runbook, tutorial, generated report, or issue
closeout payload does not promote a reserved or internal row into public
runtime behavior.

## Superclean Boundary

`tmp/` and `artifacts/` are machine-owned output roots. They can contain build
products, reports, packages, compiler artifacts, and replay evidence, but they
are not source-of-truth inputs for capability state. Durable support claims live
in checked-in source, tests, schemas, and the support matrix.

Explicit non-goals for cleanup work:

- Do not convert generated reports into editable support truth.
- Do not treat a passing generated report as issue closure without the matching
  checked-in owner surface and validation evidence.
- Do not revive retired modes, retired adapters, alternate acceptance paths, or
  compatibility labels as public Objective-C 3.0 support.
- Do not replace the `npm run objc3c -- <action>` bridge with direct helper
  commands in public docs.

## Repository Map

- `native/objc3c/`: compiler, parser, sema, lowering, IR, runtime, C API, and
  driver implementation
- `stdlib/`: checked-in standard-library workspace and module contracts
- `showcase/`: current example portfolio
- `docs/tutorials/`: getting-started, guided walkthrough, and comparison guides
- `docs/support/`: capability matrix, evidence map, schema examples, and claim
  responsibility
- `docs/runbooks/`: operator and validation workflows
- `docs/objc3c-native/src/`: implementation narrative source
- `schemas/`: artifact and report schema contracts
- `scripts/`: build, validation, packaging, report, and workflow tooling
- `tests/`: native behavior, conformance, runtime, stress, and tooling coverage
- `site/`: generated public overview
- `tmp/`, `artifacts/`: machine-owned build, report, package, and compiler outputs

## Reading Path

Use this order when you are new to the repository:

1. Compile `showcase/auroraBoard/main.objc3` or
   `showcase/signalMesh/main.objc3`.
2. Read [`docs/tutorials/getting_started.md`](docs/tutorials/getting_started.md).
3. Read [`showcase/README.md`](showcase/README.md) and choose the example story
   that matches your question.
4. Check exact support in
   [`docs/support/capability_matrix.md`](docs/support/capability_matrix.md).
5. Open [`docs/objc3c-native.md`](docs/objc3c-native.md) when you want the
   compiler/runtime architecture.

## Development Notes

- Keep user-facing workflow commands on `npm run objc3c -- <action>`.
- Treat `native/objc3c/` as the compiler/runtime implementation root.
- Treat `stdlib/` as the checked-in standard-library root.
- Treat `showcase/` as the checked-in example source root.
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
