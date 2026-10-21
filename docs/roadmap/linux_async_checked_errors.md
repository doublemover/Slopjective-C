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
existing C++ value result support. Objective-C async methods, async Blocks, pure Objective-C
async lowering and compiler actor isolation remain future compiler milestones.
Public async/await and actor capability predicates remain reserved.

Extension ABI 16 requires a full rebuild of every object, provider, PCH,
semantic interface and package. Promise layout changes for every task result
type. The task service remains ABI 3; reserved object-error bridge symbols ship
in a separate archive member so ordinary C++ task programs retain their existing
link dependencies. ARC and MRC providers use the same owned error representation.

The [installed SDK acceptance receipt](../support/linux_async_cxx_callables_acceptance.json)
binds this contract to its exact compiler, runtime, source inputs and archive.
It records 209 compiler tests, 57 AST tests, 318 current tooling tests, all 22
installed package gates and 74 instrumented configurations. Native execution
covers sixteen C++20/23, O0/O2 and ARC/MRC provider/consumer configurations,
including checked object, Block and void results, copied error handles, native
exception boundaries, cancellation, deferred cleanup and 64 cross-worker actor
error paths. Eight LLDB configurations inspect resumed scalar, owned-result and
checked-handler frames. Source-free package consumers retain combined async and
typed-error identity after provider source removal and relocation.

The receipt also verifies a standalone C++ task link without libobjc and explicit
rejection of extension ABI 15 by the installed task header. Fresh Debian 13 and
Ubuntu 26.04 consumers run as ordinary users under the explicit
`task-destinations` scope: archive integrity, relocation, compiler/ownership,
debugger/embedding, structured runtime, coroutine and source-free async package
checks. The full 22 package gates run during SDK publication. Sanitizers
instrument runtime services and clients; GNU libobjc is loaded as shipped.
The [latency report](../support/linux_async_cxx_callables_latency.json) records
five quiet repetitions against the existing release budgets.
