# Native thread-safety contracts in published packages

Source 2.164 / System 2.90 publishes Clang's native thread-safety contracts through
ordinary declarations, records, templates and source-free inline bodies. The
supported family includes capability/shared/reentrant/scoped types, guarded and
pointer-guarded storage, acquisition ordering, acquire/release (shared, exclusive
and generic), try-lock success values, capability assertions, required/excluded
locks, returned-lock aliases and explicit analysis suppression.

Declaration identity records the annotation kind, capability name, lock mode,
ordered argument expressions, delayed template recipes and try-lock success
condition. Parameter and template references bind lexical identities. Native
canonical printing preserves contracts in declarator scope and before free
function assembly labels. Inherited contracts rebind earlier prototype parameter
names to the definition's names. Identical inherited contracts coalesce across repeated sealing, while distinct
arguments and modes remain distinct. Guarded field annotations precede their
initializers, including mutable/aligned/bit-field storage. Attribute expression
imports must own their
destination AST after the source AST is destroyed.

`objc3.native-thread-safety.v1` is a new required reader capability. Missing,
invented, unsupported-reader and namespace-owned fabricated claims reject in the
production validator and codec. The extension/runtime ABI and interface schema
layouts remain unchanged. These annotations describe lock contracts; their
ordinary upstream analysis and explicit suppression semantics remain in force.

Twelve compiler/PCH commands and 80 AST controls cover native diagnostics,
imported AST replay, guarded field layout/initializers, canonical
source/fingerprint stability, relocation, changed capability/mode/order/success
contracts and downstream reader admission. Private lock references must satisfy public
dependency/export policy. Four publication profiles and three source-free native
configurations remove
the provider and compile two consumers with thread-safety warnings treated as errors,
check successful guarded execution and reject a missing-lock consumer. Rebuilt
compiler/SDK acceptance remains separate from focused production-source checks.
