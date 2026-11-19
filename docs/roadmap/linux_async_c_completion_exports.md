# Native async exports to C

Source 2.293 / System 2.219 admits fully populated 64-bit and 128-bit
generic C vectors through generated checked and async entries on Linux
x86-64 and AArch64. The generated headers preserve GNU `vector_size` and
Clang `ext_vector_type` declarations. Wider and target-specific vector types
still need their own calling convention contracts.

Native x86-64 C17/C23 callers exercise exact checked results, original error
identity and asynchronous vector callbacks across ARC/MRC and all provider
standards. Source-free imports preserve the declarations. Four AArch64 QEMU
clients verify both vector widths with the AArch64 GNUstep and service runtime.
Native AArch64 archive qualification remains pending.

Source 2.240 / System 2.166 implements explicit C completion exports for
native async frames. The SDK header `<objc3c/runtime/objc3_async_c.h>` declares a
typed `f_completion_t` and `f_async_c` from the original parameter list and
result type. Objective-C and Objective-C++ providers use
`<objc3c/runtime/objc3_async_c_export.h>` to define the corresponding symbol.

```objc
#include <objc3c/runtime/objc3_async_c_export.h>

OBJC3_ASYNC_C_DECLARE(increment, int, int value);

static int increment(int value) async { return value + 1; }
OBJC3_ASYNC_C_DEFINE_VALUE(increment, int, increment(value), 0, 0, 0, int value)
```

A C consumer includes the shared declaration and passes the original arguments,
an opaque context and a typed callback. Use the void declaration and definition
forms to omit the result parameter. The object and Block definition forms load
the corresponding owned pointer representation. The provider validates the
native result descriptor before it submits the frame. The explicit checked
argument describes the source effect. Pass zero executor and actor arguments
to select the global executor, or select one destination explicitly.

Successful submission clears the carrier before scheduling. The scheduler holds
the task and its native result through one callback. The callback may run before
the thunk returns. It receives the original opaque context, which the caller
owns. Retain objects and errors and copy Blocks inside the callback to keep
them afterward. Unconsumed MRC source parameters retain their borrowing
contract, so the caller keeps those inputs valid while the frame uses them.

Status values are 0 for success, 1 for error and 2 for cancellation. Successful
callbacks receive no error. Checked Objective-C errors reach the callback as
their original objects. Other checked failure transport maps to a stable
Objective-C error value. Cancellation carries the distinct process-wide
`Error` value recognized by `objc3_async_c_error_is_cancellation`. If nonthrowing
work cannot start or fails inside the scheduler, its callback records
cancellation. A null callback or a mismatched frame result violates the export
contract.

The native test compiles providers as Objective-C C17/C23 and Objective-C++
C++20/23 in ARC and MRC. It removes the provider source before compiling the
separate C caller. The caller checks the aggregate and void callback signatures,
retained object, Block and checked error lifetimes, nil success, cancellation
before entry, a closed actor and 256 global-worker callbacks whose contexts are
freed during delivery. All eight provider/consumer configurations pass locally.

Source 2.241 / System 2.167 publishes these typed declarations in canonical
semantic interfaces. The sidecar binds the exact public completion header.
Import reconstructs that header before merging callable declarations, including
the callback's native argument order, status enum and Objective-C error type.
An unowned C ABI package can set its source root to include the shared public
header and preserve the physical `_async_c` symbols. Eight interface clients
compile from provider-free published declarations and link with the same
provider objects as the separate C clients. The reader rejects a rehashed
sidecar that removes the completion header capability.

Source 2.248 / System 2.174 admits public SDK cancellation and completion
functions and status constants in published inline bodies. The v2 reader
capability pins their exact header. Body identity uses the bound SDK declaration
and contract, so the provider and consumer can install the static inline
checkpoint at different paths. A source redeclaration or changed header bytes
cannot supply the pinned identity.

