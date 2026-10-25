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

Source checks pass: the 24-mode spawn/join matrix, all 12 coroutine/group
configurations, and all five standalone runtime suites. The C matrix also passes
24 modes under ASan/UBSan and 24 under TSan. TSan uses a separately built,
fully instrumented GNU Objective-C runtime so its Block retain/release atomics
participate in the happens-before graph. The coroutine sanitizer matrix passes
all 12 configurations, including cross-executor nontrivial C++ results.

Full runtime instrumentation exposed a race in GNU libobjc's lazy global
`NSAutoreleasePool` mode switch. The pinned Linux GNUstep 2.2 patch now uses
native per-thread ARC pools exclusively. Foundation fallback callbacks, mode
switching and the legacy retained-return slot are removed. Return-value claiming
checks nonempty pool bounds and handles nil without reading before pool storage.
A failed pool allocation terminates rather than losing an autorelease owner.
The Linux runtime profile locks the native ARC policy and exact patch bytes.

The runtime builder accepts an explicit sanitizer and binds the compiler/runtime
libraries into its separate development build identity. Allocator-failure
witnesses use link wrapping, preserving sanitizer interceptors. Worker-signal
and callback-fence fixtures now complete their lifetime handoffs before stack
state can be destroyed.
Normal, ASan/UBSan and TSan standalone suites pass after those repairs.

Broader source and coordinated SDK delivery checks follow this coherent span.
The current 33-gate SDK remains the earlier delivery; source validation does not
claim these newer headers/runtime changes are already present in that archive.

These joins supply a prerequisite for structured-scope lowering. Automatic
scope finalization, shielded caller unwind, native group-result iteration and
complete profile admission remain open. A normal adapter await cannot substitute
for the runtime's direct cancellation-shielded cleanup registration.
