# Native work-item factory capture recipes

Source 2.205 / System 2.131 extends capture proofs from forwarded Block values
to factories that construct a new Block from scalar or Objective-C parameters.
A bounded recipe retains every possible returned environment through pure
branches, switches, fallthrough/break and conditional expressions. Static C++
methods are admitted alongside inline/internal free functions. Selected C17
generic/choose expressions and null environments retain their ordinary rules.

Each captured formal is mapped to its actual caller argument. The boundary
checks the caller's value contract and mutable cleanup provenance instead of
substituting a plain formal type. Nested produced Block values retain these
checks when captured by another work item. Block-valued parameters recursively
validate every actual environment under a shared depth/work/cycle budget;
plain formal Block types never grant unknown captures a Sendable promise. Arguments used only for control or
not captured at all do not become transported environments. Source diagnostics
point to the actual value supplied to the factory.

ARC uses the native returned-Block ownership path. Capturing MRC results require
an explicit copy through the reserved `_Block_copy` runtime ABI: global C
linkage, exact pointer signature, ordinary calling convention and no replacement
or effectful entry policy. The compiler already relies on this runtime contract
for generated Block copies. Unrelated functions with the same source spelling,
changed symbol bindings and arbitrary calls cannot establish the recipe.
No copy, result claim or release is synthesized by the proof itself.

The proof excludes state-changing bodies, local capture aliases, nonstatic
receivers, shared `this`, reference/consumed
parameters, weak/unsafe/volatile/atomic parameter storage and unresolved bodies.
Ordinary direct literals retain their existing checks. Every actual captured
argument is validated before completing the returned literal's proof; shared
literal nodes cannot hide a different caller's value. Syntax, depth, arguments,
switch labels and capture work remain independently bounded.

The native package witness adds simple, branching/static and nested factories
on top of the 14 work-item shapes. Producer sources are removed before two
direct and two directed consumers execute normal and pre-entry-cancelled work.
Exact object retirement, work and defer counts remain checked in ARC and MRC.
Runtime ABI 24, interface schema 2.2 and reader capabilities are unchanged.

The positive factory header fails against Source 2.204 and passes with this
implementation. Validation passes 288 source/PCH commands across the factory,
control-flow, helper and mutable-binding suites in C17/C++20/C++23, ARC/MRC and
all four profiles. Four optimized native configurations execute 1,216 task
scenarios across two direct and two directed consumers. Unsafe objects,
cleanup-managed values and unknown nested callbacks reject through imported
factory bodies. The full isolation suite, production codec, all 122 AST
controls and 16 tooling checks pass; fresh patch application reproduces all
changed LLVM sources and manifest digests match. Matching SDK/archive and
full-profile acceptance remain separate.
