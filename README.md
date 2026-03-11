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
- Compiler implementation: `native/objc3c/`
- Detailed spec source: `spec/`

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

## Quickstart

Build the public site overview:

```powershell
python scripts/build_site_index.py
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
pwsh -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1
```

If `llc.exe` is already on `PATH`, you can omit `OBJC3C_NATIVE_EXECUTION_LLC_PATH`.

## Other Useful Commands

Rebuild the generated site output with the standard package script:

```powershell
npm run build:spec
```

## Spec Structure

The spec is split into a small set of cross-cutting reference documents plus the numbered language parts.

Important entry points:

- `spec/INTRODUCTION.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`

Language parts:

- `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
- `spec/PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md`
- `spec/PART_2_MODULES_NAMESPACING_API_SURFACES.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `spec/PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md`
- `spec/PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md`
- `spec/PART_6_ERRORS_RESULTS_THROWS.md`
- `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md`
- `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md`
- `spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md`
- `spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md`
- `spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md`
- `spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md`

## License

No repository-wide license file is currently present. Add one before treating this as a redistributable public package.
