# Linux open-issue implementation sprint

The 2026-10-30 repository inventory contains **51 open issues**. The
[source-owned inventory](../support/linux_open_issue_review.json) records every
issue, its category, body digest and implementation status. This sprint keeps
compiler correctness, runtime correctness and assurance proposals distinct;
passing tooling checks alone does not complete a native implementation issue.

## Qualification and runtime corrections

- #8280: CTest inventory owns assertion policy, with forced-include compilation
  guards, an expected-failure control and always-active map-count operations.
- #8281: exact foundation CMake bytes join seals, build IDs and SDK reuse keys;
  compiled stamps and driver versions bind the configuration digest; build
  reports verify the effective assertion/RTTI/EH/project/target configuration.
- #8322: native frontend/runtime test sources join release qualification identity
  without invalidating the LLVM foundation for test-only changes.
- #8323: all 64 optimization-removable Python qualification assertions become
  explicit errors; optimized-interpreter controls defend the gates.
- #8324: ordinary negative compiler cases require exit 1. Source-build wrappers
  retain their inner compiler status so a diagnostic followed by a crash cannot
  satisfy rejection qualification.
- #8325: fresh preprocessing checks tokens and header dependencies before reuse,
  including selected interface replays, then repeats discovery before publication.
  Admitted semantic-module names bypass only Clang's native module loader during
  these probes; unregistered native imports retain their ordinary diagnostics.
- #8326: the stable activation reader resolves `current` once into a concrete
  generation. Prepare the reader before the sole atomic pointer commit; existing
  shells remain pinned. Failure and process-termination controls cover activation.
- #8293: last executor release from a runtime callback detaches worker handles;
  worker-owned leases preserve queue draining without cross-worker join cycles.
- #8294: map replacement presence is independent of the value address, and invalid
  nonzero storage is rejected before payload callbacks or snapshot publication.
- #8297: callable-produced frames undergo admission before wrapper construction;
  malformed producers cannot launder their ABI through an admitted outer frame.
- #8298: result extraction holds an operation-local task lease through user copy
  construction, error propagation and exceptions.
- #8305: failed unpublished registry insertion rolls back its new token while
  retaining published tombstones and destroying payloads outside the lock.

Issue #8279 was already fixed on this branch; its contributor/onboarding repair stays
part of PR #8278. The twelve new fixes pass source validation. The inventory retains all earlier
failed/interrupted attempts and the exact final report digests.

## Compiler contract corrections

Commit `0f9632db9` addresses fourteen additional findings:

- #8306 and #8314: explicit Block casts and implicit callable conversions retain
  directional borrowed/noescape contracts and native consumed/result diagnostics.
- #8316: GNU statement-expression results retain value, ownership, effect and
  referent facts without taint from discarded expressions.
- #8315: dependent if-let/if-var lists revalidate Boolean conditions and recreate
  full-expression cleanup when instantiated.
- #8308: ordinary and async autorelease pools use the common compound-body guard
  continuation and source-cleanup lowering.
- #8309 and #8317: managed C record helpers skip unnamed bit-fields, specialize
  on actual source/destination alignment and preserve array-element alignment.
  Underaligned strong cleanup avoids a runtime barrier that requires aligned
  pointer storage, including at O0.
- #8310: interface admission and replay bind primitive widths/alignment, signed
  char/wchar, enum and record packing, target features/ABI and Clang ABI mode.
- #8311 and #8312: C/Objective-C TLS and nested pointer nullability participate
  in semantic identity before canonical type erasure.
- #8318–#8321: category protocols, ivar bitfield width, property selectors and
  required/optional state, generic variance/bounds and specialized superclasses
  retain identity, printing and dependency admission.

The latest batch passes **36 frontend suites**, **239 native unit tests**,
**415 compiler regressions** and **144 focused tooling tests**. Three compiler
cases remain unsupported. Normal guard/pool/record and ASan/UBSan checks each
pass **24 configurations / 60 commands**. Async source/PCH checks pass
**48 configurations / 336 commands**. Source-free packages pass **17
configurations / 119 commands**, including eager and source-directed imports.
The paired ABI import tests reject mismatched consumers before declaration
import; private category protocols and generic dependencies also fail admission.

