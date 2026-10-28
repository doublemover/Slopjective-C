# Generic local callable contracts

Source 2.40 extends closed generic local C++ callables with expression
constraints, `requires` expressions and dependent `noexcept`. Native printing
retains the trailing constraint after the complete lambda declarator. Template
and requirement parameters have lexical identities; concrete specializations
retain their complete bodies and types. Named concepts still require the
separate public template interface contract and diagnose explicitly. Unresolved
overload candidates enter sorted identity and dependency checks, so a private
constraint-only overload cannot disappear when the interface is regenerated.
Native import registers the owning function and its parameters before importing
the trailing constraint, breaking recursive requires-body context imports while
retaining the complete clause and its parameter-pack substitution index.
Restored function type locations retain their parameter bindings, and imported
requirement parameters retain both their owning body and declaration-list
membership. Source-free fingerprints therefore see the complete native AST.

Named structural template values retain their recursive base, field and array
contents. Floating-point values bind their exact bits, preserving signed zero
and distinct NaN payloads. Null pointer and null member-pointer leaves are
admitted; address-bearing values, unsupported representations and excessive
depth/size fail closed. Structural template parameter objects bind their type
and value without compiler addresses or synthetic declaration names. Every
specialization argument type participates in visibility and dependency checks,
including unused structural parameters whose types would otherwise disappear
from the instantiated body.

This is a hard cutover to `objc3.cxx-generic-local-lambdas.v2` and its matching
identity marker. The earlier v1 capability is absent from current consumer
admission lists. The compiler, sidecar codec, module validation, loading,
resolution and ordinary import share the new contract. Public templates,
escaping closure types, local/unnamed specialization types, effectful generic
callables and public unions remain separate implementation work.

Authored witnesses cover source/PCH printing and code generation, pure
C++20/23 source-free object consumers and Objective-C++ ARC/MRC packages.
Structural bases, arrays, nullable fields, floating-point bit identity,
constraint-local parameters and native `noexcept` evaluation exercise the new
contract together. Validation and installed SDK delivery are recorded
separately; SDK 2.33 remains the accepted archive.
