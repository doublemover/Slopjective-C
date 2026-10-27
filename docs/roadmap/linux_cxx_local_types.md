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
deduced return or another exported signature. Generic/dependent bodies, unnamed
local tags and unions retain their existing admission boundaries.

Maintained fixtures cover local enums, nested records, nontrivial copying and
destruction, virtual methods, RTTI, constexpr evaluation and classes inside
lambdas. Source-free consumers call both imported inline bodies and a provider
bridge with the consumer object linked first. Changed enum values and member
bodies must change semantic fingerprints; erasing the capability is rejected.

This source milestone does not establish complete C++ template, ownership,
async or profile conformance. Installed SDK/archive delivery follows the
coordinated implementation batch.
