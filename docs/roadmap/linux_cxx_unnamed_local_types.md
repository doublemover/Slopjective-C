# Published unnamed C++ local types

The 2.38 source span extends published concrete C++ bodies to unnamed local
records and enums, including records nested inside local records. Local aliases,
constant evaluation, member bodies, implicit copying/destruction, captured
records and native virtual dispatch retain their ordinary language semantics.
The types remain inside their containing definition. Public unnamed nested types,
escaping local types, unions and dependent/template bodies remain open.

Anonymous type references bind to lexical slots rather than producer filenames
or physical source locations. Definition printing retains owned tag definitions
inside declaration groups; their complete fields also enter the body fingerprint.
Changing an otherwise unused field's layout must change the public contract.
Implicit unnamed structor references use native kind and function type identity,
without a source-located class spelling.

Interfaces require `objc3.cxx-unnamed-local-types.v1` alongside the existing local
and executable definition capabilities. Emission, encoding, decoding, module
validation, loading, resolution and import enforce the same identity/capability
relationship. This is a source hard cutover under interface schema 2.2; no old
artifact fallback is provided.

Maintained witnesses cover regenerated and relocated interfaces, nested records,
local aliases, enums and constexpr evaluation, plus rejected escapes/unions and
missing capabilities. Source-free package witnesses add copied records and
closures with exact native destruction counts, unnamed derived classes with
virtual dispatch/RTTI, and provider/consumer calls into the same inline bodies.
The [source receipt](../support/linux_cxx_unnamed_local_types_progress.json)
records all 33 frontend suites, 332 tooling tests and 24 source-free package
configurations / 96 commands across normal, eager and ASan/UBSan execution.
SDK delivery remains separate; SDK 2.33 remains accepted.
