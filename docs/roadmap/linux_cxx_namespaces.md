# C++ named namespace packages

The concurrency 2.11.0 source milestone adds named, nested and inline namespace
exports to semantic packages, plus namespace aliases. It builds on the
[virtual-class milestone](linux_cxx_class_dispatch.md). The accepted SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json); this source work
has no new SDK/archive qualification claim.

The required `objc3.cxx-namespaces.v1` capability binds complete namespace paths
and inline state on declarations and their namespace containers. Canonical names
retain inline segments instead of using Clang's shortened display names. Scoped
aliases retain native USRs, so identical alias spellings in different namespaces
remain distinct. Namespace aliases bind both the written target and its resolved
namespace, with the same explicit export/dependency rules as other public types.

An export can select a complete namespace or individual qualified declarations.
Selecting a leaf preserves its enclosing namespace declarations; referenced types
still require exports or resolved dependencies. Anonymous implementation
namespaces stay private. Canonical source reopens namespace scopes around each
selected declaration, preserving order without printing implementation bodies or
unselected API. Reopened namespaces contribute one canonical container identity.
Out-of-line function qualifiers are omitted inside their generated wrappers.

Emitter and import fixtures cover nested/inline scopes, namespace aliases,
overloads, distinct scoped type aliases, selective exports and module-owned
symbols. The source-free runtime fixture calls namespaced ordinary functions,
reads namespaced data and awaits a namespaced native frame after provider
removal. Namespace attributes, using declarations/directives and publishing one
native declaration through multiple lookup names retain explicit boundaries,
alongside concrete inline/template bodies, defaults and nested class declarations.

Dynamic class identity additionally binds RTTI mode; the admitted virtual
package contract requires RTTI, preventing mixed RTTI vtable/consumer contracts.

Broader compiler/facade/runtime/sanitizer validation and installed/archive gates
remain deferred to the combined implementation batch. Public profile predicates
and native Linux AArch64 remain open.
