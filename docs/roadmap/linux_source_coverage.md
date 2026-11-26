# First-party implementation source coverage

Issue [#8290](https://github.com/doublemover/Slopjective-C/issues/8290) now has an
executable first-party measurement pilot. It builds the compiler facade/embedding
library and standalone runtime with Clang 23 source coverage and atomic counter
updates in an isolated temporary tree. Ordinary builds keep coverage disabled.
Custom product-compiler object commands receive the same coverage flags.
The frontend variant also retains its admitted ASan/UBSan implementation profile;
these are diagnostic measurements, not release-performance samples.

```sh
python3 -B -m scripts.objc3c_toolchain.source_coverage \
  --output out/source-coverage --jobs 2
python3 -B -m scripts.objc3c_toolchain.coverage_controls \
  --output out/source-coverage-reader-controls
```

The source-owned policy selects 66 frontend and nine runtime CTests. The
standalone `objc3_text_scalar_boundary` case links the production runtime
archive.
Thirty-two frontend CTests partition the complete emitter catalogue by ordinal.
The uninstrumented core runs the full emitter suite as one program. Actual
Ninja commands, compilation databases and dependency closures establish object
provenance. The source inventory independently requires 24 embedding and 19
standalone runtime translation units in each respective graph, with
instrumentation flags, ELF counter sections and compiled mappings. Per-suite
exports, matrix merges and the terminal reader use the same exact source
inventory, including uninstantiated headers. The current 108-candidate
inventory adds seven shipped stdlib inline headers and two platform translation
units to the published 99-candidate baseline. The inventory identifies the CLI
entry points, SDK adapter and experimental Darwin registration as outside the
selected routes. Configured header templates retain source hashes separately
from generated mapping coordinates. Tests have separate profile roots
with binary-build-ID/module-signature names and locked merging across fork and
exec. This retains one raw profile per module rather than a complete shared
compiler profile for every process. Real native branch controls cover both
forked and exec children in standalone and shared-library configurations. The
coverage owner requires all four native controls without skips before building
LLVM. JUnit results must match the selected test.
Missing profiles, failed tests, reader failures or changed inputs cannot pass.

Each indexed profile's build ID must equal the selected executable's ELF note.
Live coverage exports use that linked binary, including its selected inline and
COMDAT variants. Relocatable objects supply a **separate zero-profile mapping
view** for compiled but unlinked code. Mixing them into live counter exports
would produce mismatched function hashes; bring-up exposed and rejected that
configuration. Reports retain both mapping and executed denominators rather
than hide discarded code in a percentage. Per-file region/branch/function/line
summaries and uncovered branch records remain separate for every test/config.
No whole-language or repository-wide sufficiency percentage is generated.

The historical initial pilot passed its fourteen suites. An earlier hosted run
passed shards zero through five before shard six exceeded 900 seconds. Both
published Linux receipts retain all 74 suites under the 32-shard emitter
policy. Shards retain the complete catalogue and the exact disjoint assigned
executions. Shards from the same partition share a test binary hash. Both
producer and terminal verifier recompute this proof
from JUnit. Passed output is bounded at 16 MiB so CTest’s default 1-KiB
truncation cannot erase the execution inventory. Instrumented CTests use the
hosted job deadline and respect explicit fixture timeout properties. Raw
profiles remain bounded at 256 MiB per first-party suite.
Command receipts record wall time, CPU time, and the peak RSS of an individual
process among the command and its waited descendants. Foundation receipts
record compilation and staging costs separately from the coverage runs.

Five independent controls prove concurrent process profile isolation, an eligible zero-hit function and
both ordinary/recovery decision arms, wrong-binary rejection, missing-profile
rejection and corrupt-profile rejection. The recovery paths return the same
semantic result, so the measurement demonstrates an additional executed branch
without confusing coverage with correctness.

Frontend witnesses link large instrumented Clang archives. Build products live
in `/tmp`, with one linked witness resident at a time and a free-space preflight.
Successful admission reclaims the temporary build; indexed profiles, compressed
raw profiles, diagnostics, commands and hashes remain under `out`. The large raw
LLVM JSON exports are parsed into the per-file report, hashed and size-recorded,
then reclaimed; retaining both forms provided no additional replay value.
Failed builds retain their temporary tree and receipt. A failed build may be
reused only after checking its tool/source/header/object inputs; every test and
profile collection runs anew. This is compiled-product reuse, not reused native
execution. The earlier persistent-storage failure remains a failed attempt.

The integrated compiler owner now uses the isolated `compiler-source-coverage`
foundation. It derives whole-file and changed-hunk denominators directly from
the admitted patches, verifies every eligible implementation object's flags,
hashes and nonempty ELF coverage sections, and executes the ObjC3 lit suite plus
the selected native unit binaries. A separate lit discovery report names every
test. The coverage owner reconciles those names with the execution results and
requires an explanation for each unsupported case. ELF identities tie each
profile to the executed main and allowed loaded modules. Unrelated support-tool
profiles remain separate.
Exports retain zero-hit and missing-mapping files rather than silently reducing
the denominator. The changed-hunk view records region entries and both arm counts
for every branch decision intersecting a maintained hunk. Source, object, DSO
and tool identities are checked again after execution.

```sh
python3 -B -m scripts.objc3c_toolchain.compiler_source_coverage \
  --output out/compiler-source-coverage
```

The [reviewed baseline index](../../toolchains/llvm/source-coverage-baselines.json)
binds the compiler receipt and archive digest for each published host. Both
reports retain 268 eligible files, including five mapped files with zero hits and
29 files without emitted mappings. The x86-64 report covers 138,215 of 353,164
whole-file branch arms, and AArch64 covers 138,578 of 353,168. Both report 35,505
covered patch-hunk region entries out of 44,734. [x86-64 archive](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/artifacts/14697446144) · [AArch64 archive](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/artifacts/14698955958)

Pass a verified release ZIP or a qualified local report with `--baseline`.
Use `--compare-current` to compare retained receipts without rebuilding LLVM.
The comparison records eligible-file additions and removals, source and mapping
changes, and hit deltas for comparable files. It reports lost and gained branch
arms at their exact source coordinates even when the total is unchanged. The
published receipts predate the changed-branch metric, so comparisons against
them identify that missing data.

The existing native assurance lane selects reader controls, first-party coverage
and patched-compiler coverage together when required. The source-coverage merge
and terminal gates passed on the published x86-64 and AArch64 preview. The
stricter first-party inventory and changed-hunk branch comparison still need
matching hosted execution. [x86-64](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/job/136764381872) · [AArch64](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/job/136767054311)

The first-party owner can report inventory additions, removals and changed
source bytes against either digest-reviewed release archive without building.

```sh
python3 -B -m scripts.objc3c_toolchain.source_coverage \
  --compare-published-inventory /path/to/published-coverage.zip \
  --output out/first-party-coverage-inventory-diff.json
```

## First-party rollback witness and observed costs

Both digest-pinned archives from `8fe5c2fd89008d1c090a9355c11bdda01ceb9309`
retain the same map iterator rollback result. The ordinary associative suite
recorded true/false counts `[0, 0]` at both ownership cleanup checks. The
collection ownership suite reached each cleanup arm once after an iterator
observed a changed generation. The standalone x86-64 rebuild reproduced
`[0, 0]` and `[1, 0]` for both decisions using the current runtime source and
matching LLVM profiles. Its receipt SHA-256 is
`59c57390d98934f3ca4b1e796c7127aca266d9f86c41e5778a029d6c4bdfbd3f`.
[Published runtime checks](https://github.com/doublemover/Slopjective-C/blob/8fe5c2fd89008d1c090a9355c11bdda01ceb9309/native/objc3c/src/runtime/stdlib/generic_associative_service.cpp#L558)

The eight runtime suites on each published host report the text service as
`mapped-but-not-linked`. A local x86-64 run used the same instrumented text
service object with the original and updated scalar boundary fixture. The
byte-search past-end decision at `text_service.cpp:314:13–314:44` gained
true/false hits from `[0, 0]` to `[1, 2]`. The added case checks an empty
search at the exact byte end, output clearing on a past-end request, and a
successful retry on the same text. The updated fixture also passed against
the published x86-64 header and archive at `-O0` and `-O2`. Local receipt
SHA-256 `851740285f2ff80e73fa3810cc90bc531def3ff65265530931e195bafe5e0b32`.

The new standalone CTest passed in Release and coverage builds on x86-64.
Its indexed profile matched the executable's ELF build ID and reproduced
`[1, 2]` for the past-end decision. Local receipt SHA-256
`7554369690a16cbcb79a8f0efe8687e5b0797e16df940e30cc2a65601a8fb4e6`.

Each published host retained one cold instrumented foundation build and eight
first-party partitions. Component command totals add work across partitions.
The partition wall time is the longest observation. Archive bytes include
compiler, first-party and reader-control evidence.

| Observation                                           |                    x86-64 |                   AArch64 |
| ----------------------------------------------------- | ------------------------: | ------------------------: |
| Instrumented foundation build, s                      |                   2,752.1 |                   2,411.3 |
| Longest first-party partition, s                      |                   1,772.9 |                   1,885.3 |
| Frontend build / execution / export command totals, s | 1,041.8 / 5,316.1 / 184.6 | 1,165.6 / 6,942.4 / 206.9 |
| Runtime build / execution / export command totals, s  |       31.5 / 0.455 / 2.85 |       37.0 / 0.736 / 3.57 |
| Peak raw frontend profile bytes per suite             |               200,187,104 |               200,179,520 |
| Compressed evidence archive bytes                     |                 7,889,238 |                 7,892,097 |

A local x86-64 pilot used the published SDK compiler and identical text-service
source at O0, with three interleaved builds per profile. Each execution verified
32,768 pairs of valid scalar slices and checked out-of-bounds returns. Median
plain and instrumented compile/link wall times were 1.382 and 1.681 seconds.
Peak compile/link process RSS medians were 160,208 and 164,092 KiB. Execution
wall time medians were 0.078 and 0.086 seconds. Instrumentation added 109,064
bytes to the executable and produced a 10,952-byte raw profile per run. The
pilot receipt SHA-256 is
`cd4e3eeb8425c67188fe2ffe77fec83eb65a2996922810d8ab562c3c972fa1ca`.
Peak RSS for the full hosted foundation and suite matrix is still unmeasured.
