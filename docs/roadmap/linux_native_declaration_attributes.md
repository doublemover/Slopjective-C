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
Combined compilation, replay, runtime and sanitizer checks are pending. SDK 2.62
remains the accepted archive; full language/profile conformance remains open.
