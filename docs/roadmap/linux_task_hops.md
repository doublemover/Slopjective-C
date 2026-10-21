# Linux task destinations and actor resumption

Tasks can start on actor mailboxes and move between executors and actors without
blocking a worker. The runtime retains each destination and serializes native
frame access across the handoff. Compiler actor isolation remains a separate
language milestone.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>

int update(objc3_actor_t *destination) __objc3_async {
  __objc3_await objc3::hop_to(destination);
  __objc3_await objc3::task_yield;
  return 42;
}

auto running = update(second_actor).start_on(first_actor);
```

Standard C++20/23 coroutines use the same `hop_to` awaitable with `co_await`.
`start_on(actor)` queues the first resume in that actor's mailbox. `start(executor)`
selects an executor without an actor. `hop_to` retains its argument immediately,
so its move-only `task_destination` value can safely be passed into a lazy frame.
Other borrowed coroutine arguments still follow native C++ lifetime rules.

Every successful hop suspends and queues a continuation, including hops to the
current actor or executor. A different actor on the same executor changes the
logical actor identity. Hopping to an executor clears actor affinity. The new
destination remains selected through subsequent yields and dependency waits.
Awaiting a lazy child inherits the parent's current destination; the parent's
continuation retains its own destination even when a child hops elsewhere.

Each resume segment is a mailbox job. Suspension permits other actor jobs to
run, so callers must recheck state after awaiting. The runtime publishes a
continuation only after the active native resume callback has returned. This
also applies when another thread completes a dependency during that callback.
A fast destination cannot resume or discard the frame while the source still
uses it.

C clients select exactly one of `executor` or `actor` in
`objc3_task_start_options`. `objc3_task_hop(task, executor, actor)` changes that
selection from the task's active resume callback. The callback returns to
publish the requested suspension.
`objc3_task_start_child` starts a child on the active parent's destination;
its supplied options leave both destination fields null. These operations reject
overlapping suspension requests and calls outside the task's active resume.
Failed starts preserve caller ownership of the frame.

Cancellation remains cooperative. A queued hop observes cancellation before
executing subsequent user code. Cancelling an actor discards queued resumes and
terminalizes their tasks as cancelled, reclaiming each frame once. Closed or full
mailboxes and failed executor dispatch likewise terminalize deferred resumes.
A failed dispatch cannot strand a frame after the originating callback returns.

Actor close rejects new jobs while allowing already queued jobs to run. Actor
cancellation affects queued jobs; a task suspended on an independent dependency
still needs task or group cancellation to wake it promptly. Close and join task
groups before closing their actors. Actor join covers mailbox work, while group
join covers task completion, registered completion callbacks and frame cleanup.
Pending task waits and group joins return `NOT_READY` from any runtime executor
callback: a task can hop back to that executor after the wait begins.
Group cancellation marks sibling and ancestor cancellation flags before
publishing failure. It dispatches their wakeups after releasing group and failure
publication locks, so a failed dispatch can complete tasks and reenter group APIs
without deadlocking.

Mailbox affinity governs resume segments. Completion observers and forced frame
destruction retain the public task service's existing callback rules and may run
on the thread that completes or releases a task. This runtime bridge does not
authorize actor-isolated access from arbitrary destructors or completion
callbacks. Compiler-enforced isolation and its cleanup rules require their own
implementation and qualification.

Service ABI 3 adds the actor destination to task start options and changes the
coroutine promise's scheduling contract. Rebuild runtime clients, native coroutine
providers, PCH files, semantic interfaces and binary packages together. The compiler
extension ABI remains 14; compiler sources and the qualified LLVM foundation are
unchanged by this runtime milestone.

Qualification extends the existing installed C/Objective-C/C++ runtime,
source-free coroutine provider, native async PCH and relocated package suites.
Contracts include same-actor hops, different actors sharing an executor,
cross-worker actor hops, inherited children, retained lazy destinations,
cancellation, queue exhaustion and deferred publication. ARC objects, Blocks and
native exception payloads are exercised across actor suspension with instrumented
runtime services and clients.

The [acceptance receipt](../support/linux_async_checked_errors_acceptance.json) binds the
exact SDK archive, all 22 installed package gates, 66 sanitizer configurations
and fresh Debian 13/Ubuntu 26.04 task-destination qualification. The fresh runs
include relocation, embedding, ordinary-user runtime/coroutine matrices and
source-free async packages. The LLVM compiler foundation is byte-identical to
the preceding native async release. The
[latency report](../support/linux_async_checked_errors_latency.json) records five quiet
command measurements against the current budgets.

Six additional O0 LLDB cases inspect local identities in a frame resumed after
an actor hop, across C++20/23 and Objective-C++ ARC/MRC. Reproduce them from a
checkout against the installed SDK:

```sh
python -B scripts/qualify_linux_task_hops_debugger.py \
  --sdk-root out/toolchains/linux-sdk/current \
  --output-dir out/task-hops/hop-debugger
```
