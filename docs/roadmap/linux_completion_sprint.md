# Linux feature completion and SDK delivery

The [System 2.50 producer-transfer span](linux_block_producer_transfers.md)
adopts retained/reclaimed Block results into locals, scalar/nested members,
default initializers, assignments and single consumed direct calls. Native
ownership supplies the value without an invented caller copy; produced values
remain guarded across throwing destination evaluation. Block admission cuts over
to v3. All 66 compiler cases, 48 AST controls, six new source-free profiles,
eighteen adjacent groups, twelve instrumented native configurations / 42 commands,
four provider/consumer executions, codec/isolation and 120 decoder replays pass.

The [System 2.49 Block-mutation span](linux_block_mutation.md) separates
RHS evaluation, heap copy, destination-address evaluation and ownership transfer.
A new native unwind guard repairs retained-RHS leakage on an address exception;
failed stores preserve the old slot and borrowed loan. Nullable/null-store
proofs, MRC stable stores and automatic-owner-unwind v2 publication are aligned.
All 66 compiler cases, 45 AST controls, ten new source-free profiles, sixteen
adjacent groups, twelve instrumented native configurations / 42 commands, four
package executions, codec/isolation and eighty production decoder replays pass.

The [System 2.48 computed/nested Block span](linux_computed_block_arrays.md)
extends exact registration across nested rows, sequenced/conditional expressions,
scalar captures, stable native member/index reads, strong local bindings and
consumed arguments. Source-free admission cuts over to subobject v5. All
66 compiler cases, 44 AST controls, twelve instrumented native configurations /
42 commands, ten new source-free profiles, fourteen adjacent groups, codec/
isolation and forty decoder replays pass. Standalone value bodies require Block
expression v2 admission, including C17 without constructors or literal Blocks.

The [System 2.47 Block-array span](linux_block_array_members.md) registers
completed strong member-array elements before later copies, unwinds exact
prefixes, preserves weak/null/filler behavior and emits counted release groups.
Publication cuts over to native-subobject v4. All 66 affected compiler cases,
four new source-free profiles, thirteen adjacent groups, 40 AST controls and
twelve instrumented native configurations / 42 commands pass, together with
codec/isolation controls and forty production decoder replays. Nullable-receiver
proofs include hidden copy failure before constructor-body refinement.

The [frontend 2.120 specialization span](linux_incomplete_specializations.md)
adds opaque class partial/explicit specializations, external/member variable
partials and native standard-library customization declarations. Namespace,
ordinary member and dependent member graphs preserve selection through replay
and later completion. Admission cuts over to incomplete-declaration v2. Four new source-free profiles,
nine adjacent groups, codec/isolation, forty decoder replays and four instrumented
selection/completion witnesses pass.

The [frontend 2.119 declaration span](linux_incomplete_template_declarations.md)
publishes primary forward class templates, external variable templates and nested
opaque heads, preserving defaults, constraints, ABI ownership and later
completion. Production readers, emitters and the fuzz runner share one capability
catalog, repairing C-inline import admission. Two new source-free profiles, eight
adjacent groups, codec/isolation units, forty production decoder replays and two
instrumented package-declaration witnesses pass. Installed package consolidation
follows the remaining implementation sprint.

The [System 2.46 virtual-subobject span](linux_virtual_subobjects.md)
models complete/base-object construction and destruction as separate correlated
paths, including virtual diamonds, delegation, function-try epilogues, abstract
constructor initializer exclusion and anonymous-union active-member cleanup.
Nullable receiver proofs use the same alternatives. ARC Block members separate
implicit copy failure from slot release. Ordinary unnamed records publish stable
owner/path identities. Source-free publication cuts over to subobject v3 and
unnamed-record v2. All 65 affected compiler cases, six new source-free profiles, fourteen adjacent
frontend groups, codec/isolation controls, 39 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass. Two final source/PCH fixture matrices
also pass after adding the Block-array boundary control.

The [System 2.45 delegation/function-try span](linux_delegation_function_try.md)
adds whole-object cleanup after successful delegation, function-try initializer
and destructor epilogue coverage, implicit rethrow/suppression and noexcept
handler boundaries. Constructor source replay preserves try/initializer ordering.
All 62 affected compiler cases, four new source-free profiles, nine adjacent
frontend groups, codec/isolation controls, 33 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.

The [System 2.44 subobject span](linux_native_subobject_cleanup.md)
adds constructor completed-prefix unwind, destructor member/base cleanup on
return/exception paths, primitive ARC subobjects and native noexcept termination
boundaries, with owning-declaration-aware publication. All 61 affected compiler cases, four new source-free profiles, eight adjacent
frontend groups, codec/isolation controls, 29 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.
Wider subobject forms retain separate proof gates.

The [System 2.43 array-group span](linux_array_construction_groups.md)
adds bounded direct throwing array construction with native prefix cleanup and
matched free, automatic throwing array destruction and secondary termination,
and empty-array constructor/destructor bypass. All 60 affected compiler cases, four new source-free profiles, seven adjacent
frontend groups, codec/isolation controls, 26 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.

The [System 2.42 ordinary-owner span](linux_automatic_owner_unwind.md)
activates complete local unwind for every native handler, admits nonthrowing
implicit ARC capture copies, registers heap-copy failure and repairs discovery of
raw inline-storage closure loans. Catch-all and standalone handler printing is
repaired. All 59 affected compiler cases, six new source-free profiles, seven
adjacent frontend groups, codec/isolation controls, 24 AST controls and twelve
instrumented native configurations / 42 commands pass. Broader exceptional
ownership remains in the ledger.

The [System 2.41 allocation/array span](linux_array_and_allocation_cleanup.md)
adds matched scalar deallocation after constructor failure, nontrivial array
unwind/termination, empty-array behavior and bounded repeatable array construction.
Borrowed deletion is diagnosed. Twenty-two AST controls, twenty-four focused
compiler commands pass. All 79 affected compiler cases, eight allocation/disposal
source-free profiles, six adjacent groups, codec/isolation, 22 AST controls and
twelve instrumented native configurations / 42 commands pass.

The [System 2.40 disposal span](linux_native_disposal.md) adds mandatory scalar
deallocation after destructor failure and exact null-operand branching, including
noreturn destructors. Twelve source/PCH commands, all twenty AST controls and
twelve instrumented native configurations pass. All 78 affected compiler cases,
four source-free profiles, five adjacent groups and codec/isolation controls pass.

