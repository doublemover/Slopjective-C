# Slopjective-C

Slopjective-C is an Objective-C 3.0 draft specification, native compiler/runtime,
and validated toolchain project.

This repository now has four live surfaces that matter:

- a working draft of the Objective-C 3.0 language and runtime model,
- an in-tree native implementation under `native/objc3c/`,
- a checked-in standard library, tutorial, and showcase surface under
  `stdlib/`, `docs/tutorials/`, and `showcase/`,
- a large validation, packaging, release, and distribution workflow surface
  rooted in `scripts/`, `tests/`, and `docs/runbooks/`.

## Current Status

The project is well past a grammar sketch.

Today it has:

- a real native `objc3c` compiler,
- real LLVM IR and native object emission,
- a native runtime archive and emitted runtime metadata for the object-model
  surface,
- a runnable subset of `.objc3`,
- a checked-in standard library workspace with canonical modules under
  `stdlib/modules/`,
- checked-in tutorials, canonical conversion guides, and capability-backed
  showcase
  examples,
- integrated workflow surfaces for performance, conformance, stress,
  external-validation, packaging, release operations, and distribution
  credibility,
- support boundaries routed through the schema-backed capability matrix,
  evidence map, and capability-claim responsibility contract.

Current public support states:

| Area | Capability boundary | Public support wording |
| --- | --- | --- |
| Parser syntax, typed semantic flow, strict runtime-dispatch lowering, IR module emission, strict dispatch diagnostics, and runnable smoke | `implemented` where rows exist | Supported only at the exact row/evidence scope in `docs/support/capability_matrix.md`. |
| Native module decomposition, public C runtime result APIs, workflow bridge, and JSON/schema helpers | `internal` | Owner and evidence surfaces, not language support claims. |
| Full object-model runtime realization and property/ivar/reflection closure | `reserved` | Unclaimed until implemented matrix rows link executable evidence. |
| Blocks, full ARC automation, `throws`, async/task/actor runtime closure, metaprogramming/property behavior runtime closure, and broader interop | `reserved` unless narrower rows say otherwise | Unavailable as public support outside exact implemented rows. |
| Retired adapters, alternate acceptance paths, retired-source lanes, retired command surfaces, and generated-output completion | retired wording, not a support state | Rejection, absence, or source-hygiene inventory only. |

## Runnable Subset

The current runnable subset is real, but it is still a subset. The capability
matrix is the support boundary; prose in this README is only a routing summary.

What you can compile and run today:

- `module` declarations,
- global and local `let` bindings,
- `fn`, `pure fn`, and external function declarations,
- integer and boolean values,
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

The repo also now carries dedicated conformance and runnable-package workflows
for block/ARC, concurrency, error handling, interop, metaprogramming,
object-model, storage/reflection, and release-candidate validation. That means
those feature families have owner/evidence surfaces. It does not mean every
corresponding language/runtime behavior is implemented.

Reserved or unclaimed runtime-backed areas:

- class/protocol/category/property runtime realization outside implemented
  matrix rows,
- property synthesis and reflective runtime consumption outside implemented
  matrix rows,
- escaping block/byref runtime behavior,
- full ARC automation,
- runtime-backed closure for `throws`, async/await, tasks, actors, macros,
  property behaviors, and broader interop.

## Start Here

Use the shortest path that matches what you are trying to do.

