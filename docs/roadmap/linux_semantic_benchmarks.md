# Current semantic workload benchmarks

The current workload version **2** extends the original nine rows to **18**.
It adds three public-runtime families at 32, 256 and 2,048 elements, with sixteen
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

The receipt inventories each row's inputs/generator, scale, compiler flags,
seed/rounds, completed-work oracle, cache state, measurement boundary and timeout.
Each new family has a paired bounded-delay control with unchanged semantic
results. The native gate requires every actual warmup and repetition, all three
delay controls and complete lifetime counters. Calibration compares matching
workload versions and produces separate scaling curves for all six phases and
families. Compiler object samples remove the previous object before execution;
timeouts and interruptions kill/reap the process session. Native regression
controls compile and execute all nine new family/scale combinations and reject
wrong-checksum, missing-destruction and missing-callback implementations.

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_benchmarks \
  --sdk /path/to/candidate --extended-runtime \
  --output out/semantic-runtime-measurement --repetitions 3
```

These local public-runtime controls do not replace qualification with the sealed
SDK. Source/PCH/interface import costs, ARC/MRC object lifetimes, generated async
frames, cancellation workloads, worker-pool throughput and cold build/install
costs remain separate #8291 work. Timing remains advisory pending observed
runner variance and reviewed latency, throughput and memory goals.

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

A bounded same-input delay control must preserve semantic results and show its
minimum intended timing effect. Zero/partial work, incorrect checksums, absent
samples, process failure/timeout and concurrent SDK mutation fail. Distribution
summaries retain spread and absolute/relative effects. Timings remain
**advisory and inconclusive for regression gating** until runner-specific limits
and noise are calibrated; no universal slowdown allowance is invented.

This first selection does not measure cold builds, archive/install work,
ARC/MRC object lifetimes, source/PCH/interface cache behavior or concurrent task
throughput. Those dimensions require further workload definitions and measured
cost before expanding the existing budgeted CI/release lanes. No new schedule or
performance claim follows merely from successful measurement.

The original native CI selection used a three-repetition characterization
for compiler, runtime, stdlib, workload, foundation and runner changes and its
complete periodic/release selection. It seals only the required current compiler,
linker, resources, runtime archive/dependency and headers; it builds no archive.
The terminal gate checks all nine phase/scale summaries, each actual warmup and
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

Calibration rejects duplicate receipts and different host, SDK, workload,
instrumentation or repetition contexts. It recomputes every summary from raw
samples, rechecks runtime semantic work, then retains pooled and per-receipt
medians, actual deviation, per-workload RSS, scaling points and total runner and
receipt cost. The output remains explicitly advisory: measured spread does not
become a regression allowance. Promotion still requires reviewed per-workload
absolute effects and memory goals. Further workload families remain open under
#8291.
