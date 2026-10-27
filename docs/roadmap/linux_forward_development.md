# Objective-C 3.0 on Linux

The target is Objective-C **3.0 as defined by `spec/`**, implemented in the pinned
native Clang toolchain and GNUstep 2.2 runtime. C17 and C++20/23 are underlying
source modes. Linux x86-64 is the active implementation target. Windows,
compatibility with retired frontends and rollback variants are outside this plan.

The [capability authority](../../schemas/objc3-language-capabilities.json) owns
public compiler claims; the [library inventory](../../stdlib/module_inventory.json)
records implemented and remaining module contracts. Source implementation,
validated compiler behavior and installed SDK delivery are separate checkpoints.
The current **2.33.0 SDK** delivers source milestones 2.10–2.33, including
C++ executable packages, selected-arm captures, export roots/local types,
aggregate/consuming/class/super isolation and expanded record-field key paths. Semantic interface schema **2.2** is a hard cutover;
regenerate dependent objects, PCH, interfaces and packages.

The verified LLVM foundation passes **375 compiler regressions** (three platform
cases unsupported) and **179 native unit tests**. This delivery qualifies that exact
foundation and passes all **33 frontend suites**, nine GNUstep client tests,
37 installed consumer commands, 13 native integration nodes and all **33 installed
SDK gates**. The installed five-family matrix passes **168 configurations and
1,110 commands**. Compiler source identity is
`llvmorg-23.1.3-0d261d1ca552-e3f042ad7bc3-00b75dc3`.

The [current acceptance receipt](../support/linux_source_batch_2_33_acceptance.json)
binds the exact archive, compiler/runtime identities and qualification reports.
That archive passes the native-feature suite on fresh **Debian 13** and
**Ubuntu 26.04** as ordinary users, including the complete five-family matrix,
expanded key-path packages, relocation/reinstall, embedding and debugger audits.
Both owned containers are removed. The
[combined source receipt](../support/linux_source_batch_2_33_progress.json)
records the complete normal, ASan/UBSan and TSan matrices, each with
168 configurations / 1,110 commands, plus 287 tooling tests.
Earlier milestone sections retain their historical source-checkpoint status;
the acceptance receipt above supersedes their deferred-delivery statements.
Full language/profile conformance and native Linux AArch64 remain open.

The [System 2.10–2.12 source receipt](../support/linux_captured_callable_ownership_progress.json)
records **398 compiler regressions**, **190 native unit tests**, all **33 frontend
suites**, **15 tooling tests** and **24 source-free package configurations /
96 commands** across normal, eager-import and ASan/UBSan execution. Three platform
cases remain unsupported. These source checks include mutable and record-captured
consuming closure publication; SDK 2.33 remains the accepted archive.

The [current operand/fill source receipt](../support/linux_operand_fill_ownership_progress.json)
records **400 compiler regressions**, **190 native unit tests**, all **33 frontend
suites** and **331 tooling tests** (three Windows/PowerShell checks skipped).
Normal, ASan/UBSan and TSan each pass the complete five-family source/PCH matrix:
**168 configurations / 1,158 commands per mode**. Source-free packages pass
**24 configurations / 96 commands** across normal, eager-import and ASan/UBSan
execution, including builtin bodies, complete fills and counted captured releases.
SDK 2.33 remains the accepted archive; installed delivery follows this batch.

## Known memory-fill ownership effects

The [System 2.13.0 source span](linux_borrowed_memory_fills.md) recognizes known
memory-fill destinations as nonescaping writes and clears obsolete provenance
only after a proven complete direct-local overwrite. Partial, uncertain and
wildcard sibling facts remain conservative; owner overwrites expire old loans.
Broader checks remain batched with the isolation operand span.

## Isolation operand ownership

The [concurrency 2.37.0 source span](linux_isolation_operand_snapshots.md) gives
isolated function arguments and message operands source-ordered ARC/MRC object
and Block snapshots. Shared precise snapshot cleanup protects later-argument
replacement, native exceptions, cancellation and failed entry allocation.
Broader validation follows the implementation batch; SDK 2.33 remains accepted.

