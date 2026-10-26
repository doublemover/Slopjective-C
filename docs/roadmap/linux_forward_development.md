# Objective-C 3.0 on Linux

The target is Objective-C **3.0 as defined by `spec/`**, implemented in the pinned
native Clang toolchain and GNUstep 2.2 runtime. C17 and C++20/23 are underlying
source modes. Linux x86-64 is the active implementation target. Windows,
compatibility with retired frontends and rollback variants are outside this plan.

The [capability authority](../../schemas/objc3-language-capabilities.json) owns
public compiler claims; the [library inventory](../../stdlib/module_inventory.json)
records implemented and remaining module contracts. Source implementation,
validated compiler behavior and installed SDK delivery are separate checkpoints.
The current SDK passes a rebuilt LLVM foundation: **368 native Clang/index/query
regressions** (365 ObjC3), three unsupported cases and 166 AST/matcher/source-tooling
contracts. Compiler, editor and debugger targets build. The delivery passes all
32 facade suites, **nine GNUstep client tests**, 37 installed consumer checks,
13 selected native integration nodes and all **33 installed SDK qualification
gates**. Compiler source identity is
`llvmorg-23.1.3-0d261d1ca552-20ec9169ff00-00b75dc3`.
The [current acceptance receipt](../support/linux_dynamic_frame_storage_acceptance.json)
records exact source, runtime policy, archive and report hashes. Fresh Debian 13
and Ubuntu 26.04 both pass that exact archive, including ordinary-user compiler,
package, runtime and debugger consumers. Both owned containers are removed.

The installed five-family matrix passes 168 configurations / 966 commands. The
current source/PCH dynamic-storage, spawn, scope and C++ frame-value matrix passes 24 configurations /
132 commands in each normal, ASan/UBSan and TSan mode (72 / 396 total). Current
clients and generated frame/value operations are rebuilt; unchanged service and
instrumented GNU runtime libraries retain their qualified binary identities.
Three focused compiler regressions and 22 native tooling checks pass. The final
12 feature-authority/onboarding checks also pass after receipt reconciliation.
GitHub documentation and tooling checks pass on the pushed implementation.

The preceding [2.5.0 frame-value receipt](../support/linux_cxx_frame_value_acceptance.json)
retains its 72 / 324 source and 168 / 942 installed matrices.
The preceding [2.4.0 scope receipt](../support/linux_cxx_group_scope_acceptance.json)
retains its broader source 156 / 842 matrices and standalone service suites.
Earlier source `49f322e67e0d` retains both complete five-family sanitizer matrices
(336 / 1,884), and earlier tooling retains its original scopes. Historical and
overlapping batches are not summed. Public async/actor predicates and full
Core/Strict/System conformance remain reserved.

## Dynamic native-frame storage

The [2.6.0 dynamic-storage delivery](linux_dynamic_frame_storage.md) admits
lexical VLAs and invocation-lifetime builtin allocas, with suspension-safe heap
ownership, checked sizing, explicit alignment and owned allocation failures.
The maintained C/C++ spawn witnesses cover cancellation, defer ordering, backward
jumps, deterministic OOM and storage held through registered child-group joins.
All three source instrumentation modes, rebuilt foundation, installed SDK gates
and exact-archive fresh-distribution checks pass. The acceptance receipt above
binds the complete delivery; earlier scopes retain their original evidence.

## Nontrivial C++ explicit-frame values

[C++ frame values](linux_cxx_frame_values.md) ship in concurrency module 2.5.0.
Factories construct independent captures before initial suspension, including
move-only parameters and const copying. Typed result operations preserve
copy/nonthrowing-destruction and service-error contracts through native awaits
and group quiescence. Partial capture construction unwinds synchronously.

Logical return locals use independent aligned storage; ordinary NRVO cannot
reuse the factory's physical frame-carrier return slot. Private record and enum
results share the typed task bridge's nominal descriptor within a translation
unit while remaining distinct across translation units. Source/PCH and
instrumented lifetime witnesses cover both repairs. Reference parameters,
implicit C++ receivers and nontrivial class semantic package projections remain
open. Dynamic storage is delivered above.

## C++ coroutine group scopes

[C++ task-coroutine scopes](linux_cxx_group_scopes.md) extend the compiler-owned
protocol to ordinary coroutine syntax and native C++ async bodies. The current
concurrency module is 2.6.0. A scope-aware initial awaiter reserves the guard
before body entry; promise state retains nontrivial values and errors through
the final join, publishing without resuming a completed coroutine. A retained
current-scope view shares the existing descendant-only group tree.

