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

## Start Here

Use the shortest path that matches what you are trying to do.

| If you want to... | Start here | Then do this |
| --- | --- | --- |
| understand the project | [published site](https://doublemover.github.io/Slopjective-C/) | read the status table and the spec map first |
| build or evaluate the toolchain | [README.md](README.md) | follow `Fresh Setup`, then `First Working Session` |
| inspect implementation boundaries | [docs/objc3c-native.md](docs/objc3c-native.md) | then open `native/objc3c/` |
| run exact package-script workflows | [docs/runbooks/objc3c_public_command_surface.md](docs/runbooks/objc3c_public_command_surface.md) | use the mapped `npm run ...` entrypoint instead of guessing |
| follow old spec links or archived anchors | [legacy spec redirects](docs/reference/legacy_spec_anchor_index.md#legacy-files) | use this only for compatibility lookups |

Documentation boundary:

- human-facing onboarding:
  - `README.md`
  - `site/index.md`
- implementation-facing narrative:
  - `docs/objc3c-native.md`
  - `docs/objc3c-native/src/*.md`
- generated operator appendix:
  - `docs/runbooks/objc3c_public_command_surface.md`
- compatibility-only redirect layer:
  - `docs/reference/legacy_spec_anchor_index.md`
- machine-owned outputs, not onboarding:
  - `tmp/`
  - `artifacts/`
  - `reports/`

If you are new to the repo, stay out of `tmp/` and the legacy redirect index until you actually need them.

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

## First Working Session

If the goal is simply to prove the repo is alive, use this order:

1. Build the native compiler:

```powershell
npm run build:objc3c-native
```

2. Build the public site overview:

```powershell
npm run build:spec
```

3. Compile the canonical hello fixture:

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/readme-hello --emit-prefix module
```

4. Run the bounded default validation:

```powershell
npm run test:fast
```

5. If you need execution smoke, set `llc.exe` only when it is not already on `PATH`:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Program Files\LLVM\bin\llc.exe'
npm run test:objc3c:execution-smoke
```

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

Use package scripts for normal work. The common ones are:

- `npm run build:objc3c-native`
- `npm run build:spec`
- `npm run compile:objc3c -- ...`
- `npm run test:fast`
- `npm run test:smoke`
- `npm run test:ci`
- `npm run package:objc3c-native:runnable-toolchain`

Rules:

- prefer the public `npm run ...` surface over direct Python or PowerShell when a wrapper already exists,
- treat `native/objc3c/` as the only supported compiler implementation root,
- use `docs/runbooks/objc3c_public_command_surface.md` for the synchronized command/action/backend reference,
- use `docs/runbooks/objc3c_maintainer_workflows.md` for maintainer-only workflow maps.

## Other Useful Commands

Rebuild the generated site output with the standard package script:

```powershell
npm run build:spec
```

## Spec Structure

The spec is organized as a small set of cross-cutting reference documents plus the numbered language parts.

Start here:

- [Published site overview](https://doublemover.github.io/Slopjective-C/)
- [Introduction](https://doublemover.github.io/Slopjective-C/#intro)
- [Specification map](https://doublemover.github.io/Slopjective-C/#toc-front-matter)
- [Module metadata and ABI surface tables](https://doublemover.github.io/Slopjective-C/#d)
- [Conformance profile checklist](https://doublemover.github.io/Slopjective-C/#e)
- [Legacy spec redirects](docs/reference/legacy_spec_anchor_index.md#legacy-files) for archived anchors and compatibility links only

Language parts:

| Part | Focus |
| --- | --- |
| [Part 0](https://doublemover.github.io/Slopjective-C/#part-0) | Baseline rules, terminology, and normative references |
| [Part 1](https://doublemover.github.io/Slopjective-C/#part-1) | Versioning, compatibility, and conformance claims |
| [Part 2](https://doublemover.github.io/Slopjective-C/#part-2) | Modules, namespacing, and public API surfaces |
| [Part 3](https://doublemover.github.io/Slopjective-C/#part-3) | Types, nullability, optionals, generics, and key paths |
| [Part 4](https://doublemover.github.io/Slopjective-C/#part-4) | Ownership, retainable objects, and lifetime rules |
| [Part 5](https://doublemover.github.io/Slopjective-C/#part-5) | Control flow, safety constructs, and execution behavior |
| [Part 6](https://doublemover.github.io/Slopjective-C/#part-6) | Errors, `Result`, and `throws` |
| [Part 7](https://doublemover.github.io/Slopjective-C/#part-7) | Async/await, actors, and concurrency rules |
| [Part 8](https://doublemover.github.io/Slopjective-C/#part-8) | System programming extensions and low-level features |
| [Part 9](https://doublemover.github.io/Slopjective-C/#part-9) | Performance controls and runtime dynamism boundaries |
| [Part 10](https://doublemover.github.io/Slopjective-C/#part-10) | Metaprogramming, derives, macros, and property behaviors |
| [Part 11](https://doublemover.github.io/Slopjective-C/#part-11) | Interoperability with C, C++, and Swift-facing surfaces |
| [Part 12](https://doublemover.github.io/Slopjective-C/#part-12) | Diagnostics, tooling, and conformance testing |

## License

No repository-wide license file is currently present. Add one before treating this as a redistributable public package.
