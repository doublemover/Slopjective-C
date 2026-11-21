# Owned one-shot task factories

Source 2.154 / System 2.80 extends owned child, detached and task-group factories
to callable objects whose call operator is available only on an rvalue. This
includes move-only objects, inferred group results and explicitly typed native
frame factories. An available lvalue call keeps priority, preserving ordinary
mutable/reusable factory behavior. The rvalue fallback invokes the owned object
once; it does not shorten its storage lifetime.

The existing pinned callable context retains the factory through destruction of
its inner coroutine frame. Cancellation before entry destroys the factory
without invoking it. A throwing factory retains the original exception and
releases its storage. Native frames still undergo descriptor/result admission
before an outer frame is published. Runtime service ABI 4, extension ABI 24 and
frame ABI 1 layouts are unchanged.

Source-free instantiated signatures may name the invocation projection. Only
that exact helper from the checksum-pinned task header is admitted; another
header or application declaration cannot acquire that admission. The group
publication controls exercise move-only rvalue callbacks and children, exact
owning-value destruction, results and cancellation after provider removal.

`task_spawning/rvalue_factories.h` adds executable child, detached, explicit
and inferred group, raw-frame, overload-priority, failure and cancellation
controls to the existing source/PCH fixture. Weak owners verify capture
retirement, including the worker lease that can outlive completion publication.
The larger spawn/capture/value and profile roadmap remains open.