The [System 2.39 initialized-owner span](linux_initialized_owner_prefixes.md)
tracks completed ARC record/array/capture prefixes, failure unwind and transfer
to complete storage. Owner/unowned snapshots preserve actual evaluation order;
local owning lambdas retain their native capture order and elided temporary
lifetime. All 77 affected compiler cases, six source-free profiles, five adjacent
groups, codec/isolation, 19 AST controls and twelve instrumented native
configurations / 42 commands pass.

The [System 2.38 finalizer span](linux_native_finally.md) models `@finally`
registration around try/catch, cleanup order, native unwind and replacement
return/throw/jump transfers. It also repairs GNUstep C++ rethrow ownership and
adds source-free capability admission. All 76 affected compiler cases, six
source-free profiles, four adjacent frontend groups, codec/isolation and 19 AST
controls pass; both twelve-configuration native scope and frame-exception
matrices pass under ASan/UBSan.

The [System 2.37 managed-scope span](linux_managed_runtime_scopes.md) models
native pool queues, normal-only synchronous pool exits, async drains and exceptional
cleanup, plus exact synchronization snapshots and unlock ordering. Proven ready
awaits remain usable inside monitors; actual suspension is diagnosed, including
ordinary C++ task coroutines. Source-free replay carries exact semantic admission.
The optional native feature includes observed runtime operations and exact destruction.

The [System 2.36 receiver-result span](linux_borrowed_receiver_results.md)
implements native getter pointer/reference loans, self/owner checks, messages,
properties, superclass and stable Class bindings, with source-free identity and
capability admission. Async and isolated crossings reject lifetime erasure
independently of raw-pointer Sendable admission. All forty-two affected compiler
cases, six source-free profiles and twelve instrumented native configurations pass;
five adjacent frontend groups and codec/isolation controls also pass.

The [System 2.35 interior-storage span](linux_borrowed_interior_storage.md) adds
inline ivar/array/record loans and separate C++ reference-address lifetimes,
including copy/capture propagation, builtin address-taking and source-free
capability admission. All thirty-nine borrowed compiler cases, six source-free
profiles, three adjacent frontend groups, codec/isolation units and twelve
instrumented native configurations pass. The wider exceptional cleanup and ownership/profile backlog remains open.


The [System 2.34 handler span](linux_borrowed_catch_bindings.md) implements catch
owner initialization, ARC binding cleanup, rethrow lifetime regions and opaque
borrowed payload obligations. Eighteen compiler commands, sixty-eight affected
compiler cases, nineteen AST controls and twelve instrumented native configurations
pass. The complete shared visitor rebuild passes nineteen AST controls, all six
source-free profiles and three adjacent frontend groups; the full sixty-eight
compiler case consolidation also passes after rebuilding.


The [Concurrency 2.118 span](linux_native_frame_exceptions.md) implements typed
nontrivial/move-only C++ property writes and owned native exception transport,
including exact construction order, nil/partial/cancellation cleanup and bounded
shared record graphs. Source-free replay preserves its SDK/EH contracts under
`objc3.async-entry-abi.v5`. Thirty new compiler commands, ninety-two affected
compiler cases, four new frontend profiles, codec/isolation controls and fourteen
instrumented native configurations pass. Resource/effectful accessors and wider
ownership/profile work remain; matched SDK delivery follows consolidation.

This sprint targets the remaining Objective-C 3.0 specification and Linux
x86-64 roadmap contracts. Implementation precedes consolidated native
verification and a coordinated SDK release. Windows and retired compatibility
routes are outside the sprint. Existing quick PR checks stay automatic; extended
compiler campaigns remain selectable and are required for final acceptance.

## Implementation ledger

