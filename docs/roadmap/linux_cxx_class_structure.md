# Nonvirtual C++ base and defaulted-member packages

The concurrency 2.9.0 source span admits nonvirtual single/multiple inheritance,
empty bases and in-class defaulted special members in semantic interfaces.
The active SDK remains the accepted
[2.8.1 delivery](../support/linux_cxx_deleted_member_acceptance.json).
The new compiler source identity is
`llvmorg-23.1.3-0d261d1ca552-e926e2771bde-00b75dc3`.
Extension ABI 21, interface schema 2.1, service ABI 4 and frame ABI 1 are unchanged;
regenerate compiler-dependent artifacts for this source cutover.

The required `objc3.cxx-class-structure.v1` capability accompanies the existing
class-lifecycle capability. Identity binds ordered direct bases, canonical base
types, public/protected/private access, native byte offsets, aggregate/empty
traits and existing layout/calling facts. Base types must be exported locally
or supplied by resolved dependencies, including private and protected bases.
Reordering equal-size empty bases or changing access invalidates the semantic
fingerprint even when size and alignment stay the same.

In-class defaulted default/copy/move constructors, copy/move assignments and
destructors retain native Clang behavior. Implicitly deleted defaults remain
unavailable. Defaulted declarations bind defaulting, inline and user-provided
state. Implicit exception and constexpr behavior derives from the bound class,
base and member declarations; lazy evaluation or body instantiation in a
provider must not change interface identity. Explicit exception contracts remain
distinct. Consteval members, defaulted comparisons and out-of-line defaulting
remain rejected alongside concrete inline/constexpr/template bodies, member
initializers, default arguments, nested declarations, virtual bases and dispatch.

Canonical Clang printing preserves a declaration newline for instantiated
`= default` members and omits their unwritten implicit exception specification.
AST import preserves implicit inline state, trivial-for-call state and actual
deletion, including implicitly deleted defaults. Completing structural import
restores the source definition bits after base/member insertion; incremental
insertion must not count an already-recorded data base twice and corrupt the
standard-layout trait. Native regressions cover these importer and printer cases.

Source-free provider/consumer fixtures exercise an empty base, distinct data
bases, nonzero receiver adjustment, private/protected access, aggregate
initialization and trivial/defaulted copying. A nontrivial derived value uses
defaulted copy/move/assignment/destruction, heap storage, borrowed inherited and
derived async members, and typed frame arguments/results. Eager import tests
check access, standard-layout and defaulted construction in C++20 and C++23.

The source compiler passes 371 Clang/index/query regressions and 167 native
AST/matcher/clangd contracts. The C++20/23 ARC/MRC O0/O2 execution preflight passes
all eight cases in each normal, ASan/UBSan and TSan mode (24 cases / 96 commands).
It removes provider source before consumer compilation and checks the complete
published class metadata. Unchanged service archives and instrumented GNU
variants are reused; normal GNU and runtime headers come from accepted 2.8.1.
All 32 source facade suites, 14 installed publication-rejection fixtures and
29 tooling checks pass.

SDK packaging is deferred for this implementation batch. Installed qualification,
immutable package publication/relocation and the focused fresh Debian/Ubuntu
class-package checks must bind the new SDK before it replaces 2.8.1. Source
preflight results do not establish those delivery checks. Public async/actor
predicates and full Core/Strict/System conformance remain reserved.
