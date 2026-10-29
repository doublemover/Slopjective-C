# Standard C++ coroutine types and traits on Linux

Source implementation **2.90** extends C++ semantic packages to public standard
coroutine handles, suspend/no-op types and provider-authored full and partial
`std::coroutine_traits` specializations. Typed handles retain their native
promise access, erasure, address identity and frame destruction behavior.

`objc3.cxx-standard-coroutines.v1` binds declaration identities to the actual
system `<coroutine>` header content. Canonical replay and consumer imports
prepare that header even when a module contains only types and signatures.
User declarations placed in `std` retain ordinary dependency/export checks;
user promise types and traits aliases cannot hide excluded dependencies.
Capability and owner mismatches fail with `O3IFC099`.

Traits specializations publish through their named return-type export root.
Their checked arguments and complete definitions have independent identities;
the standard primary and its namespaces remain header-owned. Specialization
collision checks use exact native identities, and specializations do not become
ordinary namespace lookup entries. Return types and promise definitions retain
the provider's module ownership. Standard-library primaries acquire no package
ABI tags.

The shared fixture covers typed and erased handles, direct standard suspension,
full/partial trait selection, public traits aliases, ordinary/generic/member and
returned closure coroutines, provider-created frames, cancellation, exception
cleanup and exact lifetime counts. Compiler witnesses exercise header placement,
eager/source-directed imports, type-only modules, two independent providers,
canonical replay, relocation, specialization changes and private-dependency
rejections. Combined validation is pending. SDK 2.62 remains the accepted archive;
full language/profile conformance and general standard-library publication remain
separate work.
