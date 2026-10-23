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

Cancellation marking, wake traversal, and final ancestry-token reclamation use
allocation-free iterative walks. A deep structured tree no longer adds one
native call frame per child while requesting cancellation or releasing its last
descendant. Registration prunes expired links before cancellation and freezes
existing child positions afterward. An authored 1,536-frame native C chain
crosses executor queue admission, checks every cancellation/cleanup, then drops
late dependency notifications and the completed ancestry chain.

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

## Canonical main and global executors

Part 7 §7.4.2 now has concrete C runtime destinations:
`objc3_executor_get_main(&executor)` and
`objc3_executor_get_global(&executor)`. Each accessor returns a retained handle;
repeated access preserves the same logical executor identity. A process-lifetime
runtime lease prevents the last client release from closing shared scheduling.
Initialization errors are reported through the ordinary service status and may
be retried. The global executor has a bounded pool of four workers, allowing
independent callbacks to run concurrently. Its reserved workers do not consume
any of the eight caller-created serial worker slots. Actor mailbox jobs and
native task resumes retain their own exclusion rules. Partial pool creation
failure stops and joins already-created workers before returning the error;
no partially initialized destination is published.

The main executor is manual. Linux process main-thread identity, rather than
the thread that first asks for the handle, governs pumping. A non-main thread
receives `NOT_READY` with zero executed callbacks; it cannot execute main-affine
work. Hosts pump main work through the existing executor API. Explicitly created
manual executors retain their existing serialized pumping rules. No struct
layout or service ABI version changes accompany the additive accessors.

Authored C and C++ witnesses cover stable destinations, initialization from a
worker, rejection of pumping on another thread, current logical identities,
four simultaneously active global callbacks, and releasing all client handles
while shared work is pending. Compiler attributes and imported isolation metadata can now identify
these real destinations, but implicit cross-executor call lowering remains a
compiler responsibility. Validation is deferred.

## Operation lifetime and nonblocking runtime waits

Group registration, recursive close, blocking completion iteration, and join
hold operation leases separately from their task observers. In particular,
registering an already-terminal task can invoke a synchronous failure observer
which releases the last caller group reference. Registration bookkeeping still
owns the group until it finishes. Group join keeps that ownership through native
frame and callback quiescence after task completion publication. Actor joins
likewise keep an internal operation lease while context cleanup finishes.

Pending task waits on manual destinations return `NOT_READY`, allowing the owner
to pump. Pending actor joins and the internal blocking group-next operation also
return `NOT_READY` from any runtime executor callback, including a different
executor. Such callbacks may be needed to unblock the target's future work.
Completed observations remain immediately available. Authored witnesses cover
reentrant last-owner release during terminal registration, manual-task waits,
and a pending cross-executor actor join. Execution remains deferred.

The canonical-global fixture also authors two simultaneously active actor
mailboxes, repeated mailbox cleanup, and a task that repeatedly yields on the
four-worker pool. These distinguish allowed concurrency between independent
actors from required exclusion within each actor and each native task frame.

## Named executor identity and lifetime

`objc3_executor_register_named(name, executor)` retains an executor under an
owned exact-byte name. Names use the ordinary string bytes accepted by
`objc_executor(named("..."))`, without Unicode normalization or case folding.
An active duplicate naming the same executor returns `OK`; a conflicting
destination returns `INVALID_ARGUMENT`. Concurrent registrations publish one
binding. The named namespace is independent of canonical main/global.

`objc3_executor_get_named(name, &executor)` returns a retained destination,
`NOT_READY` for an unregistered name, or `CLOSED` for a retired name.
`objc3_executor_unregister_named(name)` permanently retires a binding and drops
its registry owner after releasing the registry lock. Repeat retirement is
idempotent. Retained task/client destinations remain valid and finish under
ordinary executor ownership rules. A retired name cannot be rebound: lexical
same-name executor proofs must not silently identify old and new destinations.
Hosts retire names explicitly. Names and retirement tombstones have process
lifetime, avoiding automatic shutdown races with executing callbacks.

`objc3_executor_identity` and `objc3_executor_is_current` expose the logical
executor identity and current execution affinity for retained destinations.
They are independent of physical worker identity. Same executor affinity does
not prove same actor instance isolation. C and C++ fixtures author owned-name,
case-sensitive identity, idempotence/conflict, registration-race, retained-client
retirement, and reentrant registry access during last-worker shutdown witnesses.
No execution or installed export qualification is claimed yet.

## Allocation-independent async service failures

Native C frame service failures use
`objc3_async_error_create_service(status, &error)` and
`objc3_async_error_get_service(error, &status)`. Each bounded nonzero service
failure has an immutable process-owned error in static placement storage.
Materializing or retaining it allocates nothing and cannot saturate a reference
count, including while reporting `OUT_OF_MEMORY`. Ordinary owned-error
retain/release usage remains valid. `OK`, `CANCELLED`, and unknown statuses are
rejected, preserving genuine cancellation as its distinct completion kind.

