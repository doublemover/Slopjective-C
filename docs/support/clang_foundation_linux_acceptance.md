# LLVM 23 Linux milestone acceptance

The October 18, 2026 milestone qualifies the Clang migration frontend on
Debian 13 x86-64 with GNUstep 2.0. Its frozen SDK identity is
`llvmorg-23.1.3-0d261d1ca552-2fb07f866fe4-fd761312`.
The [machine-readable receipt](clang_foundation_linux_acceptance.json) records
versions, evidence hashes, measurements, cache identity and remaining work.
[Reproduction and installation](../../toolchains/llvm/README.md) use the canonical
six-patch series and locked host tools. The initial uncommitted cloud setup and
portable-artifact changes were preserved, reviewed and integrated.

## Correctness and distribution

| Check                          | Result                                                                    |
| ------------------------------ | ------------------------------------------------------------------------- |
| Clang extension lit            | 160 pass; 3 unsupported target/runner cases                               |
| Native AST contracts           | 46 pass                                                                   |
| Facade CTest                   | 16 pass                                                                   |
| Pinned Linux runtime CTest     | 5 pass                                                                    |
| Installed native validation    | 13 nodes pass; 76 executable witnesses, including 32 expected traps       |
| Focused tooling run            | 221 pass; 3 skip; 33 subtests pass                                        |
| Public compiler smoke          | Pass, including 61 behavior cases and 26 runtime acceptance cases         |
| Showcase performance portfolio | All three compile/runtime workloads pass with raw sample packets          |
| Dispatch performance           | One warmup and three measured acceptance-case samples pass                |
| Fresh Debian container         | Seal, sample rebuild/run, LLDB and installed LLVM/Clang CMake client pass |
| Extracted SDK archive          | Independent verification and sample rebuild/run pass outside the checkout |

The managed typed-error witnesses cover object/Block success values, owned
factories, MRC heap Blocks consumed by ARC, callbacks, C++ members, Objective-C
messages, propagation, failures, cleanup and native exceptions at O0/O2.
A new no-return regression fixes a compiler crash in the typed-error epilogue.
Semantic interfaces retain return ownership and reject erased required semantics;
ARC and MRC consumers use separate ABI views of the same provider object.
Managed try-optional, aggregate/reference success returns and async error
execution remain closed.

Public `defer` is advertised only for Linux x86-64, GNUstep 2.0, compatibility
profile. Generated macros, native feature queries, parser behavior, installed
consumers and editor tests agree with the [capability authority](../../schemas/objc3-language-capabilities.json).
Other public language gates retain their current boundaries.

The sealed SDK contains 3,880 entries and compresses to 386,828,898 bytes.
Its SHA-256 and independent seal are recorded in the receipt. A fresh
`debian:13-slim` container used the SDK and pinned Python without mounting the
checkout. Its external CMake consumer also exercises Debian's CMake 3.31.6;
foundation development remains pinned to CMake 4.4.4. Header dependencies for
zlib/zstd are required by installed CMake targets and are documented explicitly.

## Development throughput

Measurements use this workspace's four-CPU quota and 16 GiB memory limit.
SDK compilation uses four compile jobs and one link job, incremental Ninja
outputs and a content-identified compiler cache. Warm SDK runs overlapped
fresh-container setup; their range describes this shared host.

| Measurement                                                      | Observed seconds       | Same-host acceptance envelope  |
| ---------------------------------------------------------------- | ---------------------- | ------------------------------ |
| Idempotent environment setup, pinned tools already installed     | 17.527                 | Recorded warm setup            |
| Public smoke baseline / after                                    | 97.204 / 83.992        | Existing composite budget: 150 |
| Runtime acceptance before / after                                | 61.801 / 39.262        | Existing child budget: 60      |
| Full installed native matrix                                     | 42.661                 | 60                             |
| Native evidence reuse, three trials                              | 11.446, 10.012, 10.192 | 15                             |
| Warm SDK build/test/stage, three trials, including command setup | 78.967, 66.633, 61.340 | 90                             |
| Facade no-op build plus 16 tests, three trials                   | 3.095, 2.523, 2.409    | 5                              |
| SDK packaging / verified cache hit                               | 34.156 / 14.807        | Cache hit: 20                  |

The existing public composite gates retain `validation_timing_budgets` as their
decision owner. Additional envelopes above are milestone acceptance limits for
this host, recorded alongside the raw samples; compare future runs on an
equivalent profile before changing them. The narrow runtime budget failures
were retained and resolved through implementation changes.

Profiling attributed about 17 seconds to repeated input hashing. Runtime
acceptance now performs four full compiler/runtime reads and reuses 138 admitted
reads per session, with a fresh final content check before publishing PASS.
Same-size/same-mtime mutation tests reject changed inputs. Sources, outputs and
negative reuse tests continue to hash fresh. Python startup measured
0.024–0.042 seconds; PowerShell measured 0.484–0.558 seconds. The Linux native
runner invokes Python/Clang directly. Source content stamps prevent stale
frontend binaries from qualifying, and SDK identity changes rebuild the small
set of affected translation units.

Ninja and sealed SDK reuse provide the measured benefit. LLVM's PCH-heavy
compilation still produces many uncacheable ccache calls; no high hit-rate claim
is made. Aggregate cgroup memory includes substantial filesystem cache and had
no OOM events. A comparable clean LLVM timing and compiler peak-RSS baseline
remain unmeasured: the initial build was interrupted and shared capacity.
These results do not compare LLVM 22 versus 23 or establish a runtime speedup.
The runtime benchmark's case latency includes compilation/linking and probes.

## Remaining qualification

- Requalify LLVM 23 native execution on Windows/macOS and audit current Windows
  build tools and SDKs. Historical Windows evidence is retained with its scope.
- Promote the legacy public compiler's Linux platform row through its own
  `libobjc3-runtime.so` evidence. The migration SDK's libobjc2 proof feeds the
  existing [platform review process](../runbooks/objc3c_platform_toolchain_support_matrix.md).
- Complete public Optional/Result and throws syntax before opening their gates.
- Run representative release LTO/PGO experiments and record comparable clean
  build, runtime and peak-RSS results. Development builds keep those expensive
  optimization profiles separate.
