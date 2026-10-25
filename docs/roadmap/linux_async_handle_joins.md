# Native task and group handle joins

This source span adds nonblocking handle awaits on the existing shared frame
ABI. It follows the [33-gate delivery](../support/linux_async_lexical_pool_acceptance.json);
that installed archive does not include these newer runtime/header changes.
The concurrency module's source version is 2.1.0. Runtime service ABI 4, async
frame ABI 1, native extension ABI 21 and interface schema 2.1 are unchanged.

`objc3_async_frame_from_task` creates a lazy frame with an independent reference
to an existing task and its actual canonical result descriptor. A task must have
a declared result type or an already available typed completion. Factory failure
leaves output storage unchanged. Starting transfers the frame normally; failed
start or type adoption leaves its owner responsible for disposal.

The adapter registers a nonblocking dependency, then forwards the original value,
error or cancellation. Publication copies/retains the borrowed completion while
the dependency is still owned. Copy failure preserves the service error,
including allocation failure. Cancelling or disposing an adapter drops its lease
without explicitly cancelling another owner's shared task. Ordinary last-owner
abandonment rules still apply.

`objc3_async_frame_from_group` retains a group and returns a void frame. Its first
resume closes the complete group tree and awaits quiescence. Construction leaves
registration open; even an empty join queues a continuation. Caller cancellation
remains observable. The adapter rejects its own/direct structured parent's task
or group membership, including nested membership, so the additional frame cannot
hide those self-dependency cycles. Identity tokens avoid retaining an ancestor or
comparing a recycled task address.

Objective-C C17 uses a typed declaration, followed by an ordinary native await:

```objective-c
OBJC_TASK_JOIN_DECLARE(join_count, int);
OBJC_TASK_JOIN_DECLARE_THROWING(join_checked_count, int, id<Error>);

int count_existing(objc3_task_t *task) __objc3_async_frame {
  return __objc3_await join_count(task);
}

void finish_existing(objc3_task_group_t *group) __objc3_async_frame {
  __objc3_await objc3_task_group_join_async(group);
}
```

Typedef result types support records, object pointers and Blocks. Copy-out checks
canonical identity, layout and result kind; a mismatched declaration reports
`TYPE_MISMATCH`. The explicit throwing declaration preserves the checked error
domain for `try` and native typed handlers. Reserved physical factories return
allocation-independent error frames, preserving invalid-input/OOM failures before
typed result extraction.

C++ uses `objc3::join_task<T>(raw_handle)` and
`objc3::finish_task_group(group_handle)`. They return lazy typed tasks, retain
independent handles immediately, and release failed adoptions without consuming
the caller's reference. Nontrivial C++ results use the existing result operations.

Focused checks pass: standalone runtime build/lifecycle execution, C17 ARC/O0
source and MRC/O2 PCH spawn witnesses, and a C++23 MRC/O2 coroutine/group preflight.
Witnesses cover scalar/record/void/object/Block results, checked errors, independent
leases, lazy closure, cancellation, type mismatch, failed allocation/copy,
empty groups, nested/self cycles and nontrivial C++ string results. The existing
spawn and coroutine runners enroll these source witnesses; sanitizer runtime
source lists include the new adapter member. Broader source validation follows
this coherent span. No SDK archive is rebuilt during this development step.

These joins supply a prerequisite for structured-scope lowering. Automatic
scope finalization, shielded caller unwind, native group-result iteration and
complete profile admission remain open. A normal adapter await cannot substitute
for the runtime's direct cancellation-shielded cleanup registration.
