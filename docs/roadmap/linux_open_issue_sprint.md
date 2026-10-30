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

**19 issues remain** in the queue below. The two compiler issues, remaining
runtime/performance corrections and independent assurance/platform work remain
open. This checkpoint does not claim the full backlog is complete.

## Remaining issue queue

| Issue | Remaining scope |
| --- | --- |
| [#8300](https://github.com/doublemover/Slopjective-C/issues/8300) | Build generic maps and sets privately before publication without cloning every partial result |
| [#8301](https://github.com/doublemover/Slopjective-C/issues/8301) | Avoid whole-array payload reconstruction for every generic append/set |
| [#8303](https://github.com/doublemover/Slopjective-C/issues/8303) | Make generic-collection rollback tests actually account for destroyed candidate payloads |

| Issue | Remaining scope |
| --- | --- |
| [#8307](https://github.com/doublemover/Slopjective-C/issues/8307) | Preserve borrowed obligations carried by C++ base subobjects |
| [#8313](https://github.com/doublemover/Slopjective-C/issues/8313) | Bound aggregate interface-resolution memory and avoid retaining every unselected artifact |

| Issue | Remaining scope |
| --- | --- |
| [#8282](https://github.com/doublemover/Slopjective-C/issues/8282) | Make native sanitizer qualification fail closed against ambient options and verify detection with paired controls |
| [#8283](https://github.com/doublemover/Slopjective-C/issues/8283) | Integrate native Clang/Linux execution into truthful PR and periodic assurance gates |
| [#8284](https://github.com/doublemover/Slopjective-C/issues/8284) | Add isolated compiler and complete runtime instrumentation profiles |
| [#8285](https://github.com/doublemover/Slopjective-C/issues/8285) | Add fork-matched static analysis and checked lock contracts for the native compiler/runtime implementation |
| [#8286](https://github.com/doublemover/Slopjective-C/issues/8286) | Verify real Objective-C 3 IR across optimizer/backend stages and retain reducible miscompile evidence |
| [#8287](https://github.com/doublemover/Slopjective-C/issues/8287) | Add bounded coverage-guided fuzzing for the current Clang frontend and paired semantic interfaces |
| [#8288](https://github.com/doublemover/Slopjective-C/issues/8288) | Add independent generated semantic oracles for carrier ownership, cleanup and separate compilation |
| [#8289](https://github.com/doublemover/Slopjective-C/issues/8289) | Generate replayable task/group/actor schedules with independent lifetime and quiescence oracles |
| [#8290](https://github.com/doublemover/Slopjective-C/issues/8290) | Measure current compiler and runtime source/branch coverage with complete provenance |
| [#8291](https://github.com/doublemover/Slopjective-C/issues/8291) | Benchmark real ObjC3 semantic workloads beyond warm trivial-C command latency |
| [#8292](https://github.com/doublemover/Slopjective-C/issues/8292) | Validate preserved Swift-facing metadata with a real, version-pinned Swift consumer |

| Issue | Remaining scope |
| --- | --- |
| [#8206](https://github.com/doublemover/Slopjective-C/issues/8206) | Platform And Toolchain Expansion |
| [#8228](https://github.com/doublemover/Slopjective-C/issues/8228) | Linux x64 Support |
| [#8229](https://github.com/doublemover/Slopjective-C/issues/8229) | macOS arm64 Support |

Native macOS arm64 completion requires a suitable host. The accepted Linux
SDK remains 2.62; these source changes do not qualify a replacement archive.
Windows work and retired compatibility routes remain outside this sprint.
