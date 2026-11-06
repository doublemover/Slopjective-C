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
ordinary and instrumented profiles. These changes have passed patch admission
and test-source compilation; the corrected compiler build and native execution
remain pending while the earlier full scan collects the rest of its findings.
The external-field regression reproduces the exact `DeclBase.cpp` null-member
access under UBSan on the preceding compiler. Its four companion cases pass on
that compiler. The template-name dumper now asserts the using-shadow invariant;
the extended upstream AST-dump regression checks both the shadow and its target
and passes native `FileCheck` before the pending compiler rebuild.
Matcher construction now forwards its tuple once through `std::make_from_tuple`,
using guaranteed copy elision for nonmovable matcher objects. The new regression
checks distinct move-only elements and mutable/const reference preservation.
Five companion cases pass against the changed header overlay and matched preceding
compiler libraries. The non-pointer parent arena and external-load implementation
still require the pending rebuilt compiler; these controls do not imply that the
entire fork-fix batch has passed.

The matched analyzer cannot deserialize the foundation's bootstrap-produced
CMake PCH. Analysis validates the exact PCH/wrapper pair and rebuilds the wrapper
with the admitted fork using the real TU's definitions, include order, target,
language and feature flags. Identical variants share a content-addressed matched
PCH. The bootstrap serialization, wrapper, matched serialization, compiler and
both original/projected commands and hashes are retained. The forced textual
include remains in the projected command, and a missing or invalid matched PCH
fails instead of disabling validation. Each TU has a bounded ten-minute analysis
allowance; a timeout remains a failed TU.

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

The `Linux implementation analysis` workflow owns the durable gate. Its
concurrency policy finishes the active cold build and queues the newest
revision; newer pushes do not repeatedly discard an in-progress fork build. Pull
requests run it only when the analyzer/toolchain, native implementation or
compilation graph changes, and pass those exact changed paths to the dependency
selector. Native source scopes independently select the integrated frontend and
standalone runtime; they do not reanalyze maintained Clang TUs unless a shared
fork/tool/checker input changed. Weekly, manually dispatched and Linux SDK
release runs select the complete owned-TU inventory at one immutable SHA. The
workflow builds the admitted `compiler-asan-ubsan` foundation,
separate integrated-frontend and standalone-runtime databases, and the matched
analysis tool in its own six-hour job so the already broad native qualification
cannot consume the analyzer's time budget. A second job admits only the exact
revision, canonical plan, complete per-TU inventory, paired detector controls,
clean diagnostics and content-hashed retained evidence.

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
