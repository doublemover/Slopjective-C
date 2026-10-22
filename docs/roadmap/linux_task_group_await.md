# Linux nonblocking task-group joins

The development runtime adds `objc3_task_group_await` and the retained C++
`objc3::task_group_handle`. These changes await build, execution, sanitizer and
installed-SDK validation. Existing acceptance receipts describe their recorded
SDK inputs and do not qualify this addition.

```cpp
#include <objc3c/runtime/objc3_task.hpp>

objc3::task<int> finish_batch(objc3::task_group_handle group) {
  co_await group;
  co_return 42;
}

auto group = objc3::task_group_handle::create();
auto nested = group.child();
nested.add(work);
group.close();
auto result = finish_batch(group).start(executor);
```

Close fixes membership throughout the nested group tree before an await is
accepted. Joining a group that contains the awaiting task is rejected. Several
independent tasks can await the same group; group completion does not consume
the direct-child results returned by `objc3_task_group_next`.

Every successful await queues a continuation, including a previously completed
or empty group. The continuation retains its parent's current executor or actor
destination. Workers never block, and manual executors require their owner to
pump normally. Readiness includes nested groups, terminal tasks, native frame
destruction and completion callbacks active at the quiescence observation.
Callbacks registered after that observation do not invalidate a stored join
result. Persistent weak notifications also cover callbacks registered after a
task's earlier terminal completion.

C frame providers call
`objc3_task_group_await(group, parent, &frame->join_status)` from the parent's
active resume. Successful registration requires suspension. The status slot
must remain in the frame until resumption. The runtime stores `OK`, `CANCELLED`,
or `NOT_READY` for failed failure publication before queueing that continuation.
Rejected registration preserves the slot. The scheduler protects slot access
with the parent's task lock and suspension generation; cancellation, terminal
completion and abandoned frames invalidate later notifications.

This handle exposes shared library ownership. Callers explicitly close and join
groups before releasing their last owner. Cancellation of one waiting parent
leaves shared group tasks running; `group.cancel()` requests cancellation of the
group tree. Dispatch failure terminalizes an unreachable parent as cancellation.
The compiler-recognized structured task scope, its automatic cancellation and
scope-exit join, native C async syntax and public async/actor capabilities remain
separate milestones.

The new exported symbol requires the matching runtime archive. Service ABI 4
record layouts are unchanged, and there is no fallback to a blocking operation.
The updated runtime header changes the exact SDK frame-header identity, so
compilers, headers, source-free interfaces and runtime archives must be rebuilt
and packaged together.

The focused `objc3_task_group_await` CTest covers manual and worker executors,
nested groups, independent waiters, callback and destructor gates, late callback
registration, ownership, cancellation, cycle rejection, actor resumption and
dispatch pressure. The installed coroutine qualifier includes this witness in
its C++20/23 and Objective-C++ ARC/MRC matrix. Its source-free provider also
contains a group-awaiting coroutine, and the sanitizer qualifier instruments
the dedicated witness with the runtime services.
