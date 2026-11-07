# Virtual object variants and anonymous active members

The [System 2.47 span](linux_block_array_members.md) now adds per-element
registration and cleanup for bounded ARC Block member arrays. Current
subobject publication requires v4; the v3 capability and array boundary below
record the original 2.46 span.

System source 2.46 gives native constructors and destructors separate
complete-object and base-object cleanup graphs. Both versions share their source
body and meet only at the function exit. Virtual initializer side effects and
virtual cleanup therefore stay correlated with the physical object variant.

Complete construction registers virtual bases, direct nonvirtual bases and members
in native order. Base construction skips virtual initializers entirely. Cleanup
destroys the completed prefix in reverse. Complete destruction handles virtual
bases after members and nonvirtual bases; base destruction leaves them to the
most-derived object. Abstract constructors have only the base form, so even
virtual initializer argument effects are excluded.

Function-try dispatch, explicit returns, goto backpatches, cleanup registration
and exception termination state are built independently for each variant.
Delegation failure after successful target construction uses the complete or
base destructor as appropriate. A virtual diamond destroys its shared virtual
base once. Nullable receiver analysis uses these same alternatives: assignment
in an ignored virtual initializer cannot prove a base-constructor receiver
nonnull. Stabilizing the receiver in the body remains valid.

Indirect anonymous-union initialization registers the active leaf after its
successful construction. Constructor failure destroys that leaf exactly once.
The parent destructor does not implicitly invoke an anonymous union destructor;
its user-authored active-arm cleanup remains an ordinary native operation.

ARC strong and weak Block member slots now use native nonthrowing release
operations. A strong member's implicit heap copy has its own exception edge,
even when the AST contains no explicit ARC copy cast. Registration follows a
successful copy. Source/PCH and CFG controls distinguish zero, partial and full
initialized prefixes. Native faults are armed inside the initializer sequence
after parameter retention, then target the first and second actual field copies;
body/destructor failures retain exact owner destruction and deferred ordering.
The primitive copy/capture predicate remains unchanged. Block array fields retain
an explicit proof gate until per-element copy registration is implemented.

Source-free publication now includes ordinary unnamed member-record graphs and
anonymous unions. Their methods, nested class/alias templates, enum projections,
member pointers, overloads and template arguments bind to the owning declaration
and lexical member order. Canonical signatures and native names no longer depend
on anonymous-tag source coordinates. Same-layout siblings remain distinct after
relocation, printing and import. The native package fixture includes independent
provider/consumer calls and type/value checks for these projections.

Publication makes hard cutovers to `objc3.native-subobject-cleanup.v3` and
`objc3.cxx-unnamed-record-projections.v2`, with matching identity markers and
package acceptance. Extension ABI 24, native frame/service ABIs and semantic
schema 2.2 remain unchanged. New source/PCH diagnostics, independent CFG path
controls, source-free replay and exact native traces defend both variants,
abstraction, diamonds, delegation, indirect-member cleanup and Block copies.
All 65 affected compiler cases, six new source-free profiles, fourteen adjacent
frontend groups, codec/isolation controls, 39 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass. Two final source/PCH fixture matrices
also pass after adding the array boundary control. The newly extended installed
package fixture awaits the final matched SDK; these development-source checks
do not claim archive qualification. Resource subobject recipes, managed initializer temporaries and
broader ownership/profile gaps remain tracked separately.
