# Linux native async function development

The native compiler stage connects logical-result async functions to resumable
Objective-C++20/23 task frames in the Linux x86-64 LP64 GNUstep 2.2 compatibility
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

Use the installed compiler in Objective-C++20/23 mode with
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

Awaiting a lazy task starts a private child on the parent's executor. Leaving
that await cancels an unfinished private child, including parent cancellation,
exception unwinding and frame abandonment. Awaiting an existing `task_handle<T>`
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
Extension ABI 14 requires a full rebuild of objects, PCH files, semantic
interfaces and packages.

This stage supports ordinary free functions with C++ value or void results and no throws
effect. Direct Objective-C object and Block results remain reserved.

The remaining language work includes pure Objective-C async lowering, checked
typed-error propagation across suspension, async methods and Blocks, and compiler
actor isolation. These require separate execution and ownership qualification
before the public language capability can be promoted.
