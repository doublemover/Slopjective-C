# Public projections of C++ factory result types on Linux

Source **2.80** publishes aliases, fields, bases, callbacks, shared storage and function
signatures using direct local result types from complete module-owned concrete
factories. Each canonical type binds its ordered nominal factory origins,
including native tag kind, name and disambiguator. Unrelated anonymous results
and reversed callback arguments retain distinct identities despite equal layouts.

Native anonymous local-tag identities use their enclosing callable and native
mangling number. Import copies that number before recursive type/body use, keeping
multiple anonymous tags distinct after relocation and replay. Public projections
check the visibility and dependency policy of their originating factory.
The capability `objc3.cxx-factory-type-projections.v1` follows complete declaration
owners; missing capabilities and namespace-only owners fail with `O3IFC089`.

Implementation witnesses cover named/anonymous result aliases, overloaded entry
points, ordered callbacks, aggregate fields and bases, pointer/reference signatures,
provider-owned functions with nontrivial value passing, shared inline storage
and concrete member factories.
The [focused source receipt](../support/linux_cxx_factory_type_projections_progress.json)
records all 42 native import-family tests, the emitter suite, 84 tooling tests
and four representative source-free configurations / 24 commands passing; broader source/sanitizer and SDK
delivery remain batched. Template-argument use, generic/closure factory origins,
other unnamed-type projections, custom promises and full profile conformance stay
open. SDK 2.62 remains accepted; native Linux AArch64 remains unqualified.
