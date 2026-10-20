# Linux structured concurrency runtime

The public runtime API exposes task groups and serial actor mailboxes through
installed C headers and `objc3c::RuntimeServices`. It builds on native task
completion, explicit value operations and bounded serial executors. Language
async/await syntax, compiler actor isolation and automatic capture adaptation
remain reserved pending their own compiler implementation and qualification.

Include `<objc3c/runtime/objc3_runtime_services.h>` from the installed SDK. A
CMake consumer imports its include path and thread dependency through the runtime
target:

```cmake
find_package(objc3c REQUIRED CONFIG)
target_link_libraries(application PRIVATE objc3c::RuntimeServices)
```

Groups retain registered tasks and nested groups. Closing a group recursively
prevents registration. Cancellation requests cooperative cancellation throughout
the tree. The first failed task requests sibling cancellation through its
ancestors before entering the group's completion queue. `objc3_task_group_next`
polls direct task completions without blocking or pumping. It returns a retained
task, `NOT_READY`, or `CLOSED` when a closed group's direct tasks have drained.
Nested groups join recursively and keep separate completion queues.
Inspect a retained task with `objc3_task_get_completion` or register a completion
waiter; an already completed task invokes the waiter immediately. A polled view
is borrowed until task release; a callback view is borrowed for callback scope.
Callers copy a payload or retain an error before keeping it independently.

Join requires a closed group and includes terminal completion, registered
callbacks and frame cleanup. A wait on pending work from its own task or any
runtime executor callback returns `NOT_READY`. Manual executor owners pump explicitly and retry.
Callers close and join before releasing their final group reference.

Actors serialize jobs on a chosen executor. Successful enqueue transfers context
ownership; failed enqueue leaves it with the caller. The context destructor runs
once after execution or cancellation. Same-actor reentry queues the new job.
Actor and executor hop facts use logical identities; two actors on one executor
still have distinct actor isolation domains. Facts provide runtime identity and
scheduling information, not compiler enforcement of actor-isolated data access.

Actor close prevents enqueue; cancellation also discards unexecuted work while
active jobs finish cooperatively. Join includes all context cleanup. An actor
callback cannot join itself, and pending manual/current-executor waits return
`NOT_READY`. Last caller release cancels remaining mailbox work. Callbacks obey
the public C ABI's ownership and exception contract.

The installed suite covers C17, Objective-C17 ARC/MRC, Objective-C++20/23 ARC/MRC
and C++20/23 at O0/O2, plus C++20/23 worker execution at both optimization levels.
It checks nested cancellation, nonblocking completion polling, manual pumping,
ownership transfer, reentry, hop identities, serial mailboxes and cleanup. The
worker probe demonstrates simultaneous execution on two executors, serialized
actor queues, 130 actor jobs and 128 group tasks.

The [Linux acceptance receipt](../support/linux_task_coroutines_acceptance.json)
binds 22 installed language/optimization executions, native worker tests,
instrumented ASan/LeakSanitizer/UBSan/ThreadSanitizer at O0/O2, and fresh
Debian/Ubuntu archive qualification to the exact implementation and SDK seal.
The [native coroutine bridge](linux_task_coroutines.md) adds queued dependency
handoff, cancellation wakeup, compiler-owned frames and callback autorelease pools.
The [task destination bridge](linux_task_hops.md) adds actor-bound resumable tasks
and explicit executor hops while preserving mailbox identity across suspension.
