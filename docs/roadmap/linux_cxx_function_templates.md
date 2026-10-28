# Published native C++ function templates

The 2.43 source span publishes ordinary namespace-scope native C++20/23 function
template definitions. Their parameter heads, defaults, typed signatures,
constraints and executable bodies participate in semantic identity. Consumers
instantiate new type, scalar non-type and pack combinations from the published
source. Concepts retain their existing nominal ownership and native subsumption.

Module-owned templates carry a native ABI tag rather than one concrete asm
label. Each specialization keeps its native template-argument mangling under
the owning module/package identity. Producer and consumer instantiations can
therefore share a specialization and its local static storage. Constraint-bearing
heads contribute to stable declaration identity so constrained overloads remain
distinct even when native USRs omit their constraints.

Interfaces require `objc3.cxx-function-templates.v1` and the C++ data contract.
Dependency and export checks traverse heads, defaults, constraints and bodies,
including the concrete arguments of calls to published templates. Encoding and
module validation reject inconsistent capabilities. Printed source and native
import preserve definitions for consumer-side constant evaluation and codegen.
Native replay retains leading template attributes and primary definitions;
written calls recreate implicit instantiations without invented explicit pack
specializations. Structural equivalence compares ABI tags, head and trailing
constraints, constrained type parameters and named concept arguments, preserving
distinct overloads during import.

This span admits explicit-return-type definitions with ordinary native effects.
Declaration-only, deduced-return,
member, variadic, effectful and template-template parameter forms remain gated,
as do public class/variable templates and the separate dependent-local-type and
escaping-closure boundaries. It does not claim full profile conformance or
rebuild the accepted SDK 2.33 archive.

Authored witnesses cover source relocation/reprinting, constrained overloads,
consumer-only scalar/type/pack instantiations, shared specialization-local
storage, by-value native class copy/move/destruction and exception unwinding.
