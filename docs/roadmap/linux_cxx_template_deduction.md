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
