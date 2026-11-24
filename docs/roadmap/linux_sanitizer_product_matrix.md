# Parallel sanitizer product qualification

The Linux release matrix qualifies thirteen logical sanitizer product owners
using one exact SDK archive and one exact instrumented compiler producer.
Runtime schedule qualification has an independent job. The explicit local
all-owner command still runs each complete product owner.

| Partition | Assigned work                                                 |
| --------- | ------------------------------------------------------------- |
| 0         | Structured concurrency, native async, and C++ task coroutines |
| 1         | ASan/UBSan O0 source feature cases                            |
| 2         | ASan/UBSan O2 source feature cases                            |
| 3         | C frame bridge and installed ownership                        |
| 4         | ASan/UBSan O0 PCH feature cases                               |
| 5         | ASan/UBSan O2 PCH feature cases                               |
| 6         | Four ASan/UBSan and TSan factory variants at O0 and O2        |
| 7         | TSan O2 source feature cases                                  |
| 8         | TSan O0 source and PCH feature cases                          |
| 9         | TSan O2 PCH feature cases                                     |

The source and PCH parts each run 60 of the original 120 cases for their
owner. All 480 maintained feature cases still run once. The ten assignments
request 17 runtime profile builds across jobs, down from 22. The colocated
factory variants use one instrumented frontend facade. Each job builds the
profiles its assigned work needs and runs its existing sanitizer admission
controls.

## Archived cost evidence

The published `v0.1.0-preview.1` used four whole feature jobs. They ran for
4,407–8,220 seconds on x86-64 and 5,928–11,787 seconds on AArch64. The six
other product jobs took 485–1,413 seconds across both hosts. [Exact job receipts](../releases/preview-qualification.json)

| Archived variant      | Hosted job (s) | Owner (s) | Source commands (s) | PCH commands (s) | Isolation commands (s) |
| --------------------- | -------------: | --------: | ------------------: | ---------------: | ---------------------: |
| x86-64 TSan O2        |          8,220 |  7,895.54 |            3,278.00 |         3,268.22 |               5,635.21 |
| AArch64 ASan/UBSan O2 |         11,787 | 11,480.65 |            5,056.94 |         5,069.90 |               8,781.58 |

Each row comes from 120 passing cases and 1,113 retained build and execution
commands in the exact artifact at `8fe5c2fd89008d1c090a9355c11bdda01ceb9309`.
The timings above sum each command under its recorded case. The archived
AArch64 ASan/UBSan O0 job lasted 8,746 seconds, so it uses the same transport
split. The unsplit TSan O0 job took 5,904 seconds on x86-64 and 5,928 seconds
on AArch64. [x86-64 TSan O2 artifact](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/artifacts/14700602616) · [AArch64 ASan/UBSan O2 artifact](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/artifacts/14703912109)

For the two measured variants, charging all of each original job's
non-command time to each new transport job gives a longest-job estimate of
4,952 seconds for x86-64 TSan O2 and 6,730 seconds for AArch64 ASan/UBSan O2.
That is about 40% and 43% less time for those individual variants. The estimate
assumes the original command costs and concurrent runners. A hosted run on the
new assignment will measure actual matrix wall time.

## Completeness and replay

The terminal job requires all ten distinct partition receipts with a common
revision, plan, sealed SDK and campaign source inventory. Each feature
transport binds its exact sanitizer, optimization, case inventory, batch
receipt and passing execution logs. The terminal records the complete
source/PCH union under the original thirteen logical owners. Missing,
repeated, failed or substituted cases and transports reject the union.

The merge keeps the seventeen exact product-evidence receipts and all six
sanitizer and optimization runtime configurations. It consolidates replay
bytes into a verified campaign-owned store under the existing 3 GiB budget.
