# Nonvirtual C++ base and defaulted-member packages

The accepted concurrency 2.9.0 delivery admits nonvirtual single/multiple
inheritance, empty bases and in-class defaulted special members in semantic interfaces.
The [acceptance receipt](../support/linux_cxx_class_structure_acceptance.json)
binds the source checkpoint, installed SDK and exact archive. The earlier
[source progress receipt](../support/linux_cxx_class_structure_progress.json)
retains its source-only checkpoint and deferred checks; those delivery checks
are now complete. The compiler identity is
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

The coordinated SDK build passes all 32 facade suites, nine GNU runtime client
tests, 37 installed consumer checks, 13 integration nodes / 76 runtime executions
and all 33 installed gates. The installed five-family matrix passes 168
configurations / 966 commands. Immutable source-free native async packages pass
12 modes / 36 executions, including eight C++ structure modes, and exercise
publication, installation and relocation without provider recompilation.

Fresh Debian 13 and Ubuntu 26.04 pass the same archive in `cxx-class-packages`
scope. Ordinary-user checks cover 12 native async package modes / 36 executions, 12 C++
interface executions / 98 commands / 18 rejection cases, and the common stdlib
40-execution / 258-command matrix per distribution. Installation, relocation,
reinstallation, embedding and debugger audits pass. The broader fresh feature
matrix retains its historical 2.8.0 archive scope; the complete installed feature
matrix above binds this new SDK. Both owned containers are removed.

Superseded delivery artifacts are reclaimed; one SDK installation and archive
remain. Development frontend and GNU client paths bind the current installation.
Public async/actor predicates, full Core/Strict/System conformance, native Linux
AArch64 and the remaining implementation roadmap stay open.

The [2.10.0 source milestone](linux_cxx_class_dispatch.md) extends these bounds
to virtual bases and dispatch. Its implementation and focused checks are
separate from this accepted 2.9.0 archive.
