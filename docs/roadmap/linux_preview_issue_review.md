# Linux preview issue review

The published [v0.1.0-preview.1 SDK](../releases/status.md) comes from
`8fe5c2fd89008d1c090a9355c11bdda01ceb9309`. The earlier
[issue inventory](../support/linux_open_issue_review.json) remains the record
of the source repairs and their original criteria. The
[qualification receipt](../releases/preview-qualification.json) records which
hosted jobs passed for the published archives. A passing release gate supplies
evidence for its executed scope. Each issue still has its own acceptance criteria.

## Resolved acceptance findings

| Issue                                                             | Evidence                                                                                                                                                                                                                                                                                                                                                            |
| ----------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [#8324](https://github.com/doublemover/Slopjective-C/issues/8324) | The direct compiler and package wrapper require ordinary diagnostic termination. The [regression controls](../../tests/tooling/test_qualification_integrity.py) reject signals, translated crashes, timeouts, unrelated errors, and zero exit. The shipped compiler and package qualification passed on both hosts.                                                 |
| [#8326](https://github.com/doublemover/Slopjective-C/issues/8326) | The installer commits activation through one generation pointer. The [installer controls](../../tests/tooling/test_toolchain_install.py) cover interruption before and after replacement, stable activation, retries, relocation, restrictive permissions, and failure after activation. The exact downloads passed fresh install rehearsals on both architectures. |

## Remaining acceptance

| Issue                                                                                                                                                                                                   | What the preview establishes                                                                                                         | Remaining decision or evidence                                                                                                                                                                |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [#8228](https://github.com/doublemover/Slopjective-C/issues/8228)                                                                                                                                       | x86-64 SDK build, packaging, clean installation, compiler, linker, debugger, and native execution on Debian 13 and Ubuntu 26.04.     | Check unsupported host ABI diagnostics separately from the supported preview path. Reconcile the umbrella's older platform and package requirements with the current Linux delivery contract. |
| [#8282](https://github.com/doublemover/Slopjective-C/issues/8282)                                                                                                                                       | The instrumented qualification and terminal gates passed on both hosts, with the actual-product checks participating in the release. | Finish the issue-level comparison of the original detector and ambient-option controls to the exact archived receipts.                                                                        |
| [#8283](https://github.com/doublemover/Slopjective-C/issues/8283)                                                                                                                                       | Native release gates, exact attempted job inventory, and elapsed times on both hosts.                                                | PR check enforcement settings and evidence for routine cadence remain independent of release qualification.                                                                                   |
| [#8285](https://github.com/doublemover/Slopjective-C/issues/8285)                                                                                                                                       | Eight fork-matched implementation-analysis partitions and their terminal gate passed on each host.                                   | Review checker and suppression policy and dependency-aware PR selection before claiming the full issue criteria.                                                                              |
| [#8287](https://github.com/doublemover/Slopjective-C/issues/8287), [#8289](https://github.com/doublemover/Slopjective-C/issues/8289), [#8290](https://github.com/doublemover/Slopjective-C/issues/8290) | The integrated frontend, native schedule, and coverage jobs passed on both hosts.                                                    | Map the original replay, model coverage, and changed-region criteria to the component receipts. A single green summary cannot establish every requested control or comparison baseline.       |
| [#8288](https://github.com/doublemover/Slopjective-C/issues/8288)                                                                                                                                       | The existing generated semantic checks and coverage owners have separately recorded inputs.                                          | Verify adaptive source-generation comparison against the matching qualified coverage build.                                                                                                   |
| [#8291](https://github.com/doublemover/Slopjective-C/issues/8291)                                                                                                                                       | Both releases recorded 39 measurement workloads and 25 import/rebuild phases with passing correctness.                               | Calibrate timing with an admitted baseline and reviewed latency, throughput and memory goals before making it a pass criterion.                                                               |
| [#8322](https://github.com/doublemover/Slopjective-C/issues/8322), [#8323](https://github.com/doublemover/Slopjective-C/issues/8323)                                                                    | Native test sources join release reuse identity and optimized Python qualification controls pass.                                    | Retain the end-to-end warmed SDK mutation witness and compare the original per-declaration metadata mutation criteria, respectively.                                                          |
| [#8206](https://github.com/doublemover/Slopjective-C/issues/8206), [#8229](https://github.com/doublemover/Slopjective-C/issues/8229)                                                                    | Linux archive scope only.                                                                                                            | Other platform decisions and host qualification belong to their own milestones.                                                                                                               |

The other compiler, runtime, and build repairs retain their per-issue status
and witnesses in the [source audit](../support/linux_open_issue_review.json).
The release verifies the actual selected test inventory. Where the issue
specifies an additional negative, configuration, optimization, or replay
variant, use that variant's receipt to close the issue.

## Release CI cost

The x86-64 qualification took 4h 54m and AArch64 took 6h 32m. The longest
instrumented consumer took 137 minutes on x86-64 (partition 9) and 196 minutes
on AArch64 (partition 5). The ordinary SDK producers took 107 and 114 minutes,
respectively. The complete job inventory and timings are in the
[qualification receipt](../releases/preview-qualification.json).

The preview also uploaded an already merged core archive of 2.41 GB on x86-64
and 2.53 GB on AArch64, then downloaded both again in the terminal jobs. The
updated [native workflow](../../.github/workflows/native-assurance.yml) merges
the original owner artifacts in the terminal job and publishes its result and
operational cost receipts. It still executes the disjoint-union and exact
archive checks on the underlying bytes. The next release will measure the
transfer savings and give a baseline for instrumented consumer scheduling.