## Captured callable paths

The [System 2.12.0 source span](linux_borrowed_callable_paths.md) forwards known
invocation effects through callable fields and array elements of captured records.
Only invoked paths contribute effects; nested captures, aggregate init-captures,
constant/dynamic indices and structural ARC copies share the same storage facts.
The combined source receipt above records its native/frontend/package checks.

## Mutable lambda capture precision

The [System 2.11.0 source span](linux_borrowed_mutable_captures.md) admits mutable
synchronous lambdas whose lifetime-sensitive captures stay stable while ordinary
scalar state changes. CFG summaries qualify writes and address/reference exposure,
including ARC captures keeping another loan alive. Broader checks remain batched
after implementation; SDK 2.33 is the accepted archive.

## Consumption through local closures

The [System 2.10.0 source span](linux_borrowed_closure_consumption.md) propagates
known captured-owner consumption through local Block/lambda invocations and
invoked callable captures. Separate effect facts preserve non-invoking copies,
local aggregate/array aliases and ARC callback writeback provenance. Broader
checks remain batched after implementation; SDK 2.33 is the accepted archive.

## Canonical retainable C families

The [System 2.9.0 source span](linux_retainable_families.md) implements canonical
family and operation attributes, validates aliases and redeclarations, enforces
integrated-type ARC restrictions, and records release effects and analyzer
summaries. PCH/import, declarations and source-free interfaces retain contracts.
SDK 2.33 remains the accepted archive.

The [combined System 2.7–2.9 source receipt](../support/linux_closure_family_ownership_progress.json)
records **395 compiler regressions**, **190 native unit tests**, all **33 frontend
suites**, **15 tooling tests** and **24 source-free package configurations /
96 commands** across normal, eager-import and ASan/UBSan execution. Three platform
compiler cases remain unsupported. Broader combined runtime/sanitizer validation
and SDK delivery follow a larger implementation batch.

## Evaluated borrowed getter owners

The [System 2.8.0 source span](linux_borrowed_getter_snapshots.md) snapshots loans
when stable getter owner arguments are evaluated. Later argument replacement or
consumption expires that original binding, while independent pins protect it.
Conditional/comma selection and message/operator dispatch share the same flow.
SDK 2.33 remains the accepted archive.

## Owners used inside later-invoked closures

The [System 2.7.0 source span](linux_borrowed_future_captures.md) caches symbolic
CFG summaries of concrete lambda/Block owner use. Local aliases and copied/init/
nested captures retain future borrowing requirements; consumption invalidates
later invocation while independent owners and pins preserve qualified calls.
SDK 2.33 remains the accepted archive.

## Block lifetime copy and unwind

The [System 2.6.0 source span](linux_borrowed_block_pins.md) models Block pin
acquisition and ARC consumed-argument copies as separate throwing CFG operations.
Failed copies unwind only completed bindings and preserve pre-entry owner claims.
Callable ownership checks also follow arrays and deep pointer containers.
SDK 2.33 remains the accepted archive.

## Foreign consumption and Block owners

The [System 2.5.0 source span](linux_borrowed_foreign_consumption.md) preserves
foreign consumed-parameter types under ARC/MRC, expires known CF owner claims
and tracks consumed Block owners. Bound C++ member calls retain consumed and
noescape contracts. Native structural equivalence, ODR identity and source-free
imports preserve callable metadata. SDK 2.33 remains the accepted archive.

## Callable ownership contracts

The [System 2.4.0 source span](linux_callable_ownership_casts.md) preserves MRC
retained callback results and checks explicit function/member-pointer casts,
including nested callbacks. Consumed/retained obligations cannot be erased and
noescape/borrowed permissions cannot be invented. PCH and source-free imports
retain ownership identity. SDK 2.33 remains the accepted archive.

## Annotated consumed owners and borrowed loans

The [System 2.3.0 source span](linux_borrowed_consumed_owners.md) adds annotated
MRC object-parameter/receiver consumption, typed function/Block callbacks and
C++ calls. Evaluated owner identities survive later argument effects; independent
pins protect loans and overlapping consumption is rejected. MRC consumed types
retain their canonical ownership contract. SDK 2.33 remains the accepted archive.

