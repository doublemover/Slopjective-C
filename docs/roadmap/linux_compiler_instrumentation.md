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
ownership, then checks object bytes and actual instrumentation symbols. Custom
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

The isolated planner and SDK prerequisite tests pass. Six real native evidence
controls accept valid instrumentation and reject disabled/recovering flags,
missing or inconsistent compilation ownership, substituted ordinary objects and
incorrect custom source records. CMake rejects ordinary foundation headers for
the instrumented embedding profile. Ordinary facade configuration and four
custom-runtime/assertion CTests pass after the profile integration.

The full instrumented LLVM/Clang build is underway. Compiler implementation
execution qualification remains open until that build and the runner above pass.
No new SDK archive has been built for this work.