| Area               | Concrete remaining contract                                                                                                 | Current state                                                                                |
| ------------------ | --------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------- |
| Native publication | C/Objective-C C17 `overloadable` declarations, conditional overload identities, canonical attributes and source-free replay | Source authored; native frontend replay/identity controls pass; SDK verification pending                                  |
| Native publication | Native allocator ownership, typed tags, expected formats and builtin diagnostic mappings                                    | Source authored; six source modes and focused syntax checks pass; native frontend replay passes; SDK consolidation pending |
| Native publication | Declaration-only template functions/members/static data, semantic attributes, C inline bodies and SDK task promise projection  | Source authored; twelve development frontend groups and focused compiler controls pass; SDK consolidation pending |
| Native publication | ISO C external-inline and inherited/redeclared inline providers with one native definition and source-free consumers       | Source authored; six native link configurations, four native-frame start/await/dispose configurations and focused frontend controls pass; SDK consolidation pending |
| Native publication | Content/configuration-bound native C++ standard headers and consumer declaration reuse | Source authored; six frontend profiles, thirteen AST tests, twelve existing groups and both source-free native modes pass with libstdc++ and libc++; assertion and exception/RTTI configurations pass |
| Native publication | User standard-library class/constexpr-variable specializations, including partial patterns, argument packs and native primary ABI | Source authored; six frontend profiles and two instrumented source-free provider/two-consumer executables pass; wider profiles and SDK consolidation pending |
| Native publication | Inherited formatter state, iterator traits, deduced member identity and unique native virtual slots | Source authored; fifteen AST infrastructure tests, twelve affected frontend groups and all four source-free libstdc++/libc++ native configurations pass; SDK consolidation pending |
| Native publication | Remaining dependent lookup, declaration/predicate forms and wider standard-library configurations | Open |
| Ownership/System   | Failed scalar construction, nontrivial array disposal, zero-element behavior, bounded nonthrowing array construction and borrowed deletion | 79 compiler cases, eight source-free profiles, six adjacent groups, codec/isolation, 22 AST controls and 12 instrumented native configurations / 42 commands pass; SDK pending |
| Ownership/System   | Throwing scalar delete deallocation, null/noreturn branches and exact exceptional cleanup order | 78 compiler cases, four source-free profiles, five adjacent groups, codec/isolation, 20 AST controls and 12 instrumented native configurations / 42 commands pass; SDK pending |
| Ownership/System   | Completed primitive ARC record/array/capture prefixes, owner snapshots, native capture order and exceptional cleanup | 77 compiler cases, six source-free profiles, five adjacent groups, codec/isolation, 19 AST controls and 12 instrumented native configurations / 42 commands pass; SDK pending |
| Ownership/System   | Native finally cleanup, handler ordering, replacement transfers and C++ rethrow payload ownership | 76 compiler cases, six source-free profiles, four adjacent groups, codec/isolation, 19 AST controls and two 12-configuration instrumented native matrices pass; SDK pending |
| Ownership/System   | Native pool release queues, async drains, synchronized receiver snapshots and suspension restrictions | 71 compiler cases, six source-free profiles, five adjacent groups, codec/isolation, 19 AST controls, 12 native scope configurations and four existing async configurations pass; SDK pending |
| Ownership/System   | Complete finite local pointer-target sets through aliases, copies and joins                                                 | Source authored; focused source/PCH diagnostics pass                                         |
| Ownership/System   | Type-carried borrowed callback results/owner indices, deduction, native identity, PCH and published interfaces              | Source authored; focused source/PCH/native identity checks pass                              |
| Ownership/System   | Borrowed results from proven immutable program storage, aliases, subobjects and constexpr calls                             | Source authored; eighteen source/PCH commands and all twenty borrowed regressions pass         |
| Ownership/System   | Aliased memory fills/copies, conditional payloads, returned destinations and partial-write target invalidation              | Source authored; eighteen new source/PCH commands and all twenty-one borrowed regressions pass |
| Ownership/System   | Constant byte-range memory operations on fields/arrays, aliases, returned destinations and callable effects                    | Source authored; eighteen new source/PCH commands, all twenty-two borrowed regressions and frontend controls pass |
| Ownership/System   | Built-in pointer mutations, expression snapshots, bounded array cursor loops and partial direct representation writes | Source authored; eighteen new source/PCH commands and all twenty-two existing borrowed regressions and frontend replay/consumer controls pass; SDK consolidation pending |
| Ownership/System   | Stored byte cursors, exact typed representation lvalues and bounded position sets | Source authored; source/PCH matrices and six native frontend replay/consumer modes pass; SDK consolidation pending |
| Ownership/System   | Immediate C++ reference captures, local reference members and nonlocal reference-output permissions | Source authored; twenty-four new source/PCH commands and affected native frontend controls pass; SDK consolidation pending |
| Ownership/System   | Immediate primitive/default/generic calls and read-only mutable borrowed/owned records; opaque record mutation | Source authored; twelve new source/PCH commands, all thirty-five borrowed compiler regressions, twelve affected frontend groups and four instrumented native configurations pass; SDK consolidation pending |
| Ownership/System   | Scope-bound direct-call reference lambdas, repeated/default/generic/pack/loop invocation and retained mutation hazards | Source authored; twelve new source/PCH commands, all thirty-six borrowed compiler regressions, three frontend groups and four instrumented native configurations pass; SDK consolidation pending |
| Ownership/System   | Closed trivial callable copy/move and reference-alias graphs, extended literal references and explicit generic invocation | Source authored; twelve new commands, all thirty-seven borrowed regressions, three frontend groups and four instrumented native configurations pass |
| Ownership/System   | Remaining indirect writes/aliases, mutable/reference/generic/lazy captures and exceptional cleanup                          | Open                                                                                         |
| Ownership/System   | Explicit MRC/family retained claims and exact known consumption counts                                                      | Source authored; focused source/PCH diagnostics pass                                         |
| Ownership/System   | MRC/foreign ownership effects and complete Strict System diagnostics                                                        | Open                                                                                         |
| Concurrency        | Persistent native await syntax and instantiation through ordinary C++ task templates                                        | Source authored; compiler/source-PCH/AST checks and 944-case storage witness pass; native frontend replay passes; package execution pending |
| Concurrency        | Manual raw-frame factories, ordinary task function pointers and MRC Block environment ownership across spawn/group calls       | Source authored; source/PCH, two focused native configurations and four package-source modes pass |
| Concurrency        | Native frame/task lowering, C group plans and convenience APIs in all strict profiles | Source authored; 125 new compiler commands, fifty affected regressions, eighteen source-free frontend profiles and thirty-six instrumented native configurations pass; optional full optimization matrix and SDK consolidation pending |
| Concurrency        | Remaining child/detached/group spawn and capture forms, abstract group contracts                                            | Open                                                                                         |
| Isolation          | MRC consumed object/Block parameters, consumed native-frame receivers and synchronous init-family crossings                 | Source authored; focused native source/PCH/runtime execution passes                          |
| Isolation          | MRC consumed C++ task parameters/receivers and ARC mutable consuming task receivers                                         | Source authored; focused native execution passes                                             |
| Isolation          | Plain opaque CF consumed arguments across synchronous, native-frame and C++ task entries                                    | Source authored; focused native execution passes                                             |
| Isolation          | Ordinary C++ coroutine function, Objective-C++ method and Block consumed ownership                                          | Source authored; focused source/PCH and native execution pass                                |
| Isolation          | NSObject-backed CF/NS storage, strong/weak/unsafe captures and result ownership                                                | Source authored; three focused native configurations pass                                     |
| Isolation          | Explicit C++ aggregate/union lifetimes, static member entries and dependent native-frame awaits                               | Source authored; focused source/PCH/native execution passes                                   |
| Isolation          | C++ instance executor entries, virtual contract inheritance, receiver categories and dependent native-frame member awaits    | Source authored; source/PCH and focused native execution pass; package execution pending |
| Isolation          | Constructed/reference/resource nil results across native frame/task and implicit actor/executor entries, private descriptors and task-only PCH recovery | Source authored; twelve instrumented native configurations / 43 commands, eighteen compiler commands, sixteen AST tests, six frontend profiles and codec/isolation units pass; wider consolidation pending |
| Isolation          | Synchronous property get/set/update, custom/class/super accessors, actor backing affinity, same-actor calls, independent record snapshots, nil results and cancellation | Source authored; thirty compiler commands, seventeen AST tests, six frontend profiles and twelve instrumented native configurations pass; wider ownership/effect/profile contracts and SDK pending |
| Isolation          | Remaining crossings and managed values                                                                                      | Open                                                                                         |
| Isolation          | Native-frame initializers and retained object/opaque CF result ownership                                                    | Source authored; focused native source/PCH/runtime execution passes                          |
| Isolation          | Task-ABI initializer/object/CF results and typed nil entries                                                                | Source authored; focused native execution passes                                             |
| Isolation          | Concurrent C++ callable capture policy and immutable local Block alias proofs                                               | Source authored; thirty source/PCH commands pass; native frontend replay passes; package execution pending            |
| Isolation          | Conditional/nested concurrent Block proofs, bounded alias graphs and explicit owned snapshots                                | Source authored; fifty-four focused compiler commands and two spawn runtime configurations pass |
| Isolation          | Complete Sendable and Strict Concurrency enforcement                                                                        | Open                                                                                         |
| Isolation          | Typed nil native-frame messages and consumed argument cleanup                                                               | Source authored; focused source/PCH/runtime execution passes                                 |
| Key paths          | Dependent applications and packs in generic functions, including source-free templates                                      | Source authored; native source/PCH and focused runtime pass; package verification pending                              |
| Key paths          | Dependent-root literals, qualified roots and packs, including source-free templates                                         | Source authored; native source/PCH and focused runtime pass; package verification pending                              |
| Key paths          | Dependent type constructors, deduction, partial ordering and typed generic package APIs                                     | Source authored; native compiler/PCH and focused runtime pass; package verification pending                            |
| Key paths          | Effectful/isolated and ownership-bearing paths                                                                              | Open                                                                                         |
| Key paths          | Nonnull/scalar/aggregate nullable-chain result policy and implementation                                                    | Source authored; compiler/PCH and focused runtime pass; package verification pending                           |
| Language/library   | Remaining specified generic, property, reflection/metaprogramming and interoperability contracts                            | Open                                                                                         |
| Profiles           | Core, Strict, Strict Concurrency and Strict System conformance                                                              | Open                                                                                         |