## Manual owner release and borrowed loans

The [System 2.2.0 source span](linux_borrowed_manual_release.md) tracks manual
instance-release messages, known aliases and conditional owner-claim state.
Earlier lifetime pins remain independent; late pins and expired new acquisitions
are rejected. Nested Block/lambda bodies inherit that state. Other manual and
foreign consumption effects remain open. SDK 2.33 remains the accepted archive.

## Borrowed C++ lambda captures

The [System 2.1.0 source span](linux_borrowed_cpp_captures.md) preserves borrowed
provenance through qualified C++ lambda value captures and nested bodies.
Local/copy/noescape calls retain frozen aliases and ARC owner snapshots; hidden
escapes, expired calls, persistent capture stores and future reference/address
mutations are rejected. Mutable/reference/generic precision remains bounded.
SDK 2.33 remains the accepted archive.

## Published SDK runtime awaiters

The [2.36.0 source span](linux_published_runtime_awaiters.md) admits pinned SDK
runtime declarations in public signatures and concrete inline async bodies.
Checkpoint/yield, executor/actor hops, task handles and completion-order groups
retain written dependencies under required runtime-declaration semantics.
Application-defined opaque types and changed runtime headers remain rejected.
SDK 2.33 remains the accepted archive.

## Task and frame interoperability

The [2.35.0 source span](linux_task_frame_interop.md) adds native-frame awaits of
C++ tasks and supported task-ABI isolation targets. It preserves qualified
records, ARC ownership, checked errors, actor/executor destinations and lexical
class/super dispatch through the shared owned carrier. Focused compiler,
frontend, source-free package and sanitizer witnesses pass; SDK 2.33 remains the
accepted installed archive.

## Published async definitions

The [2.34.0 source span](linux_published_async_bodies.md) publishes concrete
inline native-frame and ordinary C++ task bodies around logical async calls and
isolation entries. Source projection retains written dependencies, native import
preserves coroutine/template origins, and backend registration supplies imported
helper definitions. Required async-definition semantics and exact task-header
provenance bound the contract. SDK 2.33 remains the accepted installation.

Earlier bounded implementation and qualification checkpoints follow.

The preceding [2.7.0 frame-binding receipt](../support/linux_cxx_frame_borrow_acceptance.json)
retains its source/PCH reference/receiver, dynamic-storage, scope and frame-value
matrix: 24 configurations / 132 commands in each normal, ASan/UBSan and TSan mode
(72 / 396 total), including resumed-parameter debugger probes.
The preceding [2.6.0 dynamic-storage receipt](../support/linux_dynamic_frame_storage_acceptance.json)
retains its 72 / 396 source and 168 / 966 installed matrices.
The preceding [2.5.0 frame-value receipt](../support/linux_cxx_frame_value_acceptance.json)
retains its 72 / 324 source and 168 / 942 installed matrices.
The preceding [2.4.0 scope receipt](../support/linux_cxx_group_scope_acceptance.json)
retains its broader source 156 / 842 matrices and standalone service suites.
Earlier source `49f322e67e0d` retains both complete five-family sanitizer matrices
(336 / 1,884), and earlier tooling retains its original scopes. Historical and
overlapping batches are not summed. Public async/actor predicates and full
Core/Strict/System conformance remain reserved.

## C++ local lambda expressions

The [2.21.0 source milestone](linux_cxx_lambda_expressions.md) binds local
non-generic closures, capture/member identities and referenced dependencies in
inline/default contracts. SDK/archive work and broad frontend/sanitizer
validation remain deferred.

## C++ namespace attributes and policy

The [2.20.0 source milestone](linux_cxx_namespace_attributes.md) binds modeled
namespace attributes, contextual visibility and inherited namespace SPI policy.
Native import and printing preserve attributed extensions. SDK/archive work and
broad validation remain deferred.

## C++ immediate defaulted members

