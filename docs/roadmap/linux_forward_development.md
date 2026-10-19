# Linux forward development

The active product is the Clang-only Linux x86-64 SDK. The
[acceptance receipt](../support/linux_managed_call_acceptance.json) identifies its
exact archive, source, native execution and fresh-distribution evidence. Tool
versions come from [the manifest](../../toolchains/llvm/manifest.json); public
language claims come from [the capability authority](../../schemas/objc3-language-capabilities.json).
Historical custom-parser and Windows plans do not govern this development path.

## Next: complete the public synchronous core

Carry the qualified native Optional/Result and typed-error implementation through
its remaining ownership, transfer, assignment, storage and callable ABI bounds.
Explicit local ARC copy/move construction is now qualified for direct strong,
weak and unsafe object leaves and strong Blocks, including const copies, weak
zeroing, typed-error optionalization and native unwinding. Source tags survive
moves; mutable strong/weak payloads are cleared by native ARC initialization.
Implicit direct-carrier copies and assignment now extend that ownership contract
to Objective-C and Objective-C++. Assignment snapshots the selected RHS before
evaluating the LHS or releasing old storage, preserves source values under
copying, and returns the actual destination lvalue in Objective-C++. Installed
O0/O2 witnesses cover all Result arm transitions, self/aliased assignment, weak
zeroing, Blocks, typed-error optionalization, sequencing and native unwinding.
Live clangd probes cover both languages. The
[managed call ABI](linux_managed_call_abi.md) now extends direct carriers through
ordinary functions, callbacks and C++ members under extension ABI revision 3.
Caller-owned snapshots preserve each argument at evaluation time and clean up
at full-expression boundaries; returns initialize fresh caller-owned storage.
Nine installed executions cover separate provider/consumer compilation at
O0/O2/ThinLTO, PCH and semantic module imports, including virtual adjustment
thunks, register exhaustion and native exception cleanup. Nested storage remains
separate implementation work.
Managed MRC `try?`, weak/unsafe Block carriers and nested managed carriers still
need explicit semantics and lowering. Define the public bounded contract and
syntax together with their feature queries and capability predicates. Promote a
feature only when that contract has complete evidence.

Acceptance requires O0/O2 execution across functions, callbacks, Blocks, C++
members and Objective-C messages; ARC/MRC cleanup and native unwinding; installed
semantic interface identity; and live Linux editor behavior. Extend the installed
imports/managed-values/errors/cleanup sample as the public syntax becomes ready.
Private migration intrinsics do not justify a public feature claim.

The next implementation sequence is:

1. Extend managed callable ownership into Objective-C messages and Block value
   signatures with an explicit runtime encoding and copy/dispose contract, then
   compose managed signatures with typed-error success/error transport. Preserve
   the qualified caller-owned snapshot and return-storage rules through each
   callable form; reject unsupported conventions before emitting a call.
2. Extend active-arm lifetime operations through nested carriers, arrays and
   enclosing records. Cover partial initialization, assignment, destruction and
   exceptions before admitting those storage forms to the callable ABI.
3. Finish the bounded public Optional/Result and synchronous throws contract,
   including ownership and module interface identity. Add public syntax and
   promote its feature predicates only after that complete contract passes the
   installed Linux suite.
4. Publish immutable packages from the qualified interface and object inventory,
   with explicit ownership projections and consumer verification. Keep registry
   transport separate from content identity and installation correctness.

## Installed module foundation and structured concurrency

The installed source example now rebuilds both providers and consumers, emits
reproducible dependency records, and reuses compilation through content identity.
O0/O2 qualification covers implementation and API invalidation, private header
edits with preserved timestamps, damaged artifacts and rejected package,
fingerprint and ownership locks outside the checkout. Fresh Debian and Ubuntu
archive qualification runs this suite as an ordinary user.

The offline source-package builder now resolves explicit multi-package identity
locks, public/private transitive visibility and a shared dependency closure. It
retains content-verified compilation records and a reproducible package
installation inventory; the installed suite covers ARC/MRC at O0/O2. See the
[source-package contract](linux_source_packages.md). Next, define public module
syntax together with its language and interface contract, then add immutable
package publication and explicit cross-ownership adaptation. The
[package milestone acceptance receipt](../support/linux_source_graph_acceptance.json)
records that milestone's installed and ordinary-user distribution results. The
current SDK receipt above repeats those checks for the active archive.
Remote registries and general package management remain separate work.

Build task groups, cancellation and actor isolation on that module and ownership
foundation. Existing independent executor services are useful implementation
components; they are not evidence that async/await or actors are public language
features. Require deterministic cancellation/error/cleanup tests, native threaded
execution and appropriate sanitizer evidence before promotion.

## Linux architecture and release performance qualification

Add AArch64 as a separate host/runtime/ABI milestone. Cross-compilation and
emulation are useful probes; the support declaration requires a real native host
running the installed archive, ownership, interface, editor and debugger suite.
Keep x86-64 support claims separate until that evidence exists.

Compare ordinary development builds with dedicated release ThinLTO/PGO builds
using representative compiler and runtime workloads. Record clean and incremental
builds, edit-to-test latency, cache hits, runtime throughput and memory scope.
Current command-latency measurements cover a small C program; they do not replace
cold LLVM build or application runtime throughput measurements. The
[latency report](../support/linux_development_latency.json) provides repeatable
warm measurements and budgets for its recorded CPU model and quota.

Refresh stable upstream releases at each milestone boundary, then freeze and
qualify one coherent SDK. Keep each milestone reviewable and committed, update
its acceptance receipt, and derive repeated documentation from the authorities.
