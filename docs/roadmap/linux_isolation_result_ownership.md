# Native initializer and foreign result ownership

Concurrency source 2.99 admits native-frame init-family isolation crossings and
retained native-frame object results. Source 2.100 adds plain opaque CF results
from synchronous isolated calls and native frames. This development batch passes 36 new source/PCH CodeGen commands, six CF
admission controls and all 44 affected compiler regressions. Three focused native
configurations pass 58 commands: C17/MRC/O0/source, C++20/MRC/O2/source and
C++23/ARC/O2/PCH. Each includes 232 CF and 118 initializer/result scenarios,
alongside the preceding ownership witnesses. Full frontend/package, instrumented
and SDK acceptance follow the remaining implementation sprint.

## Initializers and object results

Native-frame messages preserve the ordinary receiver-dependent `instancetype`
and nullability rules in their logical await results. This applies to direct
same-executor calls and generated isolation entries, including inherited methods.
Native copy-out checks the exact producer descriptor. C++ task callers use a
typed forwarding bridge to publish the refined result while retaining their
source through cancellation and unused destruction. The physical entry still
returns the common frame carrier. Logical retained
result attributes never become retained-object ABI flags on that carrier.

Initializer receivers consume the incoming claim under ARC and MRC. Ramps,
frame pins, mutable ARC self storage and post-entry manual handoff retain their
separate ownership. Retained native-frame results adopt the source's transferred
+1 before making the frame's independent result copy. Self, nil and replacement
results balance independently of the original receiver pin. Manual initializer
bodies remain responsible for their body claim, including cleanup across
cancellation; the MRC witnesses use ordinary native cleanup attributes.

The batch also repairs compiler-generated reads of unused frame parameters.
These internal reads use synthetic locations rather than pretending that the
source referenced an unused declaration.

## CF results

`cf_returns_retained` and `cf_returns_not_retained` identify plain opaque CF
result pointers. They do not turn every pointer of that type into owned storage.
Generated result tables have distinct CF copy, move and destroy operations;
they are never reused for an unowned pointer with the same representation.
Copy retains, move transfers and clears the source, and destruction releases.
Every generated CF ownership call skips null.

A retained producer transfers its manual +1 into result staging. A borrowed
producer supplies +0; the frame takes an independent CF claim before producer
cleanup or suspension can remove its original owner. Isolation thunks preserve
the distinction between synchronous retained results and +0 awaited values.

Native await copy-out holds an independent CF owner through the complete
expression, after child disposal. A caller that stores the raw pointer beyond
that expression must acquire its own CF claim. The witnesses do so before the
expression ends. Initialized frame slots carry temporary owners across shared
cleanup continuations, conditional paths and cancellation. A branch-local LLVM
value is never used as the owner for a shared cleanup block.

## Publication and maintained witnesses

`objc3.async-consuming-ownership.v2` is the hard source-free semantic cutover for
consuming arguments/receivers and these native-frame result contracts. Native
identities, published-body visitors and every interface reader require it.
A reader that advertises only v1 cannot consume the new interfaces. Signature,
body-only, replay and missing/invented-capability controls include retained
object and CF results.

`native-isolation-initializer.m` and `native-cf-results.m` cover C17/C++20/C++23,
ARC/MRC and source/PCH CodeGen. The maintained native feature runner compiles
independent ARC/MRC initializer and CF-result providers. Their controls cover
both frame-allocation failures, pre-entry and suspended-body cancellation,
completed-result cancellation, nil, receiver replacement and precise object/CF
destruction. CF controls also drop the borrowed producer's original owner
before consumer resumption. Frontend controls check concrete logical result
types and source-free ownership requirements.

[Source 2.102](linux_task_result_ownership.md) implements the task-ABI initializer,
object/CF result and nil-entry counterparts. Focused native execution passes. NSObject-backed CF storage and general foreign ownership families
retain separate implementation work. Managed union
frames still require an active-arm ownership contract.

All eight affected frontend units and the expanded emitter/isolation regression
sources pass compile checks. Their execution remains part of consolidation.

[Source 2.101](linux_native_nil_frames.md) adds typed nil native-frame messages
and consumed-argument cleanup. Its entry semantic v2 accompanies the separate
consuming-ownership semantic v2.
