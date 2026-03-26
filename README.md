# Slopjective-C

Slopjective-C is an Objective-C 3.0 draft specification and native compiler project.

This repository contains two things that matter:

- a working draft of the Objective-C 3.0 language and runtime model,
- an in-tree native implementation under `native/objc3c/`.

## Current Status

The project is no longer just a grammar sketch.

Today it has:

- a real native `objc3c` compiler,
- real LLVM IR and native object emission,
- a runnable subset of `.objc3`,
- emitted runtime metadata for the object-model surface,
- ongoing work to finish full runtime realization of classes, protocols, categories, properties, blocks, and ARC.

What is still incomplete:

- full live Objective-C 3.0 object-model runtime behavior,
- full property/ivar/runtime reflection closure,
- full blocks and ARC automation,
- advanced error, concurrency, metaprogramming, and broader interop features.

## Runnable Subset

The current runnable subset is real, but it is still a subset.

What you can compile and run today:

- `module` declarations,
- global and local `let` bindings,
- `fn`, `pure fn`, and external function declarations,
- integer and boolean values,
- baseline alias surfaces such as `BOOL`, `NSInteger`, and `NSUInteger`,
- Objective-C-flavored signature aliases such as `id`, `Class`, `SEL`, `Protocol`, and `instancetype`,
- control flow:
  - `if` / `else`
  - `while`
  - `do while`
  - `for`
  - `switch`
  - `break`
  - `continue`
  - `return`
- arithmetic, comparison, logical, bitwise, shift, ternary, and grouping expressions,
- bracket message-send syntax through the current runtime dispatch path,
- deterministic selector/string pool and metadata-bearing object emission,
- ownership-baseline runtime behavior for supported retainable object storage.

What is not fully runnable yet:

- full class/protocol/category/property runtime realization,
- full property synthesis and reflective runtime consumption,
- full escaping block/byref runtime behavior,
- full ARC automation,
- advanced language features such as `throws`, async/await, tasks, actors, macros, and broader interop closure.

## Where to Start

