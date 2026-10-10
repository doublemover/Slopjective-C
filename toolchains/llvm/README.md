# Pinned LLVM foundation

The foundation is prepared from LLVM `llvmorg-22.1.6`, pinned to resolved
commit `fc4aad7b5db3fff421df9a9637605b9ca5667881`. The canonical downstream
changes are the ordered patch files listed in `patches/series` and locked by
their SHA-256 values in `manifest.json`. Do not maintain a second editable copy
of patched Clang files.

## Prepare and verify

From the repository root, use:

```powershell
python -m scripts.objc3c_toolchain prepare --fetch
python -m scripts.objc3c_toolchain verify
```

Preparation writes only under `out/toolchains/<build-id>/src`. It verifies the
upstream commit, patch order and patch hashes, applies patches in order, and
records a prepared tree digest. Repeated preparation never overwrites an
existing tree; use `verify` to check it. A missing or mismatched marker, wrong
base, or changed source tree is an error and requires an explicit manual
recovery after preserving the tree for inspection.

For baseline comparisons only, `prepare --fetch --pristine-baseline` creates a
separately named upstream-only tree. It is never identified as the ObjC3 fork.
After patch authors export their changes into the canonical series, use
`seal-development` to compare the editable development tree against a temporary
Git index containing exactly the pinned base plus the ordered patches. This
comparison includes untracked source files and ignores only the preparation
marker. `verify-development` repeats that comparison and checks the seal. It
does not reset, stage, or rewrite source files.

## Reproducing the build configuration

`cmake/ObjC3LLVMCache.cmake` and `manifest.json` record the deterministic Ninja
Release configuration: assertions, RTTI and exceptions enabled; X86 target;
Clang, LLD and clang-tools-extra projects; Windows VS 2022 v143 with the
manifest-selected SDK and dynamic release CRT; no forced libc++; no bootstrap;
eight compile jobs and one link job. The initial named build graph includes
`clang`, `clangTooling`, `clangIndex`, `clangCodeGen`, `clangFrontend`,
`clangDriver`, and the lit utilities `FileCheck`, `count`, `not`, and
`llvm-config`. The last tool is required by lit's actual site configuration.
`build-plan test` uses the admitted Python executable with `-B` and records
`PYTHONDONTWRITEBYTECODE=1` in its required command environment. Set that
environment value when executing the plan: lit workers launch fresh Python
processes that do not inherit `-B`. Together these settings keep generated
bytecode out of the sealed source tree.
The build plan emits ordered command groups: the Clang compiler and test tools
first, then the facade's Clang libraries, then the focused native type-contract
test. The test plan records that native test executable as a follow-up after
the extension lit suite. The build requests only those libraries and
their dependency closure; it avoids aggregate `clang-libraries` and
`llvm-libraries` targets. `lld-link` is produced as an alias of the `lld`
executable and is therefore not named as a Ninja target. The CMake generator
is passed on the command line because CMake selects it before loading an
initial cache file.
Run `build-plan configure`, `build-plan build`, or `build-plan stage` to print a
machine-readable command and identity. These actions never execute CMake.
Their identity includes the sealed source digest, host compiler and SDK input
hashes, CMake/Ninja and Python versions and executable hashes, CRT, build
options and target set. Python 3.13 is selected through the Windows Python
launcher and passed as `Python3_EXECUTABLE`; no global PATH changes are made.
Each plan records source and generated include roots, including the configured
`ObjC3Foundation.h`, along with hashes for build-tree and staged `LLVM_DIR`,
`Clang_DIR`, and Clang resource directories whenever those paths exist. This
lets a native consumer use the build-tree SDK during F1, then switch to the
staged SDK after install. Source preparation alone does not claim a built or
staged SDK.

The source exporter runs as `python -B -m
scripts.objc3c_toolchain.export_development_patches`. It rejects a native feature
table that differs from `schemas/objc3-language-capabilities.json`. Regenerate
the table and support view with `python -B -m
scripts.objc3c_toolchain.generate_features` before export. Native macros and
`__has_feature` use that same table; qualified target, runtime and profile
evidence is required before a public capability becomes true.

The focused `ObjC3TypeContractsTests` target validates native QualType,
target layout, ARC and record-operation facts. It uses Clang's distinct-unit
test helper and deliberately avoids building AllClangUnitTests. Those facts
are inputs to Sema and CodeGen; they do not establish carrier call ABI or
extension-language qualification by themselves.

The canonical stack has separate ownership for language/build identity, native
defer parsing/persistence, payload facts, canonical carrier/function types, and
CodeGen qualification boundaries. Export requires exactly one owner for every
changed or new source path. Canonical type regressions cover aliases, payload
qualifiers, type interning and rehashing, PCH/import, templates, C compatibility,
index identity and pointer overload mangling. CodeGen checks actual carrier
storage and debug layout, rejects unqualified native boundaries before output,
and keeps empty GNUstep COFF modules free of discarded metadata registration
symbols. The corresponding executable runtime fixtures use both GNU-style
Clang and clang-cl client drivers. These checks are scoped evidence for the
internal migration; they do not enable a public language capability.

Reserved `__objc3_some`, `__objc3_none`, `__objc3_has_value`, `__objc3_ok`,
`__objc3_err` and `__objc3_is_ok` expressions now use native AST nodes and
InitializationSequence conversions. Their source type information survives
aliases, templates, AST import, PCH and module serialization. Tentative C++
declaration/type-id parsing preserves these expression tokens, including inside
parentheses and conditions. Stored payloads initialize their actual hidden
record fields; native source argument temporaries belong to the enclosing full
expression rather than an intrinsic's internal parse boundary.

The current CodeGen slice initializes and copies supported plain payloads by
their explicit active tag, snapshots sources before assignment, and evaluates
query operands once. Zero, false and null payloads retain presence independently
of their bits. It rejects whole enclosing aggregate copies, object-representation
bitcasts and atomic accesses which would bypass the selected-arm adapter.
Nontrivial C++/ARC carrier lifetimes, payload extraction and carrier by-value
call ABI remain gated. Executable expression fixtures run at O0/O2 through both
GNU-style Clang and clang-cl; the facade checks identical native IR through its
C++ compile session and versioned C embedding API, including failed-output
rollback. These scoped migration checks do not promote public feature gates.

Configure from a VS 2022 x64 Developer Command Prompt with the selected SDK
installed. The helper verifies the v143 compiler/linker, the manifest's
Windows SDK version, CMake and Ninja before it emits a build plan. No global
PATH changes are required.

For a long Windows build, an implementation-program runner saved under
`temp/full-compatibility` can be started with:

```powershell
python -B -m scripts.objc3c_toolchain.run_owned_background --runner temp/full-compatibility/run_compiler_sdk.ps1 --label sdk-run-1
```

The runner must enter and verify the developer environment before building.
The launcher uses noninteractive PowerShell with closed stdin and
`CREATE_NO_WINDOW`, writes separate logs under `out/toolchains/objc3-development`,
and records its process identity under the task's temp area. Labels are unique
so a later run cannot overwrite existing evidence. It starts one runner without
changing terminal preferences or stopping existing processes. Check the runner's
status and exit evidence before reporting a build result.

When rebasing LLVM, first update the annotated tag and resolved commit together,
review the semantic/ABI delta, update each patch in canonical order and its
hash, then verify preparation from a clean generated destination. Preserve old
prepared trees as evidence until no active build depends on them.
