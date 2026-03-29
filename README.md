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
- Detailed legacy spec redirects: [Legacy spec redirects](docs/reference/legacy_spec_anchor_index.md#legacy-files)

If you only want the high-level picture, read the published site first. If you want the exact language rules, use the legacy redirect index under `docs/reference/legacy_spec_anchor_index.md`. If you want the implementation, start in `native/objc3c/`.

## Documentation Audience Map

The repository now has three live documentation audiences. Use the right one first.

| Audience | Start here | Use it for | Stay out of |
| --- | --- | --- | --- |
| First-time reader | [site/index.md](site/index.md) | project overview, implementation status, spec map | `tmp/`, `reports/`, historical spec redirects unless you need compatibility links |
| Builder / evaluator | [README.md](README.md) | setup, build commands, validation entrypoints, repository layout | deep runtime/source fragments until you need implementation detail |
| Implementer / maintainer | [docs/objc3c-native.md](docs/objc3c-native.md) and [`native/objc3c/`](native/objc3c/) | native compiler/runtime boundaries, emitted artifacts, live proof paths | archived milestone closeout material |

Documentation working boundary:

- public onboarding surface:
  - `README.md`
  - `site/index.md`
- implementation-facing docs surface:
  - `docs/objc3c-native.md`
  - `docs/objc3c-native/src/*.md`
- compatibility-only redirect surface:
  - `docs/reference/legacy_spec_anchor_index.md`
- machine-owned or proof-owned surfaces, not user-facing onboarding:
  - `tmp/`
  - `artifacts/`
  - `reports/`

Explicit non-goals for the public onboarding surface:

- do not make new readers navigate archived milestone material,
- do not treat generated proof artifacts as primary documentation,
- do not force users through the legacy redirect index unless they need old links or exact archived anchors.

## Repository Layout

- `docs/reference/legacy_spec_anchor_index.md`
  - compatibility redirects for archived spec, planning, governance, and conformance anchors
  - use this when older docs, reports, scripts, or conformance metadata still cite the retired `spec/` tree
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

## Native Build Surface

Current live build/test surface:

- `npm run build:objc3c-native`
- `npm run build:objc3c-native:contracts`
- `npm run build:objc3c-native:full`
- `npm run build:objc3c-native:reconfigure`
- `npm run test:smoke`
- `npm run test:ci`
- `npm run test:objc3c:full`
- `npm run proof:objc3c`

Behavior:

- native builds run through the CMake/Ninja-backed wrapper
- build trees and transient evidence stay under `tmp/`
- published binaries and libraries stay under `artifacts/`
- milestone-era planning and closeout material is archived under `tmp/archive/`

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
- build-surface evidence and summaries live under `tmp/reports/`

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

No additional package-script compatibility aliases remain supported.
Maintainer-only package scripts are limited to repo hygiene, task hygiene, and boundary checks.
Prefer the public package-script surface over direct Python or PowerShell invocations when a public wrapper already exists.
`native/objc3c/` is the only supported compiler implementation root. The earlier
prototype Python compiler surface has been retired into the governance archive
and is not an operator path.
The documented public package commands are thin wrappers over
`scripts/objc3c_public_workflow_runner.py`.
See `docs/runbooks/objc3c_public_command_surface.md` for the synchronized
command/action/backend reference.
Maintainers should use `docs/runbooks/objc3c_maintainer_workflows.md` for the
canonical operator workflow map.

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

- [Legacy spec redirects](docs/reference/legacy_spec_anchor_index.md#legacy-files)
- [Table of contents](docs/reference/legacy_spec_anchor_index.md#table-of-contents)
- [Introduction](docs/reference/legacy_spec_anchor_index.md#introduction)
- [Syntax catalog](docs/reference/legacy_spec_anchor_index.md#attribute-and-syntax-catalog)
- [Lowering and runtime contracts](docs/reference/legacy_spec_anchor_index.md#lowering-and-runtime-contracts)
- [Module metadata and ABI tables](docs/reference/legacy_spec_anchor_index.md#module-metadata-and-abi-tables)
- [Conformance profile checklist](docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist)

Language parts:

| Part | Focus |
| --- | --- |
| [Part 0](docs/reference/legacy_spec_anchor_index.md#part-0-baseline-and-normative-references) | Baseline rules, terminology, and normative references |
| [Part 1](docs/reference/legacy_spec_anchor_index.md#part-1-versioning-compatibility-conformance) | Versioning, compatibility, and conformance claims |
| [Part 2](docs/reference/legacy_spec_anchor_index.md#part-2-modules-namespacing-api-surfaces) | Modules, namespacing, and public API surfaces |
| [Part 3](docs/reference/legacy_spec_anchor_index.md#part-3-types-nullability-optionals-generics-keypaths) | Types, nullability, optionals, generics, and key paths |
| [Part 4](docs/reference/legacy_spec_anchor_index.md#part-4-memory-management-ownership) | Ownership, retainable objects, and lifetime rules |
| [Part 5](docs/reference/legacy_spec_anchor_index.md#part-5-control-flow-safety-constructs) | Control flow, safety constructs, and execution behavior |
| [Part 6](docs/reference/legacy_spec_anchor_index.md#part-6-errors-results-throws) | Errors, `Result`, and `throws` |
| [Part 7](docs/reference/legacy_spec_anchor_index.md#part-7-concurrency-async-await-actors) | Async/await, actors, and concurrency rules |
| [Part 8](docs/reference/legacy_spec_anchor_index.md#part-8-system-programming-extensions) | System programming extensions and low-level features |
| [Part 9](docs/reference/legacy_spec_anchor_index.md#part-9-performance-and-dynamism-controls) | Performance controls and runtime dynamism boundaries |
| [Part 10](docs/reference/legacy_spec_anchor_index.md#part-10-metaprogramming-derives-macros-property-behaviors) | Metaprogramming, derives, macros, and property behaviors |
| [Part 11](docs/reference/legacy_spec_anchor_index.md#part-11-interoperability-c-cpp-swift) | Interoperability with C, C++, and Swift-facing surfaces |
| [Part 12](docs/reference/legacy_spec_anchor_index.md#part-12-diagnostics-tooling-tests) | Diagnostics, tooling, and conformance testing |

## License

No repository-wide license file is currently present. Add one before treating this as a redistributable public package.
