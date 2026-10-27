# C++ nested type packages

The concurrency 2.12.0 source milestone admits nested named classes, enums and
type aliases through the owning class's export policy. Native member functions,
lifecycle operations, scoped enumerators and field layout remain attached to
their original declaring type. Private/protected names retain native access
checking; a public alias can expose an otherwise private nested type normally.

Required `objc3.cxx-nested-types.v1` semantics bind nested declaration kinds,
identities and access on both the nested type and its containing record. Nested
type membership is canonicalized as a set: dependency import can materialize a
nested type before unrelated members without changing its ABI. Field order,
base order and virtual slots retain their existing ordered identities.

Source-free fixtures copy/move nontrivial nested values through native frames,
call nested async members, use scoped enum/type aliases, expose a private type
through a public alias, and reject direct private/protected name access. Nested
records retain module-owned native type ABI tags and method symbols.

Namespace import also filters ungranted SPI and private dependency members from
namespace lookup. Lexical declarations stay alive for native type/ABI references;
a subsequent explicit import/grant can promote the member without rebuilding its
AST. Focused SPI fixtures cover eager and source-directed imports.

Unnamed nested types, concrete inline/constexpr/template bodies, member
initializers, default arguments and using declarations/directives retain their
separate boundaries. The accepted SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json). Broad source,
sanitizer, installed and archive validation remain deferred to the combined
implementation batch.
