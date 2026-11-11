# Result-inferred task groups and source-free group helpers

Source 2.150 / System 2.76 makes `objc3::with_task_group(callback)` preserve the
owned task result returned by its callback. The old default selected `void` and
could silently discard a typed result. Deduction now covers typed/void tasks,
function pointers, move-only factories and generic/mutable callbacks. A borrowed
task reference supplies no ownership and cannot infer a group result. A raw
`objc3_async_frame` erases its logical result type and requires explicit `<T>`;
its descriptor still rejects a mismatched result at runtime.

The group algorithm, callable pinning and result aliases now live in the
qualified task header, included by `objc3.concurrency`. Published bodies can
reference the public group operation through its exact task-header contract.
Only pinned result aliases qualify as SDK result projections. Dependency checks
use a closed alias's exact result instead of treating its callback closure as an
escaped public signature; callback captures and invocation remain checked in
the published body. A different header is rejected before source-free admission.
The header hash changes without changing extension ABI 24, service ABI 4 or
frame ABI 1 layouts.

Native consolidation also repaired a CodeGen crash: ordinary local initializer
probing queried an unevaluated template destructor exception specification before
establishing that a checked operation existed. The checked-operation scan now
runs first; genuine checked results retain their Sema-prepared destructor rules.
Twenty-four new source/PCH commands cover ordinary lazy destructor scopes under
C++20/23 and all four profiles. All eight selected checked-result compiler cases
pass, as do the 64 AST controls.

The broader task fixture now verifies original native exception identity rather
than obsolete cancellation conversion. Detached completion can precede frame
retirement: manual-factory tests retain function-pointer producer storage through
bounded retirement observation and still require exact factory/resume/destructor
and pin counts. This removes a race in the test's own borrowed storage lifetime.

Verification passes four C++20/23 ARC/MRC publication profiles with relocation,
source-free replay, typed consumers and mismatched-header rejection; fourteen
adjacent frontend groups; and 700 exact decoder replays across 35 families.
Thirteen native ASan/UBSan configurations perform 64 commands/operations:
four source/PCH spawn configurations, five strict-profile configurations and four
source-free provider/two-consumer configurations. They cover nontrivial result
lifetimes, generic/mutable callbacks, move-only callable ownership, child result
iteration, failure/cancellation and callback cleanup. Source-free consumers keep
typed results and original errors after provider removal.

The required abstract group surface in Part 7 §7.5.3 has concrete create/add,
result-iteration and implicit-join APIs on the delivered C/C++ scope protocols.
Further work concerns remaining spawn/capture and value-contract forms and full
profile acceptance. The [receipt](../support/linux_deduced_task_groups_progress.json)
retains focused evidence and the preceding full compiler checkpoint. Final
matched analysis and one SDK follow the remaining feature implementation.
