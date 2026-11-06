# Linux feature completion and SDK delivery

This sprint targets the remaining Objective-C 3.0 specification and Linux
x86-64 roadmap contracts. Implementation precedes consolidated native
verification and a coordinated SDK release. Windows and retired compatibility
routes are outside the sprint. Existing quick PR checks stay automatic; extended
compiler campaigns remain selectable and are required for final acceptance.

## Implementation ledger

| Area | Concrete remaining contract | Current state |
| --- | --- | --- |
| Native publication | C/Objective-C C17 `overloadable` declarations, conditional overload identities, canonical attributes and source-free replay | Source authored; native replay and SDK verification pending |
| Native publication | Remaining dependent lookup, declaration/predicate forms and standard-library publication | Open |
| Ownership/System | Indirect writes/aliases, borrowed results, mixed/generic/lazy captures and exceptional cleanup | Open |
| Ownership/System | MRC/foreign ownership effects and complete Strict System diagnostics | Open |
| Concurrency | Remaining child/detached/group spawn and capture forms, abstract group contracts | Open |
| Isolation | Remaining executor/actor crossings, MRC/CF/init-family forms and managed values | Open |
| Isolation | Complete Sendable and Strict Concurrency enforcement | Open |
| Key paths | Dependent/effectful/isolated and ownership-bearing paths | Open |
| Key paths | Nonnull/scalar/aggregate nullable-chain result policy and implementation | Policy and implementation open |
| Language/library | Remaining specified generic, property, reflection/metaprogramming and interoperability contracts | Open |
| Profiles | Core, Strict, Strict Concurrency and Strict System conformance | Open |

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
