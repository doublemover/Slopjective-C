# Linux open-issue implementation sprint

The 2026-10-29 repository inventory contains **51 open issues**. The
[source-owned inventory](../support/linux_open_issue_review.json) records every
issue, its category, body digest and implementation status. This sprint keeps
compiler correctness, runtime correctness and assurance proposals distinct;
passing tooling checks alone does not complete a native implementation issue.

## Current implementation batch

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

## Validation checkpoint

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

**38 issues remain** in the queue below. Platform/assurance work and the compiler
correctness defects are still open; this checkpoint does not claim the full
backlog is complete.

## Remaining issue queue

| Issue | Remaining scope |
| --- | --- |
| [#8295](https://github.com/doublemover/Slopjective-C/issues/8295) | Scope retired actor-object bindings to object lifetime, not a reusable address |
| [#8296](https://github.com/doublemover/Slopjective-C/issues/8296) | Bound task-group depth or traverse and reclaim group trees without recursive native stack |
| [#8299](https://github.com/doublemover/Slopjective-C/issues/8299) | Trampoline failed task wakes instead of recursively cancelling dependency chains |
| [#8300](https://github.com/doublemover/Slopjective-C/issues/8300) | Build generic maps and sets privately before publication without cloning every partial result |
| [#8301](https://github.com/doublemover/Slopjective-C/issues/8301) | Avoid whole-array payload reconstruction for every generic append/set |
| [#8302](https://github.com/doublemover/Slopjective-C/issues/8302) | Centralize validated text construction before publication and eliminate redundant UTF-8 copies |
| [#8303](https://github.com/doublemover/Slopjective-C/issues/8303) | Make generic-collection rollback tests actually account for destroyed candidate payloads |
| [#8304](https://github.com/doublemover/Slopjective-C/issues/8304) | Index concrete i32 map/set membership instead of scanning every prior element |

| Issue | Remaining scope |
| --- | --- |
| [#8306](https://github.com/doublemover/Slopjective-C/issues/8306) | Reject Block casts that erase or fabricate callable ownership contracts |
| [#8307](https://github.com/doublemover/Slopjective-C/issues/8307) | Preserve borrowed obligations carried by C++ base subobjects |
| [#8308](https://github.com/doublemover/Slopjective-C/issues/8308) | Preserve guard continuations and source cleanup ownership inside autorelease pools |
| [#8309](https://github.com/doublemover/Slopjective-C/issues/8309) | Skip zero-width bit-fields before requesting storage in managed C record helpers |
| [#8310](https://github.com/doublemover/Slopjective-C/issues/8310) | Bind effective primitive/target ABI settings into semantic-interface identity and replay |
| [#8311](https://github.com/doublemover/Slopjective-C/issues/8311) | Include C and Objective-C thread-storage class in semantic-interface fingerprints |
| [#8312](https://github.com/doublemover/Slopjective-C/issues/8312) | Preserve pointer nullability in semantic identity before canonical type erasure |
| [#8313](https://github.com/doublemover/Slopjective-C/issues/8313) | Bound aggregate interface-resolution memory and avoid retaining every unselected artifact |
| [#8314](https://github.com/doublemover/Slopjective-C/issues/8314) | Reject implicit callable conversions that fabricate borrowed permissions or erase borrowed results |
| [#8315](https://github.com/doublemover/Slopjective-C/issues/8315) | Revalidate dependent Boolean conditions when instantiating if-let/if-var |
| [#8316](https://github.com/doublemover/Slopjective-C/issues/8316) | Preserve borrowed facts through GNU statement-expression results |
| [#8317](https://github.com/doublemover/Slopjective-C/issues/8317) | Preserve actual subobject alignment in managed C-record helpers, or reject underaligned storage |
| [#8318](https://github.com/doublemover/Slopjective-C/issues/8318) | Preserve category-adopted protocols in canonical source, identity and dependency checks |
| [#8319](https://github.com/doublemover/Slopjective-C/issues/8319) | Bind Objective-C ivar bitfield shape into semantic-interface identity |
| [#8320](https://github.com/doublemover/Slopjective-C/issues/8320) | Include property accessor selectors and required/optional state in semantic identity |
| [#8321](https://github.com/doublemover/Slopjective-C/issues/8321) | Bind lightweight Objective-C generic signatures and specialized superclasses into class interface identity |

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