- Public overview: [https://doublemover.github.io/Slopjective-C/](https://doublemover.github.io/Slopjective-C/)
- Draft overview section: [Introduction](https://doublemover.github.io/Slopjective-C/#intro)
- Current implementation / module-boundary status: [Module Metadata and ABI Surface](https://doublemover.github.io/Slopjective-C/#d)
- Compiler implementation: [`native/objc3c/`](native/objc3c/)
- Detailed spec source: [`spec/`](spec/)

If you only want the high-level picture, read the published site first. If you want the exact language rules, use the files under `spec/`. If you want the implementation, start in `native/objc3c/`.

## Repository Layout

- `spec/`
  - the detailed draft specification
  - language parts, ABI notes, compatibility rules, conformance material, and planning packets
- `native/objc3c/`
  - the native compiler, runtime-facing lowering, and toolchain integration work
- `site/`
  - the generated public overview page
- `scripts/`
  - build, validation, and documentation tooling
- `tests/`
  - tooling, conformance, and runtime-oriented test coverage

## Dependencies

This repository is currently easiest to use on Windows with PowerShell 7.

Minimum practical setup:

- PowerShell 7 (`pwsh`)
- Node.js and `npm`
- Python 3 with `pip`
- LLVM installed either at `C:\Program Files\LLVM` or pointed to by `LLVM_ROOT`

What the native build script actually expects from LLVM:

- `clang++.exe`
- `llvm-lib.exe`
- `libclang.lib` or `clang.lib`
- LLVM headers under `include/`

What the fuller native compile-and-run path also expects:

- `llc.exe`

Useful optional LLVM tools for inspection and debugging:

- `llvm-readobj.exe`
- `llvm-objdump.exe`

Python packages:

- there is no repo-wide `requirements.txt` at the moment
- for normal local validation, install at least:
  - `pytest`
  - `jsonschema`

Notes:

- `npm` scripts in this repo run through PowerShell 7 because `.npmrc` sets `script-shell=pwsh`.
- If LLVM is not installed in `C:\Program Files\LLVM`, set `LLVM_ROOT` before building.
- The build script does not install LLVM for you.

## Fresh Setup

From a fresh clone on this machine:

```powershell
git clone https://github.com/doublemover/Slopjective-C.git
cd .\Slopjective-C
npm ci
python -m pip install --upgrade pytest jsonschema
```

If LLVM is installed somewhere other than `C:\Program Files\LLVM`, set:

```powershell
$env:LLVM_ROOT = 'D:\path\to\LLVM'
```

Build the native compiler and runtime archive:

```powershell
npm run build:objc3c-native
```

Expected artifacts:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

## Native Build Surface Modernization

Current truthful state:

- `npm run build:objc3c-native` is now a PowerShell wrapper over a persistent
  CMake/Ninja build tree under `tmp/build-objc3c-native`.
- That command reuses the configured build tree across invocations, publishes
  the native binaries and runtime archive into `artifacts/`, and then
  regenerates the current frontend packet family.

Frozen future command surface (`M276-A001`):

- `npm run build:objc3c-native`
  - eventual fast local binary-build default after parity proof lands
- `npm run build:objc3c-native:contracts`
  - reserved contracts and packet-generation path
- `npm run build:objc3c-native:full`
  - reserved full-build path for closeout and CI
- `npm run build:objc3c-native:reconfigure`
  - reserved fingerprint refresh / self-healing configure path

Planned policy boundary:

- local issue work should eventually use the fast default path
- packet generation should be callable independently where dependency shape
  permits it
- milestone closeout and CI should retain a truthful full-build path
- persistent local build trees will live under `tmp/`
- published binaries and libraries will remain under `artifacts/`

The command split is fully implemented now.

Parity freeze (`M276-A002`):

- the current PowerShell build and the current `CMakeLists.txt` already cover
  the same in-tree native source set
- the remaining parity gaps are tool discovery, `libclang` linkage,
  compile-definition shape, compile-database policy, and final `artifacts/`
  publication behavior
- the frozen compile-database target path is
  `tmp/build-objc3c-native/compile_commands.json`
- the authoritative toolchain root remains `LLVM_ROOT`, with the wrapper
  responsible for carrying that into any future configure step

`M276-C001` is now the first landed implementation step after that parity freeze:

- native binary builds route through CMake/Ninja
- `tmp/build-objc3c-native/compile_commands.json` is emitted on configure
- warm invocations reuse the persistent build tree
- canonical binaries still publish to `artifacts/bin` and `artifacts/lib`

`M276-C003` now decomposes the generated frontend packet family by dependency
shape inside that wrapper:

- `source-derived`
  - `frontend_modular_scaffold.json`
- `binary-derived`
  - `frontend_invocation_lock.json`
  - `frontend_core_feature_expansion.json`
- `closeout-derived`
  - `frontend_edge_compat.json`
  - `frontend_edge_robustness.json`
  - `frontend_diagnostics_hardening.json`
  - `frontend_recovery_determinism_hardening.json`
  - `frontend_conformance_matrix.json`
  - `frontend_conformance_corpus.json`
  - `frontend_integration_closeout.json`

Current truthful boundary after `M276-C002`:

- `npm run build:objc3c-native`
  - fast public binary-build path
  - uses `binaries-only`
- `npm run build:objc3c-native:contracts`
  - public source-derived plus binary-derived packet path
  - uses `packets-binary`
- `npm run build:objc3c-native:full`
  - public full binary-plus-packet path
  - uses `full`
- `npm run build:objc3c-native:reconfigure`
  - public forced-configure binary-build path
  - uses `binaries-only` plus `-ForceReconfigure`

`M276-D002` now adds the shared native build helper:

- `scripts/ensure_objc3c_native_build.py`
  - `--mode fast`
  - `--mode contracts`
  - `--mode full`
  - `--force-reconfigure`
- active readiness runners can now ask for a usable native build without
  hard-coding raw npm build semantics
- forced reconfigure now routes through the wrapper without deleting the
  persistent build tree

`M276-D001` now migrates the active `M262`/`M263` issue-work range onto the
shared helper.

- active lane `A` through `D` readiness runners now acquire binaries with:
  - `python scripts/ensure_objc3c_native_build.py --mode fast`
- those runners now emit deterministic helper summaries under:
  - `tmp/reports/<milestone>/<issue>/ensure_objc3c_native_build_summary.json`
- lane `E` aggregators remain orchestration layers over the migrated lane
  runners instead of reacquiring native builds themselves
- the truthful boundary after `M276-D001` is:
  - fast helper mode for active issue work
  - full builds reserved for closeout, CI, and deliberately cross-cutting proof
    paths

`M276-D003` preserves compatibility for older raw-build callers during the
transition.

- historical raw-build callers remain supported through the public default build
  command:
  - `npm run build:objc3c-native`
- that compatibility path is truthful but narrow:
  - canonical native binaries are refreshed at `artifacts/`
  - packet generation is not implied
  - downstream historical checkers remain responsible for their own proof work
- representative coexistence proof now uses:
  - active helper runner: `scripts/run_m262_a001_lane_a_readiness.py`
  - historical raw-build runner: `scripts/run_m257_a001_lane_a_readiness.py`

## When to use each command

| Command | Use when | Result |
| --- | --- | --- |
| `npm run build:objc3c-native` | ordinary local native compiler work | refreshes native binaries through the persistent CMake/Ninja tree |
| `npm run build:objc3c-native:contracts` | packet/checker work that needs the public contract packet surface | refreshes the source-derived plus binary-derived packet surface |
| `npm run build:objc3c-native:full` | milestone closeout or deliberately broad validation | refreshes native binaries and the full packet family |
| `npm run build:objc3c-native:reconfigure` | toolchain drift, path drift, or stale fingerprint mismatch | forces a fresh configure against `tmp/build-objc3c-native`, then rebuilds binaries |

Build-tree facts:

- persistent build tree:
  - `tmp/build-objc3c-native`
- compile database:
  - `tmp/build-objc3c-native/compile_commands.json`
- fingerprint:
  - `tmp/build-objc3c-native/native_build_backend_fingerprint.json`

Fingerprint inputs:

- `generator`
- `cmake`
- `ninja`
- `clangxx`
- `llvm_root`
- `llvm_include_dir`
- `libclang`
- `build_dir`
- `source_dir`
- `runtime_output_dir`
- `library_output_dir`
- direct-object-emission and warning-parity flags

Supported stale-tree recovery:

- use `npm run build:objc3c-native:reconfigure`
- do not delete the build tree

CI and closeout semantics:

- local issue work benefits from the persistent tree under `tmp/build-objc3c-native`
- CI runners start from clean workspaces, so `fast`, `contracts`, and `full`
  describe proof scope rather than cache expectations
- the active Windows core workflow proves `build:objc3c-native` plus
  `build:objc3c-native:contracts`
- the manual compiler closeout workflow proves `build:objc3c-native:full`
- milestone closeout evidence for this build-surface tranche lives under
  `tmp/reports/m276/M276-E002/`

## Public Command Surface

Use these package scripts for normal operator workflows:

- `npm run build`
- `npm run build:objc3c-native`
- `npm run build:objc3c-native:contracts`
- `npm run build:objc3c-native:full`
- `npm run build:objc3c-native:reconfigure`
- `npm run build:spec`
- `npm run compile:objc3c -- ...`
- `npm run lint:spec`
- `npm run test`
- `npm run test:smoke`
- `npm run test:ci`
- `npm run test:objc3c`
- `npm run test:objc3c:execution-smoke`
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:full`
- `npm run package:objc3c-native:runnable-toolchain`
- `npm run proof:objc3c`

All other package scripts are compatibility or internal surfaces retained during cleanup and may change without notice.
Prefer the public package-script surface over direct Python or PowerShell invocations when a public wrapper already exists.
`native/objc3c/` is the only supported compiler implementation root. The earlier
prototype Python compiler surface has been retired into the governance archive
and is not an operator path.
The documented public package commands are thin wrappers over
`scripts/objc3c_public_workflow_runner.py`.

## Quickstart

Build the public site overview:

```powershell
npm run build:spec
```

Check that the generated site is up to date:

```powershell
python scripts/build_site_index.py --check
```

Build the native compiler:

```powershell
npm run build:objc3c-native
```

Compile the basic native hello-world fixture:

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/readme-hello --emit-prefix module
```

Run the native execution smoke suite:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Program Files\LLVM\bin\llc.exe'
npm run test:objc3c:execution-smoke
```

If `llc.exe` is already on `PATH`, you can omit `OBJC3C_NATIVE_EXECUTION_LLC_PATH`.

## Other Useful Commands

Rebuild the generated site output with the standard package script:

```powershell
npm run build:spec
```

## Spec Structure

The spec is organized as a small set of cross-cutting reference documents plus the numbered language parts.

Start here:

- [Spec root](spec/)
- [Table of contents](spec/TABLE_OF_CONTENTS.md)
- [Introduction](spec/INTRODUCTION.md)
- [Syntax catalog](spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md)
- [Lowering and runtime contracts](spec/LOWERING_AND_RUNTIME_CONTRACTS.md)
- [Module metadata and ABI tables](spec/MODULE_METADATA_AND_ABI_TABLES.md)
- [Conformance profile checklist](spec/CONFORMANCE_PROFILE_CHECKLIST.md)

Language parts:

| Part | Focus |
| --- | --- |
| [Part 0](spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md) | Baseline rules, terminology, and normative references |
| [Part 1](spec/PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md) | Versioning, compatibility, and conformance claims |
| [Part 2](spec/PART_2_MODULES_NAMESPACING_API_SURFACES.md) | Modules, namespacing, and public API surfaces |
| [Part 3](spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md) | Types, nullability, optionals, generics, and key paths |
| [Part 4](spec/PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md) | Ownership, retainable objects, and lifetime rules |
| [Part 5](spec/PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md) | Control flow, safety constructs, and execution behavior |
| [Part 6](spec/PART_6_ERRORS_RESULTS_THROWS.md) | Errors, `Result`, and `throws` |
| [Part 7](spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md) | Async/await, actors, and concurrency rules |
| [Part 8](spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md) | System programming extensions and low-level features |
| [Part 9](spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md) | Performance controls and runtime dynamism boundaries |
| [Part 10](spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md) | Metaprogramming, derives, macros, and property behaviors |
| [Part 11](spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md) | Interoperability with C, C++, and Swift-facing surfaces |
| [Part 12](spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md) | Diagnostics, tooling, and conformance testing |

## License

No repository-wide license file is currently present. Add one before treating this as a redistributable public package.