Packed runtime witnesses cover C managed records. C++ nontrivial record objects
retain native alignment requirements. The accepted SDK archive is unchanged.

## Runtime lifetime and collection corrections

Commits `334c61b80`, `a2dbf6edc` and `039c2f16f` address five more issues:

- #8295: generation witnesses scope retired actor bindings to one live object
  lifetime; delayed cleanup cannot erase a new object's retirement at the same
  address. GNUstep publishes association classes and ARC flags atomically against
  concurrent external retain/release operations.
- #8296: task groups reject depth 65 before allocation or parent mutation; final
  release drains an intrusive queue instead of recursively deleting descendants.
- #8299: failed wakes transfer an existing task lease into a thread-local
  trampoline, including executor saturation and closed-actor cancellation.
- #8302: one strict UTF-8 pass establishes immutable text boundaries. Concatenation,
  slicing and builder operations reuse validated offsets, reserve transactionally
  and copy each output payload once.
- #8304: concrete i32 maps/sets use a private hash index while retaining insertion
  order, transactional insertion and no-op iterator validity.

Focused witnesses pass **30 configurations / 60 commands** across O0/O2 normal,
ASan/UBSan and TSan builds. All owned service translation units and the selected
GNUstep dependency are instrumented for sanitizer witnesses. Actor cases cover
ARC/MRC, deterministic address reuse, delayed cleanup, concurrent bind/retain and
allocation failures. Group/wake cases exercise a 512 KiB stack and 4,096 tasks.
Text and index tests defend operation counts, fault rollback and independent
contents/order oracles. Nine semantic mutants are rejected by the real regressions.
The runtime implementation checkpoint also passes all **36 frontend suites**.
These checks do not instrument the host compiler or qualify a new SDK archive.

## Generic collection construction and ownership

Commits `a9ce2eae5` and `66ca65643` address #8300, #8301 and #8303:

- Map/set constructors own one private construction snapshot and publish once.
  New keys are copied once; each input map value is copied once, with duplicate
  replacement preserving the first key and insertion slot. Strict hash-coherence
  checking still compares earlier keys; total construction is not claimed linear.
- Arrays retain immutable 32-element chunks behind a persistent three-level
  index. Append/set copy at most 32 payloads, three metadata nodes and 192
  shared-pointer slots. Ordered erase still copies shifted contents.
- A shared logical-instance ledger separates caller inputs, initialized
  candidates, published versions and copied-out values. It checks cleanup before
  readback, including zero-sized logical resources and reused storage addresses.
  Existing handwritten rollback counters also check before output destruction.

The current combined build passes **37 frontend suites**. The three collection
CTests share one registration owner and pass **18 executions across six profiles**:
Debug and RelWithDebInfo, each normal, ASan/UBSan and TSan. The profiles compile
all seventeen service translation units. Eight paired semantic controls reject
missing/duplicate destruction, destruction of failed destinations and restored
whole-array/partial-map reconstruction. Exact work bounds cover 32, 128, 512 and
2,048 elements. Fault sweeps cover ordinary/aligned allocations and every
callback position, including publication and mutation-metadata failures.

Gated reads overlap mutation and final public release through get, view and
array/map iterators. Retained versions preserve their logical instances until
read completion; losing outputs are destroyed once. Mutation-winning callback
reentry and post-commit destructor reentry retain their distinct outcomes.
Zero-sized/over-aligned values, capacity/overflow boundaries, duplicate order,
chunk boundaries and remove-to-empty pass independent contents/lifetime checks.
These checks do not measure uninstrumented speedup or instrument the host compiler.

## Strict sanitizer policy and canonical runtime profiles

