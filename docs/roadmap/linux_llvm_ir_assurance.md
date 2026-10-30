# Native LLVM IR assurance

Issue [#8286](https://github.com/doublemover/Slopjective-C/issues/8286) now has an
executable development lane over actual Objective-C 3 frontend output. It adds
structural checks and failure reproduction to the existing semantic witnesses.
Verification alone does not establish semantic equivalence.

## Selected execution surface

[The corpus](../../toolchains/llvm/ir-corpus.json) selects eight authored witnesses:
explicit captures in ARC/source and MRC/PCH modes; managed function/message/Block
calls; guard cleanup; carrier transfers; native async frames; typed keypath PCH;
and managed semantic-interface imports. C17, C++20 and C++23 are represented.
Every case runs O0/O2. Captures add a public `-Os` canary. Captures, managed calls
and async also run the actual production ThinLTO route.

This is **17 configurations, 29 translation units and 37 runtime executions**,
not the full language/ownership/import Cartesian product. Runtime success uses
the existing assertions, destruction counters, active-arm checks, cleanup/event
ordering and operand-evaluation checks. Production and split routes must each
pass those assertions independently.

For each translation unit the runner retains:

1. Source/header copies, selected PCH/interface artifacts, source/tool/runtime
   hashes, exact driver commands and the native driver's `-###` cc1 route.
2. Real frontend IR with the intended frontend optimization setting and LLVM
   passes disabled, canonical verified input, optimized bitcode/text and object.
3. Production-driver optimized IR/object, separately verified and executed.
4. `opt -verify-each`, `llc -verify-machineinstrs`, DWARF verification and real
   source line tables. Target triple, layout, CPU and feature attributes are
   recorded from the actual frontend module.
5. Production ThinLTO prelink bitcode and every saved per-module backend stage,
   including import, optimization and pre-codegen IR. The combined link index
   and `preopt.bc` alone cannot satisfy this coverage requirement.

All-optnone O2/size modules fail admission. Canonical instruction-body comparison
must show real O2 transformation; metadata renumbering is ignored. Expected and
executed cases, translation units, routes and named stages are reconciled.
Timeouts retain an incomplete failure report. Repeated whole-module verification
made the 2,034-function async provider exceed its 900-second O2 cap, so this lane
selects the original `native_leaf` and `native_parent` implementations with a
focused client and exact runtime oracle. The broad async witness remains in the
ordinary source/runtime qualification. Other commands default to 180 seconds.

## Fork and pipeline identity

[Development tools](../../toolchains/llvm/assurance-tools.json) add `opt`,
`llvm-dis` and `llvm-reduce` to the existing sealed compiler ASan/UBSan build.
They are not SDK distribution components and never resolve from PATH.
The owner checks the canonical plan, effective CMake configuration, actual
transitive Ninja/compilation graph, instrumentation symbols, ordered archive
membership, executable bytes and loaded ELF libraries. Installation may change
only the exact CMake-owned RUNPATH string; every other ELF byte must match.

A successful tool receipt can be reused after all input, object, archive,
executable and loaded-library hashes are checked again. The inventory, owner,
helpers and build graph participate in that receipt's identity. A matching
version banner alone is insufficient. This does not rebuild or promote an SDK.

The split ARC path preserves Clang's `BackendUtil.cpp` extension points:

```text
-passes=default<O2> -verify-each
-passes-ep-pipeline-start=function(objc-arc-expand)
-passes-ep-scalar-optimizer-late=objc-arc
```

O0 and MRC omit those ARC callbacks, matching the driver. A capability control
requires both passes to execute; ignored extension syntax fails. Generic opt
without these callbacks crashed the capture oracle while the production driver
passed; the retained failure is evidence about that incomplete diagnostic
recipe, not a production compiler regression. Replaying with the correct
callbacks passed.

LLVM 23 rejects `default<Os>`. The size canary uses the supported O2 pipeline with
the frontend's actual `optsize` policy, while preserving the public driver `-Os`
execution. The split recipe remains additional to the production driver and
ThinLTO, whose language, coroutine and target pipeline ownership stays intact.

## Execution and triage

After building the compiler instrumentation foundation and the three development
tools, select the ordinary owned runtime archive and GNUstep dependency:

```sh
python3 -B -m scripts.objc3c_toolchain.llvm_ir \
  --output out/llvm-ir-current \
  --runtime /absolute/path/to/libobjc3_runtime_services.a \
  --objc-runtime /absolute/path/to/libobjc.so \
  --facade /absolute/path/to/matching/objc3c
```

An optional `--tools-receipt out/previous/tools.json` rechecks an existing
development bundle. It does not skip corpus execution. Evidence directories
must be fresh and disjoint from inputs. Each command retains argv, cwd, permitted
environment, timeout, status, diagnostics and output hashes. Failed source,
PCH/interface, last valid IR and generated artifacts remain available.

The reducer consumes a reviewed `objc3c.llvm-interestingness.v1` JSON specification
with explicit command arrays, tool/dependency hashes, time limits and one exact
failure signature. `semantic_spec` creates a verified split-pipeline template;
ARC failures must preserve the extension-point arguments from the failing
qualification command. Link inputs can bind other objects and the exact runtime.
Diagnostic specifications require a particular diagnostic and exit status;
semantic specifications require successful verification/optimization/backend/link
prerequisites and the same exact program result twice. Parse, link, loader,
unknown-pass and timeout failures cannot count as a semantic mismatch.

```sh
python3 -B -m scripts.objc3c_toolchain.llvm_reduce reduce \
  --spec /absolute/path/to/interestingness.json \
  --input /absolute/path/to/original.ll \
  --reducer /absolute/path/to/matched/llvm-reduce \
  --output out/llvm-reduction-current

python3 -B -m scripts.objc3c_toolchain.llvm_reduce bisect \
  --spec /absolute/path/to/interestingness.json \
  --input /absolute/path/to/original.ll --limit -1 \
  --output out/llvm-bisect-current
```

Reduction retains the original and spec, minimizes a copy, then replays the
reduced result. Opt-bisect records a selected boundary and the exact failure
predicate; optional pass participation and changed downstream behavior prevent
automatic attribution. Source/parser defects require source reduction.

Independent controls exercise malformed dominance IR, an actual invalid-MIR PHI
rejected by the backend verifier, an absent pass, tool substitution, missing
outputs, crash, timeout, malformed DWARF and unrelated parse/link failures.
An actual llvm-reduce campaign removes an irrelevant function while preserving
the deterministic result; canonical IR printing need not reduce byte count.
The synthetic controls are not presented as frontend-generated semantic proof.

## Current limits

The complete bounded source/PCH/interface matrix passed: 17 configurations,
29 translation units, 37 runtime executions and 603 recorded commands. The
independent control campaign and real reduction also passed. Final results belong
to their machine-readable receipts; the accepted SDK archive remains unchanged.

The isolated ASan/UBSan LLDB consumer also passed after matching the foundation's
TableGen, Debuginfod and HTTP development archives. Its receipt verifies 921 LLDB
objects, 119 archives, both debugger executables, exact LLVM/Clang/shared-ASan
loader graphs, and paired clean/detecting ASan and UBSan controls. Link-only reuse
may retain compiled objects, while source, bootstrap, compile-option and TableGen
changes are rejected.

The [expensive-check preset](../../toolchains/llvm/expensive-checks.json) has a
separate source/configuration/build/stage identity and never substitutes for an
ordinary foundation or embedding consumer. Its measured admission pilot found
8.33 GiB of comparable build files plus 3.49 GiB staged files. With 25% headroom
and a 2 GiB reserve it needs about **16.8 GiB free**, versus **11.7 GiB available**
on this instance. It reports `resource-constrained`, `qualified: false`; no
expensive-check build is claimed. Resource admission is an estimate from a real
comparable build, not a measurement of expensive-check execution.

Optimized DWARF structure and source tables are covered. Optimized carrier,
cleanup and coroutine **LLDB value/state expectations**, their optimized-out
controls, and a maintained CI execution hook remain open. Existing O0 debugger
qualification keeps its separate intent. Debugify is diagnostic only and is not
used to assert frontend source-mapping accuracy. These remaining items keep
#8286 open; tool inventory and source-only tests do not close it.
