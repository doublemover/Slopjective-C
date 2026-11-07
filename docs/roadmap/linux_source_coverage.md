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

The source-owned policy selects six frontend and eight runtime CTests. Actual
Ninja commands, compilation databases and dependency closures own object
provenance. Each object must contain coverage counters/data/mappings and the
required flags. Every eligible first-party implementation/header file remains
visible, including uninstantiated headers. Tests have separate profile roots
with process/module-safe names; JUnit results must match the selected test.
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

The initial real pilot passes all fourteen suites. Five independent controls
prove concurrent process profile isolation, an eligible zero-hit function and
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
