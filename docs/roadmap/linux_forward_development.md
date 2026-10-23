# Objective-C 3.0 on Linux

The target is Objective-C 3.0 as defined by `spec/`, implemented by the patched
Clang toolchain. C17 and C++20/23 name the underlying Clang source modes; they
are not separate Objective-C product versions. Linux x86-64 with GNUstep 2.2 is
the currently qualified runtime target. Windows and compatibility with retired
frontends are outside the active development plan.

The [language capability authority](../../schemas/objc3-language-capabilities.json)
records public compiler claims. The [library inventory](../../stdlib/module_inventory.json)
records native library coverage against the
[standard-library contract](../../spec/STANDARD_LIBRARY_CONTRACT.md). Existing
historical milestone checklists do not establish current Clang conformance.

## Delivered foundations

The [current SDK receipt](../support/linux_defer_scope_release_acceptance.json) identifies
the activated archive, its 26 installed suites and fresh Debian 13 / Ubuntu 26.04 checks. That release includes
extension ABI 20, service ABI 4, the shared C/C++ async frame/result contract, native standard-library modules,
per-callback and per-resume autorelease pools, fair actor scheduling and the
current SDK-independent package install command. Deferred cleanup now preserves
managed source-scope releases and native action order, copied bodies and original
exception state; checked-handler initialization and unwind diagnostics also
pass the coroutine gate. New development must produce a
new archive and receipt before it is described as delivered in that SDK.

The public synchronous compiler supports native `defer`, `Optional`, `Result`
and checked `throws` in the bounded compatibility profile. Native `match` now
constructs aggregate, reference, complex/vector and C++ member-pointer results;
its bindings use stable snapshots and independent mutable case storage, with
native weak loads and C++ copy/exception rules. Its ownership and
ABI tests cover strong/weak/unsafe objects, Blocks, nested carriers, arrays,
records, heap storage, direct dispatch, typed errors and native unwinding.

Native semantic interfaces preserve callable effects, ownership, availability,
module-owned names, explicit exports and SPI. Source graphs and immutable binary
packages support separately compiled and source-free providers. Installed editor
and LLDB tests exercise the same canonical identities.

The structured runtime supplies groups, cancellation, task outcomes, serial
actors, executor destinations and suspended dependency/hop scheduling. The
Objective-C++ coroutine compiler supports async functions, methods and Blocks
through reserved spellings. Public async/await and actors remain reserved.
The [shared ABI contract](linux_async_c_abi.md) no longer exposes C++ promise
layout at the C frame boundary.

## Implementation in progress

The shared native source tree now implements ordered public guard and optional
bindings, lazy coalescing, reference optional access, scalar resource cleanup
and explicit transfer operations, mixed C++/ARC carrier locals, source C++
records with carrier fields, and selected-payload LLDB inspection. Native CFG
work includes selected match/coalescing branches and resource stages. Native-C
logical async declarations and calls bridge to the C frame ABI; definitions
still require the separately specified state-machine lowering.

Task-group work includes nonblocking completion, typed result reservation and
iteration, cancellation-safe output slots and retained C17 frame results. Library
adapters preserve domain errors and lazily construct selected Result arms;
strict Error-factory checks are native Sema operations. Source traversal,
libclang indexing, AST matchers, clang-query and focused clangd consumers have
regressions enrolled in the foundation build. Raw PCH/PCM identity now rejects
incompatible Objective-C 3 artifacts even when ordinary validation is disabled.

The consolidated development foundation now passes: compiler, facade libraries,
editor tools and LLDB build successfully; 293 Clang regressions pass with three
unsupported, alongside 148 native AST/CFG/CodeGen, matcher and clangd contract
tests. Eighteen focused tooling tests and eight coroutine harness tests pass.
The admitted execution matrix also passes 1,045 commands: 404 native AST
configurations, 24 defer, 24 guard, 24 optional-access and eight coalescing
source/PCH configurations. SDK/archive qualification is stopped while the next implementation batch is developed.
The repaired standard-library overlay passes 40 executions / 198 commands, and
the async C ABI MRC lifetime witness passes 100 repeated executions. Strict
factory contracts check all three profiles: ARC additionally compiles the
managed carrier ABI; MRC checks Sema without admitting unsupported managed
carrier code generation. Installed SDK qualification remains required before
new release receipts or capability claims. The active
SDK above still predates this wave. Managed release queues retain an explicit
native-EH CFG ownership boundary while normal CFG and native execution checks
remain independent.

Native Objective-C async definitions, public spawn/isolation/Sendable, complete
strict profiles and native AArch64 qualification remain outstanding. Reference
optional access follows Part 3 v1, which deliberately excludes scalar/struct
optional chaining. Borrowed-pointer provenance, escape checking and explicit capture rules from
Part 8 remain outstanding. Resource field annotations are deliberately rejected in v1 under
Part 8 §8.3.6; aggregate-resource lowering is a separate v2 capability.