The [2.19.0 source milestone](linux_cxx_immediate_defaulting.md) binds consteval
defaulted members and explicit out-of-line constexpr state, preserving native
constant evaluation across package boundaries. SDK/archive work and broad
validation remain deferred.

## C++ out-of-line defaulted special members

The [2.18.0 source milestone](linux_cxx_out_of_line_defaulting.md) binds native
user-provided special-member ABI and preserves out-of-line defaulted definitions
with their namespace and module-owned symbols. SDK/archive work and broad
validation remain deferred.

## C++ defaulted comparisons

The [2.17.0 source milestone](linux_cxx_defaulted_comparisons.md) binds native
member and hidden-friend comparisons, associated record export/SPI policy and
argument-dependent lookup. Broad source and SDK/archive validation remain
deferred for the source batch.

## Eager runtime-bearing semantic imports

The [2.16.0 source milestone](linux_eager_semantic_imports.md) parses the pinned
runtime header before eager declaration merging and configures source lookup
with resolver-verified complete lock identities. It supports source-free eager
runtime consumers alongside source-directed consumers. SDK/archive delivery and
broad validation remain deferred.

## C++ inline and constant definitions

The [2.15.0 source milestone](linux_cxx_definitions.md) preserves concrete inline,
constexpr and consteval bodies, constructor initializers and inline/constant
data. Typed body contracts use lexical local identities and explicit dependency
policy. SDK/archive work and broad validation remain deferred.

## C++ default arguments and member initializers

The [2.14.0 source milestone](linux_cxx_declaration_expressions.md) binds concrete
declaration expressions, native reference targets and required export/SPI
boundaries. Member defaults also participate in containing record identity.
Broad source and SDK/archive validation remain deferred for the source batch.

## C++ using declarations and namespace lookup

The [2.13.0 source milestone](linux_cxx_using.md) adds namespace/member using
declarations, overload introductions, inherited constructors and namespace
using-directive lookup. Explicit target identities preserve export/dependency and
SPI boundaries. SDK/archive work and broad validation remain deferred.

## C++ nested types and namespace visibility

The [2.12.0 source milestone](linux_cxx_nested_types.md) adds nested named
classes, enums and aliases, native nested access control and source-free nested
frame values. It also prevents public namespaces from exposing ungranted SPI or
private dependency members, while supporting later explicit grant promotion.
SDK/archive delivery and broad validation remain deferred for the source batch.

## C++ namespace exports

The [2.11.0 source milestone](linux_cxx_namespaces.md) adds named/nested/inline
namespace and namespace-alias exports, complete scoped identity, selective
publication and source-free namespaced native-frame consumers. It builds on the
virtual-class source span below. SDK 2.9.0 remains active; broad validation and
installed/archive delivery remain deferred for the combined implementation batch.

## C++ virtual class packages

The [2.10.0 source milestone](linux_cxx_class_dispatch.md) adds virtual bases,
shared diamonds, pure/final/covariant overrides, virtual destruction and
module-owned RTTI/vtable/type names. Source-free fixtures combine dynamic casts,
virtual receiver adjustment and derived native-frame arguments/results. SDK
2.9.0 remains active; broader validation and SDK delivery are deferred until
the source implementation batch is coherent.

Error-constrained factories already pass the accepted SDK's 36 positive/negative
strict-profile checks. The errors module inventory now records that implemented
contract; full Core/Strict profile conformance remains open.

## Nonvirtual C++ bases and defaulted members

The [accepted 2.9.0 delivery](linux_cxx_class_structure.md) adds nonvirtual single and
multiple inheritance, empty-base layout, access-aware base identity and in-class
defaulted special members. It also repairs native printer/importer state needed
for stable defaulted APIs. New execution fixtures cover nontrivial derived frame
values and receiver adjustment. Source validation, all 33 installed SDK gates and
focused exact-archive Debian/Ubuntu qualification pass. Source-free immutable
native async packages pass 12 modes / 36 executions on the host and each fresh
distribution, including eight C++ class structure modes.

## Deleted C++ aggregate member identity

