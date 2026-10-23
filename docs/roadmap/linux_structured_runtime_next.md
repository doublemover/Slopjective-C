# Linux structured runtime development

This file tracks maintained runtime work following the task-destination and
nonblocking group-await milestones. Implementation and fixture authoring are
frontloaded; no build, execution, sanitizer, or installed-SDK qualification is
claimed for these additions. Existing receipts describe their recorded inputs.

## Structured child cancellation

`objc3_task_start_child`, including `objc3_async_frame_start_child`, now registers
the child's cancellation ancestry before publishing its first resume. The child
inherits the parent's current destination and cancellation state. A later parent
request marks every live structured descendant before dispatching wakeups;
suspended descendants resume at their cancellation observation point. Cancelling
one child leaves its ancestors and siblings alone. `objc3_task_start` remains an
independent start without inherited cancellation.

Cancellation links retain ancestry tokens, with weak links toward descendants
and weak task pointers. They retain no task handle, result, executor, or native
frame. Consequently parent cancellation still reaches a live grandchild after
the intermediate child has completed and its last handle has been released.
Registration and cancellation share a token lock, so a worker cannot observe an
uncancelled child created after its parent's request. Cancellation marking never
invokes callbacks or releases retained task records under group/publication
locks. Wake dispatch and failure cleanup run after those locks have been dropped.

The installed C frame fixture authors witnesses for cancellation before spawn,
cancellation of already-suspended descendants, child-only cancellation, detached
independence, and a released intermediate child. The actor-scheduling fixture
authors a worker registration race with a cancellation request made during the
active parent resume. These witnesses use existing maintained suite entrypoints;
execution is deferred. Public service ABI layouts and versions are unchanged.

Cancellation ancestry is a cancellation relationship. The shared task API still
requires callers to join owned work explicitly; it does not introduce automatic
structured scope finalization or compiler actor isolation.

## Lost continuations and frame cleanup

Actor-discarded resumes, deferred dispatch failures, and last-owner abandonment
now publish task cancellation and request cancellation of structured descendants
before terminalizing the unreachable parent. A child that has hopped to a
different executor is still reached. The active resume flag continues protecting
native frame access until the originating callback and its autorelease pool have
finished. An unexpected exception from a resume callback is contained and follows
the same cancellation cleanup path, rather than stranding the frame silently.

Maintained fixtures author abandoned-parent, cancelled-actor, closed-hop, and
throwing-resume witnesses. They check descendant wakes on another executor,
exactly-once frame cleanup, and harmless late dependency notifications. These
paths remain subject to the deferred validation phase.
