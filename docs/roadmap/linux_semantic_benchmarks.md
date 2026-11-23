# Current semantic workload benchmarks

The current workload version **6** extends the original nine rows to **39**.
It adds ten public-runtime families at 32, 256 and 2,048 elements, with sixteen
rounds per sample:

- UTF-8 builder construction, exact scalar/byte counts, byte traversal, scalar
  slicing, concatenation, scalar search and destruction.
- Generic arrays with nontrivial copy/destruction callbacks, retained aliases,
  mutation and reads. Independent value checks and a balanced lifetime ledger
  reject leaks; the existing 32-element chunk contract bounds payload copying.
- Actor-affine tasks that resume twice, complete once and destroy their frames
  before group quiescence. Completion-order consumption checks every child.
  A manual executor admits batches of 256 below mailbox capacity; this measures
  sustained scheduling work rather than worker-pool throughput.
- Dependency cancellation that suspends each task on a shared pending task,
  cancels a bounded group, and verifies two resumes, one cancelled completion and
  one frame destruction. The independent dependency remains uncancelled through
  group quiescence and is disposed explicitly afterwards.
- Four private worker executors running two-resume tasks in bounded groups.
  Actual callback affinity proves all four destinations executed. Completed-task
  throughput includes executor startup, frame allocation, join, result consumption
  and worker shutdown, rather than measuring pending submissions.
- Canonical global-pool tasks with stable executor identity across sixteen
  rounds. A bounded probe proves four distinct workers execute simultaneously
  before the timed interval. Steady-state timing then includes allocation, two
  resumes, completion consumption and frame teardown. Actual callback thread
  identities must belong to that admitted pool; scheduling fairness is not
  invented as a per-sample requirement.
- ARC and MRC object allocation, retained alias creation/release, copied Block
  capture/invocation/release, final destruction and weak zeroing. Both modes
  perform the same observable work; counters describe explicit aliases and
  lifetime boundaries, rather than optimizer-dependent raw retain/release calls.
  Each iteration drains its autorelease pool and requires zero live objects.
  MRC compilation explicitly enables weak support.

The receipt inventories each row's inputs/generator, scale, compiler flags,
seed/rounds, completed-work oracle, cache state, measurement boundary and timeout.
Each new family has a paired bounded-delay control with unchanged semantic
results. The native gate requires every actual warmup and repetition, all ten
delay controls and complete lifetime counters. Calibration compares matching
workload versions and produces separate scaling curves for all thirteen phases and
families. Compiler object samples remove the previous object before execution;
timeouts and interruptions kill/reap the process session. Native regression
controls compile and execute the twenty-four value/task/ownership family/scale combinations in
ordinary and ASan/UBSan client builds. The value/task controls instrument the
standalone services as well; the additional ownership controls use the pinned
ordinary GNUstep dependency. These are distinct instrumentation scopes. Wrong-checksum,
missing-destruction, missing-callback and missing-cancellation implementations
are rejected. Per-sample task throughput and its summaries are recomputed from
native completed-task/callback counts and work intervals. Thread counts are recorded per row;
worker workloads use one caller plus four scheduling threads.

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_benchmarks \
  --sdk /path/to/candidate --extended-runtime \
  --output out/semantic-runtime-measurement --repetitions 3
```

The 2026-11-05 matched-compiler checkpoint passes all 39 frontend/runtime rows,
ten runtime delay controls, six generated-frame semantic mutants and the
25-phase source/PCH/interface/cache pilot on one sealed development SDK.
Independent SDK tests pass all fourteen generated-frame cases. The canonical
terminal verifier accepts all 64 phase summaries from a copied evidence root.
The compiler implementation carries ASan/UBSan; timed service clients and the
GNUstep dependency are ordinary builds. This is development-stage execution;
no release archive is built or qualified. Matching hosted observations and
reviewed latency, throughput and memory goals remain before timing promotion.
The [build/install cost owner](linux_semantic_build_costs.md) records work already
selected by native CI without introducing another qualification cadence.

Issue [#8291](https://github.com/doublemover/Slopjective-C/issues/8291) adds real
language/runtime work alongside the existing explicitly labeled warm command
latency probe. The first workload version measures ObjC3 Optional/Result,
guard/match/defer frontend syntax and object generation at 32, 256 and 2,048
declarations. A sustained concrete i32-map workload measures creation, insertion,
lookup, count and teardown at the same bounded sizes.

Each runtime sample checks an independent checksum and exact completed-operation
count. Its internal steady-clock interval includes allocation and teardown and
excludes process startup/output. Whole-command wall/CPU time and `wait4` RSS
remain separate: RSS is the largest individual process in that command and its
waited descendants, not aggregate process-tree memory. Raw samples and compiler
object/link preparation measurements are retained.

Both SDKs are sealed and verified before and after measurement. Compiler/runtime
loader graphs, sanitizer markers, flags, workload bytes, host/cgroup context and
SDK identities are recorded. Instrumented compiler timings are labeled; an
instrumented runtime is rejected for ordinary runtime measurement. The same
workload runs on each SDK, with one warmup and alternating SDK order. No compiler
cache launcher is used, and warm shared filesystem caches are stated explicitly.

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_benchmarks \
  --sdk /path/to/candidate --baseline-sdk /path/to/baseline \
  --output out/semantic-workload-measurement --repetitions 3
```

