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
ordinary compound body, preserving Block AST serialization and import contracts.
O0 capture debug locations refer to the owned Block in the resumed frame.
Source-free interfaces preserve async Block aliases and async functions returning
those Blocks; providers can be removed before consuming relocated packages.

Extension ABI 19 is a hard compiler/header/interface/PCH/provider cutover. Rebuild
all participants. The task header rejects extension ABI 18. Task service ABI 3
and promise storage layout remain unchanged. Public async/await and actor feature
predicates remain reserved pending pure Objective-C lowering and compiler actor
isolation. Inferred Block results, variadic signatures, reference or volatile
results and other targets remain outside this stage.

Qualification is in progress. Publication requires the complete compiler suite,
installed package gates, ownership and debugger matrix, source-free packages,
exact-SDK sanitizer checks, fresh Linux distributions and retained latency data.