The unvalidated implementation batch adds contextual public `take`, `reset`
and `discard`, prefix `@resource(F, invalid: X)` declarations, resource AST import
and structural identity, strict proofs for unspecified receivers, and C++ error
factory thunks that avoid eager copies and construct factories only on failure.
New parser, PCH, strict-flow, import/index and execution fixtures accompany the
implementation. A further chunk implements prefix `@cleanup(F)` and canonical
`cleanup(F)` headers with checked native value, direct reference and address
calls. Cleanup is unconditional, reads the final value, preserves callback
mutations, and shares registration order with defer and resources. Native unwinding, CFG
cleanup elements, protected jumps, AST import, PCH and deferred ARC source
release scopes are integrated. Forwarded block storage remains alive until the
callback returns. Path-sensitive analysis remains explicitly unsupported for
these native cleanup nodes. New semantic, IR, CFG, import/PCH and runtime
fixtures are authored; no post-batch compiler build or execution is claimed.
Strict profiles also diagnose direct non-local transfers that may bypass a
registered defer, cleanup or resource action, including signal jumps and
`-fno-builtin` C library declarations. A unique automatic jump buffer used only
by jump primitives can preserve existing registrations at its local save point;
actions registered afterward remain diagnosed. Initializer calls, completed
inner scopes, control-statement scopes and nested callable ownership retain
separate registration boundaries. This is a local direct-call analysis; it does
not claim interprocedural wrapper or arbitrary jump-buffer alias analysis.
The next authored slice adds typedef/using resource contracts: public
`@requires_cleanup`, `@resource_family` and `@invalid`, with canonical GNU header
forms recorded in B.8.7. Cleanup associations survive redeclarations, PCH and AST
import. Structural matching and importer merge checks distinguish resource
families, callbacks and converted sentinel values while retaining the scalar
ABI. Alias redefinitions cannot silently change an inherited contract.

Strict-system flow now tracks automatic typed handles with manual, deferred or
automatic cleanup, typed ownership returns, selected branches, loops and native
exception exits. It diagnoses missing cleanup, repeated/wrong-family closes,
untracked storage, dropped acquisitions and incompatible ownership returns.
Manual close has a separate consumed state: comparing an unchanged handle to
its sentinel cannot resurrect ownership. Typed owned returns across potentially
throwing scope cleanup are explicitly rejected until their rollback is modeled;
nonthrowing actions and known-invalid returns remain admitted. This slice does
not claim global/heap ownership, borrowed escapes, explicit capture transfer or
complete strict-system conformance. Parser/Sema, source/PCH, import/identity and
runtime fixtures accompany the implementation; no new execution result is
claimed. Combined validation is deferred until a larger batch is ready.

## Active completion sequence

Each milestone includes implementation, diagnostics, imports, ownership,
execution and installed-archive tests before its capability is promoted.

1. **Native standard library.** Replace the retired `module`/`fn` sources and
   synthetic integer helpers with canonical Clang modules backed by the real
   runtime. Ship core discovery/versioning, lazy nil-to-error helpers, validated
   UTF-8, concrete/generic collections and structured runtime declarations.
   Exercise mixed Objective-C/Objective-C++ providers, ARC/MRC consumers, O0/O2,
   cached imports and expected failures. Implement typed key paths and strict
   system helpers as their compiler semantics become available.
2. **Core type and control-flow completion.** Reconcile every Part 3 and Part 5
   requirement with the current AST and diagnostics: generic constraints,
   optional binding/chaining, `guard`, exhaustive matching, typed key paths and
   flow-sensitive refinements. Carry these through PCH and semantic interfaces.
3. **Native Objective-C async.** Lower functions, methods and Blocks to the
   existing C frame ABI without requiring C++ source. Preserve live locals,
   cleanup, owned results/errors, native exceptions and cancellation across
   suspension. Recognize structured versus detached spawn and enforce awaited
   effectful calls before promoting public syntax.
4. **Isolation and concurrency safety.** Implement actor/global-actor callable
   metadata, awaited executor crossings, isolated state access and Sendable
   transfer checks. Preserve logical identity and mailbox fairness through
   nested calls, imports and source-free packages.
5. **Ownership and strict profiles.** Complete resource, borrowed/lifetime,
   capture and escape rules from Part 8. Enable Core, Strict, Strict Concurrency
   and Strict System only when the full relevant checklist and required library
   modules pass, including required diagnostics and fix-its.
6. **Remaining specified feature sets.** Complete the specified reflection,
   metaprogramming/property behavior, foreign/Swift interop and tooling surfaces.
   Optional feature-set status must remain explicit until its entire contract
   is implemented and tested.
7. **Release and architecture coverage.** Run the actual new archive on fresh
   supported Linux distributions, publish its exact content identity and
   installation evidence, then qualify AArch64 on a native host. Emulation and
   cross-compilation can find bugs but do not establish native support.

