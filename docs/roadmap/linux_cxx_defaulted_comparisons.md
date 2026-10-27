# C++ defaulted comparisons

The concurrency 2.17.0 source milestone publishes native in-class defaulted
comparison declarations under required `objc3.cxx-defaulted-comparisons.v1`
semantics. Member and hidden-friend forms retain operator kind, immediate and
deleted state, native signatures and containing-record identity. Implicit
constexpr and exception contracts continue to derive from their bound native
base/member declarations.

Hidden friends belong to their lexical record for export selection and SPI
policy. Canonical source prints them within that record once, while sidecars
retain their native namespace-level identity. Native argument-dependent lookup
finds them without turning them into ordinary source lookup names. Module-owned
naming covers nonmember overloaded operators as well as member operators.

Source fixtures cover member equality/inequality, array members, bases and hidden
friends, compile-time comparisons and source-free provider/consumer comparisons.
Both provider-used and initially unused comparison declarations must reproduce
the same public fingerprint. Capability erasure is rejected.

Ordering operators still require their public result and dependency types to
satisfy the normal resolved-interface policy. This does not establish an
unqualified standard C++ ordering-library interface. Dependent constraints,
concrete non-defaulted friends, out-of-line defaulting and broader
ownership/profile contracts remain separate work. The accepted SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad source,
sanitizer and installed/archive qualification remain deferred for the combined
source batch.