C++ task results and awaits recognize this transport and throw
`service_failure(status)` before considering native exception or generic task
error payloads. Native empty-child/invalid-await failures can therefore retain
their original status through independent C frames and C++ consumers. Authored
witnesses cover all service statuses, independent shared error owners, ordinary
error mismatch, and C-frame publication observed by both C++ result access and
suspended awaiters. Validation is deferred.

C++ promise publication uses the same transport when result copying or native
exception-error allocation fails. Missing foreign exception carriers and failed
owned-error retention also remain explicit service failures. Only an actual
cancellation representation produces a cancelled completion. An authored
throwing-copy witness checks that `OUT_OF_MEMORY` survives result publication
and native frame cleanup rather than becoming cancellation.

## Cancellation-shielded scope cleanup joins

`objc3_task_group_await_cleanup(group, parent, &frame_status)` supplies the
mandatory quiescence wait needed after a structured scope has requested child
cancellation. It requires a closed group tree and rejects parent membership
cycles and overlapping suspensions. Callers first close/cancel their scope and
handle errors from those operations. Registration failure preserves the status
slot. The C++ owner-aware request `co_await group.cancel_and_join()` performs
those operations and registers the cleanup join through its active promise.

Only this wait is shielded. The owner's monotonic cancellation bit remains set,
and the next ordinary checkpoint still observes cancellation. Cleanup success
always queues a continuation after nested task completion, foreign completion
callbacks, and native frame cleanup. A failure-publication error is reported only
after actual quiescence, even when its completion queue entry was suppressed.

The cleanup waiter retains its parent and group independently of caller handles.
When ready, the group lease transfers through deferred publication into the
selected resume slice. Frame cleanup on actor discard or failed dispatch still
owns that lease. Manual resume/completion requests return `NOT_READY` while
cleanup is pending; forced runtime cancellation waits until quiescence before
reclaiming the parent. Dispatch and all retained-owner release happen outside
task/group/link locks. Empty groups also suspend and queue.

Releasing the transferred scope lease remains active task work after native
frame destruction. This includes group-owned child error and value payload
destructors. Outer joins observe quiescence only after that release finishes;
reentrant cleanup keeps the parent callback identity and cannot wait on itself.
An authored nested-scope witness blocks an inner error destructor after parent
completion and verifies that its outer join still cannot resume.

Authored C17 and C++ witnesses cover cancellation already set on entry, repeated
cancellation, rejected overlapping suspension and premature wake/completion,
abandoned parent/group owners, nested groups, actor resumption and discard,
foreign callback/destructor gates, empty deferred publication, and the C++
cleanup request followed by a normal cancellation checkpoint. These backing
hooks do not claim compiler automatic structured-scope finalization. Validation
remains deferred.

## Explicit structured child entry destinations

`objc3_task_start_child_on` and `objc3_async_frame_start_child_on` start an active
parent's structured child on an explicitly retained executor or actor. They
inherit cancellation ancestry before the child's first queue publication and
preserve the parent's own destination. The inherited-destination child APIs
retain their existing contract. These hooks support awaited entries annotated
with main/global/named affinity without temporarily changing the caller's
continuation destination. Frame ownership transfers only on successful start.

Authored C and shared-frame C++ witnesses cover explicit actor/executor entry,
cross-executor parent resumption, cancellation before child creation, initial
child cancellation observation, invalid-destination ownership preservation, and
exactly-once frame cleanup. Compiler attribute lowering remains its separate
implementation milestone; validation is deferred.

## Explicit Objective-C object actor associations

`objc3_actor_bind_object(object, actor)` pins a live Objective-C object and its
mailbox. Duplicate binding to the same actor is idempotent; a conflicting active
actor is rejected. `objc3_actor_get_for_object` returns a retained actor,
`NOT_READY` for an absent association, or `CLOSED` for a retired association.
There is no pointer-address guess or automatically inferred actor instance.

`objc3_actor_unbind_object` requires the old actor already closed and quiescent.
Its check never pumps or blocks. Successful retirement drops object and actor
owners outside every runtime mutex. The old actor cannot accept further jobs,
so a still-live object may subsequently be bound to another mailbox without
overlapping old isolated segments. Repeated unbind reports `CLOSED`. Hosts pair
every successful binding with explicit unbind; compiler setup/teardown must own
that pairing before automatic actor lifetime can be claimed.

Object pins reside in their own runtime archive member. Pure value task/actor
clients retain their libobjc-free link. The maintained native async ARC/MRC
provider authors real object deallocation, reentrant lookup during deallocation,
pending mailbox cleanup, independent retained lookups, conflicting concurrent
registration, and closed-old-mailbox rebinding witnesses. Its existing
instrumented native-runtime source list includes the new member. Validation
and installed symbol/package qualification remain deferred.

`objc3_actor_get_executor(actor, &executor)` returns the actor's exact immutable
backing destination with an independent retained lease, including after mailbox
closure. Its identity agrees with `objc3_actor_executor_identity`; sharing a
backing executor never proves that distinct actor instances share isolation.
Compiler entry lowering can compare this destination against canonical
main/global/named affinity before entering an explicitly bound actor. Authored
witnesses cover concurrent queries, distinct actors on one destination, and
using the retained executor after all actor/caller executor handles are gone.
