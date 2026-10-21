# Linux native async function development

The native compiler stage connects logical-result async functions to resumable
Objective-C++ (C++20/23 mode) task frames in the Linux x86-64 LP64 GNUstep 2.2 compatibility
profile. The reserved compiler spellings
are `__objc3_async` and `__objc3_await`. Public `async`/`await` support and compiler
actor isolation remain reserved in the capability authority.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>

int leaf(int value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}

int parent(int value) __objc3_async {
  return 1 + __objc3_await leaf(value);
}

// Creating the call copies value arguments into a lazy native frame.
auto pending = parent(41);
auto running = std::move(pending).start(executor);
```

Use the installed compiler in Objective-C++ (C++20/23 mode) mode with
`-fcoro-aligned-allocation`. Link the runtime through `objc3c::RuntimeServices`
or use the [direct-link options](linux_task_coroutines.md). Each root task starts
on an explicit public runtime executor. Native await uses the compiler's
coroutine machinery, suspends without blocking and queues subsequent execution.

The canonical function type retains its logical result and async effect. An
evaluated async call produces `objc3::task<R>` for explicit start or nested await.
Return statements initialize the logical result before publishing it through
the promise. The private physical return ABI uses the task class, including its
native C++ return-object convention. Every participant must use the same compiler
extension ABI and runtime header.

Awaiting a lazy task starts a private child on the parent's current destination. Leaving
that await cancels an unfinished private child, including parent cancellation,
exception unwinding and frame abandonment. The runtime also supports
[actor-bound starts and executor hops](linux_task_hops.md). Awaiting an existing `task_handle<T>`
keeps its shared dependency semantics. Cancellation remains cooperative;
code between suspension points must check cancellation when necessary.

Arguments and references follow C++ coroutine lifetime rules. Pass owned values
when they must survive suspension. ARC owns native local objects and Block
captures through native frame cleanup. MRC code supplies explicit ownership.
Results require copy construction and nonthrowing destruction, matching the
public task runtime. Ordinary C++ exceptions retain their owned exception payload
and rethrow at await or result access.

PCH serialization preserves the derived task representation alongside the
logical function signature. AST import transfers it into the receiving context.
That representation does not replace canonical source effect identity.
Source and immutable package builders supply the full-frame allocation option
and link the installed task runtime. Imported function declarations preserve the
async effect and logical result; providers retain their module-owned native
symbols after relocation.
Extension ABI 19 requires a full rebuild of objects, PCH files, semantic
interfaces and packages.

This stage supports free functions, [C++ member callables](linux_async_cxx_callables.md)
and [Objective-C methods](linux_async_objc_methods.md), with C++ value, void, Objective-C object/protocol/Class and Block results. The
[checked async error contract](linux_async_checked_errors.md) adds checked
throws with propagation, optionalization and suspending typed handlers. The
[checked value result contract](linux_async_value_results.md) includes copyable
C++ records, complex values and member pointers, with completion inspected before
result construction. The
[owned result contract](linux_async_owned_results.md) retains objects and copies
Blocks independently of frame cleanup. Successful reads and awaits return +0
values through the current autorelease pool; ARC locals retain across later
suspension, while MRC code retains explicitly. Task and handle types normalize
direct Objective-C pointer ownership for a consistent native C++ ABI.

Objective-C method frames own their receivers before initial suspension under
both ARC and MRC; native C++ member frames retain their borrowed-`this` contract.
Dynamic, protocol, superclass and direct messages preserve the logical effects
and return physical task carriers. Nil sends yield empty tasks whose start or
await reports an invalid-argument service failure.

[Async Objective-C++ Blocks](linux_async_blocks.md) own a heap copy before
initial suspension, preserving captures and checked errors after the caller's
Block leaves scope. Source-free aliases and factories retain their effects.

The remaining language work includes pure Objective-C async lowering and
compiler actor isolation. These require separate execution and ownership qualification
before the public language capability can be promoted.

The [installed SDK acceptance receipt](../support/linux_async_blocks_acceptance.json)
binds this stage to its exact compiler, runtime, archive and source inputs. It
records sixteen native execution configurations across all ARC/MRC
provider/consumer pairs, source-free object and Block package consumption,
frame debugging, instrumented runtime/client checks and ordinary-user execution
on fresh Debian 13 and Ubuntu 26.04 systems. The
[latency report](../support/linux_async_blocks_latency.json) retains the measured
command costs and budget comparisons for the qualified SDK.
