# Linux native Objective-C C17 async definitions

This development slice lowers reserved `__objc3_async` free-function definitions
and `__objc3_await` in Objective-C 3 C17 mode to the existing shared C frame ABI.
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

Returns first acquire independent result storage. Objects are retained and
Blocks are copied while their lexical storage is live. A result is published
only after lexical return cleanup, before final suspension. Failed result copies
terminalize through cancellation without reporting an uninitialized success.
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
handle leaves scope. Cancellation and foreign completions bypass checked
handlers. A native Objective-C exception can enter an ordinary source `@catch`;
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
error propagation/recovery and a separately compiled C++ carrier consumer.

Qualification must later build the compiler and run those fixtures in C17
ARC/MRC provider/consumer pairs and C++20/23 consumers, then exercise resumed
frames, sanitizer instrumentation and source-free semantic/package transport.
No existing SDK receipt establishes acceptance of this new source.

## Remaining definition work

Native Objective-C method and async Block definitions still need their own
entry/capture integration. Variable-length automatic storage and dynamic
`alloca` are diagnosed because their storage is not represented by this
fixed-layout frame implementation. Awaitable runtime handles, explicit yield
and hop syntax require further source-level protocols; this slice accepts
logical async calls through the shared frame boundary. Executor/isolation
metadata checks are developed separately and must precede any synchronous
await identity path.

Public async/await and actor capability predicates remain reserved. The existing
SDK/archive documentation continues to describe its previously built sources.
