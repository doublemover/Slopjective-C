# Linux preview issue review

The published [v0.1.0-preview.1 SDK](../releases/status.md) comes from
`8fe5c2fd89008d1c090a9355c11bdda01ceb9309`. The earlier
[issue inventory](../support/linux_open_issue_review.json) remains the record
of the source repairs and their original criteria. The
[qualification receipt](../releases/preview-qualification.json) records which
hosted jobs passed for the published archives. A passing release gate supplies
evidence for its executed scope. Each issue still has its own acceptance criteria.

## Resolved acceptance findings

| Issue                                                             | Evidence                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| ----------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [#8324](https://github.com/doublemover/Slopjective-C/issues/8324) | The direct compiler and package wrapper require ordinary diagnostic termination. The [regression controls](../../tests/tooling/test_qualification_integrity.py) reject signals, translated crashes, timeouts, unrelated errors, and zero exit. The shipped compiler and package qualification passed on both hosts.                                                                                                                                              |
| [#8325](https://github.com/doublemover/Slopjective-C/issues/8325) | Both installed build routes rediscover headers before reuse. A fresh replay with the published x86-64 SDK exercised warm reuse, an external edit with preserved size and timestamp, a nested existence-only probe, and creation and removal of a higher-priority header. The incremental and clean builds agreed on output and interface identity. The [source-graph controls](../../tests/tooling/test_source_graph.py) also exercise in-flight lookup changes. |
| [#8326](https://github.com/doublemover/Slopjective-C/issues/8326) | The installer commits activation through one generation pointer. The [installer controls](../../tests/tooling/test_toolchain_install.py) cover interruption before and after replacement, stable activation, retries, relocation, restrictive permissions, and failure after activation. The exact downloads passed fresh install rehearsals on both architectures.                                                                                              |
| [#8313](https://github.com/doublemover/Slopjective-C/issues/8313) | The shipped resolver budgets aggregate bytes and declarations, coalesces identical copies, releases unselected bodies, and validates rereads. The native resolver and interface suites passed a focused replay using the published foundation.                                                                                                                                                                                                                   |
| [#8287](https://github.com/doublemover/Slopjective-C/issues/8287) | The published release ran three bounded campaigns with production compiler and decoder coverage on each host. Both receipts contain ten paired seeds, eight frontend seeds, and six passing controls for persistent reset, exact replay, controlled detection and reduction, and missing production feedback.                                                                                                                                                    |

## Implementation batches

The backlog review covers the 46 GitHub issues that were open at the start of
the batch. Compiler lowering and interface identity had separate owners so
AST, Sema, and CodeGen work could proceed alongside the native facade. The
two runtime batches shared the installed compiler and runtime inputs. Linux
delivery and Darwin bootstrap ran separately. Assurance and integration used
the published SDK for bounded native checks and reserve refreshed SDK builds
for the integrated changes.

The reviewed backlog now has 40 closed issues. The local source checks
below use the published SDK or a compiler and facade relinked from reviewed
objects. The refreshed Linux SDK and native Apple Silicon runs keep their own
exact-revision receipts.

| Batch                                    | Issues                            | Verified work                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| ---------------------------------------- | --------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Clang ownership and lowering             | #8306–#8309, #8314–#8317          | The AST relink preserved borrowed callback parameters and return owners through templates, canonical types, and calls. Three AST unit tests and 104 frontend commands passed. The managed record qualifiers passed 48 additional source/PCH IR and diagnostic checks and eight native executions.                                                                                                                                                                                 |
| Native semantic interfaces               | #8310–#8313, #8318–#8321          | All 15 registered suites passed, including 174 emitter and 24 import cases. Four supplemental probes verified relocated category protocols, ivar layout, accessors, nullability, and generic dependencies. The current interface version requires regenerated locks.                                                                                                                                                                                                              |
| Runtime collections                      | #8280, #8294, #8300–#8305         | The native sanitizer, lifetime, rollback, retained-storage, and scaling checks passed. Hosted x86-64 and AArch64 calibration each measured 39 standard workloads and 48 collection and text cases. The timings are advisory.                                                                                                                                                                                                                                                      |
| Runtime lifecycle and schedules          | #8289, #8293, #8295–#8299         | The fixed campaign passed 96 generated histories, 24 maintained scenarios, five semantic controls, and fresh replay. The additional split-epoch sample passed 24 sanitizer runs and a fresh 12-variant replay. All eight fixed release history digests are unchanged.                                                                                                                                                                                                             |
| Implementation assurance                 | #8282, #8284–#8288, #8290         | Both published analyzer gates replayed all 246 variants. Fresh native checks detected a dependent unchanged-TU finding after a same-mtime header edit. The generated oracle covers 50 transitions and nine ordered ownership pairs, and reproduced a sanitizer-clean reduced mismatch. The relocated transitive interface check passed eight cases. The exact hosted optimizer/reducer, analyzer pilot, source feedback, and compiler graph checks remain in progress or pending. |
| Linux installation and Darwin bootstrap  | #8206, #8228, #8229               | Linux x86-64 package admission and compatibility controls passed. The Apple Silicon C bootstrap and its independent archived Mach-O and DWARF verification passed. The repaired registrar, stateless instances, and portable services passed native Apple Silicon execution. Inheritance, fixed storage, typed integer arguments, ARC, and the isolated SDK install have local compiler and tooling evidence. The expanded native package run remains open.                       |
| Build identity, PR gate, and measurement | #8281, #8283, #8291, #8322, #8323 | CMake changes invalidate SDK admission, and both native test roots invalidate stale objects. A warm exact-input PR passed 16 selected CTests and its terminal check. The release producer now has a strict warm archive replay. The cold PR, full-size warm archive replay, and refreshed architecture SDK runs retain separate acceptance evidence.                                                                                                                              |

The published frontend and schedule artifacts can be identified after
GitHub's artifact retention expires. Each receipt identifies the published
revision as the tested source, and each ZIP matched the SHA-256 reported by
GitHub. The source audit retains the original acceptance criteria and scope
for each issue. [Source audit](../support/linux_open_issue_review.json)

| Host    | Frontend replay and coverage                                                                                                                                                   | Runtime schedules                                                                                                                                                              |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| x86-64  | [14695620467](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/artifacts/14695620467), `b66de514ee0ea3699923e72319a420d13741107d0f5f639f010e869c2fa24de5` | [14696985748](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/artifacts/14696985748), `60cec4c03ef3db4b97841efa0690178a61def57bb82823934c1523093cb2b59f` |
| AArch64 | [14694191758](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/artifacts/14694191758), `0e547852d5658d94974924559b25d975904d133dea2df7f8955804f2a54b83e7` | [14697953470](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/artifacts/14697953470), `857b16b8e3d1c8813a988063c7aead2b80f113554392098f907823125b2731f8` |

For #8281, the sealed x86-64 release was admitted against its unchanged
foundation configuration within two seconds. Changing `CLANGD_TIDY_CHECKS`
preserved file length and modification time, changed the build configuration
digest from `1fe5e2a6` to `601631db`, and denied reuse of that release. Two
separate CMake runs against the digest-verified upstream source recorded the
owned cache trace and the effective setting changing from `OFF` to `ON`.
Restoring the file restored the original admission. This was a configure-only
comparison. Full LLVM rebuild and warmed archive publication remain separate
qualification steps. An unchanged patched tree can now refresh its source seal
when the configuration alone changes before a new build. A missing or
inconsistent configuration seal fails admission. Configuration changes after
build start still invalidate the result.

The local native PR suite reused the published foundation. All ten
runtime CTests and 104 of 105 frontend CTests passed. The full run exposed a
SPI test with an obsolete capability list. The corrected test passed using
the current capability catalog in a separate native replay. The tooling-only
selection retained the full CTest inventory and passed all six required
frontend witnesses in 12.38 seconds using warmed objects. That timing covers
the witness execution and evidence check on an existing configured build.

A separate tooling-only checkout based on the preview adoption branch passed
the complete warm PR route. It readmitted the sealed preview foundation,
compiled and ran ten runtime and six frontend CTests, and retained 68,604 bytes
of native evidence. The terminal verifier independently confirmed every test
and source identity in the retained proof. The native execution took 133.84
seconds with a fresh consumer build. The tested fixture commit was
`05c97fcbf7a765b9842760d9fd94c3196b9f9809`.

The hosted cold PR passed at merge commit
`422e9d875786d9556ecea2fe14f4fe3e6bd35344`. Native execution took 94 minutes,
including 56.2 minutes of foundation work. The terminal verified 1,168
foundation results, 105 frontend CTests, and 10 runtime CTests. An independent
replay checked the archived receipts against the exact merge commit. The
reported ZIP SHA-256 is
`52d5303175f331aaa9761279071965d7f433485656105128e661f11ac6c67c7e`.
[Cold PR run and retained evidence](https://github.com/doublemover/Slopjective-C/actions/runs/45289344500)

The adaptive dispatcher scopes its history to recent runs on the default
branch and delegates to the canonical native qualification. A read-only check
against the live GitHub history found the next run was still subject to the
existing cadence. The default branch has yet to run the revised dispatcher.

The optional source-feedback run qualified its covered Clang on 268 owned
sources, then exhausted the paired experiment's first strategy budget. Its
full profile merge and export took 109 and 179 seconds. The revised comparison
runs six trials per strategy and reports actual wall and CPU time. It uses the
hosted job's limit. Each case pools its own binary profiles before indexing.
A native LLVM 23.1.5 replay reduced three raw profiles to one and
preserved the exact entry points, branch counts, and regions. The new
release-sized comparison awaits hosted verification.
[Retained source-feedback timings](https://github.com/doublemover/Slopjective-C/actions/runs/45289388519)

The refreshed AArch64 producer measured 268 patch-owned Clang files.
The retained report lists 234 executed files, five mapped files with zero hits,
and 29 files without emitted mappings. Independent replay checked the SHA-256
of the plan, producer, and all eight first-party partition archives and verified
the disjoint union of 74 suites at `af41a499e`. A subsequent x86-64 coverage
run of the standalone text test linked the production runtime archive and
measured both arms of the text bounds check. The updated coverage catalogue
contains 75 suites. [AArch64 release run](https://github.com/doublemover/Slopjective-C/actions/runs/45289389357) · [Source coverage policy](linux_source_coverage.md)

The repaired Apple Silicon registrar passed at
`ebb30539dd68d63f18e1a0f41e9f724d674028ea`. The native report records
39 bootstrap commands and 60 registration commands. The preflight linked
and ran with Xcode 16.4 libc++ and SDK 15.5. Static and dynamic registration,
stateless instances, ordered map operations, and text scalar boundaries
passed natively. Replaying the retained archive verified the command logs,
Mach-O bytes, and source hashes. Its ZIP SHA-256 is
`7f46ec3d0f861484837947bae70b78611b949de93e3d94aa67a7210b1ca6229f`.
[Native registrar receipt](https://github.com/doublemover/Slopjective-C/actions/runs/45292115173/artifacts/14876712288)

The expanded candidate adds stateless inheritance, fixed root storage,
one signed integer argument, and an owned ARC lifetime runtime. The native
Apple Silicon run qualified all of them and replayed collection, text, ARC,
ASan, and UBSan consumers from the relocated experimental SDK. The
independent terminal passed at `4a7e25d76` and recorded SDK seal
`ecd5e4f0b8396dd23373f37f0012f5d70a86facb2c38a993d9af8b1e32bd3e88`.
The separate managed replay installed that exact package, ran the installed ARC
consumer, reused the verified generation, and uninstalled it on native macOS
15.8.2 arm64. The hard-cutover update requires a second admitted package.
[Expanded native SDK qualification](https://github.com/doublemover/Slopjective-C/actions/runs/45299864475) · [Managed lifecycle replay](https://github.com/doublemover/Slopjective-C/actions/runs/45305498679)

## Release CI cost

The x86-64 qualification took 4h 54m and AArch64 took 6h 32m. The longest
instrumented consumer took 137 minutes on x86-64 (partition 9) and 196 minutes
on AArch64 (partition 5). The consumer execution step accounts for roughly 134
and 194 minutes respectively. The ordinary SDK producers took 107 and 114
minutes. The complete job inventory and timings are in the
[qualification receipt](../releases/preview-qualification.json).

The preview also uploaded an already merged core archive of 2.41 GB on x86-64
and 2.53 GB on AArch64, then downloaded both again in the terminal jobs. The
updated [native workflow](../../.github/workflows/native-assurance.yml) merges
the original owner artifacts in the terminal job and publishes its result and
operational cost receipts. It still executes the disjoint-union and exact
archive checks on the underlying bytes. The next release will measure the
transfer savings. The revised instrumented schedule distributes source and
PCH invocations across the ten jobs while covering all 480 maintained feature
cases. It seeds 17 profile builds across those jobs, down from 22. The
[runtime matrix](linux_sanitizer_product_matrix.md) records the archived
measurements and the per-job estimates.
