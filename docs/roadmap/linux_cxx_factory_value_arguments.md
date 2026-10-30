# Structural C++ factory values on Linux

Source **2.83** adds structural non-type template arguments whose types originate
in complete public constexpr factories. Argument identity binds the exact nominal
factory origin and converted value bits. Closed generic origins retain function
and enclosing-class bindings. Ordinary address arguments retain their separate
visibility and dependency checks.

Published primary templates, aliases, recursive packs, constrained partial and
full specializations, function/variable specializations, shared storage and closed
generic lambdas can use these values. Nested arrays and address-bearing values
retain native structural representation and checked referenced declarations.
`objc3.cxx-factory-value-arguments.v1` follows complete published owners; capability
and namespace-owner mismatches fail with `O3IFC092`.

The [focused source receipt](../support/linux_cxx_factory_value_arguments_progress.json)
records the emitter suite, 84 tooling tests, 16 source-free configurations /
96 commands and a full-fixture smoke case passing. Broader source
and sanitizer matrices stay batched. SDK 2.62 remains accepted. Other closure and
dependent unnamed-type projections, remaining language/profile implementation and
native Linux AArch64 qualification remain open.