Add `--collection-scaling` to retain per-phase samples and timing summaries for
collections and text. Each phase checks completed work, values or byte samples,
and lifetime balance where its API exposes ownership callbacks.

- Concrete i32 maps and sets cover build, mixed hit/miss lookup, duplicate
  insertion or replacement, ordered iteration, removal and teardown at 1, 32,
  128, 512 and 2,048 entries.
- Generic maps and sets cover distinct, duplicate-heavy, and colliding keys at
  the same scales with integer or owned-string payloads. Construct and release
  have separate timings and a combined summary. Mid-input callback rejection
  checks rollback, and copy, equality, and destructor counts describe the work.
- Generic arrays cover append, repeated middle replacement, indexed reads,
  iteration and teardown for both payloads. A contiguous bulk-create route
  supplies the corresponding read and teardown measurements.
- ASCII and mixed-width UTF-8 with embedded NUL run at 1 KiB, 64 KiB, one byte
  below 1 MiB and exactly 1 MiB. The phases cover raw create, concat including
  equal operands, scalar slicing, builder append and repeated finish, teardown,
  and rejected imported scalar counts.

An independent executable linked to the same SDK archive tracks cumulative
allocation requests and requested bytes. A separate diagnostic build uses
candidate compiler and checkout service sources to measure live index requests,
array metadata copying, and text decoder and byte-copy work. Live index bytes
include the index object, nodes, and buckets, with allocator overhead outside
that total. Those independent diagnostics accompany the advisory archive timings.

A bounded same-input delay control must preserve semantic results and show its
minimum intended timing effect. Zero/partial work, incorrect checksums, absent
samples, process failure/timeout and concurrent SDK mutation fail. Distribution
summaries retain spread and absolute/relative effects. Timings remain
**advisory and inconclusive for regression gating** until runner-specific limits
and noise are calibrated; no universal slowdown allowance is invented.

The historical first selection did not measure cold builds, archive/install
work, ARC/MRC object lifetimes, source/PCH/interface cache behavior or concurrent
task throughput. Those dimensions require further workload definitions and measured
cost before expanding the existing budgeted CI/release lanes. No new schedule or
performance claim follows merely from successful measurement.

The original native CI selection used a three-repetition characterization
for compiler, runtime, stdlib, workload, foundation and runner changes and its
complete periodic/release selection. It seals only the required current compiler,
linker, resources, runtime archive/dependency and headers; it builds no archive.
That original terminal gate checked all nine phase/scale summaries, each actual warmup and
three measured samples, runtime work-count oracles and the paired delay control.
Missing samples or wrong semantic results block correctness. Timings stay
advisory, with no performance-pass claim and no automatically accepted baseline.

The original native CI protocol passed nine phase/scale rows and forty-five command
samples. A separate two-SDK experiment compares the admitted 75cfe6c21f70 and
616ca86288bb compiler checkpoints with eighty-seven retained samples. Both use
instrumented compilers and ordinary runtime archives; the result is an
uncalibrated diagnostic-build comparison, not an uninstrumented speedup claim.

Independent hosted receipts can now be pooled without converting observed noise
into an invented product limit:

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_benchmark_calibration \
  --receipt /path/to/run-a/qualification.json --label RUN_A_URL \
  --receipt /path/to/run-b/qualification.json --label RUN_B_URL \
  --output out/semantic-workload-calibration
