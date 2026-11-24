# Fork-matched implementation analysis

Issue [#8285](https://github.com/doublemover/Slopjective-C/issues/8285) extends the
checked executor-lock pilot with a narrowly selected clang-tidy/Static Analyzer
lane. This analyzes compiler/runtime implementation translation units. It does
not qualify the analyzer's understanding of Objective-C 3 programs.

The development tool compiles the unchanged pinned clang-tidy engine/CLI and
three reviewed bugprone checks against the admitted instrumented LLVM/Clang
DSOs. The foundation is not reconfigured, and the tool is not shipped in SDKs.
Its selected checks, effective configuration, source/configuration identity,
loaded libraries, compiler objects and headers are recorded. Paired null-access
and corrected controls establish that the chosen analyzer actually diagnoses a
defect before project results can qualify.

The runner preserves compilation variants and response-file bytes. It checks
the real Ninja graph against each compilation database, retains actual header
closures, and selects all affected translation units for a header edit. Missing
commands, changed flags, parse errors, timeouts, crashes and findings fail.
There is no diagnostic suppression baseline. Per-check timing is enabled on
every action so a bounded failure identifies the expensive checker instead of
leaving another opaque translation-unit timeout. Full selection includes native
frontend/runtime variants, standalone runtime variants and all maintained
patch-touched Clang implementation TUs; test/vendor/generated-only exclusions
are reported explicitly.

The empty baseline is enforced before analysis starts. A `NOLINT` directive in
first-party implementation or headers, or added to a maintained Clang library or
header, fails review with its repository path and line. The check covers the
maintained patches' added text.

The preparation queue now holds at most twice the worker count and harvests
completed native actions before preparing more PCH/TU work. Each unit persists
its result, diagnostics and raw command independently; preparation or worker
failure drains already-running proof and preserves the original exception.
Unexpected infrastructure errors also leave a terminal failed campaign. Real
spawned actions, a failing producer and a clean sibling beside a blocker
diagnostic defend checkpoint durability. The focused analysis/checkpoint/gate
selection passes 97 tooling tests. A reset can still interrupt running actions;
only independently verified completed actions qualify for manual reuse.

The current full scan has exposed additional AST infrastructure paths. The
canonical patch stack now handles an external lexical load containing only
previously loaded fields, returns a persistent declaration-list tail after
singleton rebuilding, gives non-pointer parent nodes map-lifetime arena ownership,
and separates one callback round from fixed-point iteration while preserving
ref-qualified call operators. A focused native test source covers empty and mixed
external chains, declaration-list erase shapes, noncopyable callback reuse and
repeated parent-map clearing. The requires-clause mangler also asserts the resolved
template invariant, with the upstream concept-mangling regression selected in
ordinary and instrumented profiles. The corrected compiler foundation now passes all 563 compiler regressions and
461 native unit cases at `f2174d730013`, including these infrastructure controls.
Complete matching implementation analysis remains pending; native unit success
does not qualify the full analyzer inventory.
The external-field regression reproduces the exact `DeclBase.cpp` null-member
access under UBSan on the preceding compiler. Its four companion cases pass on
that compiler. The template-name dumper now asserts the using-shadow invariant;
the extended upstream AST-dump regression checks both the shadow and its target
and passes native `FileCheck` before the pending compiler rebuild.
Matcher construction now forwards its tuple once through `std::make_from_tuple`,
using guaranteed copy elision for nonmovable matcher objects. The new regression
checks distinct move-only elements and mutable/const reference preservation.
Five companion cases pass against the changed header overlay and matched preceding
compiler libraries. The rebuilt compiler also executes the non-pointer parent arena and external-load
controls successfully. The complete sanitizer/analyzer inventory retains its
separate current-source gate.

The matched analyzer cannot deserialize the foundation's bootstrap-produced
CMake PCH. Analysis validates the exact PCH/wrapper pair and rebuilds the wrapper
with the admitted fork using the real TU's definitions, include order, target,
language and feature flags. Identical variants share a content-addressed matched
PCH. The bootstrap serialization, wrapper, matched serialization, compiler and
both original/projected commands and hashes are retained. The forced textual
include remains in the projected command, and a missing or invalid matched PCH
fails instead of disabling validation. Native units retain a ten-minute ceiling. The measured template-heavy feature
and identity frontend units each have a thirty-minute allowance. Maintained
Clang units have a finite one-hour ceiling: the full instrumented scan records
completed compiler actions taking 1,443–3,509 seconds and thirty other units
hitting the former ten-minute limit. These are execution bounds, not passing
results or speed targets; findings, crashes and timeouts still fail.

The preceding six-hour scan completed 206 of 235 variants: 169 passed, thirty
timed out and seven reported findings. Its remaining 29 variants were interrupted
by the hosted job limit. The retained diagnostics and exact source identities
remain failed checkpoint evidence. All seven finding units are now repaired, exported and sealed. Focused local
Static Analyzer replay clears all seventeen reported locations, and six
additional modern C++ regression suites pass. This reused-compiler replay does
not qualify the complete matching Clang-Tidy/SDK campaign.

Complete analysis now partitions the dependency-selected inventory across four
independent CI jobs using stable variant identities. Each job builds and checks
the same admitted foundation and runs the unchanged checker set with paired
detector controls. The terminal gate requires every partition index, identical
logical inventories and frozen source/header inputs, one compiler/configuration
identity, and a disjoint union containing every selected fresh native action.
Missing, duplicated, changed-input, filtered and failed partitions reject the
aggregate. Dependency-scoped local selections retain one partition. The
partition, workflow and retained-proof controls pass 127 tests.

The interface emitter is partitioned by template publication, feature detection,
AST/declaration identity, dependency admission and final publication. A private
declaration header joins those units without enlarging the installed API. All
units remain in the default compilation graph and analyzer inventory. Small
identity probes use Clang's shared dynamic visitor; the three visitors with
custom traversal queues or walk-up behavior retain their specialized traversal.
This addresses repeated CRTP instantiation and the oversized TU without removing
checks or raising its ten-minute limit. Full regression/analysis acceptance is
required after the split.

```sh
python3 -B -m scripts.objc3c_toolchain.implementation_analysis \
  --frontend-build /path/to/current/native-build \
  --runtime-build /path/to/current/standalone-runtime-build \
  --output out/implementation-analysis/evidence
```

The `Extended Linux implementation analysis` workflow owns the durable gate.
It runs on manual dispatch, the existing weekly schedule and exact-SHA SDK release
calls. The native PR gate runs the [bounded executor analysis](#required-executor-analysis-pilot)
using its existing compiler and CMake graph. Manual/local dependency selection scopes
native edits independently to the frontend and standalone runtime. Shared
fork/tool/checker changes select maintained Clang units as well. Complete extended
runs use one immutable SHA. Repeated requests for the same source revision queue without canceling active
work. A repaired release can progress alongside an older revision's analysis once
its native build has completed. The workflow restores the admitted shared
`compiler-asan-ubsan` foundation and builds separate integrated-frontend and
standalone-runtime databases and the matched analysis tool in each bounded
six-hour partition job. Native qualification has its own execution budget.
Completed analysis actions also report progress in the
job log. A second job admits only the exact
revision, canonical plan, complete per-TU inventory, paired detector controls,
clean diagnostics and content-hashed retained evidence.

Manual dispatch accepts an optional `comparison_base` ancestor SHA to run the
matched analyzer only on the selected compilation variants. The runner uses
fresh preprocessing with the actual frontend and runtime compile commands for
every native variant. An unchanged translation unit may be selected through a
transitive header even when that header belongs to a different owner directory.
Missing dependencies fail selection, and the gate checks the retained depfile
digests. The full scheduled and release scans use the complete owned inventory.

Compiler sources, instrumented build/stage products and temporary analyzer,
frontend and runtime build trees are deleted before a passing receipt is
written. The retained artifact contains identities, commands, compilation
databases, raw logs and structured diagnostics, with a 256 MiB upper bound; it
does not retain another multi-gigabyte compiler tree.

The isolated workflow holds the canonical foundation lock from its clean-root
check through analysis and cleanup. Existing development trees are preserved;
only roots created by that attempt are reclaimed. Source revision and tracked
inputs are checked at both execution boundaries. Its frontend explicitly selects
the same instrumentation profile and shared Clang library as the foundation.
Configuration/build timeouts kill and reap the complete subprocess session.

Terminal verification recomputes the selected variants from the retained header
closures, requires an exact result for every selected identity, and checks each
fresh native command, compilation database and hashed diagnostic log after
artifact relocation. It also checks the effective checker configuration, the
specific defective/corrected source diagnostics, native foundation test results
and all four frontend/runtime configure/build actions. A valid incremental scan
may select no affected variants; it must still retain the complete inventory and
execute the detector controls. Periodic and release scans select every owned TU.
PCH dependencies include the serialization's separate header closure, and missing
records are rediscovered by preprocessing the exact forced textual wrapper.
Matched PCH builds retain the real command working directory. Manual checkpoint
reuse additionally verifies bootstrap, wrapper and matched-PCH bytes.
Ninja dependency queries use build-relative PCH targets: an absolute filesystem
path is not necessarily a Ninja target alias. A real CMake/PCH build checks the
recorded serialization dependency and selects its consumer for a shared-header
edit; out-of-build dependency targets are rejected.

The initial real scan rejected a LeakSanitizer report during embedded Clang AST
teardown. `DependentDiagnostic` allocated `DiagnosticStorage` in the AST arena
without registering destruction of its heap-owning strings and overflow vectors.
The maintained fix registers storage destruction with `ASTContext`. A native AST
unit regression creates repeated long diagnostic strings and overflowing fix-it
lists, then explicitly destroys the AST. Sanitizer qualification of that repair
and completion of the full analyzer inventory are required before #8285 closes.

Interrupted scans can resume exact successful TU actions. Reuse checks the
effective original/projected command, checker configuration, current source and
header closure, compiler/tool/loaded-library identities, native exit/timeout
disposition and immutable diagnostic log. Changed inputs are misses; corrupted
proof is rejected. Prior aggregate failure remains failed, and reused actions
are labeled separately from fresh execution. Spawned worker processes avoid
forking a threaded process through the subprocess pre-exec hook. Parallelism is
bounded to one through four workers; each TU keeps its own durable result.

Manual execution accepts up to eight ordered `--resume` receipts so a focused
repair can complement a completed full scan. Each receipt must be terminal,
owned, bounded, unique and unchanged after admission. Every reused action still
passes the complete command/source/header/PCH/tool/configuration/log checks;
a corrupt candidate fails instead of falling through to a later good one.
Each action retains its actual origin, and prior aggregate failures remain
failed. CI continues to require fresh execution of all selected actions.

The first hosted run completed the instrumented foundation and native component
builds, then exposed an isolated-environment configuration failure: the pinned
Ninja directory was absent from the analyzer's strict PATH. The tool owner now
binds `CMAKE_MAKE_PROGRAM` to the admitted Ninja executable and retains both
CMake and Ninja directories. A real clean-PATH CMake/Ninja build defends that
boundary. The matched local tool builds successfully; completion of the full
analysis inventory remains required.

The repair initializes importer outputs and makes declaration/type invariants
explicit, preserves absent evaluator frames and optional outputs, and keeps
constructor bookkeeping inside the active call-frame lifetime. Coroutine
alignment and builtin-call classification are captured once across callbacks;
empty expression results, incomplete concrete keypaths and invalid cleanup
callbacks reject. Modern constant-expression, decomposition and matrix suites
are enrolled in every foundation profile. The new sealed identity is
`llvmorg-23.1.5-392dfff97d29-bace3bd39a7e-0fcb354b`; complete matching hosted
qualification is still required before SDK acceptance.

## Published preview review

The retained plans and all eight partition archives per host verify through the
terminal aggregation code at the published revision `8fe5c2fd8`. Each host's
disjoint union contains 246 fresh native actions across 180 maintained Clang,
45 integrated frontend, and 21 standalone runtime variants. Each host's
partitions agree on the analyzer binary, dependency-input identities, and
producer archive. The [receipt index](../support/linux_analysis_receipts.json)
records the exact artifact IDs and SHA-256 digests. [x86-64 gate](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/job/136767271492) · [AArch64 gate](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/job/136760510480)

- Every partition retains a corrected control and a blocker diagnostic from
  `clang-analyzer-core.NullDereference` at the intended source site. Retained
  analyzer arguments, configurations, diagnostics and build receipts verify
  against the selected compilation variants.
- The terminal gate admits both native build configurations and the fork's
  compiler inventory. Tool identity, compile database, missing dependency,
  timeout and corrupt receipt controls fail closed in the focused tooling
  selection.
- The same-mtime header-edit test uses real Ninja compile commands and fresh
  Clang preprocessing. The benign checker control produces a blocker in the
  unchanged dependent translation unit after the transitive include changes.
  The optional ancestor-comparison workflow still needs a hosted replay of this
  current selection path.
- Seven checked executor controls and `retainable-family-analyzer.m` passed in
  the published host receipts. The empty suppression policy is code-enforced for
  maintained source additions. Scheduled and release gates cover the full
  inventory.

While a current producer artifact is retained, the existing analysis runner can
reuse it through `--foundation-bundle` and `--foundation-archive-sha256` from the
same tested revision, host and runner workspace. It reconstructs the sealed
source and verifies the producer digest and transported Ninja inputs before
analysis. Diagnostic partition ZIPs support offline replay of the completed
terminal gate. A fresh native scan needs the producer's `foundation.tar.zst` and
receipt, including the staged compiler, matched PCH inputs and owning build graph.

For offline review, place the plan and eight partition archives in one directory
as `<artifact-id>.zip`. The replay command binds each ZIP to the reviewed digest
before extracting it and runs the existing terminal aggregator.

```sh
python3 -B -m scripts.objc3c_implementation_assurance.replay \
  --host linux-x86_64 --archive-directory /path/to/retained-artifacts
```

## AArch64 shared producer checkpoint

At `af41a499`, the release's AArch64 `compiler-asan-ubsan` producer passed all
ten foundation phases in 2,983.87 seconds from a cold build directory. Its
retained tests record 633 lit passes, two unsupported cases and 508 selected
native unit cases. The audit recomputed the profile identity from that commit
and verified all retained phase logs against the foundation receipt. [Producer job](https://github.com/doublemover/Slopjective-C/actions/runs/45289389357/job/138094150878) · [Diagnostics archive](https://github.com/doublemover/Slopjective-C/actions/runs/45289389357/artifacts/14876947110)

The identical compiler and diagnostics feed the eight-partition complete
implementation plan. The analysis producer receipt binds the sanitizer
producer's SHA-256, the exact selection and the inner transport digest
`dbbb7aa32e6d89ef05fd25310634b776858965ba4d28138f016ce08427ff4232`.
The transport declares 4,455 owned entries, including `llvm-tblgen` and the
original Ninja graph. All eight AArch64 jobs downloaded the shared input and
the standalone producer skipped. Partition and terminal results were pending
at this checkpoint. [Analysis plan](https://github.com/doublemover/Slopjective-C/actions/runs/45289389357/artifacts/14875979912) · [Shared transport](https://github.com/doublemover/Slopjective-C/actions/runs/45289389357/artifacts/14876987066)

## Required executor analysis pilot

The native PR job checks `service_executor.cpp` with the reviewed
`core.NullDereference` blocker. For native source changes it preprocesses that
translation unit with its real integrated Ninja compile command. An edit to a
transitive header selects the unchanged translation unit. Foundation, CMake and
pilot input changes also select it. The gate reads the admitted base SDK
manifest and exact driver bytes when the locked fork matches. When the native
PR already built its current foundation, it checks the same build receipt and
the driver's install-time RPATH transition. Both routes use the compiler already
chosen by the native job. The weekly and release clang-tidy scans retain the
full checker set and translation-unit inventories.

The terminal job checks the selected dependency closure, original compile
flags, response files, checker inventory, paired source-site controls and SARIF
results. It requires explicit evidence when the executor is outside the selected
scope. A blocker or incomplete evidence fails the native PR gate.

Local verification used the published x86-64 SDK and its matching `8fe5c2fd8`
native source. The actual CMake/Ninja executor action qualified in approximately
three seconds after SDK admission. A same-length, same-mtime transitive header
edit in an isolated control selected the unchanged executor and reported the
expected checker. Restoring the header cleared the finding. A hosted native PR
replay of this staged gate and additional promoted checker scopes remain open.
