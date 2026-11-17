# Native async work-item helper projections

Source 2.203 / System 2.129 extends capture-environment proofs through closed
native functions returning Block work items. A dedicated Block-value inference
mode preserves the union of actual caller environments through pure bounded
branches, switches, immutable aliases and nested helper calls. Ordinary scalar
resource projections retain their existing stricter type/ownership gates.

The capture collector checks every possible returned environment and ignores
unreturned argument environments. It still rejects unresolved/recursive/mutating
factories and helpers that construct a new closure around unbound parameters.
Block-value conversion cleanups retain their normal native ARC/MRC lowering;
this proof does not replace copies, result claims or releases.

Immutable global Block definitions are admitted only with internal or inline
native bindings and an available initializer. Mutable globals, declarations
without initializers and weak/replaceable bindings cannot establish a closed
capture proof. The same initializer-cycle and syntax budgets apply to these
bindings and local work items.

The source-free provider/four-consumer witness (two direct, two source-directed) expands from four to nine work
item shapes: direct/mutable/conditional environments, helper selection, nested
forwarding, immutable aliases and a global Block. Both choices run normally
and with pre-entry cancellation; exact captured object retirement, work and
defer counts remain checked in ARC and MRC. This extension changes no runtime
layout, ABI 24 contract, interface schema or reader capability.

Canonical interfaces now carry a declaration-content include guard in addition
to `#pragma once`. A dependency materialized under two paths is parsed once;
distinct declaration/ownership variants retain different guards. The guard is
computed before wrapping the canonical body, so replay and re-export do not
introduce a self-referential hash. Native package witnesses retain an external
Objective-C class to defend the duplicate-dependency failure, and focused
C/C++/Objective-C/Objective-C++ controls prove two-copy admission, exact replay,
distinct-body separation and the failing behavior when guards are removed.

Admitted dependency guards are established through a private preprocessor
prelude before deferred native AST import can lex the consumer's first header.
On-demand `@import` performs admission before consuming its semicolon, ensuring
a following textual include observes the guard. The native witness includes
a copied ABI dependency immediately after `@import` and executes every direct
and directed consumer. Native runtime/header configuration is established
before source-directed imports, as required by the current native prelude.

Focused validation passes 216 source/PCH commands across C17/C++20/C++23,
ARC/MRC and all four profiles. Four optimized C++20/23 ARC/MRC configurations
execute 576 task scenarios across two direct and two source-directed consumers,
including normal completion and pre-entry cancellation. Unsafe/unknown actual
environments reject through imported helper bodies, and ignored environments
do not contaminate a selected safe work item. The include-guard control covers
four native language modes. Fresh LLVM patch application matches the authored
Source 2.203 sources, manifest digests match and 16 tooling checks pass.
All 17 import groups, 19 publication groups, production codec and all 122 AST
controls also pass with the new source-import ordering. Full isolation/profile
and matching SDK acceptance remain in the consolidated roadmap gates.
