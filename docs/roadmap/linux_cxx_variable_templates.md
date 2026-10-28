# Public native C++ variable templates

The 2.46 source span publishes namespace-scope variable template definitions
with explicit types, type/value defaults, packs, constraints, constexpr values,
mutable shared storage and thread-local storage. Consumer instantiation uses
native C++ template mangling with the primary declaration's module-owned ABI tag.
The primary template has one interface row; its underlying variable is excluded
from ordinary lookup and export collection.

The required `objc3.cxx-variable-templates.v1` capability binds a complete head,
canonical type, inline/constexpr/storage properties and initializer contract.
Published references bind supported exact specialization argument keys.
Dependencies in template heads, defaults, types and initializers must resolve to
public declarations. Capability admission also requires the C++ data contract.

Source/reprint/relocation checks cover canonical identity. Source-free C++20/23
consumers exercise new specializations, while ARC/MRC packages exercise provider
and consumer sharing mutable storage and independent per-thread storage.
Explicit/partial specializations, opaque extern declarations, deduced types,
member templates and effectful variable types remain gated. Public class
templates remain separate implementation work. This source span does not rebuild
the accepted SDK 2.33 archive or claim full profile/AArch64 support.
