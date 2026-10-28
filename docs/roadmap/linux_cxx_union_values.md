# Published native C++ union values

The 2.41 source span admits named, nested, typedef-owned and local native C++
unions in semantic interfaces. Their native layout binds the union kind,
size/alignment, overlapping field offsets and bit widths. Existing class/member,
namespace, export/SPI and module-owned symbol contracts apply to their ordinary
C++ operations. Native ABI tags apply to unions as well as structs/classes,
matching GNU Itanium mangling and retaining distinct module-owned names through
source/PCH replay.

Source-written constructors, copies and destructors retain explicit active-member
lifetime management, including nontrivial C++ members. Inline/constexpr bodies,
local anonymous unions and closure copies retain their complete definitions and
lexical identities. Ordinary by-value parameters/results use the pinned native
C++ ABI. Structural template union values retain the selected member index as
well as its value, so equal payloads in different members remain distinct
instantiations.

Interfaces require `objc3.cxx-union-values.v1` alongside the C++ data contract.
The identity/capability relationship is checked during encoding, decoding and
module validation; loading, resolution and ordinary import admit the same
contract. C interface union identity is unchanged.

Authored witnesses cover native layout, by-value calls, explicit nontrivial
construction/copy/destruction, failed-copy capture rollback, constexpr evaluation,
named/nested/typedef/local
forms, anonymous local closure copies and structural active-member identity.
Public unnamed nested types, escaping local types, stricter union ownership
precision and the separate union frame/isolation/key-path boundaries remain
explicit. This source span does not promote full profile conformance or rebuild
the accepted SDK 2.33 archive.
