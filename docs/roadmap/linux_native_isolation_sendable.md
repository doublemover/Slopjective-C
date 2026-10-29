# Native Linux isolation and Sendable contracts

This slice implements native declaration and transfer semantics from Part 7
§§7.3–7.8, the concurrency attribute catalog, and the actor declaration grammar.
The implementation is in the canonical Clang source under
`out/toolchains/objc3-development/src/clang`; the root compiler packet exporter
owns the corresponding maintained LLVM patch and manifest.

The native compiler regressions and all 48 isolation execution configurations
pass across C17/C++20/C++23, ARC/MRC providers and consumers, O0/O2 and source/PCH
transport. The same 48 configurations pass under ASan/UBSan and TSan, including
a C++ task caller entering a C provider's actor. The installed SDK gate and native
facade contracts pass; see the [integration acceptance receipt](../support/linux_native_feature_integration_acceptance.json).
Public actor capability claims remain gated.

## Declaration identity

`actor class Name : Base ... @end` creates a normal native Objective-C interface
with persistent `ObjC3ActorAttr` metadata. Actor ancestry is queried through the
native interface graph. The native declaration printer emits `actor class`, so
source-free interfaces retain the distinction without requiring a private source
attribute spelling.

The native compiler recognizes these canonical attributes:

| Attribute                          | Native contract                               |
| ---------------------------------- | --------------------------------------------- |
| `objc_executor(main)`              | Main executor affinity                        |
| `objc_executor(global)`            | Global executor affinity                      |
| `objc_executor(named("identity"))` | Exact named executor affinity                 |
| `objc_nonisolated`                 | Nonisolated actor instance method or property |
| `objc_unsafe_sendable`             | Explicit unchecked type admission             |
| `objc_task_spawn`                  | Child task API classification                 |
| `objc_task_detached`               | Detached task API classification              |
| `objc_task_group`                  | Structured group scope API classification     |
| `objc_actor_nonreentrant`          | Targeted v1 deferred-feature error            |

Named executor identities are nonempty ordinary string literal bytes, with no
embedded NUL, normalization, or case folding. The internal `named:` prefix is
metadata; emitted source uses canonical `named("...")` syntax and escapes the
name as a C string. No new `global_actor` spelling is introduced.

Visible source redeclarations inherit existing callable contracts. Conflicting
executor, isolation, and task classifications are errors. Source properties,
class extensions, and generated accessor identity participate in the same
contract. A nonisolated actor override can satisfy a previously unisolated
ordinary/protocol member; an isolated override cannot silently strengthen that
entry requirement.

Ordinary AST attribute serialization preserves metadata through PCH/AST files.
Native structural equivalence, AST import conflicts, and ODR hashes include the
required identities. Source-free module records additionally require
`objc3.isolation-contracts.v1`; the facade compares reconstructed actor,
executor, task, and unsafe-admission contracts. Executor/task declarations cannot
be converted to indirect callable values that would erase their metadata.

## Isolation checking

Native completed-callable checking covers Objective-C/C++ functions, methods,
and Blocks. It distinguishes the same `self`/`super` actor from another receiver,
compares lexical executor identities, and checks evaluated calls from C++ default
arguments and member initializers. A property accessor observes the property's
nonisolated modifier. An explicit member executor remains an independent entry
requirement on a nonisolated method; only the type's default affinity is removed.
Nested callable bodies are checked in their own context.

Cross-actor and cross-executor calls without `await` are diagnosed. Direct actor
ivar access requires the same isolated actor context; adding `await` to a raw
ivar expression does not create an actor entry. Nonisolated methods cannot read
or write actor storage through an implicit `self` proof.

Await operands are checked before native frame or C++ awaitable conversion.
Direct synchronous executor calls with qualified value arguments and results
now form a private native async-frame thunk. Its source arguments evaluate once
on the caller, and object/block arguments receive independent frame leases under
ARC and MRC. The child frame resolves the exact main/global/named executor,
checks dynamic current affinity, and hops and suspends before invoking the
original declaration when required. Destination lookup ownership is balanced
after the runtime has retained the hop destination. The parent retains its own
continuation destination while awaiting the child. Cancellation uses the native
frame cancellation channel, and lookup/hop failures use native service errors.
Checked throws are preserved and explicitly propagated inside the thunk.

Synchronous typed instance messages use the same private native frame, with
semantic bindings that evaluate the original receiver and arguments once before
frame creation. Actor entry obtains a retained mailbox from an explicit binding
or creates an object-owned automatic mailbox under serialized first-entry lookup.
A declared executor selects the initial backing executor and must match an
existing mailbox's immutable affinity. Nil receivers keep ordinary nil-message
behavior. Retired bindings and conflicting executor affinity produce native
service failures. The actor lookup lease is balanced after the runtime retains
the destination, while the frame owns its receiver. Association teardown cancels
the automatic mailbox without retaining its source object. Native association
classes inherit their ARC policy before the first retain/release operation.

Retainable results use independent native result ownership. ARC claims a +1
result or retains/copies a +0 result before copying into the task result owner;
MRC thunks adopt and balance synchronous retained-return conventions, including
retained method families. Ordinary native await results remain +0 until copied
into the enclosing result owner. Result Sendable requirements remain enforced.
An original async native-frame target is awaited inside the private child after
entry, so its continuation inherits the hopped destination while the original
caller keeps its own destination. Checked async errors retain the native checked
transport rather than constructing a C++ promise inside the private frame.

