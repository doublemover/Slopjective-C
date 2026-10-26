# C++ coroutine structured group scopes

The source concurrency module 2.4.0 extends `objc_task_group` to ordinary C++
task coroutines and native Objective-C 3.0 C++ async functions/methods. This
development span uses the existing service ABI 4, frame ABI 1, extension ABI 21
and semantic interface schema 2.1. The accepted installed SDK remains the
[C-frame scope delivery](../support/linux_native_group_scope_acceptance.json)
until this combined compiler/library span is qualified and packaged.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>
#include <string>

objc3::task<int> child_work() {
  co_await objc3::task_yield;
  co_return 42;
}

objc3::task<std::string> batch() __attribute__((objc_task_group)) {
  auto child = child_work().start_child();
  objc3::task_group_handle::current_scope().add(child);
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
lazy task. The helper awaits that callback and implicitly joins its added
descendants before publishing the callback's result. The callable is a frame
parameter retained through the final join, including when its coroutine
callback or children borrow their closure. Throwing callbacks cancel unfinished
group members. This helper is available in the module's admitted Objective-C 3.0
C++ compatibility surface; ordinary handles remain independent.

Body locals and `defer` handlers follow lexical exit. A logical return, checked
error or native exception remains owned by the promise at final suspension.
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
ownership beyond callback completion, exceptions and void results. The rebuilt
compiler and three focused compiler regressions pass. All 24 spawn source/PCH
configurations (108 commands), 12 normal coroutine configurations (114 commands)
and 15 targeted tooling tests pass. Sanitizer and installed delivery checks
follow this coherent source span.

Custom promises without the reserved scope protocol receive a diagnostic.
Unannotated task coroutines retain explicit joins. Remaining abstract group
library forms, broader ownership/isolation/profile contracts, nontrivial values
in compiler-defined explicit C frames and native Linux AArch64 remain separate
work. Public async/actor and full-profile predicates stay reserved.