The [2.8.1 delivery](linux_cxx_deleted_members.md) binds deleted ordinary
methods on public aggregates to the lifecycle capability and semantic fingerprint.
It closes a reproduced cache-identity collision. The corrected source-free package
fixture requires the imported method to remain unavailable. All source
preflights, installed gates and focused exact-archive distribution
checks pass. The required capability and canonical deletion state prevent the
cache collision; ordinary aggregate method identities remain unchanged.

## Nonvirtual C++ class lifecycle packages

The [2.8.0 delivery](linux_cxx_class_lifecycles.md) adds bounded nonvirtual
class publication with private layout, out-of-line lifecycle/conversion members,
deleted declarations and module-owned constructor/destructor variants. Source-free
consumers execute nontrivial frame results and move-only parameters. Required
capability and overload-collision checks preserve the complete import contract.
Source qualification passes 24 cases / 96 commands across normal, ASan/UBSan
and TSan modes. All installed gates and exact-archive distribution checks pass.
Inheritance, virtual/inline/template class projections retain their separate bounds.

## C++ borrowed native-frame bindings

The [2.7.0 delivery](linux_cxx_frame_borrows.md) admits C++ reference parameters
and implicit receivers under ordinary coroutine borrowing rules. Independent
pointer bindings survive suspension without copying or destroying referents.
Virtual/member-pointer dispatch preserves adjusted `this`; C++23 explicit-object
values retain independent owned capture semantics. Captured parameter debug
locations now follow frame storage with the original argument identity. LLDB
inspects both `this` and a reference argument after suspension.

All source/PCH instrumentation, rebuilt foundation, installed gates and exact
archive distribution checks pass. Source-free C++ package consumers exercise
reference parameters and const/ref-qualified aggregate member receivers after
provider removal and relocation. The 2.8.0 delivery adds bounded nonvirtual
class lifecycle projections;
reference results retain the shared value/void limit.

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
instrumented lifetime witnesses cover both repairs. Nontrivial class semantic
package projections remain open. Borrowed C++
references/receivers are delivered above; dynamic storage
is delivered below.

## C++ coroutine group scopes

[C++ task-coroutine scopes](linux_cxx_group_scopes.md) extend the compiler-owned
protocol to ordinary coroutine syntax and native C++ async bodies. The current
concurrency module is 2.7.0. A scope-aware initial awaiter reserves the guard
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
receipt binds the 2.7.0 reference/receiver span, installed gates and fresh-distribution
evidence separately.

## Native C-frame group scopes

[Native C-frame group scopes](linux_native_group_scopes.md) provide implicit
finalization for functions and Objective-C methods marked `objc_task_group`.
The accepted SDK concurrency module is **2.9.0**. A fresh group admits only structured
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
module shipped as 2.2.0 and is retained by the 2.7.0 SDK. Awaited ARC carriers
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
implemented. The concurrency 2.7.0 headers and services are included in the current
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