Eight source-free C/Objective-C and C++/Objective-C++ configurations compile and
link the published checkpoint and checked group join without the provider
source. The C clients execute the existing success, waiter cancellation, group
cancellation and nested-failure cases through the imported body. An altered
header is rejected before publication. The rehashed missing-capability control
continues to fail in the production decoder.

Source 2.251 / System 2.177 can generate the provider thunk and C client header
from a native async function declaration. For example, a provider can declare
`int increment(int value) async;` and build the bridge with the matching SDK.

```sh
objc3c provider.m --objc3 --emit-object provider.o \
  --export-c-async increment --emit-c-async-header increment.h -- -fblocks
```

Repeat `--export-c-async` to share one header across several functions. The
header declares each `f_completion_t` and `f_async_c` in the original parameter
order with context and callback last. The compiler selects from the typed AST,
derives checked status and the void callback shape, and compiles each generated
definition through the native frontend. Use `--emit-interface` with explicit
exports to publish the same callable declarations in semantic metadata.

The generated lane admits non-overloaded free frame entries on the global
executor with builtin integer, real floating, and pointer types. Value types
are independent of provider typedefs. C callers keep pointed-to storage valid
through completion. Definitions may live in a separate translation unit when
the selected declaration has external linkage. The compiler publishes the
header with its native product and restores an existing header if product
publication fails.

Source 2.280 / System 2.206 also publishes `f_async_c_task` alongside the
canonical `f_async_c`. The companion takes the same parameters, context and
callback. On successful submission it returns an independent retained
`objc3_task_t *`. Call `objc3_task_request_cancel` to request cooperative
cancellation and `objc3_task_release` when finished with the handle. The
runtime keeps the callback alive if the handle is released early. A callback
can arrive before either entry returns. A null return signals that submission
was rejected and the callback still records the terminal status.

The separate C and source-free clients exercise task handles for checked
failures, portable records, objects, Blocks and isolated selectors. The
runtime tests cancel after suspension and check that rejected start on a
closed actor leaves the unsubmitted frame owned by its caller. Independent
Swift clients cancel before and after the handle is installed. See the
[Swift consumer evidence](linux_swift_consumer.md).

Source 2.283 / System 2.209 admits consumed Objective-C object and Block
parameters in generated checked synchronous functions and async free functions
and actor entries. C callers lend their original references. The native callee
receives an independent claim through ordinary ARC transfer or the generated
MRC retain or Block copy. An async bridge owns its captures before submission
returns and makes the consumed handoff when it enters the native call. Actor
methods can consume the receiver under the same convention.

The eight C clients check reuse of borrowed synchronous inputs after success
and typed failure, async cancellation before entry, consumed actor receivers,
exactly-once callbacks and all input destructions. Source-free clients reimport
the generated typed declaration.

Source 2.287 / System 2.213 admits named unscoped C-compatible enums with
explicit underlying integer types. Generated declarations preserve signedness,
the exact enumerator values and native size and alignment. The same enum can
appear in a checked or async signature, an aggregate field, or a callback.
Function pointer and Block callback signatures can carry the same admitted
plain aggregates as direct C entries. Generated headers verify their complete
size, alignment and field offsets.

Eight independent C17/C23 clients check ARC and MRC providers in C17, C23,
C++20 and C++23. They exercise an async callback with a signed-byte input and
an unsigned 64-bit result, checked enum failure, and a callback that passes an
aggregate containing the signed 64-bit minimum through the indirect ABI.
Four AArch64 QEMU executions also exercise the checked aggregate and its
callback in ARC and MRC with C17 and C23 callers. Source-free imports preserve
the enum signature. Matching SDK acceptance remains pending.

The separate C clients execute value, void and checked success and failure
callbacks across C17/C23/C++20/C++23 providers in ARC and MRC. The C17 MRC
control links the selected declaration to a separately compiled async body.
The interface keeps its exact SDK completion capability.

Source 2.252 / System 2.178 adds Objective-C references, ordinary typed
Blocks, and C-layout aggregates to generated exports. The header preserves
native Objective-C types and exposes their opaque pointer ABI for C. Clang
clients with Blocks enabled receive the typed Block signature. Copy a Block
or retain an object in the callback to keep it beyond callback return.

