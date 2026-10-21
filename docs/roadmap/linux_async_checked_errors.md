# Linux checked async errors

The reserved Objective-C++20/23 async compiler stage carries checked Error
objects through suspended Linux x86-64 LP64 GNUstep 2.2 tasks. A checked async
function keeps its logical result, async effect and exact error type in its
canonical function type. Its physical result is a lazy `objc3::task<R>`.

In ARC mode:

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>
@protocol Error @end
@protocol NetworkError <Error> @end

int load(bool fail, id<NetworkError> error)
    __objc3_async __objc3_throws(id<NetworkError>) {
  __objc3_await objc3::task_yield;
  if (fail) __objc3_throw(error);
  return 42;
}

int recover(bool fail, id<NetworkError> error) __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await load(fail, error));
  } __objc3_catch(id<NetworkError> caught) {
    __objc3_await objc3::task_yield;
    return caught ? 96 : 0;
  }
}
```

Creating a lazy task does not execute its body and needs no checked operation.
Awaiting an async declaration with a throws effect requires an explicit checked
form. Apply `__objc3_try`, `__objc3_try_optional` or `__objc3_try_force` around the
await, not around task creation. Propagation requires a compatible checked
handler or the same enclosing throws effect. A synchronous checked call can
also propagate into an async function after suspension.

Checked failure owns a non-null Error-bound Objective-C object. The coroutine
retains it before expression cleanup, transfers it into an owned runtime error
payload, runs lexical cleanup and reaches final suspension. Completion and
copied handles preserve ownership after the provider frame disappears. Checked
await extracts the deferred error into the consumer's checked control flow.
The synchronous hidden error-out ABI never points into a caller that has already
returned.

Typed handlers own their bound object through later suspension, including in
MRC. Ordinary MRC parameters and locals retain their existing explicit ownership
rules; the error transport does not make borrowed arguments owning. Successful
object and Block results follow the [owned result contract](linux_async_owned_results.md).

Checked errors bypass C++ `catch` and Objective-C `@catch`. Native C++ exceptions,
Objective-C exceptions and cooperative task cancellation keep their existing
channels and bypass checked handlers and optionalization. A native exception
from cleanup takes precedence over an escaping checked error. Returning normally
after handling that native exception clears the earlier staged checked error.
Allocation failure while materializing a runtime error terminalizes the task as
cancelled, consistent with failed completion publication.

Direct and indirect calls retain their canonical throws effect. Explicit
`task<R>` and handle storage enters C++ runtime interoperation and erases the
source effect from that carrier type. A root task started through that API
exposes deferred checked failure as `objc3::task_error` with its retained native error handle. Language
handlers use checked await. Private child cancellation, inherited destinations,
actor-bound execution and queued resumes retain the existing task bridge rules.

This stage admits real scalar, enumeration, C pointer, Objective-C object,
protocol, Class, Block and void results for checked async functions. C++ record,
complex, member pointer, reference, variadic and undeduced checked results remain
outside the lowering contract. Ordinary nonthrowing async functions retain their
existing C++ value result support. Async methods, async Blocks, pure Objective-C
async lowering and compiler actor isolation remain future compiler milestones.
Public async/await and actor capability predicates remain reserved.

Extension ABI 16 requires a full rebuild of every object, provider, PCH,
semantic interface and package. Promise layout changes for every task result
type. The task service remains ABI 3; reserved object-error bridge symbols ship
in a separate archive member so ordinary C++ task programs retain their existing
link dependencies. ARC and MRC providers use the same owned error representation.
