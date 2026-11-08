# Native initialization and static-lifetime publication

Source 2.145 / System 2.71 preserves native constructor/destructor priorities,
object initialization priorities, `constinit`/`require_constant_initialization`,
and explicit `no_destroy`/`always_destroy` policies in semantic interfaces.
These contracts apply to public declarations, inline variables, admitted variable
templates and local static/TLS storage in published bodies. Native Clang still
checks each attribute's subject and construction requirements.

Priority identities use their effective integer values. Expressions retain
ordinary public dependency checks, so a selected entry cannot hide a private
priority dependency. Canonical printing preserves written and inherited lifetime
attributes and gives them stable declaration positions. Local storage policy
participates in body identity, even when the local's type and initializer stay
the same. Unpublished bodies and unselected namespace siblings do not invent
reader requirements.

C++ language profiles now bind static-destructor registration policy and
thread-safe static initialization. Canonical reparses preserve `all`,
`thread-local` and `none` destructor registration instead of restoring defaults.
These profile changes are a hard identity cutover; dependent interfaces and
objects must be regenerated. `objc3.native-storage-lifetime.v1` separately binds
the new declaration/body contracts and rejects older readers or missing/invented
capabilities. Existing native-declaration-attribute v2 ownership remains in force.

Six C17/C++20/C++23 ARC/MRC publication profiles pass exact replay, relocation,
changed-priority/storage identity, private-dependency and reader controls; C++
profiles additionally check all three alternate static-lifetime options. All 61
AST controls and 600 exact production decoder replays pass. Four source-free
native ASan/UBSan configurations perform 24 compile/link/execution operations,
remove the provider source, link two independent consumers, and check startup
priority, shared local-static identity, exact process/thread destruction and
explicit destruction with implicit static destructors disabled. The maintained
CTest `objc3_native_storage_lifetime_provider` carries these controls.

The [receipt](../support/linux_native_storage_lifetime_progress.json) identifies
the prepared development source. Full current compiler/frontend consolidation is
running alongside continued implementation. Extension ABI 24/frame 1/services
4/schema 2.2 layouts remain unchanged; one matched SDK follows the feature batch.
