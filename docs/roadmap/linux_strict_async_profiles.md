# Native async lowering across strict profiles

Concurrency source 2.115 removes the diagnostic-profile restriction from native
Linux x86-64 GNUstep 2.2 frame/task lowering and C group-next plans. The standard
concurrency header exposes its native callable and group convenience APIs in
Strict, Strict Concurrency and Strict System as well as the base source mode.
Target, calling convention, result representation, ownership and body-shape
checks remain in force. The profile continues enforcing nullability, Sendable,
borrowed-lifetime and resource requirements; selecting a strict profile does not
select another async ABI.

Objective-C 3 C++20/23 now defaults to aligned coroutine allocation, so valid SDK
task definitions need no extra allocation flag. Explicit choices survive driver
forwarding and invocation round-trips, and source-interface profiles bind the
effective setting. Regenerate affected C++ interfaces, PCH and packages for this
cutover. Over-aligned native coroutine parameters are checked at runtime.
PCH loading also forwards bodyless alias/ifunc definitions to CodeGen, preserving
logical C group-next entries and alias chains. Provably non-suspending native
awaits remain valid inside `defer`; actual suspension retains its diagnostic.

Compiler witnesses check C frame and C++ task calls, source/PCH transport,
logical/physical result types and group-plan ownership under every strict
profile. Negative controls retain non-Sendable capture, nullable receiver,
resource-plan and variadic-entry diagnostics. Source-free frontend witnesses
publish/replay/relocate native frame bodies and generic task awaits in eighteen
language/ownership/profile combinations, including consumer instantiation.
All 125 new compiler commands, fifty affected native async regressions,
eighteen source-free frontend profiles and fifteen AST infrastructure tests pass.
The option controls cover defaults, both explicit choices and option ordering;
profile identities distinguish effective allocation policies. Three installed
helper import controls also pass.

The maintained `strict_async_profiles` runtime fixture checks lazy frames,
structured children, typed joins, C Optional result iteration, ordinary C++
source async tasks, owned group callbacks, exact defer work, cancellation before
entry and unstarted frame disposal. The feature runner's optional
`--feature strict-async` selection covers 72 language/ownership/optimization/
source-PCH/profile configurations. It is installed with the SDK; ordinary default
feature selections retain their scope. All thirty-six selected instrumented development configurations pass 126
compile/PCH/link/execution commands across every strict profile, language and
ownership mode. The complete optional optimization cross-product remains in
final consolidation.

This span supplies profile admission and preserves existing semantic checks.
Complete Strict Concurrency/Strict System conformance, remaining ownership and
isolation forms, source-free package consolidation and the matched SDK remain
separate acceptance work. Public async/actor feature predicates stay reserved.
Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 are unchanged.
