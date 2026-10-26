# C++ coroutine structured group scopes

The 2.4.0 concurrency delivery extended `objc_task_group` to ordinary C++
task coroutines and native Objective-C 3.0 C++ async functions/methods. This
development span uses the existing service ABI 4, frame ABI 1, extension ABI 21
and semantic interface schema 2.1. The
[acceptance receipt](../support/linux_cxx_group_scope_acceptance.json) binds the
source checkpoint, rebuilt compiler, runtime matrices and coordinated SDK delivery.
The preceding [C-frame scope receipt](../support/linux_native_group_scope_acceptance.json)
retains its historical source and archive evidence. The current
[2.5.0 frame-value delivery](linux_cxx_frame_values.md) retains these scope contracts.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>
#include <string>

objc3::task<int> child_work() {
  co_await objc3::task_yield;
  co_return 42;
}

objc3::task<std::string> batch() __attribute__((objc_task_group)) {
  auto group = objc3::task_group_handle::current_scope();
  auto child = child_work().start_child(group);
  co_return std::string("body finished; children still have to join");
}
```

The compiler selects the reserved `__objc3_group_initial_suspend` promise member
for annotated coroutine definitions, including inherited declarations and
template instantiations. The supplied task promise keeps the factory lazy. Its
first resume creates a fresh group and reserves the cleanup guard before any
body work or cancellation checkpoint. Scope-entry failure transports the
original service failure and reclaims captured parameters without entering the
body. A discarded, unstarted factory never enters a group.

`task_group_handle::current_scope()` returns an independent retained view of the
current callback's compiler-owned group. Outside an active scope it throws
`service_failure(INVALID_ARGUMENT)`. The C accessor remains borrowed. Creating,
copying or releasing a group handle alone does not establish a compiler scope.
Membership uses the existing descendant-only admission and nested-group rules.

The canonical module also supplies `objc3::with_task_group<T>(work)` (`T`
defaults to `void`). Its callback receives a retained group view and returns a
lazy task. The callback is registered in the helper's private scope before
dispatch; its argument is a separate child group for user membership and
iteration. The helper awaits the callback and joins its frame/callback cleanup and added descendants
before publishing the result. The callable is a frame
parameter retained through the final join, including when its coroutine
callback or children borrow their closure. Throwing callbacks cancel unfinished
group members. This helper is available in the module's admitted Objective-C 3.0
C++ compatibility surface; ordinary handles remain independent.
Automatic sibling-failure cancellation preserves the callback body so it can
retrieve, rethrow or handle the original child error. Explicit cancellation and
abandonment still request its cancellation. The reserved body-entry operation
is restricted to the active task's compiler-owned scope root.
`task<T>::start_child(group)` and the shared C entry
`objc3_async_frame_spawn_in_group` expose the same registration-before-dispatch
operation. Failed starts preserve the caller's lazy frame; a registration
failure executes no callback body.

Body locals and `defer` handlers follow lexical exit. A logical return, checked
error or representable C++ exception remains owned by the promise at final
suspension. Foreign exceptions keep the task bridge's existing failure transport.
The runtime closes the scope and joins complete task/frame/callback quiescence,
also requesting cancellation on throwing or cancelled exits. The notification
runs on the selected actor/executor and publishes the existing result/error
without resuming a completed coroutine. Cancellation remains monotonic and
dominates publication after cleanup. Even an empty group queues this final
notification.

The same preallocated runtime guard protects lost continuations, discarded
actor jobs and last-owner abandonment during either the body or finalization.
Earlier shielded cleanup dependencies finish before forced scope cleanup. Frame
parameters, nontrivial results and exception owners remain live until the guard
permits destruction. Result-copy failure after the join retains its service
status, including out-of-memory, rather than becoming success or cancellation.
Cleanup record leases are counted separately from ordinary owners, so the
reserved waiter cannot hide abandonment during its own join or an earlier
shielded dependency. This also fixes final-join abandonment for C-frame scopes.

Maintained witnesses cover ordinary and native async syntax, source/PCH
attribute inheritance and templates, checked-error identity, nontrivial
parameters/results, queued empty joins, pre-entry cancellation, failed entry,
failed result copying, pending cancellation, abandoned body awaits and final
suspension, actor discard, nested scopes, earlier shielded cleanup and worker
frame-destruction fences. The source-free package gate exports a scoped C++
provider, checks its `cxx-task-v1`/task-group metadata and consumes it after source
removal and relocation. Callback-helper witnesses also cover move-only closure
ownership beyond callback completion, callback-side result iteration/join,
worker-side callback frame destruction,
registration/queue rejection, original-error handling/recovery, explicit
cancellation, exceptions and void results. The rebuilt
compiler and three focused compiler regressions pass. Source validation covers
156 configurations / 842 commands: 24 spawn source/PCH configurations in each
normal/ASanUBSan/TSan mode, 12 normal and 12 sanitizer coroutine configurations,
40 shared C-frame ABI sanitizer configurations, 16 native C++ async sanitizer
configurations and four structured-runtime sanitizer configurations. GNU libobjc,
project services and clients are instrumented together in GNU-backed sanitizer
runs. All five standalone runtime suites pass normally and under both sanitizers;
22 targeted tooling tests pass. Installed and fresh-distribution evidence is
recorded separately in the receipt.

The annotation requires Objective-C 3.0 C++ language mode; the plain C++ bridge
continues to supply explicit task/group joins. Custom promises without the
reserved scope protocol receive a diagnostic.
Unannotated task coroutines retain explicit joins. Remaining abstract group
library forms, broader ownership/isolation/profile contracts, nontrivial values
in semantic-package class projections and native Linux AArch64 remain separate
work. Public async/actor and full-profile predicates stay reserved.