| Span                | Authored implementation                                                                                                                                                                                                                                                                                                                                          | Continued implementation                                                                                                                       |
| ------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| Borrowed pointers   | Contextual types, returned-owner contracts, strict CFG provenance/expiry/escape checking, nested aggregate/array paths, finite alias joins, guard/match forwarding, representation copies and noescape/constructor checks                                                                                                                                        | Remaining conservative alias/union/MRC/lambda boundaries documented in the feature roadmap                                                     |
| Explicit captures   | Strong/weak/unowned/move lists, independent object/resource owners, C++ creation move vs heap copy, ARC/MRC plain, pure C++ and ARC mixed selected-arm carrier moves, captured take/reset/discard, nested resource transfer and defer/exception rollback                                                                                                         | C++ lambda/MRC mixed and unqualified carrier moves, throwing resource disposal and general exceptional CFG ownership                           |
| Async frames        | Native C functions/methods/Blocks; lazy frames, owned results/errors, task/group joins, typed iteration and compiler-owned C-frame scopes, allocation failure transport, suspension-safe pools, typed-effect deduction, heap-owned dynamic storage, borrowed C++ bindings, captured-parameter debug locations and class lifecycle/base/virtual dispatch packages | Concrete inline/constexpr/template class projections and broader profile admission                                                             |
| Isolation/Sendable  | Persistent contracts, transitive Sendable checks, awaited main/global/named and actor-instance entry, retained actor binding, executor proof, scalar/void, retained object and qualified record results; ARC consuming entries and owning operand snapshots                                                                                                      | MRC/CF consumption, unqualified value forms, unqualified task call forms, automatic actor association and complete Strict Concurrency behavior |
| Typed key paths     | Property/record paths, precise operand snapshots/writeback, native getter ownership, nullable readonly object traversal, immutable metadata, complete class/protocol identity and source tooling                                                                                                                                                                 | Nullable writes, scalar/aggregate nullable-chain results, dependent/effectful/isolated paths and ownership-bearing values                      |
| Structured runtime  | Descendant cancellation, deep-tree reclamation, canonical/named executors, allocation-independent errors, shielded cleanup joins through resource destruction, retained actor binding, C/C++ scope guards, owned callback groups and recognized spawn APIs/macros                                                                                                | Complete abstract group library and broader profile admission                                                                                  |
| Central integration | Resource capture CFG, precise lifetime owners, public parameter/return ownership contracts, schema 2.2 physical async ABI/export-root identities, clangd source relationships and development feature runner                                                                                                                                                     | Remaining package/profile projections and full profile conformance                                                                             |

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
schema 2.2 require explicit physical async ABI and export-root identity; old interfaces/packages
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
without an SDK archive rebuild. The installed SDK passes all 168 configurations.
Current development compiler/runtime instrumentation covers the 24-configuration
spawn/frame subset described above. Mixed async provider/consumer ownership, frame
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

The current [2.33.0 acceptance receipt](../support/linux_source_batch_2_33_acceptance.json)
binds all 33 installed gates, 33 frontend suites, 37 consumer commands and 13
native integration nodes. Its archive has seal
`98855cbe62e5ddbaa74bde0a24a16a8d7c8baeb11831f4e31408b48d07644cb7`,
439,265,009 bytes and SHA-256
`f73ea1b7d57862d199b8ada3eb7fef4256f7ce4981a482db0167643c56c15f21`.

The installed release passes all 168 configurations / 1,110 commands. The
combined source batch also passes that complete matrix under ASan/UBSan and
TSan, with 287 tooling tests. The exact archive passes native-feature
qualification as ordinary users on fresh Debian 13 and Ubuntu 26.04, including
the five-family matrix, expanded source-free key-path packages, compiler
witnesses, relocation/reinstall, embedding and debugging. Both owned containers
are removed. The superseded 2.29 SDK installation and archive are reclaimed;
one installed SDK and one archive remain alongside the active development build.
Earlier receipts retain their historical scopes.

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

The [2.22.0 source milestone](linux_cxx_thread_local.md) preserves C++ thread-local
global/class-static/inline data and local static storage under
`objc3.cxx-thread-local.v1`, including native per-thread lifetime and shared
provider/consumer storage.

The [2.23.0 source milestone](linux_cxx_friends.md) preserves concrete friend
functions/types, native access and ADL, including hidden and out-of-line inline
definitions, under `objc3.cxx-friends.v1`.

The [2.24.0 source milestone](linux_cxx_local_expressions.md) preserves published
Block bodies/captures and GNU statement expressions under required
`objc3.cxx-block-expressions.v1` and `objc3.cxx-statement-expressions.v1` contracts.

The [combined 2.24.0 source validation](../support/linux_source_batch_2_24_progress.json)
passes the native/frontend/C++ package batch, all five-family source/PCH feature
matrices in normal/ASan/UBSan/TSan modes, compiler witnesses and async/task/
structured-runtime instrumentation. Exception cleanup additionally repairs and
rebuilds the pinned GNU runtime's DWARF decoder; full-width LEB values use defined
unsigned operations. Installed/archive delivery and the implementation backlog
remain separate obligations.

