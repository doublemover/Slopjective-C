# Linux compiler implementation instrumentation

Issue #8284 tracks two independent subjects: compiler implementation checks and
instrumented emitted clients with the complete runtime graph. The latter uses
`runtime_instrumentation.py` and the existing sanitizer runners. An ordinary
compiler producing an instrumented client does not qualify the compiler itself.

The owned `compiler-asan-ubsan` profile in
[`instrumentation-profiles.json`](../../toolchains/llvm/instrumentation-profiles.json)
selects LLVM's `Address;Undefined` configuration, O1, source locations, frame
pointers, assertions, RTTI and exceptions. Its configuration digest includes the
profile document and selector. Build/stage directories and compiled identities
are distinct from the ordinary foundation. The accepted SDK stays ordinary.

```sh
python -B -m scripts.objc3c_toolchain build --host linux-x86_64 \
  --instrumentation-profile compiler-asan-ubsan --stage
python -B -m scripts.objc3c_toolchain.compiler_instrumentation \
  --output out/compiler-instrumentation/current-run
```

The second command requires a fresh output directory and a complete, current,
passing build/test/stage receipt. It uses the exact bootstrap runtime and
symbolizer, strict sanitizer environment and paired ASan/UBSan/LSan controls.
The controls load the selected instrumented LLVM/Clang libraries. The compiler
PCH diagnostic matrix covers C++20/C++23 under ARC/MRC.

The runner reconciles transitive Ninja compilation commands against compilation
ownership, then checks object bytes and actual instrumentation symbols. Staged LLVM/Clang
and embedding archive members must match the exact ordered Ninja target objects
and their verified bytes. Repeated basenames in Clang target/backend directories
are disambiguated by object path/hash; globally valid objects from another target
cannot substitute for the canonical members. Custom
product-compiler objects have CMake-generated source/object records. Their flags
and symbols are checked too. Replacing an object, omitting a compilation record,
weakening flags or changing a custom object's source fails qualification.

The facade, C embedding API, dependency scanner and every registered CTest
inherit the profile. Serial frontend qualification reconciles the complete CTest
inventory, runs each test and reclaims passing executables. Compiler construction,
parser/sema failures, interface admission/recovery and session destruction retain
their existing semantic assertions. Source, object, library, bootstrap and policy
changes invalidate the result. Emitted-client/runtime qualification stays a
separate receipt with its own native dependency inventory.

SDK delivery can explicitly require this prerequisite:

```sh
python -B -m scripts.objc3c_toolchain build-sdk --fetch \
  --compiler-instrumentation-profile compiler-asan-ubsan
```

A failed prerequisite stops delivery, including when an ordinary SDK is cached.
The returned delivery report links the distinct compiler implementation receipt;
it does not relabel or mutate a cached ordinary archive's qualification.

## Boundaries and exclusions

LLVM's supported UBSan recipe excludes function/vptr checks for erased callback
and native type representations. Its pinned compile-time ignorelist additionally
covers specified libstdc++ tree/iterator/vector/shared-pointer header operations
(GCC #59829/#85930 and LLVM's sanitizer configuration). Four named BLAKE3 assembly
kernels cannot receive C/C++ instrumentation. The runner records these scopes,
source digests, upstream ownership and upgrade revalidation requirements. Runtime
suppressions and ambient option/loader overrides are not admitted.

This first compiler profile excludes clangd, clang-format and LLDB. Compiler TSan
and MSan remain unsupported here: no concurrent embedding contract or complete
MSan dependency closure is claimed. System libraries use ASan's supported native
interceptors; they are not described as rebuilt with instrumentation.

## Current evidence

The isolated foundation build, tests and stage complete in **2,105 seconds**.
Actual unchanged build-target reuse takes **1.27 seconds**, without rerunning
qualification or producing an SDK archive. The native profile passes **415
compiler regressions**, **223 selected native unit tests** and all **37 frontend
suites**. Three lit cases are unsupported; SourceTools/clang-tools-extra remain
outside this profile's declared project inventory.

The final graph verifies **3,439 instrumented objects**, five precompiled-header
inputs, four approved assembly kernels and **97 archives**. Exact ordered Ninja
inputs distinguish repeated object basenames and prevent substitution from an
unrelated target. P1689 dependency scans retain their helper/output identities
and cannot count as object compilation. Eight native graph/loader controls and
additional synthetic archive/scan/reuse controls defend the evidence boundaries.

A paired ASan control parses ObjC3 source and accesses memory inside its frontend
AST consumer. The intentional overflow requires its symbolized source site,
`HandleTranslationUnit` stack and exit 86; the corrected path must be clean.
No emitted client executable can satisfy this compiler-process control.

Execution caught a null VFS reference while deriving interface consumer ABI and
a fixture which destroyed mapped header strings before importing source locations.
The fixes initialize the admission compiler before diagnostic creation and retain
mapping owners through AST destruction. Warning-map admission/recovery is a
regression witness. Original failed receipts and compressed failing binaries/source
snapshots remain retained. All subsequent frontend tests pass.

Dedicated owned-runtime qualification additionally passes ASan/UBSan and TSan at
O0/O2, with all seventeen canonical service TUs and matching GNUstep dependencies.
Each project executable's loader graph confirms the selected native dependency.
Fresh source patch materialization, strict detector controls and standalone lock
contract CTests pass. This is separate from the compiler implementation profile.

The final qualification evidence interval is approximately **470 seconds**, with
**191 MB** of receipts/logs; frontend execution accounts for **294 seconds**. The
instance-wide memory high-water charge is 16 GiB including file cache, not an
isolated compiler RSS measurement.

For incremental development, `--embedding-build` accepts an existing isolated
embedding tree only when the foundation source/configuration and real bootstrap
tool identities match. The optional ccache PATH difference is excluded from that
ABI comparison; actual commands, object symbols/bytes, archive membership and all
selected tests are verified again. Every invocation writes a fresh evidence tree.

No ordinary SDK archive was rebuilt or promoted. Automated cadence and required
check acceptance remain tracked by #8283.