Authored witnesses cover normal/throwing/cancelled exits, nested scopes, actor
publication, worker cleanup fences, failed entry/result copying, abandoned body
awaits/final suspension and earlier shielded cleanup. Source-free package
coverage includes the C++ task entry and group identity. `with_task_group` owns
its callable through callback-frame destruction. Atomic grouped starts register
membership before dispatch; a private callback root and separate user group
support result iteration/join and original child-error handling without self-joins.
Explicit cancellation still cancels the whole scope. Cleanup leases cannot hide
last-owner abandonment during final joins or earlier shielded cleanup.

The preceding 2.4.0 receipt records its 156 / 842 source matrices. The current
receipt binds the 2.6.0 dynamic-storage span, installed gates and fresh-distribution
evidence separately.

## Native C-frame group scopes

[Native C-frame group scopes](linux_native_group_scopes.md) provide implicit
finalization for functions and Objective-C methods marked `objc_task_group`.
The current concurrency module is **2.6.0**. A fresh group admits only structured
descendants. Preallocated guards preserve frame captures, pending results and
checked errors through cancellation, abandonment and actor discard. Lexical
locals/defers follow source exit; the implicit final join waits for complete
child task/frame/callback quiescence before publication. Native exceptions keep
the established cancellation transport. Direct owning-carrier returns now use
the qualified ARC initialization guard.

All source/PCH and sanitizer matrices above cover this span. The native async
package gate exports a scoped provider, checks its physical frame and group
metadata, then executes it without provider source after relocation in four
C17 ARC/MRC O0/O2 modes. All 33 host SDK gates and the recorded fresh-distribution
checks pass. Ordinary C++ task-coroutine scopes are delivered above; remaining
abstract group-library forms stay open.

## Native C group iteration

[Native C group-result iteration](linux_native_group_iteration.md) supplies typed
`Optional(T)` results, compiler-owned layout/ownership plans and queued
completion-order reservation. It preserves present nil values, checked errors,
type rejection without consumption and cancellation ownership. The concurrency
module shipped as 2.2.0 and is retained by the 2.6.0 SDK. Awaited ARC carriers
gain initialization guards and cleanup
of discarded owned results, including qualified enclosing records. Physical
alias checks retain hidden error-output and logical error-domain contracts.
The source/PCH, sanitizer and installed SDK checks above cover this span.
Native C-frame and ordinary C++ task-coroutine scope finalization are delivered above.

## Native handle joins and runtime cutover

[Native task/group handle joins](linux_async_handle_joins.md) provide lazy
shared-frame adapters, typed C declarations with checked error transport, and
C++ raw-handle adoption/group finishing. Independent handle/result ownership,
nonblocking quiescence and hidden own/direct-parent cycle rejection are
implemented. The concurrency 2.6.0 headers and services are included in the current
33-gate SDK. Nontrivial C++ results work through raw task adapters and the
explicit frame definitions described above.

Full GNU runtime instrumentation exposed and repaired a global autorelease
mode-switch race. Linux now uses native per-thread ARC pools exclusively, with
no Foundation fallback or legacy retained-return slot. Native pool tokens remain
valid across late class registration; nil return claiming preserves an empty
pool. Allocation-failure witnesses preserve sanitizer interceptors, and worker
fixtures finish their handoffs before stack state is destroyed.
Native C-frame and ordinary C++ task-coroutine scopes now supply shielded
finalization. Remaining abstract group-library forms stay open.

## Coverage and lexical pool delivery

A coverage audit found that the separately authored typed-keypath package fixture
was absent from the earlier 31 enrolled gates. Package compilation now enables
SDK Clang modules with a private cache. Interface emission accepts canonical
system-library imports only when their module name and exact public header match
the library inventory; public dependency checking remains active. Builtin
`instancetype` no longer requires an external package.

The expanded **32-gate SDK passes**, including 12 typed-keypath package modes,
36 executions and 12 readonly rejections, source-free relocated consumers and
cross-image metadata identity. Fresh Debian 13 and Ubuntu 26.04 pass the same
archive. The [package acceptance receipt](../support/linux_keypath_package_acceptance.json)
binds this checkpoint to its reports. The earlier 31-gate receipt remains evidence
for its narrower scope; it did not establish this package coverage.

