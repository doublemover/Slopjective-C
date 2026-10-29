# Linux implementation sprint

This working span fronts development across the current PR backlog. Source
2.85 is the last completed checkpoint. The following implementation is pending
combined validation (native compiler and facade compilation pass); it does not establish profile conformance
or delivery in the accepted 2.62 SDK.

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

The source witnesses extend maintained native semantic/codegen/PCH fixtures and
runtime feature matrices. Broad compiler/frontend/tooling, sanitizer, source-free
package and SDK checks follow the implementation batch. Earlier reports remain
historical and must not be overwritten.

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