The normative Part 8 resource-union capability boundary remains explicit. A
feature without defined semantics cannot be marked complete by accepting an
arbitrary behavior. Linux AArch64 and macOS arm64 need their own native host work
and are separate from the current x86-64 SDK delivery.

## Authored publication change

C overloadable functions now enter the existing native-declaration-attribute
contract. Canonical printing retains both explicit and inherited `overloadable`
attributes. Native USRs and structural equivalence preserve ordered `enable_if`
predicates in C/C++ as well as Objective-C 3 mode, preventing same-signature C
conditional overloads from collapsing during interface publication/import.

The frontend regression covers C17 and Objective-C C17, distinct argument types,
same-signature conditional overloads, redeclarations, stable source replay,
relocation and parameter renaming, changed predicates, and unique declaration and
module-owned symbol identities. It is authored but has not run against the
corrected compiler. This extends a declaration contract; it does not complete
the broader native-publication or language/profile rows above.

## Authored dependent key-path applications

Keypath source 2.7 retains dependent get/set/metadata/readonly/KVC calls as public
syntax and written operands. Template instantiation expands argument packs,
checks the actual operand count and rebuilds the existing typed operation,
including root/value admission, writability and precise native lifetime lowering.
C++ parsing also accepts ordinary braced argument initialization. Stable profiles
and interface visitors bind the public operation and arguments, without exporting
the private syntax-marker declaration.

Compiler cases cover C++20/23, ARC/MRC, PCH replay, ordinary and all-operand packs,
invalid path types, readonly writes and zero/excess expanded operands. Runtime
witnesses check scalar/record reads and writes, metadata, exact accessor work and
object destruction. Source-free package fixtures publish and instantiate eight
generic helper forms after provider-source removal; qualification now requires
those template declarations in C++ metadata. These cases are authored and remain
pending the rebuilt compiler. Later spans below cover dependent literals and
type constructors; the effect/ownership rows remain open.

## Authored dependent key-path literals

Keypath source 2.9 retains a dependent literal's written root type and ordered
member names until substitution. Instantiation rebuilds through the ordinary
literal entry, preserving complete-class, property/field, writability, ownership
and nullable-route admission. Qualified roots and pack expansion use native
Clang type substitution. Printing and profiles retain public `@keypath` syntax;
interface dependencies and identities exclude the private syntax marker.
Published dependent bodies require `objc3.keypath-dependent-literals.v1`.

Compiler/PCH cases cover construction, nested applications, qualified lookup,
packs, and invalid roots/members. Runtime and source-free package fixtures check
concrete access, descriptor equality and checked nullable paths, with eleven
published generic helper forms. Frontend cases cover relocation/replay, changed
members, consumer instantiation and capability removal. These witnesses are
source authored and await native execution. Dependent type constructors are implemented
in the following span, which supplies their separate deduction/type contract.

## Authored dependent key-path types

Keypath source 2.10 introduces a real, always-dependent Clang type node for
`KeyPath<Root, Value>` and `WritableKeyPath<Root, Value>` patterns. It retains
root/value spelling and writability in canonical profiles, source locations,
AST import, PCH, structural equivalence, native identities and template deduction.
Substitution re-enters ordinary type admission and produces the existing native
record layout. Known invalid operands diagnose at definition; dependent invalid
operands diagnose after substitution. No proxy expression or synthetic class
layout stands in for the type constructor.

Compiler cases cover alias/class templates, readonly narrowing, root/value
inference, partial ordering/specializations and packs, plus invalid root/value and
writability/argument conflicts. Runtime fixtures use typed generic get/set APIs.
Source-free package fixtures now publish fifteen generic helpers and a typed
holder class. Frontend controls require the dependent-type capability for both
published signatures and bodies and reject capability removal. These witnesses
await native execution; affected compiler/frontend units pass syntax checks.

Extension ABI 22 is a hard compiler/PCH/interface cutover. The physical key-path
layout stays at 2. The task bridge and generated capability authority require the
matching compiler. SDK 2.62 is historical accepted delivery and is not relabeled.

The rebuilt development compiler now passes the four positive/negative key-path
syntax cases. Integration repairs cover non-pointer nullability classification,
unnamed/local linkage traversal, nested class partial-specialization parsing and
writable-to-readonly deduction. Reference/pointer controls prevent widening
ordinary C++ deduction or binding mutable references to readonly conversions.

## Authored complete finite pointer-target proofs

System source 2.17 distinguishes complete local target sets from partial may
aliases. Completeness intersects at CFG joins and survives scalar aliases,
structural aggregate copies, expression snapshots and nested capture seeding.
A singleton partial may set cannot become a definite referent. Writes through
complete local alternatives apply each destination in a separate state and join
the resulting loans, effects and owner lifetimes. Record fields and tracked
constant array offsets use the same path model.

Unknown conditional/join alternatives, global targets, dynamic indices, union
representation changes and mutable foreign calls cannot establish a local-write
proof. A noescape parameter alone does not prove that its pointee is immutable.
Tests distinguish weak multi-target clears from definite clears, borrowed escape
and post-consumption use. All eighteen C17/C++20/C++23 ARC/MRC source/PCH commands
pass, and all sixteen existing/new borrowed lit cases pass. Source-free imported record/getter/callback controls are authored and pass
compile checks; native frontend execution remains pending. General unknown
aliases, active union semantics and interprocedural mutation summaries remain open.

## Authored MRC isolation and initializer ownership