The audit also found unenrolled capture diagnostic/IR/persistence and spawn
fixtures. The new compiler-witness runner passes 153 commands, 24 source/PCH
executions and 24 intended rejections using the published compiler and repaired
source fixtures. Persistence now copies Blocks through the native runtime,
including MRC, instead of requiring Foundation Block methods. The new SDK includes
this runner, its fixtures and FileCheck as a **33rd gate**, which passes against
the sealed SDK and on both fresh distributions. The 32-gate receipt predates
these additions.

The delivered compiler source span implements [C++ coroutine lexical autorelease
pools](linux_async_lexical_pools.md): drain before publishing a suspended frame,
reopen on its actual continuation thread, restore scopes on throwing suspension,
and close exactly once on normal/exceptional exits. Focused IR and ARC/MRC runtime
preflights pass. The complete 356 compiler regressions, 166 native contracts, 16 native C++ async
sanitizer configurations and all 33 installed SDK gates now pass. Fresh Debian/Ubuntu
archive checks also pass.
Remaining items below are actual implementation work followed by validation.

## Published implementation checkpoint

The completed implementation is exported into maintained compiler patches,
sealed and delivered as the current Linux SDK. C++ frame definitions and the
five-family feature runner are integrated with compiler/editor/debugger consumers.
Combined qualification repaired EOF recovery, cross-language C-frame record
matching, Objective-C message import in key-path accessors, borrowed `noescape`
prototype flags and ARC lifetime endpoints around deferred actions. Facade
witnesses cover cancellation before source-body entry and shielded cleanup
completion. All 32 facade suites pass together in the final SDK build. C++ actor
entry additionally preserves method `nothrow`, establishes generated thunk frame
identity before code generation, and admits implicit ARC cleanup of transitively
Sendable aggregate fields. Required unchecked-transfer notes no longer depend
on a preceding warning. Postponed instrumentation also exposed and repaired
one-argument C++ `reset` parsing, physical async function hashes for UBSan and
ramp exception scratch accidentally spilled into a freed coroutine frame.
The structured runtime's global-pool fixture now joins all four callbacks before
its stack gate dies. Its ASan/UBSan and TSan O0/O2 runs pass, as do all 12 task/
group-await sanitizer configurations and all 40 C-frame ABI sanitizer executions.
Both complete feature instrumentation matrices pass: 168 configurations / 942
commands under ASan/UBSan, and another 168 / 942 under TSan. All 16 native C++ async sanitizer configurations also pass, including mixed
provider ownership and exact cleanup accounting after pre-entry cancellation.
Installed-library checks additionally fixed profile-safe concurrency imports,
matching nonthrowing system-helper definitions and false resource leaks through
synthetic terminating EH edges. Ordinary leak and cleanup-family rejections
remain enforced; full Core/Strict/System profile claims remain reserved.
Ordinary shared `__block` resource references retain the source's lexical cleanup
and sentinel invalidation, while owning by-value captures require an explicit
move. The full defer source/PCH matrix passes all 100 commands after this repair.
There are no feature worktrees or ignored draft
implementations. The canonical LLVM authoring tree remains under the existing
ignored build root; exported patches are its version-controlled source inputs.

| Span                | Authored implementation                                                                                                                                                                                                                                           | Continued implementation                                                                                                           |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| Borrowed pointers   | Contextual types, returned-owner contracts, strict CFG provenance/expiry/escape checking, nested aggregate/array paths, finite alias joins, guard/match forwarding, representation copies and noescape/constructor checks                                         | Remaining conservative alias/union/MRC/lambda boundaries documented in the feature roadmap                                         |
| Explicit captures   | Strong/weak/unowned/move lists, independent object/resource owners, C++ creation move vs heap copy, ARC carrier moves, captured take/reset/discard, nested resource transfer and defer/exception rollback                                                         | Plain/mixed/C++ carrier moves, throwing resource disposal and general exceptional CFG ownership                                    |
| Async frames        | Native C functions/methods/Blocks; lazy frames, owned results/errors, task/group joins, typed iteration and compiler-owned C-frame scopes, allocation failure transport, suspension-safe pools, typed-effect template deduction and heap-owned dynamic storage    | Reference parameters and implicit C++ receivers                                                                                    |
| Isolation/Sendable  | Persistent contracts, transitive Sendable checks, awaited main/global/named and actor-instance entry, retained actor binding, executor proof, scalar/void and retained object results                                                                             | Explicitly gated argument/call forms, task-ABI async targets, automatic actor association and complete Strict Concurrency behavior |
| Typed key paths     | Property/record paths, precise operand snapshots/writeback, native getter ownership, nullable readonly object traversal, immutable metadata, complete class/protocol identity and source tooling                                                                  | Nullable writes, scalar/aggregate nullable-chain results, dependent/effectful/isolated paths and ownership-bearing values          |
| Structured runtime  | Descendant cancellation, deep-tree reclamation, canonical/named executors, allocation-independent errors, shielded cleanup joins through resource destruction, retained actor binding, C/C++ scope guards, owned callback groups and recognized spawn APIs/macros | Complete abstract group library and broader profile admission                                                                      |
| Central integration | Resource capture CFG, precise lifetime owners, public parameter/return ownership contracts, schema 2.1 physical async ABI identities, clangd source relationships and development feature runner                                                                  | Remaining package/profile projections and full profile conformance                                                                 |

