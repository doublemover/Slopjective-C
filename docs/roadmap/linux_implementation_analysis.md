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

The matched analyzer cannot deserialize the foundation's bootstrap-produced
CMake PCH. Analysis validates the exact PCH/wrapper pair and rebuilds the wrapper
with the admitted fork using the real TU's definitions, include order, target,
language and feature flags. Identical variants share a content-addressed matched
PCH. The bootstrap serialization, wrapper, matched serialization, compiler and
both original/projected commands and hashes are retained. The forced textual
include remains in the projected command, and a missing or invalid matched PCH
fails instead of disabling validation. Each TU has a bounded ten-minute analysis
allowance; a timeout remains a failed TU.

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

The first hosted run completed the instrumented foundation and native component
builds, then exposed an isolated-environment configuration failure: the pinned
Ninja directory was absent from the analyzer's strict PATH. The tool owner now
binds `CMAKE_MAKE_PROGRAM` to the admitted Ninja executable and retains both
CMake and Ninja directories. A real clean-PATH CMake/Ninja build defends that
boundary. The matched local tool builds successfully; completion of the full
analysis inventory remains required.
