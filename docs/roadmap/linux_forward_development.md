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

## Current implementation spans

Six compiler/runtime spans and central integration work in the same canonical
source tree. Maintained code and fixtures are committed as coherent pieces
complete. Compiler changes are exported into the tracked LLVM patch stack in
coordinated source checkpoints. There are no feature worktrees or ignored draft
implementations. The canonical LLVM authoring tree remains under the existing
ignored build root; exported patches are its version-controlled source inputs.

| Span | Authored implementation | Continued implementation |
| --- | --- | --- |
| Borrowed pointers | Contextual types, returned-owner contracts, strict CFG provenance/expiry/escape checking, mutable aliases, lifetime snapshots, capture provenance and native identity | Dynamic global initialization, carrier payload/alias precision, noescape definition proofs, temporary-owner materialization and remaining diagnostic boundaries |
| Explicit captures | Strong/weak/unowned/move lists, independent object and resource owners, C++ creation move vs heap copy, qualified ARC carrier moves, captured take/reset/discard | Source-defer ordering for managed fields, full-expression temporary failure handoff, remaining native-EH CFG and destruction boundaries |
| Async frames | Native C functions, methods and Blocks; lazy frame state machines; live storage, owned results/errors and child awaits; canonical frame-vs-task type identity and C++ frame bridge | Isolation entry integration, complete definition/storage boundary coverage and structured scope lowering |
| Isolation/Sendable | Actor/executor/nonisolated/task metadata, state/call/transfer checks, transitive Sendable proof, exact identities and non-suspending await checks | Actual awaited main/global/named entry, retained actor-instance association, public spawn and complete Strict Concurrency behavior |
| Typed key paths | Native property and record-field paths, exact-once access/writeback, immutable metadata, Root/Value identity, readonly narrowing and signature-only types | Public operation source representation/tooling, nullable intermediate traversal, dependent/effectful/isolated and ownership-bearing value boundaries |
| Structured runtime | Descendant cancellation, iterative deep-tree reclamation, terminal failure cleanup, canonical/named executors, concurrent global pool, service errors, shielded cleanup joins and explicit-destination children | Actor-object association, compiler-owned structured scope finalization and remaining reentrant/abandonment edges |
| Central integration | Resource capture CFG stages, precise lifetime owners, semantic ownership/isolation/key-path contracts, schema 2.1 physical async ABI identities and clangd source relationships | Package/profile projections, source-free transport, editor operations and combined feature integration |

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

1. Finish the current ownership, async, isolation, key-path and structured runtime
   slices together. Preserve live owners through suspension and exceptions;
   retain complete callable identity across source, PCH, import and packages.
2. Complete library-recognized structured/detached spawn and the standardized
   macro surface, with scope quiescence and Sendable diagnostics. Bind actor
   instances and executor entry to actual retained runtime destinations.
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
source commits. Validation is deliberately deferred while implementation advances.

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
build/test/qualification execution remains postponed. Once validation resumes,
run necessary checks for the combined change and broaden only to resolve new
failures or unresolved concerns.
