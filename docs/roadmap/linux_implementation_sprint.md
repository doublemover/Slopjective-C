# Linux implementation sprint

The newer [ambitious implementation sprint](linux_ambitious_implementation_sprint.md)
is source-validated through **2.91**, System **2.16** and keypath **2.6**. Its
[receipt](../support/linux_ambitious_implementation_sprint_progress.json) records
all 33 frontend suites, 236 native unit tests, 412 compiler regressions, 27 tooling
tests, three full 168-case feature matrices and final source-free packages passing.
The sections below retain the original 2.87 sprint and following source spans.

This combined sprint is source-validated through **2.87**, with System **2.15**
and keypath **2.5**. The [source receipt](../support/linux_implementation_sprint_progress.json)
binds the reports and records their exact limits. SDK **2.62** remains the accepted
archive; no archive was rebuilt for these implementation checkpoints. Full profile
conformance remains open.

- Factory type graphs: nested records/enums/unions, exact lexical and overload
  origins, local member factories, aggregate wrappers, callbacks, pointers,
  references, static storage and closed generic/closure roots.
- Actor lifecycle: object-owned lazy mailboxes, concurrent first-entry identity,
  automatic cancellation on association teardown, explicit-binding conflict
  checks, immutable executor affinity and cycle-free object lifetime. The pinned
  GNUstep runtime initializes association-class ARC policy before publication;
  all ten direct runtime clients, including first-retain/teardown coverage, pass.
- Isolation: transferable Class metadata and plain union frame values. Managed
  union arms retain their separate active-member ownership boundary.
- Structured work: owned C++ callable child/detached/group spawning. A stable
  heap context keeps the callable alive through inner frame destruction, including
  cancellation and failed startup. Native frame results have explicit typed
  adapters; lambda sendability follows capture-field types.
- Borrowed ownership: instantiated synchronous generic capture CFGs, merged
  owner/effect summaries, open/async capture diagnostics and trivial union copy
  provenance.
- Foreign interfaces: native Swift importer names, async/error conventions,
  newtypes, bridges, private-import markers and Swift attribute payloads retain
  canonical declaration/parameter contracts and capability admission. This
  preserves metadata; it does not qualify a Swift compiler/runtime.
- Key paths: plain union field traversal/writeback and explicit KVC string boxing
  through validated property-only metadata. Strict profiles diagnose the loss of
  typed root/value checking; incompatible metadata returns nil.

All 33 frontend suites, 234 native unit tests and 409 compiler regressions pass;
three compiler cases remain unsupported. The maintained tooling selection passes
335 tests and 33 subtests, with three existing skips. Normal and TSan feature
matrices each pass 168 configurations / 1,206 commands. ASan/UBSan completes the
same families across complementary 96-case and corrected 72-case runs; the initial
failed report is retained. Final owned-callable checks pass 24 configurations /
132 commands per normal/ASan/TSan mode, and all ten native runtime clients pass
in each mode. Exact C++ ivar lifecycle callback types fix sanitizer-reported UB.

Final source-free factory checks pass 32 configurations / 192 commands across
normal, eager, ASan/UBSan and TSan; the full async fixture additionally passes one
normal configuration / six commands. Native replay preserves all written deduced
return graphs, CTAD expression syntax and complete lexical member order. Foreign
contracts deduplicate identical redeclaration payloads. Source-directed consumers
reuse compiler-owned `va_list` declarations even after standard-library imports.
Historical reports are retained; broader SDK and full async-package sanitizer
delivery remain separate work.

The maintained [factory batch driver](../../scripts/objc3c_toolchain/run_linux_factory_batch.py)
checks development compiler/runtime inputs directly. It validates interface
metadata, native provider symbols, immutable package installation/relocation,
provider-source removal, consumer execution and rejected template lookups across
C++20/23, ARC/MRC and O0/O2. Eager imports, the complete async package fixture and
sanitizer instrumentation are selectable. Package identities bind the supplied
compiler/runtime digests; SDK archive delivery remains a separate checkpoint.

Remaining categories include the rest of the PR's ownership, async/capture,
isolation, dependent key-path, language/metaprogramming/interop and profile work.
Native Linux AArch64 qualification requires a native host. Resource union cleanup
is outside the current normative capability in Part 8 §8.3.6.2; nullable chains
with scalar/aggregate/nonnull key-path results need a defined result policy before
admission. None of these boundaries is counted as completed by this sprint.

The [custom-promise C++ coroutine span](linux_cxx_custom_coroutines.md), source
**2.88**, also passes its combined native/frontend and source-free checks. Its
[receipt](../support/linux_cxx_custom_coroutines_progress.json) records 32 focused
package configurations / 192 commands and three full async-fixture smoke
configurations / 18 commands, including sanitizer modes. Standard coroutine
headers are prepared before eager or source-directed AST merging; exact nested
promise forward mappings preserve template friend redeclaration chains. This is
a separate checkpoint from the 2.87 feature/runtime matrices above.

The [native declaration attribute span](linux_native_declaration_attributes.md),
source **2.89**, extends packed/aligned layout, visibility and optimizer/diagnostic
contracts through template owners and specializations, including generic foreign
metadata and packed coroutine values. Its
[receipt](../support/linux_native_declaration_attributes_progress.json) records
33 frontend suites, 236 native unit tests, 412 compiler regressions and 27 focused
tooling tests passing, plus 32 focused package configurations / 224 commands and
three full-fixture smoke configurations / 21 commands. Three platform compiler
cases remain unsupported. Optional-argument comparison and recursive attribute
copying are covered by the final compiler; accepted SDK 2.62 remains unchanged.

The [standard C++ coroutine span](linux_cxx_standard_coroutines.md), source **2.90**,
adds typed standard handles, suspend/no-op types and full/partial external traits
specializations, including public aliases, header-only modules and two-provider
imports. Its [receipt](../support/linux_cxx_standard_coroutines_progress.json)
records all 33 frontend suites, 236 native unit tests, 412 compiler regressions and
27 focused tooling tests passing, plus 32 focused package configurations / 224
commands and three full-fixture smoke configurations / 21 commands. Three platform
compiler cases remain unsupported. The final compiler excludes specialization
rows from ordinary module lookup and matches its current source/catalog identity.
SDK 2.62 remains unchanged.
