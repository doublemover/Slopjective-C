# C++ instance executor entries and virtual contracts

Concurrency source 2.107 admits awaited C++ instance executor entries through
private native-frame thunks. The thunk borrows the exact source object and keeps
its const/reference category, resolved specialization, private/protected access,
and virtual versus explicitly qualified dispatch. It does not copy the receiver.
Temporary receivers retain their ordinary awaiting full-expression lifetime.
The caller keeps borrowed C++ objects alive until frame/task quiescence, as for
existing C++ native-frame receiver borrowing.

Receiver and arguments evaluate once, in source order, before frame creation.
Static calls written through an object evaluate that base on the caller without
transporting or copying it. Shared C++ receiver transfer requires explicit unsafe
Sendable admission and emits the required note. Borrowed leaves cannot be waived.
Argument/result ownership and existing native-frame/C++ task adapters retain
their typed construction, consumption, cancellation and teardown rules.

C++ virtual overrides inherit omitted executor/task metadata. Conflicts and
strengthened unqualified slots are rejected, including multiple-base mismatches
in either traversal order. The existing exact async/error override contract
remains independent. Class-template native-frame awaits defer dependent receiver
and storage checks until substitution, including known scalar results; they
cannot be folded into nonsuspending expressions prematurely.

Two new compiler regressions pass twenty-four C++20/23 ARC/MRC source/PCH
commands, covering direct/const/ref-qualified/temporary receivers, native-frame
and task targets, private entries, virtual/qualified-base dispatch, generic
methods, evaluation expressions and negative ownership/override controls.
Dependent native-frame and ordinary-coawait spelling controls also pass: four
focused tests, forty-two commands in total. Four package-source modes pass.

The shared exact-lifetime isolation witness now runs twenty-six storage/entry
combinations across ARC/MRC providers, three caller routes and six completion/
cancellation phases, plus eight unused frames: 944 cases per configuration,
648 added in this span. Deleted receiver copies/moves, live-cookie checks,
receiver/argument sequencing, exact entry/completion and destruction counts cover
const/native-frame/task-rvalue/private/temporary/virtual/qualified/static-object
forms. The complete C17 MRC O0 source suite passes all twenty-five commands with
the rebuilt compiler and current runtime services.

Source-free package fixtures publish member templates, deleted copy/move storage,
virtual override inheritance and dependent member awaits. Frontend replay and
consumer controls are authored and syntax-checked; their execution, optimized/
instrumented matrices and final SDK consolidation remain pending. SDK 2.62 and
extension ABI 23 are unchanged. Dependent awaits in ordinary C++ task templates
need a separate persistent syntax/instantiation contract and remain active work.