Named portable records carry their field declarations, including nested
records and fixed arrays. The C header checks the provider's exact size,
alignment, and field offsets. A consumer with incompatible packing fails to
compile. Generated records and reference typedefs enter the same semantic
interface export set as their thunks and callbacks.

The eight separate C executables check object identity, copied Block lifetime,
and aggregate input and result fields in addition to the scalar, void, and
checked forms.

Source 2.253 / System 2.179 generates bridges for free async declarations with
executor affinity. A private native await enters the source function on its
declared executor. C callers use the same completion types and can invoke the
bridge from any executor. The compiler retains the native typed error channel
and applies ordinary isolation admission to the generated await.

All eight C clients verify actual execution on the main and registered named
executors. A missing named executor produces checked failure for a throwing
entry and distinguished cancellation for a nonthrowing entry. Both controls
deliver one completion without entering the unavailable destination.
Wider foreign types and adapters and matching SDK acceptance remain open.

Source 2.256 / System 2.182 exports an isolated instance method selected
with `--export-c-async 'Counter::add:by:'`. The C declaration is
`Counter_add_by_async_c`. It takes the opaque receiver first, the method
arguments in selector order, and then the context and completion. A private
typed Objective-C frame performs the native await on the selected receiver.
Only the thunk and its C signature enter the exported interface, so clients
can import it independently of the actor class definition.

The receiver must be a live instance of the named actor class or its subclass.
The receiver must stay live while the thunk builds its frame. Ordinary method
ownership, isolation and checked error admission apply. The generator admits
directly declared isolated synchronous methods and native async frame methods,
including nullary and multi-argument selectors. C++ crossings require the
native nonthrowing exception contract. Nonisolated, missing and malformed
selectors and duplicate C symbols fail before product publication.

Eight separate C clients invoke two live actor instances and a closed mailbox
through their generated headers. Each body checks its actor identity.
Callbacks cover isolated state, checked success, the original typed error
object and distinguished cancellation at the closed destination.

Source 2.257 / System 2.183 roots the typed receiver in the native async
Block frame before the C thunk returns. ARC and MRC callers may release
their receiver immediately after submission. Native frame cleanup releases
the capture after completion or cancellation. Ordinary C pointer parameters
keep their source borrowing contract.

The eight actor clients pause all global workers, submit the calls, release
the caller's receiver, and then let the frames enter. The preceding MRC
bridge destroys the receiver immediately. The owning bridge delivers each
callback once and destroys all three actor objects after completion and
explicit mailbox unbind.

Source 2.265 / System 2.191 retains object arguments and copies Block
arguments in generated C exports before submission returns. The owning
native bridge keeps both inputs alive through the source call and releases
them after completion or cancellation. The caller keeps raw C pointer
storage valid through completion. The manual export macros retain their
existing ownership contract.

Generated Block aliases with typed parameters also compile in C with Blocks
enabled. Interface emission places Objective-C root and runtime-name
attributes on the class definition so repeated forward declarations from
generated signatures remain valid.

Eight C17/C23 and C++20/C++23 provider configurations in ARC and MRC
execute the generated calls with a separate C consumer. The consumer blocks
worker entry, submits, and releases its object and heap Block handles.
The tests observe the expected value, cancellation at an unavailable
executor, and destruction of both arguments on each path. Source-free
callers also compile against the generated interface.

Source 2.269 / System 2.195 resolves an actor export through ordinary instance
method lookup on the named class. Methods declared on a superclass can use the
subclass receiver in the emitted bridge. The inherited method supplies the
signature, checked error contract and isolation. The typed await binds the
actual receiver's mailbox. Nonisolated inherited methods retain the same
rejection as directly declared ones.

All eight C17/C23 and C++20/C++23 ARC/MRC configurations call synchronous and
checked async inherited methods on subclass receivers after provider source
removal. The C callers check independent receiver state, exact error identity,
and receiver cleanup after release before worker entry.

