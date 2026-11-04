# Current semantic workload benchmarks

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
