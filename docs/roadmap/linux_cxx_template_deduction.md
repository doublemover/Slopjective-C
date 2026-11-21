# Native template deduction and closure factories

The 2.49 source span admits native `auto` and `decltype(auto)` deduction in
namespace function/variable templates and ordinary class-template methods.
Complete checked definitions remain available to consumers, which instantiate
new argument types and deduce native value/reference results. Effectful template
boundaries, opaque bodies and unmodeled specialization/member-template forms
remain separate contracts.

A required `objc3.cxx-template-deduction.v1` capability binds deduced template
identities alongside their primary template and C++ data capabilities. Class and
variable templates also retain the required structure contract. Dependencies in
heads, bodies, initializers and captures receive the normal visibility checks.

Source/reprint/relocation and changed-body witnesses cover canonical identity.
Source-free C++20/23 consumers exercise scalar and reference deduction, class
accessors, closure factories and deduced inline variable templates. Runtime
witnesses copy closures holding nontrivial public class-template values and
require exact live counts and final complete destruction.

Optimized, broader and instrumented checks follow the combined implementation
batch. SDK 2.33 remains the accepted archive; this source work does not claim
full profile conformance or native AArch64 qualification.

The [combined 2.47–2.49 source receipt](../support/linux_cxx_template_generics_progress.json)
records 410 compiler regressions, 200 native unit tests, all 33 optimized frontend
suites, 335 tooling tests and 32 source-free package configurations / 128 commands
across normal, eager-import, ASan/UBSan and TSan execution. It supersedes the
focused checks and deferred combined-source checks above. SDK delivery remains
pending.