The [MRC isolation span](linux_isolation_mrc_consumption.md) admits synchronous
consumed object/Block arguments and consuming receivers under manual reference
counting. It also admits consumed native-frame parameters and receivers, with
separate ramp adoption, frame lifetime pins and body handoff after cancellation
and executor-entry checks. ARC consuming native-frame receivers use a mutable
strong slot whose cleanup follows self reassignment. Synchronous init-family
crossings preserve the source send's concrete `instancetype` result.

Compiler source/PCH controls cover C17/C++23 under ARC/MRC; runtime fixtures add
successful entry, pre-entry and inter-frame cancellation, first/second frame
allocation failure, consumed blocks and combined receiver/argument ownership.
Initializer witnesses distinguish self, nil and replacement results. All twelve new C17/C++23 ARC/MRC source/PCH CodeGen commands pass. One
C17/MRC/O0 source runtime configuration passes all synchronous/native-frame
consumption scenarios and the ARC self-reassignment controls; consolidated
matrices remain pending. The following CF span covers consumed opaque pointer
arguments; CF result/NSObject-backed storage and async initializers remain open.

Source 2.96 extends adoption, frame pins and post-resume manual handoff to C++
task parameters and consuming receivers. ARC consuming task self has its own
mutable strong slot, independent of the adopting ramp. Native witnesses now
compile the same task provider under ARC and MRC and defend cancellation both
before entry and during a yielding body, unused task destruction, and failure
of a coroutine parameter move before promise construction. All twelve added
C++20/C++23 ARC/MRC source/PCH CodeGen commands pass. Three focused runtime
configurations pass 37 commands: C17/MRC/O0/source, C++20/MRC/O2/source and
C++23/ARC/O2/PCH, each compiling independent ARC and MRC task providers. Frontend execution and complete matrices remain
pending.

## Authored CF consumed arguments and source-free ownership

The [2.97 CF span](linux_isolation_cf_consumption.md) supplies independent standard
CF snapshots, ramp adoption, frame pins and post-entry manual handoff under both
ARC and MRC. Function typedefs retain their consumption facts through admission
and generated signatures. Combined consuming Objective-C receivers and CF
arguments retain distinct owners. Generated CF operations skip null.

All eighteen CF C17/C++20/C++23 ARC/MRC source/PCH CodeGen commands and six
rejection controls pass. Three focused native configurations pass 46 commands,
including 120 CF scenarios per configuration, alongside the preceding ownership
witnesses. They check cancellation at successive entry/body stages, both native
allocation failures, typedef-based calls, unused tasks and throwing parameter
copies. The CF fixture implements the standard ownership ABI with independent
destruction counters; it does not qualify a full CF framework.

Async consuming signatures and published bodies now require
`objc3.async-consuming-ownership.v1`. Native identities and every reader preserve
that requirement; codec and contract checks reject missing/invented claims.
Signature/body-only, private-body and replay controls are authored. All eight
affected frontend units and expanded test sources pass syntax checks; frontend
execution and complete instrumented/package matrices remain pending.

## Authored ordinary coroutine ownership

Concurrency source 2.98 extends independent entry adoption, lazy-frame pins and
post-resume handoff to ordinary C++20/23 coroutines, including Objective-C++
methods and Blocks. These use their written return types and the standard C++
promise protocol. Consuming method receivers, captured Block objects, opaque CF
arguments and object/Block parameters retain separate lifetimes under ARC/MRC.
An outer synchronous checked-error contract is rejected because its entry error
slot cannot transport errors raised after suspension; native async contracts keep
their existing promise transport.

All twelve source/PCH CodeGen commands and four checked-error controls pass.
Three focused native configurations pass 46 commands, including 142 CF and sixteen
ordinary object/Block scenarios per configuration. They distinguish unused frame
destruction, failure before promise construction, successful suspension, body
cancellation and ARC self replacement. Providers compile separately under ARC
and MRC. Published consuming ordinary coroutine bodies and direct calls require
the existing ownership capability; frontend replay controls are authored and
pass compile checks. Full frontend/package/instrumented consolidation and the
replacement SDK remain pending.

## Authored initializer and result ownership

The [2.99/2.100 result span](linux_isolation_result_ownership.md) adds native-frame
initializers and retained object results under ARC/MRC, with concrete receiver-
dependent logical await results. Physical entry ABI flags describe the frame
carrier, while result storage adopts the source's transferred claim. Internal
capture reads also support unused parameters without source-use assertions.

Plain opaque retained/borrowed CF results use independent copy/move/destruction
operations. Native await temporaries hold CF owners through the full expression;
initialized frame slots preserve ownership across conditional/shared cleanup
paths. Synchronous retained and native-frame +0 thunk results retain separate
staging rules. Generated ownership operations skip null.

Publication requires `objc3.async-consuming-ownership.v2`; v1-only readers fail
admission. Compiler source/PCH controls and independent ARC/MRC native providers
cover self/nil/replacement, borrowed CF owner removal, allocation failures and
successive cancellation stages. All 36 new source/PCH CodeGen commands, six CF admission controls and 44 affected
compiler regressions pass. Three native configurations pass 58 commands, with
232 CF and 118 initializer/result scenarios each. All eight affected frontend
units and expanded test sources compile; frontend execution, complete matrices
and SDK delivery remain pending. The following task-result span supplies the task-ABI counterparts. NSObject-backed CF storage and managed unions remain explicit work.

## Authored nil native-frame messages

The [2.101 nil span](linux_native_nil_frames.md) replaces invalid zero carriers
with typed lazy zero results for eligible native-frame messages. Direct-method
nil prologues use the same owner, and actor nil entries need no mailbox.
Consumed object/Block/CF arguments are disposed without invoking the source
method, including synchronous methods whose facts live on parameter attributes.
Artificial helper debug locations preserve LLVM verification with a same-module
allocator definition.

All eighteen C17/C++20/C++23 ARC/MRC source/PCH/debug CodeGen commands pass.
The native controls now include 68 nil scenarios per configuration. Source-free
entry semantics cut over to `objc3.async-entry-abi.v2`; old-reader rejection,
reader registries and codec fuzz vocabulary are updated. Three focused native
configurations pass 58 commands with 68 nil scenarios each. All 45 affected
compiler regressions, seventeen feature-authority/codec policy controls and
compile checks for eight frontend units and expanded tests pass. Task-ABI entries
and nontrivial/resource zero-result policies remain
separate work; full frontend, instrumented/package and SDK acceptance is pending.

## Authored nullable key-path result policy

Part 3 §3.6.4.1 now defines nullable routing without changing the final `Value`
type. A missing intermediate returns null for an explicitly nullable pointer
result. Other results, including unspecified/nonnull pointers, scalars and native
records, trap before the remaining accessors. No fabricated zero or default
object is produced. Writes still evaluate their operands and skip missing routes.

