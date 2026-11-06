# Linux feature completion and SDK delivery

This sprint targets the remaining Objective-C 3.0 specification and Linux
x86-64 roadmap contracts. Implementation precedes consolidated native
verification and a coordinated SDK release. Windows and retired compatibility
routes are outside the sprint. Existing quick PR checks stay automatic; extended
compiler campaigns remain selectable and are required for final acceptance.

## Implementation ledger

| Area               | Concrete remaining contract                                                                                                 | Current state                                                      |
| ------------------ | --------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| Native publication | C/Objective-C C17 `overloadable` declarations, conditional overload identities, canonical attributes and source-free replay | Source authored; native replay and SDK verification pending        |
| Native publication | Remaining dependent lookup, declaration/predicate forms and standard-library publication                                    | Open                                                               |
| Ownership/System   | Indirect writes/aliases, borrowed results, mixed/generic/lazy captures and exceptional cleanup                              | Open                                                               |
| Ownership/System   | MRC/foreign ownership effects and complete Strict System diagnostics                                                        | Open                                                               |
| Concurrency        | Remaining child/detached/group spawn and capture forms, abstract group contracts                                            | Open                                                               |
| Isolation          | MRC consumed object/Block parameters, consumed native-frame receivers and synchronous init-family crossings                 | Source authored; focused native verification in progress           |
| Isolation          | MRC consumed C++ task parameters/receivers and ARC mutable consuming task receivers                                         | Source authored; focused native execution passes                   |
| Isolation          | Plain opaque CF consumed arguments across synchronous, native-frame and C++ task entries                                    | Source authored; focused native execution passes                   |
| Isolation          | Remaining crossings, CF result/NSObject-backed storage, async init-family forms and managed values                          | Open                                                               |
| Isolation          | Complete Sendable and Strict Concurrency enforcement                                                                        | Open                                                               |
| Key paths          | Dependent applications and packs in generic functions, including source-free templates                                      | Source authored; native source/PCH/package verification pending    |
| Key paths          | Dependent-root literals, qualified roots and packs, including source-free templates                                         | Source authored; native source/PCH/package verification pending    |
| Key paths          | Dependent type constructors, deduction, partial ordering and typed generic package APIs                                     | Source authored; native compiler/PCH/package verification pending  |
| Key paths          | Effectful/isolated and ownership-bearing paths                                                                              | Open                                                               |
| Key paths          | Nonnull/scalar/aggregate nullable-chain result policy and implementation                                                    | Source authored; compiler/runtime/PCH/package verification pending |
| Language/library   | Remaining specified generic, property, reflection/metaprogramming and interoperability contracts                            | Open                                                               |
| Profiles           | Core, Strict, Strict Concurrency and Strict System conformance                                                              | Open                                                               |

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
