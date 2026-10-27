# Callable ownership contracts and explicit casts

The System 2.4.0 source span preserves `ns_returns_retained` in Objective-C 3
MRC callable types, alongside the consumed-parameter metadata from System 2.3.
Function pointers retain result ownership through canonical types, PCH, native
AST import and source-free interfaces. Nested callable results and member
pointers contribute to the interface's required native ownership identity.
Retained callback aliases print the ownership attribute inside their callable
declarator, preserving nested callback/member ownership and valid C++ type-ids
without accumulating parentheses across serialization.
MRC calls retain their ordinary physical calling convention without added ARC
retains or cleanup releases.

Strict System explicit function/member-pointer casts reject changes to consumed
parameters and retained results, erasure of borrowed results, and invented
noescape or borrowed-parameter permissions. The check follows callbacks passed
to or returned by other callbacks; nested contracts remain invariant. Erasing
an ownership obligation through an opaque pointer or integer also diagnoses.
Converting an unknown representation to a callable with these contracts is
rejected. Preserve the declared callable type instead.

Same-contract casts, resolved overload selection, noncapturing lambdas,
user-defined conversions yielding an appropriately typed callback, null callable
values and void/bool tests remain accepted. A direct callable may drop a
noescape/borrowed-parameter permission; the resulting type cannot authorize a
borrowed argument. Ordinary unannotated opaque callbacks retain their existing
native behavior.

This span checks explicit casts; it does not establish general representation,
union, foreign-call or interprocedural ownership soundness. Block-to-object
bridges, Core Foundation consumption, MRC ownership counts and consuming
isolation entry remain separate work. Regenerate dependent objects, PCH,
interfaces and packages for the changed compiler identity. Extension ABI 21,
interface schema 2.2 and runtime layouts remain unchanged. SDK 2.33 remains the
accepted archive; full Strict System conformance remains open.

Maintained witnesses cover C17/C++20/C++23, ARC/MRC, source/PCH replay, C-style,
C++ named/functional and builtin bit casts, nested callbacks, member pointers,
retained-result MRC IR and source-free callback imports.

The [System 2.5.0 source span](linux_borrowed_foreign_consumption.md) extends
these checks to foreign consumed parameters and makes callable parameter metadata
significant to native structural equivalence and ODR identity.