Keypath source 2.8 emits a checked-read descriptor bit exactly when required.
Runtime metadata validation rejects missing, stray or contradictory flags;
source interfaces require the checked-read capability for published bodies.
Source/PCH runtime witnesses cover present reads, skipped writes, exact accessor
work and three isolated intentional-trap controls. Source-free package fixtures
replay checked scalar/record paths and inline reads. These compiler-dependent
witnesses await the rebuilt toolchain; the small metadata service regression is
independent of that rebuild.

## Analysis findings under repair

The full baseline campaign finished against its frozen original compiler source:
235 variants, 199 passed and 36 failed, including nine finite-timeout failures.
The source-owned failed-attempt summary retains the report hash and every failed
unit; none is counted as current-source acceptance.
The next canonical source batch preserves address-target facts before their CFG
transfer, guarantees address-space stripping initializes its type result, checks
complete await/yield operands and aligned-allocation casts before use, and repairs
native typeid/capture, synthesized-property and class-message recovery. Implicit
object overload resolution initializes absent-base classifications and rejects
unrelated classes; constrained member specialization starts with a real candidate.
Narrow AST/source-location invariants are made explicit at their owners. CodeGen
also preserves complete bit-integer sanitizer descriptor bytes, validates scalar
indices and clears optional native-call outputs before coroutine exit handling.
The next CodeGen findings use an explicit optional pseudo-object result, restore
the enclosing generator context after nested helper creation and enforce emitted
complex-component invariants. Nontrivial copy visitors now borrow their operands
for the prepass and forward ownership only at final dispatch; a move-only visitor
regression distinguishes observation from a destructive prepass. The three
CodeGen units and the expanded AST test pass syntax checks; rebuilt native tests
remain pending.

New address-space qualifier/array and reassigned finite-alias witnesses are
source authored. Native C++ cases also exercise implicit/explicit object bindings,
enum promotion and constrained member specialization; bit-integer cases bind
little- and big-endian descriptor bytes. Affected compiler units pass syntax
checks; these cases still require the rebuilt compiler and full native campaign. Nine
measured ten-minute timeout failures now have exact source-scoped, finite
one-hour allowances; earlier failures remain evidence and no timeout is accepted
as a passing analysis result.

## Authored task result parity

The [2.102 task result span](linux_task_result_ownership.md) adds task-ABI
initializer and retained object results, declaration-specific CF promise
ownership, concrete related-result projection and typed nil task entries.
Native-frame forwarding preserves source full-expression owners until result
storage has acquired its claim. Source-free entry and consuming semantics cut
over to v3. The maintained fixture covers three caller routes and ARC/MRC,
including suspended/completed cancellation and disappearing borrowed owners.
Focused source/PCH and native execution pass; these source changes do not close the
remaining NSObject storage, managed-value, Strict Concurrency or SDK rows.

## Consolidated verification and delivery

1. Re-run the completed full analysis inventory against the rebuilt fork and
   verify every authored finding repair and source-scoped resource allowance. Preserve failed attempts and their diagnostics.
2. Materialize the complete canonical fork patch stack, seal the source and
   incrementally rebuild the matching compiler and frontend once per completed
   implementation batch.
3. Run the complete compiler/native/frontend suites and source/PCH/source-free
   feature matrices, with matching runtime sanitizer and independent controls.
4. Complete full fuzzing, schedule, source-coverage, interoperability and semantic
   workload owners for the final source revision. Timings remain advisory until
   matching observations and reviewed goals support a performance gate.
5. Build one coordinated SDK, qualify its actual archive and installed consumers
   on the supported Linux distributions, then publish the exact acceptance
   inventory and current remaining-work list.

SDK 2.62 remains the accepted archive until that delivery completes. Source
implementation, successful native verification and shipped SDK coverage remain
separate states throughout this ledger.

## Borrowed callable result contracts

The [current callback-result span](linux_borrowed_callable_results.md) preserves
borrowed result obligations and owner indices in native function types. It adds
indirect function/Block/member loans, generic forwarding and automatic deduction,
exact template/ODR/USR/overload identities, AST/PCH persistence and source-free
capability admission. Extension ABI 23 is a hard cutover. Nineteen compiler regressions and three native AST identity tests pass;
frontend/package consolidation and final SDK delivery remain pending.

## Native allocator and diagnostic metadata

The [current native attribute span](linux_native_diagnostic_contracts.md) adds
ownership family operations, argument/type-tag contracts, expected format strings
and ordered builtin diagnostic mappings to canonical publication. Matching types
and expressions undergo dependency checks. Native declaration attribute capability
v2 is a hard cutover. Six source modes and focused syntax checks pass; native
frontend/package execution remains in the consolidation batch.

## NSObject-backed pointer storage

The [2.103 storage span](linux_nsobject_cf_storage.md) supplies independent CF/NS
entry claims and lazy pins for NSObject pointer aliases, strong/weak/unsafe slots
and ordinary/retained/borrowed results in synchronous isolation, native frames and
C++ tasks. Weak pins and MRC body handoffs load the object through its weak
operation, preserving the runtime slot representation. Canonical C++ ownership
qualifiers retain consistent union/optional lifetime facts. Interfaces require
`objc3.nsobject-storage.v1` and reject missing/invented or unsupported claims.

Focused source/PCH and native AST checks pass. The final 1,084-case native configuration
passes at C17/MRC/O0/source; both optimized configurations pass too, completing 3,252 new scenarios and 70
isolation-suite commands. Frontend/package consolidation remains pending. Managed unions, other crossings, full profiles and SDK delivery
remain separate obligations.

## Concurrent callable capture policy

The [2.104 callable span](linux_concurrent_callable_captures.md) closes shared
`this`, explicit unsafe and cleanup-managed capture admission gaps for C++
lambdas. Weak, reference, nested/init, mutable and generic captures retain their
native restrictions. ARC qualifiers and MRC inert unsafe markers both count;
unsafe admissions emit transfer notes. Block aliases follow only immutable local
initializers, and Block C++ `this` checking uses the actual pointee contract.
Thirty new source/PCH commands and the existing transfer-note regression pass.
Frontend replay/consumer controls are authored; consolidation remains pending.

## Explicit C++ values and dependent native awaits

The [2.105 value span](linux_explicit_cxx_storage.md) uses native typed lifetime
hooks for explicit C++ aggregate/union ownership and admits static member
crossings. Materialized argument snapshots preserve constructors through PCH.
Recovered or undefined storage hooks diagnose without crashing. Dependent native
awaits rebuild their isolation and task/frame contracts after substitution, with
an explicit source-free capability. Thirty-two new compiler commands and 296
native scenarios pass; the complete focused isolation configuration passes 25
commands. Optimized/instrumented and frontend/package consolidation remain
pending. Implicit C/resource union cleanup and other crossing forms stay open.

