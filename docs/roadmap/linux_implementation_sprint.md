# Linux implementation sprint

The [source 2.121 resource-property span](linux_resource_property_lifetimes.md)
unifies explicit C++ frame lifetimes for isolated values and property writes,
including move-only owners, active resource unions and composed arrays. Borrowed,
raw-resource, opaque-lifetime and volatile-record guards remain enforced. Native
copy/move/setter faults, partial acquisition, nil, cancellation and abandoned frames
pass fourteen instrumented configurations / 49 commands. Dedicated source/PCH
compiler controls cover all four profiles. A publication regression also repairs
untyped dependent-initializer capability scanning without suppressing Block values.

The [System 2.50 producer-transfer span](linux_block_producer_transfers.md)
adopts retained/reclaimed Block results into locals, scalar/nested members,
default initializers, assignments and single consumed direct calls. Native
ownership supplies the value without an invented caller copy; produced values
remain guarded across throwing destination evaluation. Block admission cuts over
to v3. All 66 compiler cases, 48 AST controls, six new source-free profiles,
eighteen adjacent groups, twelve instrumented native configurations / 42 commands,
four provider/consumer executions, codec/isolation and 120 decoder replays pass.

The [System 2.49 Block-mutation span](linux_block_mutation.md) separates
RHS evaluation, heap copy, destination-address evaluation and ownership transfer.
A new native unwind guard repairs retained-RHS leakage on an address exception;
failed stores preserve the old slot and borrowed loan. Nullable/null-store
proofs, MRC stable stores and automatic-owner-unwind v2 publication are aligned.
All 66 compiler cases, 45 AST controls, ten new source-free profiles, sixteen
adjacent groups, twelve instrumented native configurations / 42 commands, four
package executions, codec/isolation and eighty production decoder replays pass.

The [System 2.48 computed/nested Block span](linux_computed_block_arrays.md)
extends exact registration across nested rows, sequenced/conditional expressions,
scalar captures, stable native member/index reads, strong local bindings and
consumed arguments. Source-free admission cuts over to subobject v5. All
66 compiler cases, 44 AST controls, twelve instrumented native configurations /
42 commands, ten new source-free profiles, fourteen adjacent groups, codec/
isolation and forty decoder replays pass. Standalone value bodies require Block
expression v2 admission, including C17 without constructors or literal Blocks.

The [System 2.47 Block-array span](linux_block_array_members.md) registers
completed strong member-array elements before later copies, unwinds exact
prefixes, preserves weak/null/filler behavior and emits counted release groups.
Publication cuts over to native-subobject v4. All 66 affected compiler cases,
four new source-free profiles, thirteen adjacent groups, 40 AST controls and
twelve instrumented native configurations / 42 commands pass, together with
codec/isolation controls and forty production decoder replays. Nullable-receiver
proofs include hidden copy failure before constructor-body refinement.

The [frontend 2.120 specialization span](linux_incomplete_specializations.md)
adds opaque class partial/explicit specializations, external/member variable
partials and native standard-library customization declarations. Namespace,
ordinary member and dependent member graphs preserve selection through replay
and later completion. Admission cuts over to incomplete-declaration v2. Four new source-free profiles,
nine adjacent groups, codec/isolation, forty decoder replays and four instrumented
selection/completion witnesses pass.

The [frontend 2.119 declaration span](linux_incomplete_template_declarations.md)
publishes primary forward class templates, external variable templates and nested
opaque heads, preserving defaults, constraints, ABI ownership and later
completion. Production readers, emitters and the fuzz runner share one capability
catalog, repairing C-inline import admission. Two new source-free profiles, eight
adjacent groups, codec/isolation units, forty production decoder replays and two
instrumented package-declaration witnesses pass. Installed package consolidation
follows the remaining implementation sprint.

The [System 2.46 virtual-subobject span](linux_virtual_subobjects.md)
models complete/base-object construction and destruction as separate correlated
paths, including virtual diamonds, delegation, function-try epilogues, abstract
constructor initializer exclusion and anonymous-union active-member cleanup.
Nullable receiver proofs use the same alternatives. ARC Block members separate
implicit copy failure from slot release. Ordinary unnamed records publish stable
owner/path identities. Source-free publication cuts over to subobject v3 and
unnamed-record v2. All 65 affected compiler cases, six new source-free profiles, fourteen adjacent
frontend groups, codec/isolation controls, 39 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass. Two final source/PCH fixture matrices
also pass after adding the Block-array boundary control.

The [System 2.45 delegation/function-try span](linux_delegation_function_try.md)
adds whole-object cleanup after successful delegation, function-try initializer
and destructor epilogue coverage, implicit rethrow/suppression and noexcept
handler boundaries. Constructor source replay preserves try/initializer ordering.
All 62 affected compiler cases, four new source-free profiles, nine adjacent
frontend groups, codec/isolation controls, 33 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.

The [System 2.44 subobject span](linux_native_subobject_cleanup.md)
adds constructor completed-prefix unwind, destructor member/base cleanup on
return/exception paths, primitive ARC subobjects and native noexcept termination
boundaries, with owning-declaration-aware publication. All 61 affected compiler cases, four new source-free profiles, eight adjacent
frontend groups, codec/isolation controls, 29 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.
Wider subobject forms retain separate proof gates.