Commits `9d349791f` and `ddc8bb655` address #8282. An explicit environment
allowlist replaces ambient sanitizer options, suppressions, loader overrides and
symbolizers. Paired ASan, UBSan, LSan and TSan controls require the expected class,
source site and exit disposition; corrected programs must remain clean. Control
identity binds compiler/linker/symbolizer bytes, flags, resources, loaded ELF
libraries and effective environment. Changed inputs or receipts invalidate reuse.

The current feature and factory runners pass **40 configurations / 268 project
commands** under both sanitizer modes despite deliberately weakened ambient
options. Dedicated native async, coroutine, structured and C-frame lanes pass
**16, 12, 4 and 40 configurations**, respectively. Installed ARC/MRC projections
pass **12 configurations / 72 commands** against accepted SDK 2.62. The installed
lane caught an optimized-away ASan access; making the heap access volatile
restored O2 detection. C-bridge inputs now resolve before leaving the checkout.

Dedicated lanes build the canonical runtime CMake target with its per-file
Objective-C++ settings and matching pinned GNUstep variant. File API sources
must match compilation commands; every owned object must expose its sanitizer
instrumentation, and each archive member must match that object's exact bytes.
Three executable negative controls reject disabled instrumentation, an omitted
compile command and a substituted ordinary object. Eighteen policy tests also
reject weakened options, unapproved suppressions, missing flags, wrong resources,
non-detector failures and changed cache inputs/receipts.

The receipts preserve checkpoint boundaries. Runtime object instrumentation is
implemented; host compiler/frontend implementation instrumentation remains part
of #8284. No compiler stage, SDK archive or accepted installation was replaced.

## Earlier validation checkpoint

Commit `4c1754c20` passes **36 frontend suites**, **239 native unit tests**,
**413 compiler regressions** and **180 focused tooling tests**. Three compiler
cases remain unsupported. Assertion-policy controls exercise Debug, Release and
RelWithDebInfo. Deliberately wrong map counts and omitted token rollback fail
through the actual native regressions.

The three changed-runtime witnesses pass O0/O2 under ASan/UBSan and separate
TSan: **12 configurations / 24 compile-and-execute commands**. These are bounded
runtime-owner checks, not host-compiler or complete dependency instrumentation.
The five-case sealed local source-graph witness proves cold/warm reuse and
external-header creation/removal invalidation. Its SDK-shaped directory is an
integration fixture, not a qualified release archive.

**13 issues remain** in the queue below. Compiler implementation instrumentation
is [underway](linux_compiler_instrumentation.md); the independent assurance and
platform lanes remain open.

## Inherited borrowed storage

Commit `d3fde4069` addresses #8307. Both AST predicates traverse complete C++
base definitions; stored-value classification excludes callable signatures.
Physical base paths distinguish repeated nonvirtual subobjects and share virtual
base state. Base adjustments, references, aggregate initialization, structural
copies and callable paths preserve the corresponding loans. Opaque construction
and paths beyond the depth bound retain conservative aggregate obligations.

The current compiler passes **416 regressions** and **240 native unit tests**;
three compiler cases remain unsupported. Native AST tests exercise both
predicates, including private bases, templates and Optional/Result payloads.
Mandatory diagnostics cover return/global/unknown-call escape, unrelated loans
that force CFG analysis, deep/nested/array storage and independent repeated bases.
Safe clears, shared virtual writes, owned bases and callable metadata are paired
positive controls. Source/PCH coverage includes **C++20/C++23 × ARC/MRC**; the
complete PCH matrix passes **four configurations / eight commands**. The earlier
compiler misses thirteen required diagnostics in the regression control.

The borrowed-pointer frontend suite also passes **four source-free producers and
32 consumer cases**, including opaque factories and inherited template storage.
All **37 frontend suites** pass. Accepted SDK 2.62 remains unchanged.

## Remaining issue queue

| Issue | Remaining scope |
| --- | --- |


