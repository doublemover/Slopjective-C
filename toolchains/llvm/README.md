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
Direct initialized C++ carrier locals now construct their selected payload with
native InitializationSequence semantics and register tag-guarded normal/EH
destruction after successful initialization. Sema resolves actual payload
destructors, including access/deletion and declaration references; native jump
checks reject entering a scope past initialization. Empty C++ payload addresses
use AST-owned byte offsets, including explicit alignment. The admitted slice
requires a public nondeleted payload destructor and a direct Some/None/Ok/Err
initializer; carrier copy/move, temporaries, nested managed payloads and
payload extraction remain gated. Its executable counter checks cover source
temporary cleanup, selected destruction once, C++ unwinding, failed construction,
const locals and aligned empty payloads at O0/O2 through clang++ and clang-cl.
Run `scripts/objc3c_toolchain_runtime/run_carrier_cpp_lifetime_fixtures_windows.ps1`
from the admitted developer environment. This is private migration evidence.

Direct initialized ARC carrier locals now use native strong, weak or unsafe
object fields. Sema makes implicit ARC `id` ownership explicitly strong before
carrier uniquing, preserves explicit weak/unsafe ownership, and rejects
autoreleasing, Block and nested managed payload storage. CodeGen initializes
only the selected payload, stores the tag after initialization, and registers
selected normal/EH cleanup; None and inactive Result arms do not initialize or
destroy managed storage. Tag queries require an admitted direct initializer so
recovery cannot read a rejected or uninitialized local. ARC copy/move,
assignment, temporaries, payload extraction and by-value calls remain gated.
The native executable witness covers strong retention, weak zeroing, unsafe
nonretention, Some(nil), inactive arms and native C++ unwinding at O0/O2 through
clang++ and clang-cl. Its separately compiled MRC root class implements the
pinned runtime's required weak-load hook and weak-reference teardown. Run
`scripts/objc3c_toolchain_runtime/run_carrier_arc_lifetime_fixtures_windows.ps1`
from the admitted developer environment. These checks are private migration
evidence and do not enable a public language capability.

An internal by-value adapter qualifies supported plain direct carrier values
for nonvariadic C functions on `x86_64-pc-windows-msvc`. Clang's target ABIInfo
classifies the AST-owned hidden record while source declarations retain their
carrier types. Caller snapshots copy only the active arm into zeroed storage;
direct return coercions use the same adapter. Native C++ functions, other
targets, variadic calls, enclosing records, blocks and Objective-C methods
retain explicit failure boundaries. Two-TU executables include an ordinary C
consumer with private test-only mirror declarations and verify active payload
bits, independently compiled at O0/O2 with both client drivers. The mirrors do
not define a public ABI and inactive storage bytes have no promised value.
The Windows witness uses the admitted `/MD` UCRT profile, including the
`memset` required by zeroed snapshots at O0. Run it from the selected developer
environment with `scripts/objc3c_toolchain_runtime/run_carrier_abi_fixtures_windows.ps1`.
Executable expression fixtures run at O0/O2 through both
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