## Immutable program storage and borrowed returns

The [System 2.20 span](linux_static_borrowed_results.md) proves readonly program
storage using native constant evaluation and bounded pointer/conditional
composition. Automatic/TLS/mutable/union/destructible or unknown storage stays
unproven; existing tracked loan expiry and public nonescaping obligations remain.
Eighteen new source/PCH commands and all twenty borrowed regressions pass.
Frontend replay/consumer controls are authored and compile. Native dependent
awaits also retain a distinct opaque marker so ordinary C++ dependent `co_await`
keeps its spelling and publication contract through source/PCH.

## Aliased memory operations and incomplete target sets

The [System 2.21 span](linux_borrowed_memory_aliases.md) preserves loans, callable
effects and pointer/owner identities through typed aliases, nested and conditional
representation copies and returned destination pointers. Partial/open writes
invalidate target proofs, incomplete target sets retain their unknown alternatives,
and element addresses include reachable array siblings. Eighteen new source/PCH
commands and all twenty-one borrowed native regressions pass. Frontend replay and
consumer controls are authored and syntax-checked; execution and SDK consolidation
remain pending. General byte offsets, union active members and interprocedural
mutation remain open.

## Concurrent Block values and explicit owned snapshots

The [Concurrency 2.106 span](linux_concurrent_block_values.md) proves conditional,
nested and composed immutable Block environments, with shared graph proofs, cycle
and budget controls. Explicit strong snapshots preserve their actual owned storage
even when the source binding is unsafe. A native lifetime-walker repair avoids
repeated identical conditional origins before capture checking. Fifty-four
focused compiler commands and two complete spawn runtime configurations pass,
including sixty-four new exact-lifetime cases. Frontend controls are authored and
syntax-checked; their execution and final SDK consolidation remain pending.

## Dependent and checked key-path execution checkpoint

All twenty-seven native key-path compiler regressions now pass against the
current development compiler, including dependent types, applications/packs,
root literals and source/PCH replay. The checked-traversal IR oracle explicitly
covers both GNUstep dispatch forms, C/C++ symbol spelling and the trap intrinsic's
noreturn contract; LLVM 23 need not place an unreachable directly after the call.

The current runtime services were rebuilt with this compiler before execution:
the earlier service archive did not admit the new checked-read descriptor flag.
The native descriptor unit test and complete C17 MRC O0 source/C++23 ARC O2 PCH
key-path runtime configurations pass (twelve commands). These execute dependent
application/type/literal helpers, present/missing writes, exact accessor counts,
receiver cleanup and isolated child-process traps for missing scalar, record and
nonnull object reads. Frontend/source-free package, other runtime/sanitizer
configurations and coordinated SDK delivery remain pending. This checkpoint does
not implement effectful/isolated or ownership-bearing key paths.

## C++ instance executor entries

The [Concurrency 2.107 span](linux_cxx_instance_entries.md) preserves exact source
receivers, cv/ref/temporary lifetime, private access and virtual/qualified dispatch
through isolated entry. Static-object calls retain their caller-side base work.
Virtual metadata inherits or rejects across every base, and native-frame member
templates defer dependent storage checks even with a known scalar result.
Twenty-four new source/PCH commands, existing dependent-await controls, four
package-source modes and a complete twenty-five-command isolation suite pass.
Its explicit storage matrix now covers 944 cases, including 648 added member-entry
cases. Frontend/package execution, optimized/instrumented matrices and coordinated
SDK delivery remain pending. Ordinary C++ task-template dependent awaits are
implemented in the following span.

## Ordinary task-template native awaits

Concurrency source 2.108 gives dependent C++ task awaits a persistent native
syntax bit, preserved through AST import, PCH, printing and native/ODR expression
identity. Substitution rebuilds isolation, frame/task adapters and optional checked
error bindings while retaining ordinary operator lookup and ADL. Known scalar
results with instantiation-dependent operands defer as native awaits; ordinary
C++ co_await keeps its own rules. Native-frame templates retain their separate
opaque continuation protocol.

This statement-record cutover advances the compiler extension ABI and task bridge
to 24. SDK 2.62 remains the accepted historical archive. Implementation and
source/PCH/AST identity-import, callback/awaiter/ADL, package and direct task-template
ownership witnesses are implemented. The ABI 24 compiler rebuild completes; six
compiler regressions pass sixty-six commands, five native AST controls and four
package-source modes pass. The focused ARC/MRC native storage witness passes five
compile/link/execution commands and all 944 cases. Production-task execution
covers indirect plain/frame/task callbacks and value-dependent scalar awaits;
custom awaiter and ADL controls use the compiler fixture's custom promise. Three
frontend units pass syntax checks. Frontend/source-free package execution, wider
runtime/sanitizer consolidation and SDK delivery remain pending. See the
[2.108 contract](linux_cxx_dependent_task_awaits.md).

## Manual frame factories and owned callable environments

The [Concurrency 2.109 span](linux_manual_frame_factories.md) admits validated
physical frame producers and ordinary C++ task function pointers as asynchronous
work. Function designators retain their ordinary pointer decay; capture admission
continues through literal and immutable Block proofs. Namespace lookalikes and
scalar-returning producers fail.

Spawn adapters own a stable callable before invoking it. ARC and MRC Block values
receive an explicit heap copy whose release follows inner-frame destruction. Lazy
group callbacks acquire the same ownership during the ordinary creation call,
before their caller can return. A private coroutine retains the callback through
its scope's final join. Thirty focused compiler commands, including eighteen new
source/PCH commands, pass. Two native configurations pass thirty focused ownership
and failure scenarios each, and four package-source modes pass. Two complete C++20/MRC/O0/source and C++23/ARC/O2/PCH spawn configurations
also pass eleven commands. Matching frontend execution is in progress; source-free
packages, wider instrumentation and SDK consolidation remain pending.

## Publication completion and exact byte ranges

The [Concurrency 2.110 / System 2.22 span](linux_publication_and_byte_ranges.md)
closes the failures exposed by the postponed native frontend controls. Namespace
and member function-template signatures, ordinary/template static-data members
and existing lifetime/isolation attributes retain their publication contracts
without requiring unrelated definitions. Canonical task-header promises stay SDK
implementation dependencies; custom promises retain ordinary public checks.
Native printers retain type-tag identifier flags, one-based builtin mappings and
explicit empty parameter lists before Block effects. Zero builtin indices
diagnose before indexing parameter storage.

