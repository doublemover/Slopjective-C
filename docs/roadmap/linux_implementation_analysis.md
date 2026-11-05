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
