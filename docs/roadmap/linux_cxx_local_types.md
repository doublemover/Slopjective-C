# Published C++ local type contracts

The concurrency 2.29.0 source milestone publishes concrete named local classes,
structs and enums in C++/Objective-C++ inline bodies and local lambdas under
required `objc3.cxx-local-types.v1` semantics. Nested local types, member bodies,
constructors, copies, destructors and virtual dispatch retain their native AST
contracts. Local types remain inside their containing executable definition.

Expression identity binds local tags and member references to lexical slots,
including enum values and overloaded method types. Physical provider filenames
do not enter those references. Canonical source retains the owned definitions;
regeneration and import compare the complete containing declaration contract.
The capability must agree with the local-type identity at encoding, decoding
and module-contract validation. Artifact loading, resolution, ordinary import
and ownership projection recognize the same capability.

Dependency checks distinguish local members and parameters from referenced
public declarations. Local field and function types still validate external
dependencies, exports and SPI policy. Local types cannot escape through a public
deduced return or another exported signature. The [2.38 source extension](linux_cxx_unnamed_local_types.md) adds unnamed local
records and enums. Generic/dependent bodies, public unnamed nested types and
unions retain their existing admission boundaries.

Maintained fixtures cover local enums, nested records, nontrivial copying and
destruction, virtual methods, RTTI, constexpr evaluation and classes inside
lambdas. Source-free consumers call both imported inline bodies and a provider
bridge with the consumer object linked first. Changed enum values and member
bodies must change semantic fingerprints; erasing the capability is rejected.

This source milestone does not establish complete C++ template, ownership,
async or profile conformance. The
[2.29.0 SDK acceptance receipt](../support/linux_source_batch_2_29_acceptance.json)
records all installed gates and focused exact-archive Debian/Ubuntu consumers.

The [focused progress receipt](../support/linux_cxx_local_types_progress.json)
records the frontend/tooling checks and normal/ASan source-free package matrices.
