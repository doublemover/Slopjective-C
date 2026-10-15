# Pinned LLVM foundation

The foundation is prepared from LLVM `llvmorg-22.1.6`, pinned to resolved
commit `fc4aad7b5db3fff421df9a9637605b9ca5667881`. The canonical downstream
changes are the ordered patch files listed in `patches/series` and locked by
their SHA-256 values in `manifest.json`. Do not maintain a second editable copy
of patched Clang files.

Checkpoint 46 validated SDK build
`llvmorg-22.1.6-fc4aad7b5db3-2e6c31d39542-f01d29b2` on the admitted Windows
x64 profile. All 121 supported extension lit tests passed; 3 of the 124 discovered
tests were unsupported. All 44 AST checks, 15 facade CTest tests and 32 tooling
checks passed. The native match and transfer matrices passed 12 and 18 runtime
executions, respectively, across clang, clang-cl and objc3c-clang at O0/O2.
Baseline runtime coverage passed 20 expression, 8 plain C ABI, 4 C++ local and
4 ARC local executions. These are scoped migration results and do not promote
any public language capability.

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
defer parsing/persistence, payload facts, canonical carrier/function types,
source-import hooks, match/transfer AST semantics and CodeGen qualification
boundaries. Export requires exactly one owner for every
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
requires a public nondeleted payload destructor and an admitted direct
Some/None/Ok/Err or explicit carrier-transfer initializer. General carrier
assignment, temporaries, nested managed payloads and payload extraction outside
the plain match slice remain gated. Its executable counter checks cover source
temporary cleanup, selected destruction once, C++ unwinding, failed construction,
const locals and aligned empty payloads at O0/O2 through clang++ and clang-cl.
Run `scripts/objc3c_toolchain_runtime/run_carrier_cpp_lifetime_fixtures_windows.ps1`
from the admitted developer environment. This is private migration evidence.

For admitted C++ record payloads, private `__objc3_has_value` and `__objc3_is_ok`
queries borrow only the tag from existing carrier glvalues. They perform no
payload construction, copy, move or destruction; queries on those carrier
temporaries remain rejected. Ordinary calls through fixed reference parameters
use Clang's native reference transport and carry the carrier address. This does
not qualify by-value carrier transport, overloaded operator calls or additional
payload lifetime operations.

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

Private C++ `__objc3_copy` and `__objc3_move` expressions now use native AST
nodes with InitializationSequence recipes for each payload arm. Copy requires a
carrier lvalue; move accepts a carrier glvalue and presents the payload as an
xvalue, including native copy fallback for a const source. Template
revalidation, PCH and AST import preserve those recipes. CodeGen admits these
expressions only as direct local initializers with supported C++ payload
lifetimes. It evaluates the source once, constructs only the active arm and
stores the destination tag after successful construction. The source tag stays
active after a move, so source destruction follows ordinary moved-from payload
lifetime rules. Failed destination construction registers no destination payload
destruction; normal and C++ exception cleanup use the selected-arm local lifetime
path. Nonvolatile sources and destinations are required. Assignment, discarded
transfer results, arbitrary carrier temporaries, nested carrier payloads and
ARC/Block transfer remain unsupported. The tracked runtime witnesses cover
copy/move construction, const-source fallback, selected Result arms and throwing
constructors. Run
`scripts/objc3c_toolchain_runtime/run_carrier_transfer_fixtures_windows.ps1`
with `-ClientDriver clang`, `clang-cl` or `objc3c-clang` for O0/O2 checks.
These private forms leave public feature gates unavailable.

Private `__objc3_match` statement and expression forms have native AST nodes,
type-directed pattern checks, case-local binding scope, coverage diagnostics,
template revalidation, PCH and AST import. Optional case spellings remain private
`__objc3_case_some`/`__objc3_case_none`; Result patterns use `.Ok`/`.Err`.
CodeGen evaluates the scrutinee once, tries cases in source order and binds
immutable borrowed `let` names only inside their case. It admits plain
Optional/Result carriers and integral, enum and native pointer scrutinees.
Expression forms require exhaustive coverage and a supported scalar result;
their guards must be boolean and free of observable side effects. Statement
guards and selected arm bodies run native temporary and EH cleanups, including
cleanup before proceeding past a false guard. Managed payload matches, mutable
or move bindings, type tests, string patterns and aggregate expression results
remain unsupported. Run
`scripts/objc3c_toolchain_runtime/run_native_match_fixtures_windows.ps1`
with the same three client drivers for the O0/O2 runtime witnesses. This does not
qualify the public match spelling.

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

The matching facade now includes locked semantic interface import and
source-directed registry plumbing. It checks canonical `.objc3i` source/sidecar
pairs, module fingerprints, package identity and target/runtime/language profile,
reparses in dependency-first order and imports validated declarations through
Clang ASTImporter. Modeled C/Objective-C interfaces preserve canonical async and
typed/untyped throws declarations, including nested callback types, with known
required-semantics identifiers and exact declaration-identity roundtrips.
Unknown required semantics are rejected. Reparse/import helpers use isolated
physical filesystems without changing the host process working directory.
Private synchronous ARC free functions returning scalar/void values can lower
Error-object throws through the C.4.1 trailing autoreleasing error-output slot.
Direct try propagation, forced try and scalar/void optionalization use that
native ABI; ordered do/catch execution, autorelease pools within throwing
functions, other callable shapes and async
execution remain unsupported. Public
re-export edges control visibility and module provenance survives emission.
Version 3 of the C embedding API and the CLI expose
search roots, locked module requests and compiler/dependency identity records.
`--source-imports` defers resolution and visibility until ObjC3
`@import Module.Path;`; unused registry entries contribute no semantic dependency,
and unregistered imports retain Clang's native module-loader path. C++ and
Objective-C++ semantic interface interchange remain unsupported. The facade also
registers scalar-boundary text tests and the native `objc3_text_file_cli`
application fixture, which parses a bounded UTF-8 record file through opaque text
and builder services. The tracked
`scripts/objc3c_toolchain_runtime/run_source_import_fixtures_windows.ps1`
runner archives the original provider source after emitting its interface and
object, then separately compiles, links and runs eager/source-directed consumers.
It verifies the provider lock in dependency records and preserves previously
published object/dependency outputs after a wrong-lock rejection. Package
ecosystem install/reuse scripts check metadata agreement and local artifact
installation/reuse; they do not install Clang. Build or stage the matching SDK
through the toolchain path above. At checkpoint 46, both separately linked source
import consumers passed, including qualified types at file/block scope and
wrong-lock output preservation. The compiler-free package producer, clean
consumer installation and reuse passed without regenerating producer artifacts.
See
[`native/objc3c/src/clang_foundation/README.md`](../../native/objc3c/src/clang_foundation/README.md)
for CLI/API scope and the 21-case text application result.