ISO C inline bodies now publish with `objc3.c-inline-definitions.v1`, distinct
from the C++ definition contract. Source/identity, capability and owner checks
reject missing/invented claims. The following 2.111 span completes external-inline provider projection.

Exact memory byte ranges recover tracked fields and array elements from native
layout, complete local bases and constant offsets. Full copies preserve loans,
callable effects and returned destination identities; partial, padding, dynamic,
incomplete, dependent and unsupported union/base paths cannot establish a proof.
Twenty-five native compiler regressions pass, including 105 new source/PCH/print
and reparse commands. All twelve selected frontend control groups pass against
the current ABI 24 development headers/DSO and current runtime header identities.
This is development execution, not a resealed SDK qualification. Source-free
package execution, wider native/instrumented matrices and final delivery remain
pending.

## Single-provider C inline definitions

The [Concurrency 2.111 span](linux_c_inline_providers.md) gives public ISO C inline
functions one module-owned provider definition. Canonical source normalizes all
consumer redeclarations to ISO inline, including external definitions, later
external declarations and inherited inline specifiers. Storage changes are scoped
to backend observation; the source AST and publication identity retain their
original declarations. Mutable local static/thread storage and GNU inline modes
cannot acquire this ISO contract.

The maintained native control passes six C/Objective-C ARC/MRC O0/O2 configurations.
Each verifies four strong provider symbols, removes provider source, compiles two
independent consumers, checks that they emit no duplicate external definitions,
links/runs their calls and verifies shared function addresses. GNU rejection
preserves output rollback, and ordinary non-module inline behavior is unchanged.
Four affected frontend replay groups pass. Source-free package integration beyond
these native controls, wider matrices and coordinated SDK delivery remain pending.

## Pointer mutations and bounded cursor loops

The [System 2.23 implementation](linux_borrowed_pointer_mutations.md) preserves
pre/post results and actual cursor targets, invalidates stale proofs after partial
writes, and bounds same-array walks using separate nonnull facts. Unknown strides,
nullable endpoints and overlapping union storage retain conservative behavior.
Stored byte cursors and broader alias/capture/exceptional contracts remain open.

## Stored byte cursors and typed representation access

The [System 2.24 implementation](linux_borrowed_byte_cursors.md) retains independent
byte offsets through bindings, copies, joins and snapshots. Exact native range
checks support typed loads/stores, subscripts and members while unknown/partial
writes preserve obligations. Bounded proofs reject loss of alternatives at 65
positions; current storage exposure avoids stale payload snapshots. General
union/interprocedural, callable and exceptional ownership contracts remain open.

## Immediate captures and reference output storage

The [System 2.25 implementation](linux_borrowed_reference_storage.md) admits
bounded immediate synchronous reference readers with local algorithms/scalar
updates, rejects loan exports through caller references, and resolves known local
reference fields/copies to actual storage. Binding identity, local ownership and
pointer-value facts stay distinct. General joins/output summaries and broader
callable/exceptional ownership remain open.

## Reference bindings and current owner state

The [System 2.26 implementation](linux_borrowed_reference_bindings.md) gives
reference bindings an independent domain. Conditional lvalues, current stored
pointer/byte values, copied reference records, exact representation rebinding,
per-target pointer mutations and reference capture modes no longer conflate
binding identities with pointer values. Known reference/field/integer aliases,
mutable outputs and derived loans retain owner dependencies. Complete local
writes are bounded at 64 alternatives while all possible escape targets remain.
Eighteen new C++20/C++23 ARC/MRC source/PCH commands, all twenty-nine borrowed
compiler regressions and three affected source-free frontend groups pass. ABI 24
and SDK 2.62 are unchanged. General ownership and SDK consolidation remain open.

## Stored owners and field-sensitive closure requirements

The [System 2.27 implementation](linux_borrowed_stored_owners.md) admits stable
tracked field/element owners, independent record-input identities and precise
binding invalidation. Cached borrowed/consumed capture paths survive local
aliases, overwrites, record copies, nested closures and aggregate/conditional
initializers. Field operand snapshots preserve later argument expiry. Eighteen new
C17/C++20/C++23 ARC/MRC source/PCH commands, all thirty borrowed compiler
regressions and three affected source-free frontend groups pass. Unknown positions, indirect/global mutation, managed-field ownership,
lazy/exceptional analysis and SDK consolidation remain open.

## Independent ownership in copied record captures

The [System 2.28 implementation](linux_borrowed_owned_records.md) follows copied
ARC strong fields through record/base/array paths in Blocks and C++ lambdas.
Future borrowing and existing loans with a proven matching identity retain
independent field ownership, while weak/raw/reference/MRC boundaries stay
explicit. Reference-field symbolic inputs preserve their future owner effects.
Eighteen new source/PCH commands, all thirty-one borrowed compiler regressions
and three affected frontend groups pass.
Four ARC C17 O0/C++23 O2 native source/PCH configurations pass fourteen commands,
including ASan/UBSan execution and exact object destruction counts. SDK delivery
and wider manual/mutable/lazy/exceptional ownership remain open.

## Contracted owners through tracked aliases

The [System 2.29 implementation](linux_borrowed_owner_aliases.md) acquires owners
through closed record/pointer/scalar/reference and exact byte selectors. Selected
storage bindings stay frozen across alias retargeting; native operand snapshots
retain later argument expiry. Local aliases in captured records preserve cached
owner paths. Unknown/open selectors and exceeded candidate budgets diagnose.
Twenty-four new source/PCH commands, nine focused compiler regressions and three
affected source-free frontend groups pass. Wider lifetime/global/indirect
contracts and SDK consolidation remain open.

## Expression pins and primitive ARC cleanup

The [System 2.30 implementation](linux_borrowed_field_pins.md) proves field/
element/alias/NSObject pins, unchanged-source restoration, equivalent nested
snapshots and inherited closure pin state. Released pin inputs diagnose without
requiring another borrowed operation. The cleanup CFG follows native primitive
ARC record/array release ordering, nonthrowing conditional initialization and
trivial MRC stack-Block assignments. Throwing partial storage and broader
ownership actions remain guarded. Eighteen new source/PCH commands, all thirty-eight
selected borrowed/lifetime/cleanup regressions and three source-free frontend
groups pass. Four instrumented ARC C17 O0/C++23 O2 source/PCH configurations pass
fourteen commands, including exact destruction checks for field and nested pins.
SDK delivery remains in consolidation.
