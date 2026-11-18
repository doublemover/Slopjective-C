# Native async exports to C

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
