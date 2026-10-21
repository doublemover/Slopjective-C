# Linux owned native async results

Native async free functions can return Objective-C object pointers, protocol
qualified object pointers, `Class` and Block pointers in the Linux x86-64 LP64
GNUstep 2.2 compatibility profile. This extends the reserved Objective-C++20/23
`__objc3_async` and `__objc3_await` compiler stage. Public async/await, pure
Objective-C async lowering and compiler actor isolation remain reserved.
[Checked async errors](linux_async_checked_errors.md) extend this stage with
owned deferred errors and checked awaits.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>

id relay(id value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}

// An evaluated call creates a lazy objc3::task<id>.
auto running = relay(value).start(executor);
```

The canonical function type preserves its logical Objective-C result and async
effect. The physical function returns the native C++ task object. Return
initialization checks the logical result without introducing the +1 transfer
used by an ordinary Objective-C function return epilogue. The promise acquires
an independent result owner before lexical cleanup or frame destruction.

The task stores a retained object or a copied Block in an opaque owned payload.
Copying a completion payload acquires another owner; destroying it releases that
owner. A frame-local MRC Block is copied while its capture storage remains
alive. Nil object and Block results are successful, present payloads. Ordinary
C++ pointer results retain their existing value semantics.

`task_handle<R>::result()` and a successful await return a +0 Objective-C value
valid through the current autorelease pool. Releasing every task handle after
extraction does not invalidate that value before the pool drains. ARC locals
retain values across subsequent suspension. MRC code explicitly retains values
that must survive a later suspension or pool drain and balances its own +1
source references. Lazy arguments retain their native C++ and ARC/MRC parameter
rules; this result contract does not add ownership to borrowed MRC arguments.

Runtime callbacks establish an autorelease pool for each resume. Private child
awaits can release their completed child handles after extracting a result.
Cancellation, native exception unwinding and abandonment destroy frame locals
and published result storage through their respective owners. Actor hops
preserve strong frame references and the inherited child destination.

The compiler's reserved `__objc3_is_object_pointer(T)` and
`__objc3_is_block_pointer(T)` predicates select the same payload representation
in ARC and MRC. C++ conversion to `id` cannot serve this purpose: Block conversion
rules differ between those modes. The predicates accept incomplete object
declarations and preserve classification through aliases and templates.
Owned payloads, `task<R>` and `task_handle<R>` normalize the pointer's ARC lifetime
qualifier to an explicit borrowed type identity; their opaque owner supplies the
actual retain/copy behavior. This keeps result descriptors and C++ handle
parameter mangling identical across ARC and MRC without erasing the class,
protocol, Block signature or C++ value type.
Other C++ parameters and value results retain their native type and ownership
ABI. Cross-mode signatures require identical C++ types; arbitrary aggregates
whose Objective-C fields change ownership between modes need their own explicit
ABI contract.

Owned results introduced extension ABI 15. The
[checked async error stage](linux_async_checked_errors.md) introduced extension
ABI 16. [Checked value results](linux_async_value_results.md) introduced ABI 17.
[Objective-C methods](linux_async_objc_methods.md) introduced ABI 18 and own
their lazy receivers under ARC and MRC. The current [async Block stage](linux_async_blocks.md)
advances the compiler/header contract to ABI 19 and owns suspended captures.
Rebuild objects, PCH files, semantic interfaces and packages. The task service remains ABI 3. Every provider and
consumer uses the new compiler and runtime header. Native header consumers cover
all four provider/consumer ARC/MRC pairs; semantic and immutable packages retain
their explicit ownership-mode identity.
ABI 15 normalized the underlying C++ task and handle class ABI for every result type.

The qualification owner covers sixteen native provider/PCH/consumer
configurations across C++20/23, O0/O2 and all ARC/MRC pairs. O0 cases inspect
resumed scalar frames and owned-result frames in LLDB. Runtime witnesses cover
objects, protocol qualification, copied Blocks, Class, nil results, repeated
reads, copied handles, nested private children, cancellation, abandonment and
cross-worker actor hops. ASan/LeakSanitizer/UBSan and ThreadSanitizer instrument
the task services and clients in all ownership pairs. Source-free package
consumers preserve imported object and Block result types and execute after
provider sources and build directories have been removed.

The [owned-result acceptance receipt](../support/linux_async_owned_results_acceptance.json)
binds that milestone's exact SDK archive to the compiler and AST tests, 22 installed package
gates, 74 sanitizer configurations, frame debugging and ordinary-user archive
consumers on Debian 13 and Ubuntu 26.04. Fresh-distribution qualification uses
the explicit `task-destinations` scope; the full package matrix runs during SDK
publication. The [latency report](../support/linux_async_owned_results_latency.json)
records five quiet repetitions against the existing command budgets.
