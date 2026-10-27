# Concrete C++ friend contracts

The concurrency 2.23.0 source milestone models non-template friend functions and
nondependent friend types under required `objc3.cxx-friends.v1` semantics. Records
bind their friend set, signatures, inline/constexpr/consteval/deleted state and
published bodies. Function rows bind the owning record independently. Native
friend access and argument-dependent lookup remain intact after source import.

Canonical class source retains hidden inline definitions and friend types;
out-of-line inline definitions retain separate roots after their owning class.
Provider-owned out-of-line definitions keep their native module symbols.
Initialized captures and function-local thread storage retain their existing
expression/storage contracts inside friend bodies.

Export and SPI policy follow friend ownership across redeclarations. Shared
function bindings require every owning record to be exported under the same SPI
policy. Conflicting or excluded owners reject interface publication. Dependency
checks continue to cover public signatures and published executable bodies.

Source fixtures cover private-field access through friend classes and ordinary
functions, hidden comparisons, constexpr evaluation, deleted operators and
out-of-line inline definitions used by a consumer after provider source removal.
Generic/dependent friend declarations retain their template boundary.

The accepted SDK remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json).
Combined source/sanitizer matrices and installed/archive qualification remain
scheduled for the coordinated source-batch cutover; full profile conformance is
not claimed.
