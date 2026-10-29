# Linux implementation sprint

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
