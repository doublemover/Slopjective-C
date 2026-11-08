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

The source-owned policy selects thirteen frontend and eight runtime CTests.
Eight frontend CTests partition the complete emitter catalogue by ordinal; the
other five retain their ordinary selection. The uninstrumented core still runs
the full emitter suite as one program. Actual Ninja commands, compilation
databases and dependency closures own object provenance. Each object must contain coverage counters/data/mappings and the
required flags. Every eligible first-party implementation/header file remains
visible, including uninstantiated headers. Tests have separate profile roots
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

The historical initial pilot passed its fourteen suites. The expanded
current-source hosted run remains pending. All eight current native emitter
shards pass local diagnostics and cover all 164 cases. Every shard must retain
its identical full catalogue, exact disjoint assigned executions/passes and the
same executable hash. Both producer and terminal verifier recompute this proof
from JUnit. Passed output is bounded at 16 MiB so CTest’s default 1-KiB
truncation cannot erase the execution inventory. Instrumented emitter shards
each have a reviewed 900-second deadline; other selected tests retain 120
seconds. Raw profiles remain bounded at 256 MiB per first-party suite.

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
hashes and coverage mappings, and executes the ObjC3 lit suite plus the selected
native unit binaries. Actual ELF identities bind each profile to the executed
main and allowed loaded modules; unrelated support-tool profiles remain separate.
Exports retain zero-hit and missing-mapping files rather than silently reducing
the denominator. Source/object/DSO identities are checked again after execution.

```sh
python3 -B -m scripts.objc3c_toolchain.compiler_source_coverage \
  --output out/compiler-source-coverage
```

The existing native assurance lane selects reader controls, first-party coverage
and patched-compiler coverage together when required. Live integrated hosted
acceptance, matching cost observations and reviewed changed-region baseline
policy remain open under #8290/#8283. No automatic PR percentage threshold is
introduced. Compiler-process coverage, runtime execution coverage, language
conformance and fuzzer feedback remain separate claims.
