# Permanent references, callable producers and member dispatch

Source 2.167 / System 2.93 extends the permanent-result proof under
`objc3.borrowed-result-proofs.v8`. This is a reader cutover; it does not promote
full language/profile conformance or an SDK archive.

Const pointer references, known function references and local read-only reference
aliases retain their proven inputs. The checker discovers the complete direct
alias graph before scanning uses, joins every assigned backing-pointer value,
and invalidates original slots when an alias escapes, is cast for mutation or
is captured by reference in a Block. Direct reference forwarding requires the
callee's complete bounded body to preserve the binding. Unknown callees,
volatile inputs, replaceable globals and unproved mutation remain rejected.

Ordinary/static C++ methods, operators, lambdas and concretely instantiated
callables can produce permanent results and callback sets when every return path
proves a permanent base. Implicit object arguments are excluded from explicit
parameter bindings; explicit object parameters retain their real argument index.
The proof admits nonvirtual, explicitly qualified and final virtual targets.
Lazy async/coroutine calls keep their separate lifetime protocol. Unresolved
virtual dispatch, member-pointer calls and unproved receiver fields
remain outside this proof. This never infers that the receiver itself lives
forever.

Reference results require their own storage proof. A reference to an immutable
static pointer object can preserve a separately proved permanent pointee or
callback target; a reference to a local/by-value pointer object cannot. Known
function-reference factories retain their actual code identity. Wider reference
lifetimes and arbitrary object/field ownership remain separate work.

All expression, alias and recursive call checks share the 4096-unit work budget,
32-level depth limit and 64-target bound. No new runtime layout is introduced;
extension ABI 24 remains current.

Virtual call expression identity now distinguishes explicitly qualified base
calls from virtual dispatch under `objc3.cxx-virtual-call-expressions.v1`.
Required-capability agreement and reader support are checked independently.
Source-free execution controls retain the different results of a derived
virtual override and a qualified base call; namespace claims cannot fabricate
the capability.

Maintained controls cover source/PCH across C17 and C++20/23 with MRC/ARC,
source-free publication and native consumers, import after source AST destruction,
known/unknown callback alternatives, alias mutation/escape, temporary/local
reference results and qualified/final/unresolved dispatch. Broader vendor,
sanitizer, host and matching SDK consolidation remain separate gates.

The focused checkpoint passes all 18 expanded source/PCH commands, 83 maintained
AST infrastructure tests, six C17/C++20/23 MRC/ARC publication/replay profiles,
four source-free native storage/ownership configurations, codec controls and
29 related tooling tests. The native configurations retain exact cleanup counts
and independently check reference factories, callable results and both dispatch
forms. These source checks rebuild the changed LLVM objects against the existing
SDK; they do not constitute a new sealed SDK acceptance result.