Per-feature detail and authored witnesses:

- [Borrowed pointers](linux_borrowed_pointers.md) and [explicit captures](linux_explicit_capture_lists.md).
- [Native C definitions](linux_native_c_definitions.md), [shared async ABI](linux_async_c_abi.md), [handle joins](linux_async_handle_joins.md) and [structured runtime](linux_structured_runtime_next.md).
- [Isolation and Sendable](linux_native_isolation_sendable.md).
- [Typed key paths](linux_typed_keypaths.md).
- [Native module contracts](linux_native_interface_contracts.md).

## Foundations already implemented

The earlier compiler foundation includes native defer and cleanup; ordered guard
and optional bindings; lazy coalescing and reference optional access; exhaustive
match results; recursive Optional/Result ownership; C++/ARC records, arrays and
heap storage; native checked errors and lazy factories; scalar resources and
strict moved/consumed flow. Part 3 v1 deliberately limits optional access to
reference results and void sends. Resource aggregate fields are a separate v2
capability under Part 8 §8.3.6.

Module-owned names, explicit exports/SPI, source dependency graphs, immutable
packages and bounded ownership projections retain native declaration identity.
PCH/PCM replay checks the Objective-C 3 build identity. Native source traversal,
libclang, matchers, clang-query, clangd and LLDB cover the earlier admitted
surfaces. New feature-specific consumers are being extended with each source
milestone; earlier tooling coverage does not establish new feature coverage.

## Completion sequence

1. Finish the remaining ownership, async, isolation, key-path and structured runtime
   slices together. Preserve live owners through suspension and exceptions;
   retain complete callable identity across source, PCH, import and packages.
2. Complete remaining abstract group-library forms on the delivered C/C++ scope
   protocol. Preserve cancellation and retained actor/destination ownership
   through scope cleanup; ordinary C++ task-coroutine lowering and owned callback
   groups are delivered.
3. Reconcile every remaining Part 3/5 type/control-flow requirement, generic
   constraint, property/reflection/metaprogramming and specified foreign/Swift
   interop surface with real implementation. Keep optional feature-set limits
   explicit; no placeholder module counts as completion.
4. Close unsupported native-EH CFG ownership and profile diagnostics, including
   required fix-its, before promoting Core, Strict, Strict Concurrency or Strict
   System capabilities.
5. Repeat the combined compiler/facade/tooling/runtime/sanitizer batch after the
   next implementation span is coherent, then perform one coordinated SDK
   cutover. The current checkpoint has completed that sequence; avoid repeated
   archive rebuilds during frontloaded source development.
6. Complete native Linux AArch64 implementation and qualification on a native
   host. Cross-compilation/emulation can find defects but do not establish native
   support. Repeat the fresh-distribution archive checks for each future cutover.

This is a hard cutover project. Native extension ABI 21 and semantic interface
schema 2.1 require explicit physical async ABI identity; old interfaces/packages
must be rebuilt. Source, headers, objects, PCH, interfaces, packages and SDK
artifacts must agree when a
contract changes. Broad source-language/ownership projections remain gated until
they preserve the complete declaration and physical ABI contract.

Part 8.7.1 permits requiring explicit lifetime control for a nonstable borrowed
owner. The implementation chooses that specified policy; automatic temporary
owner materialization is an optional alternative, not a mandatory missing feature.

