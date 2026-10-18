# Native Clang compiler

`objc3c` is the public Linux compiler. The default CMake build uses only the
qualified LLVM/Clang 23.1.3 frontend, with native C, C++, Objective-C and
Objective-C++ types, ownership and ABI. The former optional migration build
switch, custom parser/lowering and custom runtime integration have been removed.

Use `python -m scripts.objc3c_toolchain build-sdk --fetch` from the repository
root to build, test, install, qualify, archive and activate the complete SDK.
The [root README](../../../../README.md) describes public commands and support.
For focused frontend development against an existing qualified SDK:

```sh
sdk="$PWD/out/toolchains/objc3-development/stage-linux-x86_64"
cmake -S native/objc3c -B out/toolchains/objc3-facade/build-linux-x86_64 -G Ninja \
  -DOBJC3C_CLANG_FOUNDATION_ROOT="$sdk" \
  -DCMAKE_CXX_COMPILER="$OBJC3C_BOOTSTRAP_LLVM_ROOT/bin/clang++"
cmake --build out/toolchains/objc3-facade/build-linux-x86_64 --parallel 4
ctest --test-dir out/toolchains/objc3-facade/build-linux-x86_64 --no-tests=error --output-on-failure
```

The standard driver supports compilation and linking, multiple translation
units, debug information and Clang dependency options. The semantic-interface
operations and the C embedding API share an in-process compile session. Interface
operations take a source before normalized options; raw Clang arguments follow
`--`. For example:

```sh
objc3c provider.m --objc3 --arc --emit-object Provider.o \
  --emit-interface Provider.objc3i --module Provider --package example.provider \
  -- -O2 -fblocks
objc3c consumer.m --objc3 --arc --emit-object Consumer.o \
  --module-search-path modules --import-module Provider FINGERPRINT example.provider \
  --source-imports --emit-dependency-record Consumer.dependencies.json
objc3c Consumer.o Provider.o -o application
```

Installed frontends discover their adjacent SDK or accept `--sdk-root`. Driver
and resource content hashes must match the SDK linked into the frontend. The
C API's `compiler_sdk_root` selects the same verified relocation. Installed
`objc3c::Frontend` and `objc3c::RuntimeServices` CMake targets provide explicit
embedding and independent native service libraries. Clang C++ implementation
types do not cross the C embedding API.

Public capability claims follow
[`schemas/objc3-language-capabilities.json`](../../../../schemas/objc3-language-capabilities.json).
`defer` is qualified for Linux x86-64/GNUstep 2.0/compatibility under ARC and
MRC. Optional/Result, synchronous typed errors and match fixtures remain bounded
native migration facilities; their complete public syntax is pending.
