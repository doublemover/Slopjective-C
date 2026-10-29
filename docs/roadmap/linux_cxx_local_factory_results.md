# Factory-owned local C++ result types on Linux

Source **2.79** publishes direct deduced local record, enum and union results under
their complete factory definitions. Named, unnamed and nested local tags retain
their lexical owner, layout, members and complete checked body. Results use
`auto` or `decltype(auto)`; pointer/reference escapes and types originating in
another callable remain separate publication contracts.

Native printing preserves authored deduction for local result types. Complete
anonymous definitions remain available to replay, and import retains one native
factory redeclaration chain. Canonical result identity excludes physical source
locations while complete body/member identities retain semantic changes.
`objc3.cxx-local-factory-results.v1` follows complete published owners; capability
or owner mismatches are rejected with `O3IFC088`.

Implementation witnesses cover named and anonymous records, scoped and anonymous
enums, unions, nested local types, lambda-owned local results, consumer-created
template instances and nontrivial record copy/move/destruction. The [focused source receipt](../support/linux_cxx_local_factory_results_progress.json)
records all 41 native import-family checks, the emitter suite, 84 tooling tests
and four representative source-free configurations / 24 commands passing. Broader source/sanitizer checks and SDK delivery
remain batched. SDK 2.62 remains accepted; other public/dependent type projections,
full language/profile conformance and native Linux AArch64 remain open.