```

The [Calibrate Linux preview workloads](../../.github/workflows/linux-preview-calibration.yml)
workflow installs the published SDK on a native x86-64 or AArch64 runner,
takes two complete observations on the same host, and retains the raw and pooled
receipts. Select the `collection_scaling` dispatch input to include the
collection and text phases in both observations. On a pull request, apply
the `calibrate preview` label to run the full calibration, including collection
and text phases, on both architectures. Each deliberate delay is measured
alongside a same-input control, in alternating order. The delay is ten
milliseconds per workload round. Ordinary samples retain separate timing and
memory observations. Calibration remains advisory until a performance budget
is reviewed.

For collection-enabled artifacts, keep each observation's raw `.log` files
beside its `qualification.json` when pooling across runs. Calibration verifies
their digests, phase results, and monotonic command order before pooling. The
optional pooled section retains per-receipt phase medians and raw timeline,
allocation, retained index, and other diagnostic samples. Compatible runs use
the same workload sources, executable bytes, loaded libraries and SDK.

Calibration rejects duplicate receipts and different host, SDK, workload,
instrumentation or repetition contexts. It recomputes every summary from raw
samples, rechecks runtime semantic work, then retains pooled and per-receipt
medians, actual deviation, per-workload RSS, scaling points and total runner and
receipt cost. The output remains explicitly advisory: measured spread does not
become a regression allowance. Promotion still requires reviewed per-workload
absolute effects and memory goals. Further workload families remain open under
\#8291.

Pass `--policy path/to/review.json` to evaluate selected workloads using
approved limits. The policy uses `objc3c.semantic-benchmark-review.v1` and
records an `owner`, a `reason`, an `expires_on` date in YYYY-MM-DD format,
and the exact measurement `context`. That context consists of the admitted
`host`, `inputs`, `workload_version` and SDK `instrumentation` values.

The `workloads` map names each reviewed workload. Each entry gives
`max_median_seconds`, `max_median_range_seconds` across independent receipts,
and `max_peak_process_rss_kib` for the largest individual process. Task
workloads can also set `min_median_task_completions_per_second`. Consistent
median misses or a process exceeding its memory limit produce a regression.
Mixed verdicts and timing spread above the reviewed limit are inconclusive.
The CLI writes the result and exits unsuccessfully for either outcome. The
report retains the exact policy hash and per-workload decisions. Existing
automatic timing reports remain advisory until reviewed limits are supplied.

Two independent local measurements of workload version 6 used development
build `llvmorg-23.1.5-392dfff97d29-718dd03879f4-0fcb354b` on a four-CPU
quota AMD EPYC 9V74 host. All 39 workloads completed with six retained
samples per workload. The 440 measured commands used 82.98 cumulative wall
seconds and 83.21 CPU seconds. The largest between-run median range was
58.03 ms and the largest individual-process RSS was 217,872 KiB. The two
qualification receipts totaled 694,664 bytes. Review-tool verification passes
185 focused Python tests covering timing, memory, throughput, review scope
and the surrounding assurance admission checks.

The labeled hosted run on `af41a499ec66bc83ca068553762bcc6e666a4eee`
passed on both Linux architectures with the published SDK. Each host admitted
two independent observations, 39 standard workloads, 48 additional collection
and text cases, and six measured repetitions per case. Both artifact digests
matched GitHub and both pooled receipts reproduced byte-for-byte from the raw
logs. [Hosted run](https://github.com/doublemover/Slopjective-C/actions/runs/45289421716)

| Host    | Job seconds | Command wall / CPU seconds | Artifact bytes | Artifact SHA-256                                                                                                                                                          |
| ------- | ----------: | -------------------------: | -------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| x86-64  |         146 |              86.25 / 76.94 |      1,141,415 | `091b0b7c568058976ae03584ca5e39f18cc9ca6ce07234ffa547a1fd7d032f89` [Archive](https://github.com/doublemover/Slopjective-C/actions/runs/45289421716/artifacts/14874250531) |
| AArch64 |         196 |            133.16 / 124.31 |      1,141,714 | `b035e172a59e25ed133c951d8abe81e5e3ac57984122c84c91f035b74edb5294` [Archive](https://github.com/doublemover/Slopjective-C/actions/runs/45289421716/artifacts/14873521687) |

Each host recorded 1,510 commands and 992 raw collection samples. Timings retain
the advisory decision. The largest between-observation median spread among
standard workloads was 64.0 ms on x86-64 and 48.8 ms on AArch64. Any timing gate
requires a reviewed choice of workloads and runner-specific latency, throughput,
memory and noise limits.

## Import and incremental-build pilot

The separate `semantic_import_benchmarks` owner generates actual ObjC3
`Optional(int)` providers and guarded consumers. Every exported function must
return its independent expected value; an actual completed-call counter and
checksum bind the whole result. Source and PCH consumers compile separately
from their provider. Interface consumers use the admitted fingerprint/package
lock and a dependency record after the provider source/header directory has
been deleted. PCH inputs remain available for Clang's ordinary validation.

Source/PCH commands use the installed Clang driver; interface commands use
the installed native facade, including its public import/serialization work.
These are whole-command phase costs, not isolated parser-only timings.
Each trial measures provider compilation, first and repeated consumer imports,
linking and native execution separately. PCH serialization is another phase.
Fresh objects prevent stale outputs from satisfying a sample. Fresh trial
directories and module caches distinguish first from repeated imports; the host
filesystem cache remains warm, so this is not a cold-filesystem claim.

The source-graph experiment measures a fresh build, unchanged incremental build
and equivalent private-leaf edit. It checks exact rebuilt/reused node sets and
an unchanged public fingerprint, and retains each build report. Incremental
builds still link and execute; their wall time is not labeled a zero-work build.
A valid but incorrect provider must produce the declared semantic failure.
Omitting the provider must fail with its unresolved exported symbol, rather than
an unrelated link error.

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_import_benchmarks \
  --sdk /path/to/candidate --source-graph --scale 32 \
  --output out/semantic-import-measurement --repetitions 3
```

