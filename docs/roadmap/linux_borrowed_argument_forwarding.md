# Permanent borrowed results through immutable helper parameters

Source 2.154 / System 2.80 extends complete bounded static-result proofs across
statically bound helpers that forward immutable by-value pointer parameters.
The checker substitutes actual call arguments and proves their permanent bases
in the caller environment. Local immutable aliases, nested forwarding calls,
conditional choices, branches, offsets and C++ default arguments compose with
the existing return-graph proof.

The parameter binding itself must be const, nonvolatile and by value. A
reference or writable parameter cannot acquire a permanent-base proof. Unknown,
automatic, mutable/TLS, resource-bearing or borrowed-owner bases retain their
existing obligations. Every possible return still needs proof, and normal
fallthrough still rejects. Replaceable/external C definitions, indirect calls,
instance methods, recursion and exhausted work stay conservative. Argument
binding, AST traversal and CFG construction share the same bounded work budget.

`objc3.borrowed-result-proofs.v3` is a hard reader cutover. Older readers must
rebuild published bodies; the public borrowed result and owner-index relationship
remain intact even when the implementation returns permanent storage. Runtime
and extension ABI layouts are unchanged.

The maintained compiler controls add forwarded immutable program arrays/unions,
local aliases, nested calls, branch selection and defaults, with unknown-arm,
mutable/volatile/reference/recursive and mutable-storage rejections. All eighteen
source/PCH commands pass across C17/C++20/C++23 ARC/MRC using the changed
production semantic checker. Source-free publication and native package controls
exercise the same forwarding graph after provider removal. Full rebuilt SDK
acceptance remains separate.
