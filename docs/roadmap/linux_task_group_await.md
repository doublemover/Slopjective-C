# Linux nonblocking task-group joins

The development runtime adds `objc3_task_group_await`,
`objc3_task_group_await_next` and the retained C++ `objc3::task_group_handle`.
These changes await build, execution, sanitizer and
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

Closed groups also provide typed asynchronous result iteration:

```cpp
objc3::task<int> sum_batch(objc3::task_group_handle group) {
  int total = 0;
  while (auto completed = co_await group.next<int>())
    total += completed.result();
  co_await group;
  co_return total;
}
```

`next<T>()` returns one retained `task_handle<T>` or an empty handle when all
direct tasks have been consumed. Every operation queues a continuation,
including ready results and closed drain. Nested groups supply their own
iteration results. A group with only nested tasks can finish direct iteration
before its recursive join becomes ready. A returned task is terminal; frame
and callback cleanup remain covered by the separate group join.

Registered consumers reserve in FIFO order, selecting direct tasks by
completion order. Reservation occurs under the completion-publication lock,
so a polling consumer cannot steal a task already assigned to an async waiter.
Canonical type identity, representation and layout are checked before
consumption. A mismatch reports `service_failure(TYPE_MISMATCH)` and leaves that
task available for a matching consumer. The runtime owns copied expected-type
identity bytes through suspension. Failed and cancelled child tasks are also
returned as retained handles; their ordinary `result()` operations report the
failure or cancellation. First-failure sibling cancellation precedes result
reservation as it precedes ordinary group completion publication.

Cancellation before reservation consumes nothing. Once reservation assigns an
owned task to the live parent frame, that task remains consumed. Later parent
cancellation or dispatch failure releases the reserved owner through frame
cleanup. Releasing a pending consumer invalidates its suspension generation;
its late notification cannot consume a result or write an abandoned slot.

C frame providers call
`objc3_task_group_await(group, parent, &frame->join_status)` from the parent's
active resume. Successful registration requires suspension. The status slot
must remain in the frame until resumption. The runtime stores `OK`, `CANCELLED`,
or `NOT_READY` for failed failure publication before queueing that continuation.
Rejected registration preserves the slot. The scheduler protects slot access
with the parent's task lock and suspension generation; cancellation, terminal
completion and abandoned frames invalidate later notifications.

C providers use
`objc3_task_group_await_next(group, parent, expected_type, &frame->status, &frame->result)`
with an initially null result slot. The optional expected descriptor is copied
during registration. Both slots remain in the frame until resumption. `OK`
transfers one retained task, `CLOSED` returns a null result at direct-child
drain, and cancellation or mismatch transfers no task. Frame destruction must
release a nonnull reserved result, including when resumption never occurs.
Registration failure preserves both slots. The same closed-tree and membership
cycle checks apply to joins and result iteration.

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
registration, typed FIFO result reservation, descriptor copying, ownership,
cancellation before and after reservation, cycle rejection, actor resumption
and dispatch pressure. The installed coroutine qualifier includes this witness in
its C++20/23 and Objective-C++ ARC/MRC matrix. Its source-free provider also
contains a group-awaiting coroutine, and the sanitizer qualifier instruments
the dedicated witness with the runtime services.
