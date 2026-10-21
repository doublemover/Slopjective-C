# Linux resumable task bridge

The task runtime supports a nonblocking dependency handoff and native C++20/23
coroutines in C++ and Objective-C++ programs. The compiler owns the suspended
frame; the public task service owns scheduling, completion and final frame
destruction. Objective-C `async`/`await` syntax and compiler actor isolation remain
reserved in the language capability authority.

Include `<objc3c/runtime/objc3_task.hpp>` and link `objc3c::RuntimeServices` from
the installed SDK. The CMake target supplies Clang's aligned coroutine allocation
option and LLD's DWARF dead-code tombstones. Direct compiler invocations require
`-fcoro-aligned-allocation` and these link options:

```text
-fuse-ld=lld
-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff
-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff
```

Aligned allocation uses the full compiler-computed frame alignment, including
over-aligned locals and results. The promise provides only the aligned allocation
overload, so a missing compiler option fails compilation. DWARF 5 tombstones
identify discarded template definitions without creating overlapping ranges at
address zero. Qualification verifies the linked debug information.

```cpp
#include <objc3c/runtime/objc3_task.hpp>

objc3::task<int> transform(objc3::task_handle<int> input) {
  int value = co_await input;
  co_return value + 42;
}

// executor is an explicitly selected, retained public runtime executor.
auto result = transform(input).start(executor);
```

`task<T>` is a lazy, move-only frame. Starting transfers that frame to the runtime
only on success and returns a retained `task_handle<T>`. Arguments follow native
C++ coroutine lifetime rules: pass owned values when they must outlive the call.
Use named coroutine functions rather than a temporary capturing coroutine lambda
whose closure could expire while its frame remains suspended.

Awaiting a handle always queues a continuation on the parent's executor, even
when the dependency is already complete. It never blocks a worker, pumps a manual
executor or runs the parent inline on the dependency's executor. C clients use
`objc3_task_await` from the parent's active resume callback. Self-await and a
second outstanding await are rejected. A client keeps its dependency handle
alive while inspecting the outcome.

`objc3_task_get_completion` returns a terminal view without blocking; `NOT_READY`
leaves the output unchanged. The view remains borrowed from the retained task.
`task_handle<T>::result()` copies a completed value and reports a pending result
as `service_failure(NOT_READY)`. `wait()` is an explicit blocking operation with
the public runtime's current-executor restrictions. Manual executor owners pump
explicitly. Zero and false are present values; void success has no payload.
Results require copy construction and a nonthrowing destructor.

Cancellation is cooperative. Cancelling an awaiting parent queues its
continuation so `co_await` throws `task_cancelled` and native lexical cleanup
runs. It leaves shared dependencies running. `task_checkpoint` observes
cancellation without suspension; `task_yield` queues a new resume and observes
cancellation when execution continues. Code without an await, checkpoint or
yield remains responsible for its own cancellation checks.

Releasing the last suspended parent handle terminalizes its task and destroys
its frame. Child callbacks carry a separate lifetime token and generation, not a
pointer into the frame or a retaining parent cycle. Late callbacks cannot revive
a completed parent or access a freed frame. A full executor queue or failed
dispatch cancels a waiting parent so an unreachable suspension cannot retain
its frame indefinitely. Failed result copying or error allocation likewise falls
back to terminal cancellation.

C++ exceptions are carried as owned `exception_ptr` payloads and rethrown at the
next await or result read. Foreign public runtime errors retain their opaque
payload in `task_error`. Exceptions never cross the runtime's C callback ABI.
The GNU runtime exception header owns its thrown Objective-C object. Its C++
adapter uses GNU's refcounted primary-exception initialization and destructor,
including the initial in-flight reference. This keeps captured exceptions alive
after an autorelease pool drains and balances dependent rethrow and final release.
An unrepresentable foreign exception terminalizes as cancellation rather than
publishing an uninitialized successful result.
The promise owns completion and resume operations; a handle's native pointer is
available for groups, cancellation and completion observation. Callers do not
manually complete or resume a promise-owned task.

Native C++ destructors, ARC references and Block captures remain in the compiler
frame across suspension. MRC code supplies explicit ownership, such as an RAII
owner whose destructor releases the reference. The bridge does not infer MRC
capture ownership or introduce actor isolation.

Executor callbacks establish an autorelease pool when the GNU Objective-C runtime
is linked. Each resume drains its temporaries before the next callback; strong
frame references survive suspension. Pure C/C++ services retain no mandatory
Objective-C library dependency. Task-group join covers frames and registered
completion callbacks; an executor fence can additionally observe the completed
drain of autoreleased temporaries.

The installed qualification compiles providers, removes their implementation
source, then builds and runs consumers in twelve C++20/23 and Objective-C++20/23
ARC/MRC O0/O2 configurations. Each verifies linked DWARF; O0 cases also inspect
resumed values and the coroutine caller frame in LLDB. Contracts cover shared
dependencies, cancellation, abandonment, queue pressure, foreign and native
errors, zero/false/void and over-aligned values, retained Objective-C results,
Objective-C exceptions rethrown after their autorelease pool, objects and Blocks across
suspension, a 2,048-level await chain, group integration and worker races.

The sanitizer owner builds the task, executor, group, actor and value/error
service implementations together with provider and consumer frames. It runs
ASan/LeakSanitizer/UBSan and ThreadSanitizer at O0/O2 in C++20 and Objective-C++20
ARC/MRC. The underlying GNUstep Objective-C runtime is the selected SDK binary;
these probes do not claim to rebuild it with instrumentation.

The pinned GNUstep source also fixes selector/dispatch lock ordering. Registration
acquires the runtime lock before mutating selectors and grows dispatch tables
before taking the selector lock. Growth covers the full required depth in one
operation. Dispatch's built-in selectors initialize before classes can receive
messages, avoiding lazy registration under the class-initialization lock. The
ARC/MRC threaded coroutine probes retain TSan's lock-order diagnostics.

The [acceptance receipt](../support/linux_task_coroutines_acceptance.json) binds
the installed SDK, its source and runtime identities, native/debugger matrices,
sanitizer instrumentation and fresh Debian/Ubuntu executions. The
[latency report](../support/linux_task_coroutines_latency.json) records five quiet
runs against the current regression budgets.
