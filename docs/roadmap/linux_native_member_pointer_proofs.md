# Permanent results through native member-function pointers

Source 2.169 / System 2.95 extends permanent pointer/callback return graphs under
`objc3.borrowed-result-proofs.v9`. Older readers are rejected; extension ABI 24
and the public runtime layouts are unchanged.

Known native member-function pointer values now select complete bounded return
graphs. The proof covers direct addresses, immutable global values, known local
retargeting, read-only reference aliases and parameters, conditional target sets,
helper/factory results and references to immutable permanent pointer objects.
Every possible target must prove every normal return. Null alternatives,
unknown or replaceable global values, volatile/TLS slots, escaped aliases,
dangling reference results and unresolved virtual dispatch retain rejection.
A member pointer cannot encode explicitly qualified virtual dispatch; only final
virtual targets qualify through this path. Receiver fields are not inferred to
have permanent lifetimes.

Calls retain their native member-pointer ABI and implicit base adjustments.
The physical function signature, parameter ownership and Objective-C effects
must agree. Ordinary native noexcept erasure is allowed. Explicit member-pointer
casts do not prove the receiver's dynamic subobject and remain outside admission.
Constant-expression member-pointer fields retain their exact method identity;
mutable aggregate field loads do not gain that proof.

Taking a concrete method-template's address can leave its selected body pending
until the end of the translation unit. The proof now instantiates that already
odr-used, nondependent specialization when its parsed pattern is available. It
does not use the dependent pattern as the selected body or drain unrelated
pending instantiations. Async/coroutine bodies retain their separate lifetime
protocol. Local alias/use scans, recursive calls, target discovery and return/CFG
checks share the existing 4096-unit budget, 32-level depth and 64-target limit.
Read-only forwarding checks every selected callee before retaining the binding.

Maintained controls cover direct/PCH C17 and C++20/23 ARC/MRC configurations,
positive and rejected target/reference graphs, import after destroying the source
AST, print/reparse, source-free publication and two native consumers. Native
execution makes multiple-inheritance receiver adjustment observable and checks
factory alternatives, local retargeting, permanent references, concrete template
methods, constant fields and noexcept conversion. Matching SDK, vendor/profile,
instrumentation and host consolidation remain separate gates.

The focused checkpoint passes 18 expanded source/PCH commands, all 85 maintained
LLVM AST infrastructure tests, six C17/C++20/23 MRC/ARC publication profiles,
four source-free native storage/ownership configurations, production decoder
controls and 29 related tooling checks. Maintained patches reproduce the changed
compiler/control files exactly, and their manifest digests agree. These checks
rebuild the changed LLVM objects against the existing SDK libraries; a fresh
sealed SDK acceptance result remains pending.