The authored `scripts.objc3c_toolchain.run_linux_feature_batch` driver runs capture,
key-path, native C async, isolation and spawn witnesses against an already-built compiler/runtime.
It covers source/PCH, C17/C++20/C++23, ARC/MRC, O0/O2 and mixed async provider
ownership in private temporary directories, retaining only logs and a batch
report. Explicit compiler/header/library overrides support development testing
without an SDK archive rebuild. All 168 configurations now pass against the
development compiler/runtime. Mixed async provider/consumer ownership, frame
allocation failures, exceptions, task cancellation, checked errors and independent
result owners are exercised. The same runner is enrolled in SDK packaging and
fresh-distribution qualification. Development runs can also instrument all five
families with `--sanitizer address,undefined` or `--sanitizer thread`. Supply
`--runtime-library` built with the same instrumentation and `--resource-root`
pointing to matching Clang sanitizer runtimes. The runner applies the flags to
source, PCH and final links and records the mode in its report. Services and
clients are instrumented. For full GNU runtime instrumentation, build a separate
variant with `scripts.objc3c_toolchain_runtime.build_linux --sanitizer` and pass
its shared library with `--objc-runtime`. The current spawn, coroutine/group
and native async sanitizer runs use that instrumented GNU runtime.

## Existing evidence

The current [dynamic-storage acceptance receipt](../support/linux_dynamic_frame_storage_acceptance.json)
binds the rebuilt compiler, runtime/header source, all 33 installed gates and
fully instrumented GNU runtime checks. The archive has seal
`f2dacb38501f53860aa7920ab09e71aeab56b318afc8d479e530331e7fe2fa31`,
435,388,280 bytes and SHA-256
`b0f12194d653b5eaec9455eff4aebdabc45f6ee61a0a3d2ab5a14d7d70cbb2fa`.

The host release runs all 33 installed gates. This exact archive also passes
fresh-distribution qualification: install/relocation/reinstall, embedding,
debugging, runtime/async consumers, compiler/package gates, all five feature
families and selected carrier/control-flow consumers execute as an ordinary
user. Both owned containers are removed. Superseded SDK/runtime artifacts are
reclaimed; one SDK installation and archive remain, with development facade and
GNU client configurations bound to the current installation.

The preceding [C-frame scope receipt](../support/linux_native_group_scope_acceptance.json)
records its 2.3.0 delivery. The earlier [integration receipt](../support/linux_native_feature_integration_acceptance.json)
retains its source `49f322e67e0d` broader instrumentation results. The
[package receipt](../support/linux_keypath_package_acceptance.json) records the
intermediate 32-gate delivery. The [lexical pool receipt](../support/linux_async_lexical_pool_acceptance.json)
records an earlier 33-gate archive. The [handle-join receipt](../support/linux_async_handle_join_acceptance.json)
records an earlier runtime/header delivery. The [iteration receipt](../support/linux_native_group_iteration_acceptance.json)
records the preceding compiler/runtime delivery. These are historical scopes, rather than evidence
that every earlier sanitizer matrix was rerun with the newer compiler. Full
Core/Strict/System profiles, native Linux AArch64 and the remaining implementation
roadmap stay open.

## Development operation

Keep maintained implementation under `native/`, `stdlib/`, `scripts/`, `tests/`
and the tracked compiler patches. `out/toolchains/` contains the active native
source/build and SDK installation; `out/native-validation/` contains generated
execution evidence; `out/session/` contains disposable local handoff records.
These output roots are gitignored. Retain unique authored source through Git and
pinned patch inputs; remove superseded generated artifacts rather than retaining
rollback archives or duplicate source/build trees.

The instance has four CPU quota cores and 16 GiB RAM. Use four compile jobs, one
link job and a bounded 1 GiB content-keyed ccache. Managed activation sets the
private executable tmpfs directory `/tmp/objc3c-1000` as `TMPDIR`; no extra RAM disk
is needed. Keep persistent objects/SDK/cache on disk and disposable executables
in automatically reclaimed case directories. For subsequent implementation spans,
frontload development and use focused compile checks before the combined
language/runtime/sanitizer batch. Perform a coordinated SDK cutover after that batch is coherent; broaden checks to resolve
failures or unresolved concerns.

Installed qualification gates audit the frozen SDK's full contents at entry
and exit. Nested source/package operations reuse that gate's manifest binding,
explicitly propagated to its case workers. This scope has no persistent cache;
standalone build commands retain full verification. Mutation, failed-consumer,
nested-scope and worker-isolation regressions pass. A local measurement executes
100 nested checks with two full audits in about five seconds under contention.

Runtime-only development can configure `native/objc3c/src/runtime` directly:

```sh
cmake -S native/objc3c/src/runtime -B out/runtime-development -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out/runtime-development --parallel 4
ctest --test-dir out/runtime-development --output-on-failure
```

This reuses the production service target and does not require a compiler SDK.
The task-service, structured-runtime, checked-error-bridge and key-path service
suites pass normally and under ASan/UBSan. The native compiler and all five feature
families pass, and the coordinated SDK qualification passes all installed gates.