## Synchronous C error-out exports

Source 2.270 / System 2.196 implements the trailing error-out surface in
Part 11 §11.2.2. Add `--export-c-throws lookup --emit-c-header exports.h` to
the provider's native object or IR emission command. Repeat `--export-c-throws`
for each entry and add `--export-c-async` selections to share the header with
completion APIs. Throws-only builds also work with Blocks disabled.

For a checked `int lookup(int key)`, the generated C declaration has this
shape.

```c
int lookup_throws_c(int key, objc3_async_c_error _Nullable * _Nullable out_error);
```

A nonnull slot is cleared on success and receives the exact +0 error on failure.
Retain the error before the surrounding autorelease pool drains if it is needed
later. A null slot discards the error. Use the returned value on success.
The C symbol uses the native trailing error-out convention and is included in
the semantic interface for source-free consumers.

Global nonvariadic functions with synchronous typed or untyped `throws` and
portable scalar or pointer parameters and results are supported, including
`void` results. Objective-C++ entries require a declared native nonthrowing
contract. The C signature uses ordinary C storage and carries no implicit
executor, resource cleanup or ownership transfer.

Eight native C17/C23 and C++20/C++23 ARC/MRC provider configurations link an
ordinary C caller. They exercise value, pointer and void signatures, error-out
clearing, exact error identity, discarded errors, and retaining an error across
autorelease pool drainage. Source-free import checks the generated declaration.

Source 2.272 / System 2.198 extends synchronous C exports to admitted object
and Block references and plain C-layout aggregates. Reference results use the
source's +0 lifetime. Retain an object or copy a Block to keep it through the
surrounding pool's drainage. The C header uses opaque references where the
client language lacks the corresponding native reference type. By-value records
carry static size, alignment and offset checks.

Checked C record calls use the platform's ordinary register or indirect result
convention with the same trailing error slot. They also support optionalizing
calls with plain record payloads. Records with managed, borrowed or resource
storage require their own contracts. Eight linked C clients verify small and
large aggregate returns and exact error identity, object and Block values and
source-free replay. LLVM IR tests bind the x86-64 and AArch64 result conventions.

Source 2.274 / System 2.200 preserves pointer nullability in generated C
declarations, including qualifiers within pointers and typed Block signatures.
ABI arguments retain explicit source annotations. The outer pointer result is
nullable because the C envelope uses null on checked failure or cancellation.
Unspecified pointer annotations retain their unspecified contract. Plain C
headers omit ARC qualifiers even when the provider is compiled with ARC.

Eight C provider configurations check generated declarations, nullable inputs,
nested pointers, and null checked results through linked C and source-free
consumers. Swift 6.4.2 imports a generated checked C header and links the native
object. It handles the optional result and opaque error, rejects an unwrapped
optional result, and rejects `nil` for a nonnull parameter.

Source 2.275 / System 2.201 publishes ordinary C function pointers through
generated synchronous and asynchronous signatures. The header emits typed
aliases with the original parameter and result annotations, and the public
function-pointer result allows null on failure or cancellation. The supported
callback form uses a prototype, C calling convention and portable scalar or raw
pointer types. Variadic callbacks, checked/async callback effects and native
exception specifications need a separate adapter contract.

The linked C client passes its callback and caller-owned context to a checked
entry, sees the same function pointer returned on success and invokes it after
an async completion. All eight configurations check pointer identity and value
results. Source-free import checks the function-pointer aliases. AArch64 ARC
and MRC C clients also execute the indirect aggregate and callback contracts
under QEMU.

This source milestone also admits qualified Objective-C++ free functions in
C exports. For example, `--export-c-async API::v1::work` publishes
`API_v1_work_async_c`, and `--export-c-throws API::v1::check` publishes
`API_v1_check_throws_c`. Nested namespaces and namespace aliases retain the
exact native callable. Two selections yielding the same C name are rejected.
Four C++20/C++23 ARC/MRC providers link ordinary C clients and replay the
emitted declarations after provider source removal.