The [System 2.43 array-group span](linux_array_construction_groups.md)
adds bounded direct throwing array construction with native prefix cleanup and
matched free, automatic throwing array destruction and secondary termination,
and empty-array constructor/destructor bypass. All 60 affected compiler cases, four new source-free profiles, seven adjacent
frontend groups, codec/isolation controls, 26 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.

The [System 2.42 ordinary-owner span](linux_automatic_owner_unwind.md)
activates complete local unwind for every native handler, admits nonthrowing
implicit ARC capture copies, registers heap-copy failure and repairs discovery of
raw inline-storage closure loans. Catch-all and standalone handler printing is
repaired. All 59 affected compiler cases, six new source-free profiles, seven
adjacent frontend groups, codec/isolation controls, 24 AST controls and twelve
instrumented native configurations / 42 commands pass. Broader exceptional
ownership remains in the ledger.

The [System 2.41 allocation/array span](linux_array_and_allocation_cleanup.md)
adds matched scalar deallocation after constructor failure, nontrivial array
unwind/termination, empty-array behavior and bounded repeatable array construction.
Borrowed deletion is diagnosed. Twenty-two AST controls, twenty-four focused
compiler commands pass. All 79 affected compiler cases, eight allocation/disposal
source-free profiles, six adjacent groups, codec/isolation, 22 AST controls and
twelve instrumented native configurations / 42 commands pass.

The [System 2.40 disposal span](linux_native_disposal.md) adds mandatory scalar
deallocation after destructor failure and exact null-operand branching, including
noreturn destructors. Twelve source/PCH commands, all twenty AST controls and
twelve instrumented native configurations pass. All 78 affected compiler cases,
four source-free profiles, five adjacent groups and codec/isolation controls pass.

The [System 2.39 initialized-owner span](linux_initialized_owner_prefixes.md)
tracks completed ARC record/array/capture prefixes, failure unwind and transfer
to complete storage. Owner/unowned snapshots preserve actual evaluation order;
local owning lambdas retain their native capture order and elided temporary
lifetime. All 77 affected compiler cases, six source-free profiles, five adjacent
groups, codec/isolation, 19 AST controls and twelve instrumented native
configurations / 42 commands pass.

The [System 2.38 finalizer span](linux_native_finally.md) models `@finally`
registration around try/catch, cleanup order, native unwind and replacement
return/throw/jump transfers. It also repairs GNUstep C++ rethrow ownership and
adds source-free capability admission. All 76 affected compiler cases, six
source-free profiles, four adjacent frontend groups, codec/isolation and 19 AST
controls pass; both twelve-configuration native scope and frame-exception
matrices pass under ASan/UBSan.

The [System 2.37 managed-scope span](linux_managed_runtime_scopes.md) models
native pool queues, normal-only synchronous pool exits, async drains and exceptional
cleanup, plus exact synchronization snapshots and unlock ordering. Proven ready
awaits remain usable inside monitors; actual suspension is diagnosed, including
ordinary C++ task coroutines. Source-free replay carries exact semantic admission.
The optional native feature includes observed runtime operations and exact destruction.

The [System 2.36 receiver-result span](linux_borrowed_receiver_results.md)
implements native getter pointer/reference loans, self/owner checks, messages,
properties, superclass and stable Class bindings, with source-free identity and
capability admission. Async and isolated crossings reject lifetime erasure
independently of raw-pointer Sendable admission. All forty-two affected compiler
cases, six source-free profiles and twelve instrumented native configurations pass;
five adjacent frontend groups and codec/isolation controls also pass.

The [System 2.35 interior-storage span](linux_borrowed_interior_storage.md) adds
inline ivar/array/record loans and separate C++ reference-address lifetimes,
including copy/capture propagation, builtin address-taking and source-free
capability admission. All thirty-nine borrowed compiler cases, six source-free
profiles, three adjacent frontend groups, codec/isolation units and twelve
instrumented native configurations pass. The wider exceptional cleanup and ownership/profile backlog remains open.


The [System 2.34 handler span](linux_borrowed_catch_bindings.md) implements catch
owner initialization, ARC binding cleanup, rethrow lifetime regions and opaque
borrowed payload obligations. Eighteen compiler commands, sixty-eight affected
compiler cases, nineteen AST controls and twelve instrumented native configurations
pass. The complete shared visitor rebuild passes nineteen AST controls, all six
source-free profiles and three adjacent frontend groups; the full sixty-eight
compiler case consolidation also passes after rebuilding.


The [Concurrency 2.118 span](linux_native_frame_exceptions.md) implements typed
nontrivial/move-only C++ property writes and owned native exception transport,
including exact construction order, nil/partial/cancellation cleanup and bounded
shared record graphs. Source-free replay preserves its SDK/EH contracts under
`objc3.async-entry-abi.v5`. Thirty new compiler commands, ninety-two affected
compiler cases, four new frontend profiles, codec/isolation controls and fourteen
instrumented native configurations pass. Resource/effectful accessors and wider
ownership/profile work remain; matched SDK delivery follows consolidation.

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
