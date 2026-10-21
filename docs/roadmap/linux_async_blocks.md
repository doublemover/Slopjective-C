# Linux async Objective-C++ Blocks

The reserved Linux x86-64 LP64 GNUstep 2.2 Objective-C++20/23 async stage extends
to Blocks with explicit concrete result types. Canonical Block types retain their
logical result and async/throws effects; evaluated invocations return
`objc3::task<R>`. Block descriptors encode that physical task result. Async
checked Blocks publish errors through task completion without a synchronous
error-output argument.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>

int bias = 40;
auto __unsafe_unretained block = ^int(int delta) __objc3_async {
  __objc3_await objc3::task_yield;
  return bias + delta;
};
auto pending = block(2);
// pending owns the captures even after the original stack Block leaves scope.
auto running = std::move(pending).start(executor);
```

Each invocation copies its Block before initial suspension. The frame releases
that copy on success, checked failure, native exception, cancellation and
unstarted-frame destruction. Copying preserves ordinary Block ownership rules:
strong object and Block captures, weak and unsafe captures, C++ capture copies,
and `__block` forwarding. Weak and unsafe captures do not acquire strong object
ownership. A captured C++ `this` remains a borrowed pointer. A throwing C++
capture copy unwinds the allocated frame before any task is returned.

Captured values, explicit parameters, local cleanups and checked handlers survive
suspension. Runtime actor hops preserve destination identity and serial resume
segments. Mutable captured state still requires appropriate synchronization.

Block bodies use the existing coroutine statement representation inside an
ordinary compound body, preserving the existing Block declaration APIs and PCH representation.
O0 capture debug locations refer to the owned Block in the resumed frame.
Source-free interfaces preserve async Block aliases and async functions returning
those Blocks; providers can be removed before consuming relocated packages.

Extension ABI 19 is a hard compiler/header/interface/PCH/provider cutover. Rebuild
all participants. The task header rejects extension ABI 18. Task service ABI 3
and promise storage layout remain unchanged. Public async/await and actor feature
predicates remain reserved pending pure Objective-C lowering and compiler actor
isolation. Inferred Block results, variadic signatures, reference or volatile
results and other targets remain outside this stage.

The [installed SDK acceptance receipt](../support/linux_async_blocks_acceptance.json)
binds this contract to its exact compiler, source inputs and sealed ABI 19
archive. It records 218 compiler tests, 57 AST tests, 322 tooling tests,
24 frontend CTests, eight GNU runtime tests, 37 installed driver/embedding checks
and all 22 installed package gates. Frontend CTests are built and run individually;
only passed generated test executables are reclaimed.

Sixteen C++20/23 O0/O2 configurations cover all ARC/MRC provider-consumer pairs.
Eight LLDB cases read resumed capture and local values alongside the earlier
function, member, method, owned-result and checked-handler frames. PCH tests
serialize actual async Block bodies with checked errors and nested awaits.
Execution covers stack Block destruction before first resume, C++ and object
captures, weak/unsafe semantics, `__block` forwarding, noescape function and method
arguments, throwing capture copies, cancellation and cross-worker actor hops.

Eight source-free configurations execute 24 times after provider removal and
package relocation. The same archive passes 74 instrumented configurations and
356 commands, four standalone C++/extension-ABI checks and six post-hop LLDB
cases. Fresh Debian 13 and Ubuntu 26.04 consumers run as ordinary users under the
explicit task-destinations scope. All 22 package gates run on the publication
host. Fresh SDK and scratch storage use explicit 4 GiB and 2 GiB RAM mounts
within an 8 GiB memory limit. Sanitizers instrument services and clients; GNU
libobjc is loaded as shipped. The [latency report](../support/linux_async_blocks_latency.json)
records five quiet repetitions against the existing release budgets.

The publication host also verifies archive reuse against an existing installation
in a 4 GiB RAM filesystem. This preserves every qualified archive while providing
space for the installer's second full extraction. The host SDK remains unchanged,
and the temporary RAM filesystem is released after verification. The receipt
retains the earlier disk-capacity failure and the successful RAM-backed check.
