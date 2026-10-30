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

The isolated planner and SDK prerequisite tests pass. Seven real native evidence
controls accept valid instrumentation and reject disabled/recovering flags,
missing or inconsistent compilation ownership, substituted ordinary objects and
incorrect custom source records. An actual loader control rejects a foreign
LLVM library even when its program returns the correct result. Thirteen isolated
archive controls also pass, including an archive produced by the native builder;
normal pytest execution is restored on the replacement instance. CMake rejects ordinary
foundation headers for
the instrumented embedding profile. Ordinary facade configuration and four
custom-runtime/assertion CTests pass after the profile integration.

The replacement instance completes the isolated foundation build, tests and stage
in **2,105 seconds**, passing **415 compiler regressions** and **223 selected native
unit tests**; three lit cases are unsupported. SourceTools/clang-tools-extra tests
remain outside this profile's declared project inventory. Full embedding/frontend
qualification is now running against that instrumented foundation. The original
interrupted I/O attempt and the archive-basename admission failure remain retained.
The latter motivated exact per-target Ninja membership controls, including a real
archive with duplicate basenames and rejected unrelated-object substitution.

No new SDK archive has been built for this work.