| [#8283](https://github.com/doublemover/Slopjective-C/issues/8283) | Native PR/periodic execution, revision-bound receipts and fail-closed aggregate implemented; live CI/cost and administrator acceptance pending ([details](linux_native_assurance.md)) |
| [#8284](https://github.com/doublemover/Slopjective-C/issues/8284) | Add isolated compiler and complete runtime instrumentation profiles |
| [#8285](https://github.com/doublemover/Slopjective-C/issues/8285) | Add fork-matched static analysis and checked lock contracts for the native compiler/runtime implementation |
| [#8286](https://github.com/doublemover/Slopjective-C/issues/8286) | Verify real Objective-C 3 IR across optimizer/backend stages and retain reducible miscompile evidence |
| [#8287](https://github.com/doublemover/Slopjective-C/issues/8287) | Add bounded coverage-guided fuzzing for the current Clang frontend and paired semantic interfaces |
| [#8288](https://github.com/doublemover/Slopjective-C/issues/8288) | Add independent generated semantic oracles for carrier ownership, cleanup and separate compilation |
| [#8289](https://github.com/doublemover/Slopjective-C/issues/8289) | Generate replayable task/group/actor schedules with independent lifetime and quiescence oracles |
| [#8290](https://github.com/doublemover/Slopjective-C/issues/8290) | Measure current compiler and runtime source/branch coverage with complete provenance |
| [#8291](https://github.com/doublemover/Slopjective-C/issues/8291) | Benchmark real ObjC3 semantic workloads beyond warm trivial-C command latency |
| [#8292](https://github.com/doublemover/Slopjective-C/issues/8292) | Validate preserved Swift-facing metadata with a real, version-pinned Swift consumer |

| [#8206](https://github.com/doublemover/Slopjective-C/issues/8206) | Platform And Toolchain Expansion |
| [#8228](https://github.com/doublemover/Slopjective-C/issues/8228) | Linux x64 Support |
| [#8229](https://github.com/doublemover/Slopjective-C/issues/8229) | macOS arm64 Support |

Native macOS arm64 completion requires a suitable host. The accepted Linux
SDK remains 2.62; these source changes do not qualify a replacement archive.
Windows work and retired compatibility routes remain outside this sprint.

## Interface import resource policy

Commit `32e5dcdf6` addresses #8313 and passes the interface contract, import and
module-resolution suites, including exact-limit and overflow regressions.

The resolver validates every discovered artifact pair, then releases its source,
JSON and decoded declarations before indexing the next pair. Its compact index
retains canonical identity, two digests and paths. Identical pairs coalesce;
conflicting identities or source digests still fail admission. Canonical path
ordering makes duplicate selection independent of search-root and directory
iteration order. Selected pairs are reread and both digests are checked before
loading their dependency closure. Direct-file imports apply the same selected
input, graph and parser-work budgets.

| Resource | Default limit |
| --- | --- |
| Each source or sidecar artifact | 32 MiB |
| Directory entries scanned | 50,000 |
| Artifact bytes read, including selected rereads | 512 MiB |
| Candidate scan-path strings | 16 MiB |
| Compact index strings | 16 MiB |
| Unique indexed modules | 16,384 |
| Decoded declarations, including repeated decoding | 250,000 |
| Selected artifact bytes | 64 MiB |
| Selected declarations / modules | 100,000 / 512 |
| Selected dependency edges / longest path | 8,192 / 128 |
| Parser input and retained virtual mapping bytes | 128 MiB |
| Parser declaration work | 500,000 |

`InterfaceImportTarget::resource_limits` provides explicit policy overrides and
small deterministic test limits. `O3IFC182` reports an aggregate resource limit;
existing per-file oversize and malformed-artifact diagnostics remain intact.
Charges use checked subtraction before addition, including at `UINT64_MAX`.
Readers allocate only the admitted file length and reject growth while reading.
The graph walk is iterative and computes the longest dependency path even when
shared dependencies were already visited from another root.

These are admitted-input and work limits, not an exact RSS ceiling for Clang's
internal ASTs. Parser accounting includes repeated dependency mappings/proofs,
replayed declaration work and pinned runtime mappings. Mappings remain alive as
long as the ASTs that borrow their strings. Duplicate direct-file requests still
consume read-work and byte budgets before module coalescing.
