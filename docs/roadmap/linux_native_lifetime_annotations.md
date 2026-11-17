# Native lifetime-annotation packages

Source 2.189 / System 2.115 preserves native GSL `Owner`/`Pointer` annotations,
including their optional pointee types, and `lifetime_capture_by` contracts in
source-free C++20/C++23 packages. Native lifetime diagnostics remain active after
the provider source is removed.

AST import owns the pointee type/source information, capture target arrays,
identifiers and source locations. Canonical output uses the current callable's
parameter names; unnamed referenced parameters receive collision-free names.
Owner/view annotations precede a record's name even when inherited by a forward
declaration. Capture targets bind resolved parameter positions and special
`this`, `global` or `unknown` entities, independently of their source spelling.
Implicit receiver contracts retain their function type locations and canonical
C++ attribute grammar.

The source compiler now propagates capture contracts to later declarations and
definitions, preserving their parameter positions across renamed or unnamed
parameters. Conflicting written target sets reject with a previous-contract note.
Template instantiation rebinds renamed recipes; lambda call operators resolve
their capture targets after parameter ownership and object conventions are
established. Standalone global/unknown annotations retain their meaning when a
parameter has the same name.

`objc3.native-lifetime-annotations.v1` requires a current reader. Typed pointee
dependencies, implicit receiver contracts and published expression-local contracts
contribute to package admission and semantic identity. Unresolved targets and
capture contracts outside a named callable context reject. Rehashing altered,
otherwise valid capture metadata cannot bypass semantic identity comparison.

This implements native annotation transport and enforcement. The annotation is a
promise to native lifetime analysis; it does not prove arbitrary callee behavior,
add erased callback lifetime protocols, or establish complete ownership/profile
conformance. Extension ABI 24 and interface schema 2.2 retain their layouts.
Matching SDK and native-host acceptance remain pending; focused object relinking
does not qualify a replacement archive.

Focused evidence: all six C++20/C++23 × pure/MRC/ARC optimized native
configurations pass with exact canonical replay, provider removal and two executed
consumers. Controls cover renamed/unnamed functions and templates, implicit
receivers, ordinary/generic/static/explicit-object lambdas, native dangling
captures, hidden pointee dependencies, erased callbacks, body-only capability
admission and changed-metadata rejection. All 18 source/PCH commands and 104 AST
controls pass, along with the 17-group import suite, production decoder and 70
related tooling/authority checks. Maintained patches reproduce the production and
regression sources exactly.
