# Objective-C 3.0 on Linux

The target is Objective-C **3.0 as defined by `spec/`**, implemented in the pinned
native Clang toolchain and GNUstep 2.2 runtime. C17 and C++20/23 are underlying
source modes. Linux x86-64 is the active implementation target. Windows,
compatibility with retired frontends and rollback variants are outside this plan.

The [capability authority](../../schemas/objc3-language-capabilities.json) owns
public compiler claims; the [library inventory](../../stdlib/module_inventory.json)
records implemented and remaining module contracts. Source implementation,
validated compiler behavior and installed SDK delivery are separate checkpoints.
The current large implementation batch is **authored, not built or executed**.
Public async/actor predicates and full Core/Strict conformance remain reserved.

## Paused implementation checkpoint

Development is paused at the user's request. All six compiler/runtime agents
have finished their current spans and are parked. Completed maintained code and
fixtures are committed, and canonical compiler changes are exported into the
tracked LLVM patch stack. No new implementation or validation is scheduled.
There are no feature worktrees or ignored draft
implementations. The canonical LLVM authoring tree remains under the existing
ignored build root; exported patches are its version-controlled source inputs.

| Span | Authored implementation | Continued implementation |
| --- | --- | --- |
| Borrowed pointers | Contextual types, returned-owner contracts, strict CFG provenance/expiry/escape checking, nested aggregate/array paths, finite alias joins, guard/match forwarding, representation copies and noescape/constructor checks | Remaining conservative alias/union/MRC/lambda boundaries documented in the feature roadmap |
| Explicit captures | Strong/weak/unowned/move lists, independent object/resource owners, C++ creation move vs heap copy, ARC carrier moves, captured take/reset/discard, nested resource transfer and defer/exception rollback | Plain/mixed/C++ carrier moves, throwing resource disposal and general exceptional CFG ownership |
| Async frames | Native C functions/methods/Blocks; lazy frames, owned results/errors, child awaits, allocation failure transport, suspension-safe pools and typed-effect template deduction | Ordinary C++ frame definitions, dynamic stack storage, C++ task pool handling, handle-await protocols and structured scope lowering |
| Isolation/Sendable | Persistent contracts, transitive Sendable checks, awaited main/global/named and actor-instance entry, retained actor binding, executor proof, scalar/void and retained object results | Explicitly gated argument/call forms, task-ABI async targets, automatic actor association and complete Strict Concurrency behavior |
| Typed key paths | Property/record paths, precise operand snapshots/writeback, native getter ownership, nullable readonly object traversal, immutable metadata, complete class/protocol identity and source tooling | Nullable writes, scalar/aggregate nullable-chain results, dependent/effectful/isolated paths and ownership-bearing values |
| Structured runtime | Descendant cancellation, deep-tree reclamation, canonical/named executors, allocation-independent errors, shielded cleanup joins through resource destruction, retained actor binding, inherited/detached starts and recognized spawn APIs/macros | Compiler-owned structured scope finalization, complete abstract group library and remaining combined integration |
| Central integration | Resource capture CFG, precise lifetime owners, public parameter/return ownership contracts, schema 2.1 physical async ABI identities, clangd source relationships and deferred feature-batch driver | Remaining package/profile projections, driver enrollment and combined validation of authored fixtures |

Per-feature detail and authored witnesses:

- [Borrowed pointers](linux_borrowed_pointers.md) and [explicit captures](linux_explicit_capture_lists.md).
- [Native C definitions](linux_native_c_definitions.md), [shared async ABI](linux_async_c_abi.md) and [structured runtime](linux_structured_runtime_next.md).
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

1. On resumption, finish the remaining ownership, async, isolation, key-path and structured runtime
   slices together. Preserve live owners through suspension and exceptions;
   retain complete callable identity across source, PCH, import and packages.
2. Integrate the authored recognized spawn APIs/macros, retained actor bindings
   and awaited entries with the remaining C++ frame-definition and automatic
   structured-scope finalization work.
3. Reconcile every remaining Part 3/5 type/control-flow requirement, generic
   constraint, property/reflection/metaprogramming and specified foreign/Swift
   interop surface with real implementation. Keep optional feature-set limits
   explicit; no placeholder module counts as completion.
4. Close unsupported native-EH CFG ownership and profile diagnostics, including
   required fix-its, before promoting Core, Strict, Strict Concurrency or Strict
   System capabilities.
5. Run a combined compiler/facade/tooling/runtime validation batch after the
   implementation checkpoint is coherent. Then perform one coordinated SDK
   cutover and installed-consumer run for the release. Do not rebuild or qualify
   SDK archives repeatedly during frontloaded source development.
6. Complete native Linux AArch64 implementation and qualification on a native
   host. Cross-compilation/emulation can find defects but do not establish native
   support. Qualify supported fresh Linux distributions against the final SDK.

This is a hard cutover project. Semantic interface schema 2.1 requires explicit
physical async ABI identity; old interfaces/packages must be rebuilt. Source,
headers, objects, PCH, interfaces, packages and SDK artifacts must agree when a
contract changes. Broad source-language/ownership projections remain gated until
they preserve the complete declaration and physical ABI contract.

Part 8.7.1 permits requiring explicit lifetime control for a nonstable borrowed
owner. The implementation chooses that specified policy; automatic temporary
owner materialization is an optional alternative, not a mandatory missing feature.

The authored `scripts.objc3c_toolchain.run_linux_feature_batch` driver runs capture,
key-path and native C async witnesses against an already-built compiler/runtime.
It covers source/PCH, C17/C++20/C++23, ARC/MRC, O0/O2 and mixed async provider
ownership in private temporary directories, retaining only logs and a batch
report. Explicit compiler/header/library overrides support development testing
without an SDK archive rebuild. The driver has not been executed in this phase.

## Existing evidence

The last fully validated compiler checkpoint is
`0ddde9661a53f6464d9eb7225e80eedf5fce8ae3`, native source identity
`llvmorg-23.1.3-0d261d1ca552-0a3a6c1d8422-00b75dc3`. Its compiler/facade/editor/
debugger build passed, with 293 Clang regressions (three unsupported), 148 native
contract tests and 1,045 integration commands. Separate receipts identify earlier
stdlib, carrier, async and sanitizer runs. Later focused source compilation
covered 31 C++ translation units; it did not execute the new language fixtures.
**None of those results validates the current parallel implementation batch.**

The active older SDK is identified by
[defer delivery evidence](../support/linux_defer_scope_release_acceptance.json)
and [guard AST evidence](../support/linux_guard_ast_acceptance.json). No new SDK
archive, installed-feature PASS or complete profile claim is implied by new
source commits. Validation remains deferred at this paused source checkpoint.

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
in automatically reclaimed case directories. During this development phase,
build/test/qualification execution remains postponed and development is paused.
Once validation resumes,
run necessary checks for the combined change and broaden only to resolve new
failures or unresolved concerns.