The standard-library cutover is version 2. Its inventory distinguishes completed
module contracts from partial and unimplemented modules. Importing a module
alone does not establish profile conformance. No placeholder key-path or system
module may turn a missing implementation into apparent coverage.

## Development and release operation

Use one hard cutover for source, objects, interfaces, packages and installed SDK
artifacts when an ABI changes. Retain the current archive/installation and active
compiler source/build tree. Remove superseded generated artifacts once a new
release is verified; Git and pinned source/patch inputs preserve history.
Preserve unique authored work before removing a worktree.

| Location                 | Purpose                                                    |
| ------------------------ | ---------------------------------------------------------- |
| `out/toolchains/`        | Active compiler/build and current SDK archive/installation |
| `out/native-validation/` | Current native execution receipts and diagnostics          |
| `out/session/`           | Local handoff records and PR drafts                        |
| `scratch/`               | Disposable experiments and temporary review worktrees      |

These output roots are gitignored. Maintained code belongs under `scripts/`,
`native/`, `stdlib/` or `tests/`; outputs do not belong beside source files.

This instance has a four-CPU quota and 16 GiB of RAM. Use four compile jobs and
one link job, a bounded 1 GB ccache with content-based compiler checks, and the
private mode-0700 `/tmp/objc3c-1000` directory exported as `TMPDIR` by managed
activation. `/tmp` already uses RAM; no extra ramdisk reservation is needed.
Keep persistent compiler objects, SDKs and ccache on disk. Disposable integration
executables and service archives use case-scoped temporary directories and are
reclaimed on success or failure; logs and receipts remain on disk.

Installed SDK consumers run with bounded concurrency and stop scheduling later
suites after a failure. The maintained fresh-distribution runner binds the exact
archive digest and qualifies Debian 13 and Ubuntu 26.04 concurrently with two
CPUs per container, executable tmpfs and guaranteed container cleanup. Its
`native-features` scope includes guard, optional access, recursive carrier and
LLDB witnesses alongside defer, standard-library and async/task consumers.

The ignored local `linux-dev` CMake preset targets the current frontend build.
The SDK pipeline enables all required qualification independently. Frontend
CTest executables are built and executed serially, then passed binaries are
reclaimed to stay within the disk budget. Source, configuration and actual
foundation contents determine reuse; deleting a duplicate stage does not
invalidate an unchanged foundation.

`out/bin/clangd` uses the current patched SDK, two workers and in-memory
preambles. The ignored `.clangd` and root compilation database select current
build paths. After an SDK cutover, reconfigure the frontend against
`out/toolchains/linux-sdk/current` so no deleted candidate path remains in the
compilation database.

## Development system module

The next implementation chunk adds the partial `objc3.system` native module,
with an explicit Strict System import gate. Linux file descriptors and libc heap
buffers carry cleanup/family/sentinel typedef contracts. Helpers provide explicit
unsafe adoption, descriptor duplication, exact-once POSIX close and ordinary
allocation/free. No new runtime ABI is introduced. Close preserves status/errno,
is not retried after EINTR, and retains its cancellation/exception behavior.

The maintained stdlib gate now includes twelve system execution configurations,
three ownership rejections per language/ownership pair, and rejection of imports
outside Strict System. These are authored checks awaiting combined validation;
no new PASS, SDK or profile claim is recorded. Borrowed-pointer contracts,
escape checking and complete diagnostics remain in the inventory's remaining
list.

## Development explicit lifetime control

The next source milestone implements Part 8.6 `withLifetime(expr) stmt` and
`keepAlive(expr);` as native statement nodes. Each operand is evaluated once
and initialized into an implicit precise owner. Bounded owners end after the
statement; kept owners end with the current lexical scope. Object and Block
operands are supported. MRC acquires an independent reference even for an
already owned operand; ARC uses a precise strong binding. Explicit bindings
register cleanup on exceptional exits as well as normal returns and jumps.
Source release queues keep these owners alive until explicit scope actions
finish, including defers registered before `keepAlive`.

The node, implicit declaration and source operand survive templates, PCH and
AST import. Source traversal, indexing, printing, native/dynamic matchers and
source-parent lookup expose the written operand once. CFGs have an explicit
precise-owner release element even without optional destructor/lifetime flags.
Protected jumps and strict non-local jump diagnostics include these owners.
Path-sensitive symbolic analysis still reports an explicit unsupported boundary;
block-copy, throwing initializer temporary and general managed EH CFG
restrictions are not claimed resolved. An initialization guard releases a
completed owner if destruction of a full-expression temporary throws before the
permanent lexical cleanup can be registered.

New semantic, IR, source/PCH, import/index, matcher, CFG-ordering and runtime
witnesses cover these semantics. Focused source compilation also repaired two
resource-prefix parser API mismatches and one resource diagnostic helper from
the preceding implementation batch. Compiler relinking and execution of the
new native fixtures remain pending; SDK/archive qualification remains paused.
