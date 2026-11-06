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
| Isolation          | Remaining executor/actor crossings, MRC/CF/init-family forms and managed values                                             | Open                                                               |
| Isolation          | Complete Sendable and Strict Concurrency enforcement                                                                        | Open                                                               |
| Key paths          | Dependent applications and packs in generic functions, including source-free templates                                      | Source authored; native source/PCH/package verification pending    |
| Key paths          | Dependent-root literals, qualified roots and packs, including source-free templates                                         | Source authored; native source/PCH/package verification pending    |
| Key paths          | Dependent type constructors; effectful/isolated and ownership-bearing paths                                                 | Open                                                               |
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
pending the rebuilt compiler. Dependent type constructors/literals and the other
key-path rows remain open.

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
source authored and await native execution. Dependent type constructors remain
open; literal support does not supply that separate deduction/type contract.

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

The full campaign remains bound to its original compiler source while it runs.
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
checks; these cases still require the rebuilt compiler and full native campaign. Seven
measured ten-minute timeout failures now have exact source-scoped, finite
one-hour allowances; earlier failures remain evidence and no timeout is accepted
as a passing analysis result.

## Consolidated verification and delivery

1. Finish the existing full analysis inventory and resolve every finding and
   resource-bound failure. Preserve failed attempts and their diagnostics.
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