| If you want to...                                               | Start here                                                                                       | Then do this                                                                                                    |
| --------------------------------------------------------------- | ------------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------- |
| understand the project                                          | [published site](https://doublemover.github.io/Slopjective-C/)                                   | read the status table and the spec map first                                                                    |
| build or evaluate the toolchain                                 | [README.md](README.md)                                                                           | follow `Fresh Setup`, then `First Working Session`                                                              |
| inspect the checked-in stdlib surface                           | [stdlib/README.md](stdlib/README.md)                                                             | then follow the core, advanced, and program runbooks                                                            |
| follow the runnable tutorial path                               | [docs/tutorials/getting_started.md](docs/tutorials/getting_started.md)                           | compile one example first, then use the showcase surface                                                        |
| pick the right capability-backed example first                  | [showcase/README.md](showcase/README.md)                                                         | choose `auroraBoard`, `signalMesh`, or `patchKit` before reading deeper comparison text                         |
| map ObjC2 patterns into canonical ObjC3 examples or check Swift-facing expectations | [docs/tutorials/objc2_to_objc3_migration.md](docs/tutorials/objc2_to_objc3_migration.md) | then use the broader comparison boundary only where you need it                                                |
| compare ObjC3 against ObjC2, Swift, or C++ expectations         | [docs/tutorials/objc2_swift_cpp_comparison.md](docs/tutorials/objc2_swift_cpp_comparison.md)     | then follow the showcase examples that back the comparison                                                      |
| evaluate adoption and support claims                            | [docs/runbooks/objc3c_adoption_legibility.md](docs/runbooks/objc3c_adoption_legibility.md)       | replay `npm run objc3c -- validate-adoption-legibility` and inspect the generated-output summary              |
| inspect performance surfaces                                    | [docs/runbooks/objc3c_runtime_performance.md](docs/runbooks/objc3c_runtime_performance.md)       | then use the performance and compiler-throughput commands                                                       |
| inspect conformance, fuzz, and reporting work                   | [docs/runbooks/objc3c_conformance_corpus.md](docs/runbooks/objc3c_conformance_corpus.md)         | then use the stress, external-validation, and public-conformance workflows                                      |
| inspect package, installer, and release flows                   | [docs/runbooks/objc3c_release_foundation.md](docs/runbooks/objc3c_release_foundation.md)         | then follow packaging channels, release operations, and distribution credibility                                |
| contribute a normal repo change                                 | [CONTRIBUTING.md](CONTRIBUTING.md)                                                               | stay inside the superclean boundary and use `npm run objc3c -- <action>`                                        |
| inspect runnable showcase examples                              | [showcase/README.md](showcase/README.md)                                                         | compile them through `npm run objc3c -- compile-objc3c ...` or the showcase surface check                       |
| inspect implementation boundaries                               | [docs/objc3c-native.md](docs/objc3c-native.md)                                                   | then open `native/objc3c/`                                                                                      |
| run exact public workflow actions                               | [docs/runbooks/objc3c_public_command_surface.md](docs/runbooks/objc3c_public_command_surface.md) | use `npm run objc3c -- <action>` instead of guessing                                                            |
| evaluate support status and evidence                            | [capability matrix](docs/support/capability_matrix.md)                                           | verify the linked executable evidence and [claim responsibility](docs/support/capability_claim_responsibility.md) before relying on a support claim |

Documentation boundary:

- onboarding: `README.md`, `CONTRIBUTING.md`, `site/index.md`
- tutorials and canonical conversion guides: `docs/tutorials/`
- checked-in stdlib product surface: `stdlib/`
- runnable examples: `showcase/`
- implementation narrative: `docs/objc3c-native.md`, `docs/objc3c-native/src/*.md`
- operator runbooks: `docs/runbooks/`
- support boundaries: `docs/support/capability_matrix.md`, `docs/support/evidence_map.md`, `docs/support/capability_claim_responsibility.md`
- hard-cutover capability boundaries: `docs/support/hard_cutover_capability_truth.md`
- machine-owned outputs, not onboarding: `tmp/`, `artifacts/`, `reports/`

If you are new to the repo, stay out of `tmp/` and archived redirect material until you actually need them.

## Superclean Boundary

The live repo surface is intentionally narrow.

Canonical roots:

- implementation:
  - `native/objc3c/`
  - `scripts/`
  - `tests/`
  - `stdlib/`
  - `schemas/`
- human-facing docs:
  - `README.md`
  - `CONTRIBUTING.md`
  - `site/src/`
  - `docs/tutorials/`
  - `docs/objc3c-native/src/`
  - `docs/runbooks/`
- user-facing example sources:
  - `showcase/`
- generated checked-in outputs:
  - `site/index.md`
  - `docs/objc3c-native.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
- machine-owned outputs:
  - `tmp/`
  - `artifacts/`

Explicit non-goals for cleanup work:

- reintroducing milestone-coded command names or sidecar legacy files,
- documenting retired Objective-C aliases as supported public behavior,
- describing old source modes, registry facades, adapter layers, or direct helper
  commands as supported public paths,
- treating `tmp/`, `artifacts/`, or archived redirect material as onboarding surfaces,
- changing generated checked-in outputs without updating their owner inputs.

## Repository Layout

- `native/objc3c/`: native compiler, lowering, runtime, and driver implementation
- `stdlib/`: checked-in standard library workspace and package/import contracts
- `showcase/`: runnable example portfolio
- `docs/tutorials/`: learning path and canonical conversion material
- `docs/runbooks/`: operator-facing workflow and validation boundaries
- `schemas/`: checked-in artifact/report schema contracts
- `scripts/`: build, validation, packaging, and publication tooling
- `tests/`: tooling, conformance, runtime, and stress coverage
- `site/`: generated public overview output
- `docs/support/`: capability matrix, evidence map, schema examples, and claim responsibility

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
npm run objc3c -- build-native-binaries
```

Expected artifacts:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

## First Working Session

If the goal is simply to prove the repo is alive, use this order:

1. Build the native compiler:

```powershell
npm run objc3c -- build-native-binaries
```

2. Build the public site overview:

```powershell
npm run objc3c -- build-site
```

Optional generated-doc refreshes:

```powershell
npm run objc3c -- build-native-docs
npm run objc3c -- build-public-command-surface
```

3. Compile the canonical hello fixture:

```powershell
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/hello.objc3 --out-dir <generated-output-dir> --emit-prefix module
```

4. Run the bounded default validation:

```powershell
npm run objc3c -- test-smoke
```

5. If you need execution smoke, set `llc.exe` only when it is not already on `PATH`:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Program Files\LLVM\bin\llc.exe'
npm run objc3c -- test-execution-smoke
```

## Native Build Surface

Use these entrypoints:

- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- build-native-contracts`
- `npm run objc3c -- build-native-full`
- `npm run objc3c -- build-native-reconfigure`

Operational facts:

- native builds run through the public `build-native-*` actions backed by CMake/Ninja
- the persistent build tree lives under `tmp/build-objc3c-native`
- published binaries and libraries live under `artifacts/`
- contract artifacts and summaries live under transient generated-output roots

For the exact backend and artifact contract, use
`docs/runbooks/objc3c_public_command_surface.md` and
`docs/runbooks/objc3c_maintainer_workflows.md`.

## Public Command Surface

Use `npm run objc3c -- <action>` for normal work. The public surface is now
large and purpose-specific, so use the generated appendix for the full action
map.

Common entrypoints by job:

- bootstrap and native compile:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- compile-objc3c ...`
  - `npm run objc3c -- test-default`
  - `npm run objc3c -- test-smoke`
  - `npm run objc3c -- test-ci`
- docs, tutorials, showcase, and stdlib:
  - `npm run objc3c -- build-site`
  - `npm run objc3c -- build-native-docs`
  - `npm run objc3c -- build-public-command-surface`
  - `npm run objc3c -- validate-getting-started`
  - `npm run objc3c -- validate-showcase`
  - `npm run objc3c -- validate-stdlib-foundation`
  - `npm run objc3c -- validate-stdlib-advanced`
  - `npm run objc3c -- validate-stdlib-program`
- performance and diagnostics:
  - `npm run objc3c -- benchmark-runtime-performance`
  - `npm run objc3c -- benchmark-compiler-throughput`
  - `npm run objc3c -- validate-performance-foundation`
  - `npm run objc3c -- validate-performance-governance`
  - `npm run objc3c -- validate-developer-tooling`
- conformance, stress, and public evidence:
  - `npm run objc3c -- validate-conformance-corpus`
  - `npm run objc3c -- validate-stress`
  - `npm run objc3c -- validate-external-validation`
  - `npm run objc3c -- validate-public-conformance-reporting`
- packaging, release, and distribution:
  - `npm run objc3c -- package-runnable-toolchain`
  - `npm run objc3c -- build-package-channels`
  - `npm run objc3c -- validate-release-foundation`
  - `npm run objc3c -- validate-packaging-channels`
  - `npm run objc3c -- validate-release-operations`
  - `npm run objc3c -- validate-distribution-credibility`

Rules:

- prefer `npm run objc3c -- <action>` over invoking implementation helpers for workflow actions,
- use the same public command surface for maintainer-only command-surface upkeep: `build-public-command-contract`, `check-public-command-contract`, and `check-public-command-budget`,
- treat `native/objc3c/` as the only supported compiler implementation root,
- treat `stdlib/` as the canonical checked-in standard-library root instead of inventing parallel helper trees,
- use `docs/runbooks/objc3c_public_command_surface.md` for the synchronized command/action/backend reference,
- use `docs/runbooks/objc3c_maintainer_workflows.md` for maintainer-only workflow maps.

## Spec Structure

The spec is organized as a small set of cross-cutting reference documents plus the numbered language parts.

Start here:

- [Published site overview](https://doublemover.github.io/Slopjective-C/)
- [Introduction](https://doublemover.github.io/Slopjective-C/#intro)
- [Specification map](https://doublemover.github.io/Slopjective-C/#toc-front-matter)
- [Module metadata and ABI surface tables](https://doublemover.github.io/Slopjective-C/#d)
- [Conformance profile checklist](https://doublemover.github.io/Slopjective-C/#e)
- [Capability matrix](docs/support/capability_matrix.md) for support status and executable evidence

Language parts:

| Part                                                            | Focus                                                    |
| --------------------------------------------------------------- | -------------------------------------------------------- |
| [Part 0](https://doublemover.github.io/Slopjective-C/#part-0)   | Baseline rules, terminology, and normative references    |
| [Part 1](https://doublemover.github.io/Slopjective-C/#part-1)   | Versioning, support states, and conformance claims       |
| [Part 2](https://doublemover.github.io/Slopjective-C/#part-2)   | Modules, namespacing, and public API surfaces            |
| [Part 3](https://doublemover.github.io/Slopjective-C/#part-3)   | Types, nullability, optionals, generics, and key paths   |
| [Part 4](https://doublemover.github.io/Slopjective-C/#part-4)   | Ownership, retainable objects, and lifetime rules        |
| [Part 5](https://doublemover.github.io/Slopjective-C/#part-5)   | Control flow, safety constructs, and execution behavior  |
| [Part 6](https://doublemover.github.io/Slopjective-C/#part-6)   | Errors, `Result`, and `throws`                           |
| [Part 7](https://doublemover.github.io/Slopjective-C/#part-7)   | Async/await, actors, and concurrency rules               |
| [Part 8](https://doublemover.github.io/Slopjective-C/#part-8)   | System programming extensions and low-level features     |
| [Part 9](https://doublemover.github.io/Slopjective-C/#part-9)   | Performance controls and runtime dynamism boundaries     |
| [Part 10](https://doublemover.github.io/Slopjective-C/#part-10) | Metaprogramming, derives, macros, and property behaviors |
| [Part 11](https://doublemover.github.io/Slopjective-C/#part-11) | Interoperability with C, C++, and Swift-facing surfaces  |
| [Part 12](https://doublemover.github.io/Slopjective-C/#part-12) | Diagnostics, tooling, and conformance testing            |

## License

No repository-wide license file is currently present. Add one before treating this as a redistributable public package.