The [2.25.0 source milestone](linux_explicit_capture_lists.md) adds ARC/MRC plain
and pure C++ carrier captures, recursive nested C++ binding snapshots and stable
coalescing expression contracts. Its [focused evidence](../support/linux_carrier_captures_progress.json)
records 373 native lit and 179 unit checks, three facade suites, normal and
ASan/UBSan source/PCH capture matrices, and eight source-free C++ package cases.
Mixed ARC/C++ captures and the installed SDK/archive cutover remain pending.

The [2.26.0 source milestone](linux_explicit_capture_lists.md) composes explicit
mixed ARC/C++ captures with separate cleanup partitions, owned match snapshots
and nested binding copies. Its [focused evidence](../support/linux_mixed_carrier_captures_progress.json)
records normal/ASan source/PCH capture matrices, native regressions, AST/PCH
persistence and source-free package checks. Implicit and MRC mixed captures,
unqualified carrier leaves and the installed SDK cutover remain pending.

The [2.27.0 source milestone](linux_explicit_capture_lists.md) adds native plain
carrier leaf captures, shared complex copy/construction lowering and implicit
mixed ARC/C++ Block capture ownership. Its [focused evidence](../support/linux_native_carrier_captures_progress.json)
records normal/ASan source/PCH capture matrices, four recursive mixed runtime
cases, AST/PCH persistence and eight source-free package cases. Mixed C++ lambda
captures, MRC mixed ownership and the installed SDK cutover remain pending.

The [2.28.0 source milestone](linux_immutable_packages.md) binds native export
roots in semantic interface schema 2.2. Binary packages validate scoped roots,
class members and friends directly; anonymous C typedefs retain their owned tag
definitions. The [focused receipt](../support/linux_native_export_roots_progress.json)
records three frontend suites, 179 tooling tests and eight source-free C++
package configurations with real inventory verification and offline installation.

The [2.29.0 source milestone](linux_cxx_local_types.md) adds concrete named local
classes and enums in published definitions, including nested types, member
bodies, native lifecycle, virtual dispatch, RTTI and constexpr evaluation. Lexical
identities replace physical file-based references; local types remain excluded
from exported signatures. Templates, generic/public closures and the broader
ownership, isolation, key-path and profile backlog remain open.

The [2.30.0 source milestone](linux_isolation_aggregate_values.md) admits awaited
executor/actor entry for qualified record values, ARC-managed aggregate fields
and C++ copyable results/move-only parameters. Native frame storage hooks retain
construction/destruction contracts; xvalue forwarding avoids hidden copies.
Maintained witnesses add source-free record contracts, actor/caller restoration,
cancellation and failure at successive C++ moves. The 2.33.0 SDK delivers this boundary.

The [2.31.0 source milestone](linux_isolation_consuming_ownership.md) adds ARC
consumed object/Block parameters and consuming instance receivers at supported
awaited entries. Owning receiver/argument snapshots preserve message evaluation
when later operands clear their original owners, with balanced exceptional
cleanup. Source 2.35.0 adds task-ABI targets and preserves the actual consumed
Block parameter annotation. MRC/CF consumption and init-family entry remain
open; SDK 2.33.0 delivers the preceding object ownership boundary.

The [2.32.0 source milestone](linux_isolation_class_entries.md) adds named class
messages at qualified executor entries, preserving inherited class dispatch,
retained results and consumed arguments. Focused source/PCH runtime and native
frontend/codegen checks pass; combined validation and SDK delivery follow the
implementation batch. Super messages are added by the subsequent 2.33.0 source milestone;
unqualified dynamic Class receivers remain separate work.

The [keypath 2.2.0 source extension](linux_keypath_record_fields.md) adds named
bit-fields and inherited C++ record fields, with native access/lookup, base
adjustment, signed/truncated values and exact-once property writeback. Initial
native/runtime checks pass; combined validation and SDK delivery follow this
source batch.

The [2.33.0 source milestone](linux_isolation_super_entries.md) adds awaited
instance/class/category super messages, preserving lexical superclass lookup
with independently owned self. Actor-super entry, ARC consuming receivers,
record/retained results and ordinary C++ task callers are covered by maintained
runtime and PCH witnesses. Combined validation and SDK 2.33.0 delivery pass.
