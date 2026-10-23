# Linux native Objective-C C17 async definitions

This development slice lowers reserved `__objc3_async` free-function,
Objective-C method and Block definitions, and `__objc3_await` in Objective-C 3 C17
mode to the existing shared C frame ABI.
The canonical logical function result and effects remain unchanged. The native C
symbol physically returns `objc3_async_frame`; explicit asm bindings remain
available for foreign frame factories. This introduces no runtime record-layout
or service-version change.

```objective-c
#include "objc3_runtime_services.h"

int leaf(int value) __objc3_async { return value; }
int total(int count) __objc3_async {
  int result = 0;
  for (int i = 0; i < count; ++i)
    result += __objc3_await leaf(i);
  return result;
}

// The call constructs a lazy, owned frame. Start it on an explicit executor,
// transfer it to a parent, or dispose it before start using the shared C API.
objc3_async_frame pending = total(8);
```

## Source implementation

`CGObjC3Async.cpp` emits LLVM coroutine construction and suspension intrinsics.
LLVM coroutine splitting supplies resume and destruction states and promotes
live automatic storage into the allocated frame. Allocation observes the frame's
computed alignment, including over-aligned parameter and local storage.
Allocation failure returns an empty carrier; failed runtime starts preserve a
successfully constructed carrier's ownership.

The initial suspension occurs after frame capture initialization and before the
source body. Value parameters are copied into independent frame storage. ARC
parameter ownership and native nontrivial C record operations follow their
source types; MRC pointer arguments retain their borrowed contract. A separate
ramp parameter copy and cleanup preserve the caller ABI. ARC local releases,
source defers, native C record destruction and managed carrier cleanup use the
ordinary lexical cleanup stack on continuation and destruction edges.

Method frames independently retain their receivers in ARC and MRC before initial
suspension, and preserve `self`, `_cmd` and explicit argument storage. Each async
Block invocation owns a heap copy before suspension, preserving captures and
`__block` forwarding after the source Block leaves scope. Dynamic, protocol,
superclass and direct messages use the physical frame result; nil messages return
an empty carrier. Method and Block encodings describe that carrier.

Returns first acquire independent result storage. Objects are retained and
Blocks are copied while their lexical storage is live. A result is published
only after lexical return cleanup, before final suspension. Failed result copies
terminalize through an owned service-error completion without reporting an
uninitialized success. A genuine cancelled status retains its cancellation
completion. Bounded service failures use allocation-independent immutable runtime
error owners; an impossible materialization failure crosses a fatal contract
boundary rather than being mislabeled cancellation.
Void, zero, false and nil retain the shared ABI's distinct completion semantics.
The native return initializer omits the ordinary Objective-C physical-return
retain/autorelease transfer. Per-type descriptor caches retain the canonical
identity and one nominal token for each private result type within the module.

Awaiting a logical async C call starts a private child through
`objc3_async_frame_start_child`, registers a dependency and suspends the parent.
Resume checks cancellation before inspecting or constructing a result. Every
private child owner cancels unfinished work and releases its reference when the
await scope exits. Child result extraction uses typed C copy-out; owned object
and Block results become source-level +0 values through autorelease, so ARC
locals can retain them across later execution slices. The runtime supplies the
execution-slice autorelease pool and destination preservation.

Checked child errors enter the existing `try`, optionalization and typed-handler
machinery. Error objects acquire an independent owner before the private child
handle leaves scope. Cancellation, service failures and foreign completions bypass checked
handlers. An invalid or empty awaited child preserves
`OBJC3_SERVICE_INVALID_ARGUMENT`; the C++ bridge rethrows its original
`objc3::service_failure` status. Existing C++ exception payloads retain their owned
transport through a native C parent without interpreting C++ exception storage. A native Objective-C exception can enter an ordinary source `@catch`;
an exception escaping the async body uses the existing foreign-exception
cancellation channel. Generated value and frame callbacks contain native
exception boundaries rather than unwinding across the C callback ABI.

The await AST reuses `CoawaitExpr` with the original source expression as its
common expression and opaque compiler operations for the C frame protocol.
This lets ordinary CFG construction evaluate call arguments and preserves the
logical result in existing PCH serialization. No C++ promise type or new AST
statement layout is required.

## Authored coverage and validation status

Development was explicitly authorized ahead of validation. No compiler build,
fixture execution, test suite, sanitizer run or qualification command has been
run for this slice. The implementation and fixtures remain unverified source.

New native compiler fixtures cover definition IR, private child awaits, loop
locals, lexical cleanup, checked recovery that awaits again, native exceptions,
ARC/MRC owned results, aligned records, complex values and PCH-defined async
functions. The maintained `tests/clang_foundation/native_c_async` witnesses cover
lazy disposal, preserved failed starts, actor destinations, cancellation while a
parent awaits its child, independent object/Block copy-out ownership, checked
error propagation/recovery, dynamic/protocol/direct/super/class methods, lazy
receiver ownership, copied Block captures, nil sends, and a separately compiled
C++ carrier consumer. A C++ frame factory also exercises C++ → C → C++ value
and exception transport.

Qualification must later build the compiler and run those fixtures in C17
ARC/MRC provider/consumer pairs and C++20/23 consumers, then exercise resumed
frames, sanitizer instrumentation and source-free semantic/package transport.
No existing SDK receipt establishes acceptance of this new source.

## Remaining definition work

Variable-length automatic storage and dynamic
`alloca` are diagnosed because their storage is not represented by this
fixed-layout frame implementation. Awaitable runtime handles, explicit yield
and hop syntax require further source-level protocols; this slice accepts
logical async calls through the shared frame boundary. Executor/isolation
metadata checks are developed separately and must precede any synchronous
await identity path.

## Persistent entry ABI identity

`__objc3_async_frame` is the canonical persistent spelling for a logical async
callable whose physical entry returns `objc3_async_frame`. Native C17
`__objc3_async` defaults to this identity; Objective-C++ `__objc3_async` keeps the
C++ task identity. The `AsyncFrame` effect bit is part of canonical function type
identity, folding profiles, redeclaration and override checks, overload
conversions, callback mangling, structural equivalence, ODR hashing, printing,
PCH and AST import. Frame and task physical-result caches are independent even
when both use the same logical result in one translation unit. Direct,
function-pointer, Block and method calls select their declared ABI.

An Objective-C++ `__objc3_await` of a declared frame call adapts through
`__objc3_async_from_frame<R>`. This helper adopts the owned lazy carrier and
disposes it before rethrowing if descriptor validation or adoption fails. The
compiler retains the original logical source call for checked-error analysis.
Frame-only calls need the C runtime header; adapted awaits also need
`objc3_task.hpp`. Ordinary C++ frame-ABI definitions are diagnosed before promise
lowering. Compiler-created isolated synchronous entry thunks use the native
frame generator and its cancellation/cleanup machinery; their retainable
arguments have independent frame owners under ARC and MRC before any hop.

The module facade now carries the recursive `c-frame-v1`/`cxx-task-v1` contract
under required semantic `objc3.async-entry-abi.v1` and schema 2.1. That separate
facade span owns export/import codec and package-profile bounds. This source
implementation does not qualify arbitrary cross-profile package projection.

Authored additional witnesses cover mixed direct/indirect/Block/method calls,
C++ checked frame awaits, PCH persistence, callback mangling, cache separation,
frame-only C++ calls without the task header, incompatible ABI redeclarations and
assignments, and adoption-failure ownership. These remain unbuilt and unrun.

Public async/await and actor capability predicates remain reserved. The existing
SDK/archive documentation continues to describe its previously built sources.
