# Linux async Objective-C methods

The reserved Linux x86-64 LP64 GNUstep 2.2 Objective-C++20/23 async stage supports
Objective-C instance and class methods. Dynamic messages, protocol-qualified
receivers, superclass messages and `objc_direct` methods use the same canonical
logical result and async/throws effects as native async functions.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>

@interface Worker
- (int)value:(int)n __objc3_async;
@end

@implementation Worker
- (int)value:(int)n __objc3_async {
  __objc3_await objc3::task_yield;
  return n;
}
@end

// A message creates a lazy, owned task; starting it selects the executor.
auto pending = [worker value:42];
auto running = std::move(pending).start(executor);
```

The physical message and implementation result is `objc3::task<R>`. Runtime
method encodings describe that carrier. Async checked methods publish deferred
owned errors through task completion; they have no synchronous error-output
parameter. Every overriding declaration and implementation must preserve exact
async/throws effects and the logical result type. Native Objective-C covariant
results cannot change an async task specialization.

A method frame retains its receiver before initial suspension, under both ARC
and MRC, and releases it on normal completion, checked failure, native exception,
cancellation or destruction of an unstarted task. Releasing the caller's
receiver after creating a task does not invalidate the frame. Arguments follow
the existing native async value and ownership contract; borrowed arguments do
not become owned merely because the receiver does. Native C++ member tasks
retain their separate borrowed-`this` contract.

Suspended methods preserve `self`, `_cmd`, explicit arguments, local ownership,
checked handler state and ordinary C++ cleanup. Nested async messages inherit
the parent's current runtime destination; explicit task bridge hops preserve
actor identity and serial resume segments. This does not introduce compiler
actor isolation or make unsynchronized mutable receiver access safe.

A message to a nil receiver returns an empty task carrier. Discarding or moving
that carrier is valid. Starting or awaiting it reports
`OBJC3_SERVICE_INVALID_ARGUMENT` through `objc3::service_failure`; it does not
produce a logical zero result. Dynamic sends check nil even for ignored results,
and direct-method nil returns use the physical task representation.

Methods with ownership families such as `init`, `alloc`, `new` and `copy`, related
result types, consumed receivers or explicit native return-ownership attributes
remain outside this contract. Variadic methods, reference or volatile results,
and unsupported calling conventions are rejected. [Async Objective-C++ Blocks](linux_async_blocks.md)
now own their suspended captures. Pure Objective-C async lowering remains a
separate compiler milestone. The installed
runtime header and full-frame allocation option are required, as for native
async functions.

This method milestone introduced extension ABI 18. The current [async Block
stage](linux_async_blocks.md) advances the compiler/header/interface/PCH/provider
contract to ABI 19. Rebuild all participants. Task service ABI 3 and promise
storage layout remain unchanged. The task header rejects extension ABI 18. Public async/await and actor feature
predicates remain reserved.

Canonical imports and source-free packages preserve async Objective-C method
and protocol effects, module-owned bindings, direct dispatch and logical result
identity. Imported checked methods use the same pre-result completion check,
Optional payload lifetime rules and owned object/Block results as functions.

The [installed SDK acceptance receipt](../support/linux_async_objc_methods_acceptance.json)
binds this contract to its exact compiler, source inputs and sealed ABI 18
archive. It records 215 compiler tests, 57 AST tests, 318 tooling tests,
24 frontend tests, eight GNU runtime tests, 37 installed driver/embedding checks
and all 22 installed package gates.

Sixteen C++20/23 O0/O2 configurations cover every ARC/MRC provider-consumer pair.
Eight LLDB cases inspect resumed `self` and local values alongside the earlier
scalar, owned-result, checked-handler, C++ member and checked-value frames.
PCH files serialize actual method coroutine bodies with checked throws and
nested awaits. Native execution covers unstarted frame destruction, copied
arguments, deferred errors, native exceptions, nil start/await failures,
Optional boundaries and 64 cross-worker actor-hop/cancellation paths.

Eight source-free package configurations execute 24 times after provider source
removal and relocation. The same archive passes 74 instrumented configurations
and 356 commands, four standalone C++/extension-ABI checks and six post-hop
LLDB cases. Fresh Debian 13 and Ubuntu 26.04 consumers run as ordinary users
under the explicit `task-destinations` scope. All 22 package gates run on the
publication host. Fresh containers provide a 4 GiB RAM mount for SDK staging and
a 2 GiB RAM mount for scratch, within an 8 GiB memory limit. The receipt records
these resource settings. Sanitizers instrument services and clients; GNU libobjc
is loaded as shipped. The [latency report](../support/linux_async_objc_methods_latency.json)
records five quiet repetitions against the existing release budgets.
