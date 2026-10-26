# Native C-frame structured group scopes

The Linux SDK implements compiler-owned group scopes for native C-frame
function and Objective-C method definitions carrying `objc_task_group`.
The concurrency module is 2.3.0. The
[acceptance receipt](../support/linux_native_group_scope_acceptance.json) binds
the source, compiler/runtime checks and installed archive. The exact archive
passes fresh Debian 13 and Ubuntu 26.04 qualification, including ordinary-user
consumers, source-free packages, relocation and debugging. Both owned containers
are removed. Public async/actor and full-profile predicates
remain reserved. Service ABI 4, frame ABI 1, extension ABI 21 and interface
schema 2.1 are unchanged.

```objective-c
int run_batch(void) __objc3_async_frame __attribute__((objc_task_group)) {
  objc3_task_group_t *group = objc3_task_group_scope_current(); // borrowed
  objc3_task_t *child = OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {
    // Child work, including ordinary suspension.
  });
  if (!child) return -1;
  if (objc3_task_group_add(group, child) != OBJC3_SERVICE_OK) __builtin_trap();
  objc3_task_release(child);
  return 42;
} // Compiler closes and joins before publishing the result.
```

The lazy factory retains its ordinary frame captures. A fresh group and a
reserved finalization waiter are created on first resume before any source body
work. A frame disposed before first resume never enters a scope. The current
accessor returns a borrowed handle only inside the active scope task; retain it
explicitly to keep another owner. Nested groups inherit scope identity. The tree
admits only structured descendants of that task, including through released
intermediate tasks. Independent tasks, the scope task and its ancestors are
rejected before registration.

Every body return stages its logical result in frame-owned storage. Body locals
and defers follow lexical exit; frame captures, staged values and owned checked
errors remain live while the compiler closes the tree and waits for complete
child task/frame/callback quiescence. Normal exit joins without requesting
cancellation. Checked failure, escaping native exception and cancellation also
cancel remaining work. A native exception keeps the established cancellation
transport rather than being relabelled as a checked error. Exceptions-enabled
scope definitions require Objective-C exception handling, so the native body
catch boundary can contain escaping exceptions before finalization.

The implicit join uses the scope task directly, with a reserved suspension
generation and cancellation shielding. Even an empty group queues a continuation
on the task's current actor/executor. Cancellation remains monotonic, propagates
to structured descendants and becomes the final outcome after cleanup. Scope
entry reserves its waiter; closing and requesting cancellation do not create
snapshots. Scheduling and existing completion/failure publication keep their
ordinary allocation/error contracts.

The runtime prevents manual completion from bypassing a live scope. Lost
continuations, actor discard, dispatch failure and last-owner abandonment close,
cancel and join through the reserved waiter before destroying the frame. A body
that already installed a shielded cleanup dependency finishes that dependency
first. Its late ordinary notifications cannot revive an abandoned body or write
into reclaimed frame slots. A separate scope owner preserves group lifetime
through active callbacks and frame destruction; releases run outside task and
group locks.

Maintained runtime witnesses exercise normal and cancelled finalization, nested
groups, ancestor rejection, last-owner abandonment, actor-discarded resumes and
an exception after registering an earlier shielded cleanup. Source fixtures add
pending scalar returns, original checked-error identity, escaping native
exceptions, owned optional results, receiver ownership, body-await abandonment
and nested C++ explicit-frame scopes
on an actor. Pending and already-ready worker children also verify actor
restoration and quiescence fences. Compiler regressions cover implicit calls,
deferred cleanup, C++ explicit-frame lowering, ARC debug information and exception
admission.

The maintained native async package gate also exports a scoped C-frame provider,
checks its physical frame and task-group metadata, then consumes it after source
removal and package relocation. Its four C17 ARC/MRC O0/O2 package modes are part
of the combined SDK and fresh-distribution checks. The rebuilt compiler passes
361 Clang/index/query regressions and 166 native contracts. All five standalone
runtime suites and 24 spawn configurations pass normally, under ASan/UBSan and
under TSan; the broader C-frame/coroutine/native-async instrumentation also
passes with instrumented GNU libobjc. All 33 installed SDK gates pass.

Ordinary C++ task-coroutine scope lowering, the remaining abstract group-library
forms and broader ownership/isolation/profile admission remain implementation
work. An ordinary function annotated by a library is not automatically converted
into a native frame scope; this lowering applies to admitted native C-frame
bodies. Explicit handle adapters remain available independently.
