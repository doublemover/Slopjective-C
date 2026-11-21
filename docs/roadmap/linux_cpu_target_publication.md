# Native CPU target policies in published packages

Source 2.160 / System 2.86 publishes native `target`, `target_clones` and minimum vector width
function policies. Canonical declarations retain feature strings and clone
ordering; exact policy values participate in interface identity. Ordinary
functions, C++ members and templates retain their target-specific implementations
when the provider source is removed.

AST import now preserves a function's shared canonical multiversion state,
including import into an existing forward declaration. Explicit target bodies
remain distinct during import even though Clang gives them the same ordinary
USR and function type. Copying the clone attribute alone does not establish
runtime dispatch. A permanent AST control
checks the flag and clone features after destroying the source context.

`objc3.native-emission-policies.v2` is a hard reader cutover. Namespace-owned
emission-policy claims are rejected by both the typed encoder/validator and
byte decoder. Unsupported declaration contracts retain their admission errors.
Explicit target variants receive distinct interface identities while retaining
one sealed module-owned assembly base; the backend appends target suffixes and
retains the resolver. The compiler admits the exact reserved module-label shape
for native target families, while ordinary/malformed labels keep the upstream
diagnostic. Native runtime/service and extension ABI layouts are unchanged.

Permanent borrowed-result proof does not inspect a single explicit target body
as proof of resolver-selected alternatives. A target_clones family retains one
source body for every clone and remains eligible for the ordinary bounded proof.

Publication controls cover source/PCH-independent replay, exact canonical bytes,
relocation, feature/clone changes and older/missing/invented reader claims.
Source-free native provider/two-consumer controls cover C++20 plain and C++23
ARC/MRC at O0/O2, inspect emitted target features, minimum vector width and IFUNC resolvers, and execute
the runtime-dispatched clones plus ordinary/template/member target functions.
Complete rebuilt compiler/SDK qualification remains pending.