The [2.30.0 source milestone](linux_isolation_aggregate_values.md) extends the
original scalar boundary to native record values, ARC-managed record fields and
C++ parameter/result storage hooks. Owned C++ parameters move into the target
call; results retain the native frame's copy/destruction requirements. Resource
storage, unqualified nested leaves and MRC managed aggregates remain rejected.
The [2.31.0 source milestone](linux_isolation_consuming_ownership.md) adds ARC
consumed object/Block parameters and consuming instance receivers, with
cancellation/allocation-failure adoption and owning message-operand snapshots.
MRC/CF consumption and init-family entry remain gated. C++ targets must have a nonthrowing
native exception contract, including `nothrow` on Objective-C 3.0 methods; arbitrary C++ exception transport is not silently
synthesized. Nested isolated argument expressions require their own await.
The [2.32.0 source milestone](linux_isolation_class_entries.md) adds named class
messages, including inherited dispatch and ARC consumed parameters. The
[2.35.0 interoperability span](linux_task_frame_interop.md) adds supported
C++ task-ABI isolated targets; other unqualified callable shapes retain targeted
diagnostics. The
[2.33.0 source milestone](linux_isolation_super_entries.md) adds instance/class
super entry, preserving source class/category lookup, captured self and actor
mailbox entry across native frames and C++ task callers. The compiler does not automatically
associate object allocation with actor mailboxes. These remaining obligations prevent a full actor conformance claim.

A provably non-suspending native await evaluates its original operand and warns
in strict concurrency checking. The proof visits evaluated subexpressions,
rejects buried unhandled async calls, and leaves already lowered nested awaits
intact. In Objective-C++ the identity path is limited to builtin result types;
class and ADL-capable enumeration awaitables retain ordinary coroutine lowering.
Checked throws remain visible to the normal explicit-try checker. An unnecessary
await in a defer does not manufacture a suspension.

## Sendable transfer checking

The structural classifier admits C scalar/enumeration values, actor references,
`Sendable` protocol conformance (including protocol/class ancestry), arrays,
Optional/Result payloads, and transitive value aggregates. C++ implicit record
admission requires aggregate shape and either a trivial destructor or implicit
destruction composed entirely of admitted fields and bases. ARC-owned actor and
Sendable object fields therefore remain value-like; user-provided nontrivial
destruction requires explicit unsafe admission. Fields and bases are recursively
checked. Ordinary object references and block types do not gain
Sendable status from pointer constness.

Borrowed type markers are rejected before unsafe admission. Borrowed fields and
bases propagate a non-waivable failure through nested wrappers, including a
wrapper marked `objc_unsafe_sendable`. Resource/cleanup contracts remain
non-Sendable unless the type provides explicit admission; successful reliance
on unsafe admission produces a strict transfer-site note.

Enforcement covers cross-isolation arguments, logical results and typed errors;
nonisolated actor signatures; and recognized child/detached API work-item
arguments and captures. Argument checking retains the source type before
implicit conversion, so conversion to `id` does not erase a borrowed contract or
a known concrete type's Sendable conformance.

Concurrent capture checking uses the original captured declaration and explicit
capture mode and actual logical capture storage. An explicit strong snapshot
can establish owned capture storage from an unsafe source, while the original
declaration still supplies borrowed/resource provenance. Weak capture still
requires a Sendable referenced type. Unowned or
unsafe-unretained capture requires explicit unsafe admission. Shared `__block`
state and cleanup-managed captures do not gain safety from a scalar or hidden
physical field representation. Proven immutable local block literals are
checked recursively; unknown block values need an explicit unchecked wrapper or
are rejected in strict concurrency mode. Async work-item parameters and non-void
task-handle results are validated on spawn/detached declarations. Discarded child handles in expression
statements receive the Part 7 unused-handle warning; explicit void discard and
detached APIs express intentional discard. Additional synchronous block arguments
are checked for their captures without being mislabeled as async work items.

Group scope classification is persistent metadata and can coexist with a child
or detached API classification; child and detached classifications conflict.
This slice does not infer a group-add transfer contract from an arbitrary
function name, implement task creation from attributes, or relax the independent
borrowed/resource flow rules.
Dynamic invocation with incompatible known method isolation metadata is rejected
instead of selecting an arbitrary global method-pool candidate.

## Authored regression coverage

`tests/clang_foundation/isolation_contracts_test.cpp` and its native CMake target
cover C17 and Objective-C++20/23, ARC/MRC, same-actor/executor proof, actor storage,
nonisolated signatures, strict transfer diagnostics, unsafe notes, recursive
borrowed/resource aggregates, direct and macro spawn captures, metadata-erasing
pointers, source-free interface reconstruction, PCH serialization, native AST
import conflicts, and ODR hash identity. The native C async diagnostics fixture
also covers buried async operands and a non-suspending await inside defer.
Deferred IR fixtures cover main/global/named executor entry, independent object
argument ownership, one evaluation of source arguments, and preservation of
executor-entry thunks through PCH. C++ exception-boundary and unsupported result
shapes retain targeted failure fixtures.

`tests/clang_foundation/native_isolation/provider.h`, `provider.m`, and `client.m`
provide an installed executable witness for real executor entry, explicit actor
association, caller destination restoration, cancellation before target entry,
nil-message behavior and retained result cleanup. Build modes and executable
scope are described in the adjacent README.

The regression sources are not execution evidence. Public actor/strict
concurrency feature predicates remain subject to the repository capability
matrix and its executable-evidence requirements.
