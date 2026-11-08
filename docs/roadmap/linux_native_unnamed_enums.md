# Native unnamed enum publication

Source 2.146 / System 2.72 publishes nonempty native anonymous enum groups in
file, namespace and admitted class contexts. The first enumerator anchors the
native enum owner without inventing a tag name or changing its native type.
Selecting any enumerator publishes the complete declaration group; unrelated
groups remain excluded. The enum identity retains its complete value set,
scoped/fixed state and actual integer representation.

Type identities bind each unnamed enum owner at its position in the type graph,
including nested template arguments. Equal-valued enum groups remain distinct.
Checked public template arguments accept these enum types and values; source-free
replay preserves non-type arguments, aliases, enclosing generic records, inline
helpers and constant-defined variables. Filenames are removed from native USRs
only for admitted body-defined entities or public template instantiations whose
checked argument graph supplies the unnamed enum ownership. Native scope/member
suffixes and overload distinctions remain intact.

Native enum-constant references receive one qualification-independent body
spelling while retaining their typed declaration bindings. A bounded type graph
must have a complete identity; exhaustion diagnoses instead of granting partial
admission. `objc3.native-unnamed-enums.v1` requires exact reader support, including
when only a containing signature exposes an enum type. Missing/invented claims
and older readers reject. Explicitly static/private helpers and non-inline
entries whose types lack native external linkage retain their boundaries.

Six C17/C++20/C++23 ARC/MRC publication profiles cover relocation, group
selection, explicit underlying widths, equal-valued distinct owners, generic
values/types, inline calls, and private/non-inline rejection controls. Production
decoding passes 620 exact replays across 31 independent families; nine adjacent
frontend groups also pass. The maintained source-free
native witness removes the provider source and checks distinct enum overloads,
aliases, enclosing generic return values, two consumers and native startup/
static/TLS lifetimes in four ASan/UBSan configurations.

The [receipt](../support/linux_native_unnamed_enums_progress.json) records focused
results and distinguishes the completed Source 2.145 consolidation (544 compiler
cases and 70 frontend groups) from subsequent checks. Extension ABI 24/frame
1/services 4/schema 2.2 layouts retain their existing versions. Remaining feature
implementation precedes final matched consolidation and one SDK delivery.
