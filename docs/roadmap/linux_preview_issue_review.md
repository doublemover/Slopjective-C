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

The current pass closed the Linux x86-64 support issue, the sanitizer policy
issue, the runtime lifetime and collection findings, and optimized Python
qualification. The scheduler receipt contains 96 generated executions, 24
maintained scenarios, five semantic controls, and a fresh reduced replay.
It identifies the exact source revision and all runtime profiles.

| Batch                                    | Issues                            | Verified work and remaining acceptance                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| ---------------------------------------- | --------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Clang ownership and lowering             | #8306–#8309, #8314–#8317          | Added source, PCH, installed, and async witnesses for callable ownership, inherited and statement-expression borrows, pool cleanup, Boolean conditions, and record bit-fields. The guard matrix ran 24 configurations, the async matrix ran 48, and the record qualifier ran 17 executions and eight rejections. A relinked Clang object verified the new underaligned weak storage diagnostic on x86-64 and the AArch64 target. The compiler now retains borrowed callable parameters in canonical template types. Source, PCH, and interface replay passed with the relinked compiler. The full SDK build still needs to qualify the change.                                                                                                                |
| Native semantic interfaces               | #8310–#8313, #8318–#8321          | Preserved effective primitive ABI and Objective-C declaration identity, with four native suites and three focused emitter cases passing. C and Objective-C clients exercised ordinary and thread-local storage across threads, including reverse stale locks through re-exported modules. The relinked Clang AST importer preserved source and synthesized ivar slots and layout across two imports. The interface ABI profile advances to v2. Previously emitted interface locks require regeneration. Follow-up tests covered relocated category protocol adoption and SPI, imported property and nullability diagnostics, effective wchar ABI, linked neighboring ivars, and generic covariance. Native AArch64 qualification remains open.                |
| Runtime collections                      | #8280, #8294, #8300–#8305         | Ran 21 CTest executions across Debug, RelWithDebInfo, and Release, and 12 under ASan/UBSan and TSan. The actual map-count CTest rejected an incorrect count. Installed services passed deterministic lifetime, rollback, replacement, and scaling checks on x86-64. The benchmark separates elapsed time, allocation traffic, and exact requested retained index bytes across concrete and generic collections and near-capacity text.                                                                                                                                                                                                                                                                                                                        |
| Runtime lifecycle and schedules          | #8289, #8293, #8295–#8299         | The original campaign passed on both release hosts. Added a typed result mismatch control, independently replayable histories, and lifetime checks for actor retirement, executor release, reentrant result copies, and task groups. The new x86-64 campaign passed 96 generated histories, 16 maintained fixtures, and five semantic controls. The expanded owner also checks indirect executor release, an allocation-failed wake with a surviving waiter, actor binding races, and exact result destruction. AArch64 replay of the expanded owner remains open.                                                                                                                                                                                            |
| Implementation assurance                 | #8282, #8284–#8288, #8290         | Verified release sanitizer product, analysis, and coverage jobs. Added exact detector-frame checks, an owned suppression gate, coverage section admission, and branch comparison for changed compiler hunks. A live dependency scan selected unchanged dependents from 63 compile variants for a shared-header edit. The shipped x86-64 compiler passed 30 generated cases and a backend-verifier replay. The optional lanes verify the published SDK backend, reuse exact compiler objects for semantic source feedback, and admit both digest-pinned release coverage baselines. Hosted replay of the new full compiler graph remains open.                                                                                                                 |
| Linux installation and Darwin bootstrap  | #8206, #8228, #8229               | The Linux installer checks glibc and the libstdc++ versions needed by resolved SDK libraries before activation. The x86-64 release ran on Debian 13 and rejected three incompatible host configurations. The pinned Apple Silicon workflow exercises native bootstrap. Darwin promotion requires Mach-O runtime registration and native package, install, and execution qualification.                                                                                                                                                                                                                                                                                                                                                                        |
| Build identity, PR gate, and performance | #8281, #8283, #8291, #8322, #8323 | Added exact native PR admission, warmed test-object invalidation, and optimized Python metadata omission controls. A same-size, same-mtime keypath test edit triggered a real CTest failure and restoration passed. The installed optimizer-mode package qualification executed 36 native witnesses and compared 1,475 metadata mutations. Two local observations produced six samples across each of 39 workload rows. The independent PR terminal job checks the outcome, exact selection, and retained-native evidence. Known CI tooling changes can reuse a six-test cross-layer frontend set with an admitted foundation. Both native test roots now invalidate warmed objects. A reviewed performance budget and whole-archive warm replay remain open. |

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

The experimental Darwin compiler now emits an owned Mach-O registration
initializer and dispatches stateless Objective-C 3 root-class methods.
Local cross-compilation verified arm64 object format, DWARF, and the emitted
defer operation. Native Apple Silicon registration qualification and the full
Darwin runtime and SDK remain separate milestones.

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
transfer savings and give a baseline for instrumented consumer scheduling.
