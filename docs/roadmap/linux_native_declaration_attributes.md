# Native declaration attributes on Linux

Source implementation **2.89** extends semantic publication to native record,
field and storage attributes, optimizer contracts and diagnostic annotations.
Packed/aligned records, dependent alignment, `no_unique_address`, symbol/type
visibility, inline-control and pure/const contracts, hot/cold hints, unused
markers, `nodiscard` messages and noreturn annotations retain checked identities.

`objc3.native-declaration-attributes.v1` binds supported declaration and parameter
contracts to their published owners. Identical inherited contracts deduplicate;
attribute keys exclude source positions and spelling aliases.
Capability/owner mismatches fail with `O3IFC098`. Dependent alignment expressions
retain ordinary export and dependency checks. Other attributes remain rejected.

Native declaration printing preserves definitions, inherited contracts and escaped
diagnostic messages. Source-free witnesses check packed/aligned calling ABI,
shared storage alignment, empty nontrivial member overlap and exact destruction,
closed dependent alignment, optimizer annotations and provider symbol visibility.
The same contracts cover bounded function/class/alias/variable template owners,
member templates and partial/explicit specializations, alongside generic Swift
metadata. Canonical printing sorts attribute kinds without reordering repeated
payloads. Recursive import retains one copy of an exact source occurrence;
distinct written repetitions survive. Structural comparison handles omitted
optional expression/type arguments, including standard-library owner/pointer
annotations encountered while importing the complete async graph.

All 33 frontend suites, 236 native unit tests, 412 compiler regressions and 27
focused tooling tests pass; three platform compiler cases remain unsupported.
Source-free checks pass 32 configurations / 224 commands across normal, eager,
ASan/UBSan and TSan. Three full async-fixture smoke configurations / 21 commands
also pass, one per normal/ASan/TSan mode. The
[source receipt](../support/linux_native_declaration_attributes_progress.json)
binds the final compiler and reports. These are source checks; SDK 2.62 remains
the accepted archive. Public standard coroutine types and traits are added in the
[2.90 source span](linux_cxx_standard_coroutines.md). Other unmodeled declaration
attributes, dependent lookup projections and full language/profile conformance
remain open.
