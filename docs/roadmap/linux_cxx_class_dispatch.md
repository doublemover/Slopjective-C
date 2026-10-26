# C++ virtual class packages

The concurrency 2.10.0 source milestone extends semantic packages to virtual
bases and virtual dispatch, including shared diamond bases, abstract classes,
pure/final overrides, covariant returns and virtual destructors. The active SDK
remains the accepted [2.9.0 delivery](../support/linux_cxx_class_structure_acceptance.json)
until a later coordinated SDK cutover.

Required `objc3.cxx-class-dispatch.v1` semantics accompany class lifecycle
identity. Records bind dynamic/polymorphic/abstract/final state, nonvirtual size
and alignment, primary-base selection, transitive virtual-base offsets and the
ordered virtual declarations that determine new slots. Methods bind virtual,
pure, final and overridden declaration identities. Direct base identity retains
order, access, virtualness and native offsets. Removing the dispatch capability
invalidates an artifact that contains dispatch contracts.

Module-owned C++ records use native Itanium ABI tags derived from their module,
package and declaration identity. Their source lookup names remain usable while
vtables, construction vtables, VTTs, RTTI, implicit members and thunks use the
owned type identity. The same ownership contract is checked in publication,
codec admission and import; an explicit conflicting ABI tag is rejected.
Canonical declaration printing preserves these ABI tags, and native AST import
retains tags, virtual bases, abstract/final state and override relationships.

Source-free fixtures cover dynamic cross-casts, a shared virtual base,
covariant receiver adjustment, member-pointer dispatch, deletion through a base
pointer, virtual native-frame calls, and copied/moved derived frame values.
Provider and consumer code both inspect the dynamic type. The source fixtures
also check abstract, polymorphic, final and virtual-destructor traits after
import. Focused emitter cases cover virtual-slot reordering and changes between
pure/concrete declarations or ordinary/virtual bases.

Concrete inline/constexpr/template bodies, defaulted comparisons, out-of-line
defaulting, member initializers, default arguments, nested declarations and named
namespace exports retain their separate implementation boundaries. Extension
ABI 21, interface schema 2.1, service ABI 4 and frame ABI 1 are unchanged;
compiler-dependent objects, PCH, interfaces and packages require regeneration.

This is a source implementation milestone. Broad source/sanitizer validation,
installed SDK gates and exact-archive fresh-distribution qualification belong to
the later coordinated batch. Public async/actor predicates, full profile
conformance and native Linux AArch64 remain open.