The sealed development compiler can also qualify its matching clang-format
and clangd binaries with `python -B -m scripts.objc3c_editor_tooling.qualification`.
This checks identical C17 Objective-C and C++20 Objective-C++ fixture bytes,
ordinary `async`/`throws` identifiers and selectors, private scalar Optional/Result
construction, queries and statement matches. Qualification requires an exact
format golden and idempotence, syntax checks in both language modes, the normalized
compilation database, `clangd --check`, and live stdio diagnostics, hover,
definition and completion. The live probes retain the ordinary parameter and add
the match's borrowed `const int` payload binding with its exact declaration
location. Each run preserves separate evidence with the source seal, tool hashes
and resource-header digest. Matching rebuilt tools passed this expanded
qualification on 2026-10-13 at 03:36:46 UTC: both language modes passed exact
formatting/idempotence, native syntax and `clangd --check` with zero errors.
All four live navigation probes (ordinary parameter and borrowed payload in each
mode) passed hover, definition at the exact declaration location and completion
checks with no diagnostic errors. This private editor slice does not qualify extension-aware refactoring
or enable public feature gates.

`npm run objc3c -- validate-native-foundation` runs selected native checks after
the matching sealed SDK and facade are built. Its complete inventory contains
20 nodes across the `carriers`, `typed-errors`, `native-package` and `editor` families:

- Four carrier suites (`expression`, `abi`, `cpp-lifetime`, `arc-lifetime`) each
  run with `clang` and `clang-cl`; `match` and `transfer` also admit
  `objc3c-clang`, giving 14 suite/driver nodes. Their exact IDs are
  `carriers:<suite>:<driver>` and each suite includes its O0/O2 witnesses.
- `typed-errors:clang` and `typed-errors:clang-cl` run the synchronous Error-object
  output ABI at O0/O2, including native Objective-C 2 callers, ARC pool lifetime,
  propagation cleanup and C++ exception separation. Each driver also executes
  dynamic-nil and forced-error witnesses and requires the Windows
  illegal-instruction trap code; another failure cause cannot satisfy a trap.
- `native-package:provider` emits a provider object, semantic interface and
  sidecar. `native-package:consumer-eager` and `native-package:consumer-source`
  install those products in fresh system-temp directories outside the checkout
  without provider source, then compile, link and run their consumers. They check
  the installed lock and compiler identity and preserve evidence before removing
  their scratch directories. These three nodes use `objc3c-clang`.
- `editor:qualification` uses the matching clang, clang-format and clangd tools
  and is selected under the `clang` driver.

```powershell
npm run objc3c -- validate-native-foundation --list
npm run objc3c -- validate-native-foundation --family carriers --driver objc3c-clang --run-id facade-carriers
npm run objc3c -- validate-native-foundation --family typed-errors --run-id typed-errors-private
npm run objc3c -- validate-native-foundation --node native-package:consumer-source --run-id package-source
npm run objc3c -- validate-native-foundation --family carriers --driver clang --run-id clang-reuse --reuse-report out/native-validation/prior/summary.json
```

With no filters, all 18 nodes are selected. Family, node and driver selectors are
repeatable exact names; unknown names and incompatible combinations fail.
Explicit node selection must agree with family/driver filters, and dependency
closure adds the provider for a selected package consumer. `--list` prints the
complete inventory and selection without requiring built tools. Run results
preserve unselected entries and publish `summary.json`, logs and explicit hashed
node artifacts under `out/native-validation/<run-id>` by default. Every run uses
a fresh directory. `--reuse-report` requires a compatible complete prior report;
toolchain and tracked-input identities and artifact size/hash checks decide
whether passing nodes can be reused. Changed or damaged products invalidate
their nodes and dependents for rerun. The workflow records the admitted native
toolchain before and after execution, does not install Clang and does not promote
any public capability. At checkpoint 47, all 18 nodes passed with 68 runtime
executions: 66 carrier witnesses and two compiled-package consumers. Source and
toolchain snapshots matched before and after the batch; the matching editor node
passed both language modes and all four live navigation probes. The focused
Python suites passed 188 tests, with two Windows symlink-permission skips.
Checkpoint 49 passed the expanded 20-node inventory with 80 runtime executions,
including all 12 Error-object ABI/trap witnesses. Its matching SDK passed 126
extension tests (three platform skips), 44 AST tests and all 15 facade CTest
tests, with matching source/toolchain identities across the native batch.
An unchanged checkpoint-47 follow-up reused all 18 nodes. A deliberately damaged copy of the
provider object invalidated and reran exactly the three package nodes while
reusing the other 15; the original passing artifacts remained unchanged.

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
