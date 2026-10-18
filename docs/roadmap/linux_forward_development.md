# Linux forward development

The active product is the Clang-only Linux x86-64 SDK. The
[acceptance receipt](../support/linux_sdk_release_acceptance.json) identifies its
exact archive, source, native execution and fresh-distribution evidence. Tool
versions come from [the manifest](../../toolchains/llvm/manifest.json); public
language claims come from [the capability authority](../../schemas/objc3-language-capabilities.json).
Historical custom-parser and Windows plans do not govern this development path.

## Next: complete the public synchronous core

Carry the qualified native Optional/Result and typed-error implementation through
its remaining ownership, transfer, assignment, storage and callable ABI bounds.
Managed MRC `try?`, weak/unsafe Block carriers and nested managed carriers still
need explicit semantics and lowering. Define the public bounded contract and
syntax together with their feature queries and capability predicates. Promote a
feature only when that contract has complete evidence.

Acceptance requires O0/O2 execution across functions, callbacks, Blocks, C++
members and Objective-C messages; ARC/MRC cleanup and native unwinding; installed
semantic interface identity; and live Linux editor behavior. Extend the installed
imports/managed-values/errors/cleanup sample as the public syntax becomes ready.
Private migration intrinsics do not justify a public feature claim.

## Installed module foundation and structured concurrency

The installed source example now rebuilds both providers and consumers, emits
reproducible dependency records, and reuses compilation through content identity.
O0/O2 qualification covers implementation and API invalidation, private header
edits with preserved timestamps, damaged artifacts and rejected package,
fingerprint and ownership locks outside the checkout. Fresh Debian and Ubuntu
archive qualification runs this suite as an ordinary user.

Generalize that bounded build graph into explicit multi-package dependency
resolution, transitive visibility and reproducible installation records. Define
public module syntax together with its language and interface contract. Keep
the qualified example distinct from a general package manager.

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