The existing semantic native lane selects a **32-declaration / 25-phase** pilot
with one warmup, three repetitions and a **600-second total command budget**.
It uses the same sealed development prefix as the 39 frontend/runtime rows,
adding only the installed facade, Objective-C headers and standalone SDK verifier.
No SDK archive or new cadence is introduced. Larger 256/2,048-declaration
experiments are explicit CLI selections, pending measured cost.

Terminal verification recomputes every phase, repetition, summary, product
inventory, semantic output, reuse decision and paired provider control. It
checks raw log hashes, source bytes, SDK identity and build snapshots after
artifact relocation, without the original SDK/build tree. Reports retain
instrumentation, compiler/runtime loader graphs, per-command wall/CPU/RSS and
total command/storage cost. Timings remain advisory; a correctness failure or
incomplete required experiment fails. Local controls cover orchestration and
ordinary standard-C separate-provider/PCH mechanics. They do not qualify the
unmodified ObjC3 workload with a sealed compiler fork.

The ownership loader inventory requires the selected SDK's actual GNUstep
library. Ordinary runtime measurement rejects sanitizer/profiler markers in
either the services archive or Objective-C runtime instead of mixing diagnostic
and ordinary performance samples. Ownership fixture bytes, flags and counters
join the current versioned workload/calibration identity; earlier expanded
workload versions cannot be silently pooled with version 6. Local wrong-value,
missing weak-observation and partial-round controls exit cleanly under client
ASan/UBSan and still fail the independent oracle.

## Compiler-generated frame workloads

`task_frame_workloads.cpp` uses the production typed `objc3::task<unsigned>`
bridge and actual C++ coroutine lowering, including its aligned-allocation
contract. A move-only parameter lease remains owned by each coroutine frame
until the runtime destroys it after final suspension. It is not a manually
allocated callback frame. Two bounded families cover success and cancellation
at a queued yield, with separate first/second resume counts, typed result or
cancellation extraction, completed group results and final lease destruction.
The manual scheduler runs batches of 256 and joins before accepting timing.

The checksum observes frame arguments before suspension; successful results
must also preserve them across suspension. Completed-task throughput derives
from consumed completions, rather than claiming internal callbacks ran. Creation,
resume, destruction, result/cancellation extraction and group consumption each
have independent expected counts. Both families have paired delay controls.
Three compiler-built semantic mutants alter the checksum, omit destruction
accounting or omit completion accounting; both families must reject each mutant
after a zero native exit. Controls with an unrelated mismatch cannot qualify.

The native gate requires all six generated-frame scale rows and all six fault
controls. Calibration and terminal verification recompute the typed completion
counts and control outcomes. The older bootstrap lacks the bridge's
qualified type-identity builtin; the current matched compiler has it and passes
all six scale rows, both delays and all six mutants. Explicit SDK controls run with
`OBJC3_TEST_SEMANTIC_SDK=/path/to/sealed/sdk`; a mismatched supplied SDK fails
rather than becoming an unsupported skip.

Build and distribution costs use the [operational receipt owner](linux_semantic_build_costs.md).
It records cold/incremental/invalidation foundation state and admitted archive
installation phases from existing required execution. It performs no additional
archive qualification. Native aggregate verification recomputes those observations
from retained raw receipts/logs; performance promotion still requires measured
variance and reviewed product goals.
