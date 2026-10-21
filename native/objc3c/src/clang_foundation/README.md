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

The installed `share/objc3c/examples/managed/build.py` provides a source-built
provider/consumer workflow. It compiles the MRC implementation, derives an ARC
interface from exported declarations, and compiles an ARC consumer against the
locked identity. Clang depfiles and content hashes control incremental reuse;
the consumer emits a v2 semantic dependency record. The installed `qualify.py`
checks O0/O2 builds, implementation versus API invalidation, preserved timestamps,
damaged artifacts, reproducible dependency records and rejected package,
fingerprint and ownership locks outside the checkout.

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

The native typed-error milestone includes direct `try?` initialization of strong
ARC object and Block Optional locals, plus strong Block payloads in direct
Optional/Result local construction. Presence queries borrow the active tag.
O0/O2 runtime cases exercise retained and autoreleased returns, callbacks,
throwing Blocks, C++ members, Objective-C messages, failure arms and native
exception cleanup. MRC managed `try?`, weak/unsafe Block carriers
remain outside this slice.

Explicit `__objc3_copy`/`__objc3_move` construction of Objective-C++ ARC locals
uses native strong, weak and unsafe object initialization and strong Block
initialization. The source is evaluated once and only its active arm is read.
Mutable strong/weak moves clear the source payload while retaining its tag;
const strong moves copy the payload. IR/PCH, O0/O2 ownership/unwind execution,
installed typed-error/transfer consumers and live clangd probes qualify this
bounded construction contract.
Implicit ARC copies and assignment now support those direct carrier leaves in
Objective-C and Objective-C++. Assignment retains/copies the selected RHS into
fresh storage before evaluating the LHS or destroying its old active payload.
Native strong/Block initialization and weak copy/move/destroy operations handle
replacement and exact self-aliasing. C++ assignment preserves the destination
lvalue; snapshots clean up when LHS evaluation throws. Installed O0/O2 witnesses
and live editor probes cover both languages.

The Linux direct-carrier call contract uses extension ABI revision 8. Ordinary
functions, callbacks and C++ members pass caller-owned argument snapshots by
pointer; callees borrow and may replace those snapshots. Snapshots clean up at
the caller's full-expression boundary, including later-argument and callee
exceptions. Returns initialize fresh caller-owned storage. The installed
`qualify-carrier-calls.py` suite executes separate provider/consumer builds at
O0/O2/ThinLTO, semantic imports and PCH, including weak storage, virtual
adjustment thunks and register exhaustion. Objective-C messages and Block
invocation now support the runtime-encoded direct-carrier subset. A second
installed suite, `qualify-runtime-carrier-calls.py`, covers reflection, class and
instance dispatch, protocol/super/direct calls, nil receivers, heap Blocks and
native Objective-C/C++ exceptions, with matching PCH, ThinLTO and module checks.
Live clangd probes include returned message and Block values. Synchronous typed
errors and recursive Optional/Result ownership now compose with these calls,
including owned match snapshots and optionalization of managed results.
[Owned Block value captures](../../../../docs/roadmap/linux_owned_block_captures.md)
now use active-arm helpers and exception-safe runtime copying. Ivar/reference
capture storage, arrays/enclosing records and volatile managed storage remain
diagnostic boundaries. The
[call contract](../../../../docs/roadmap/linux_managed_call_abi.md) records the
target and ownership limits; the
[runtime contract](../../../../docs/roadmap/linux_runtime_call_abi.md) specifies
the encoding and dispatch subset.
The editor and interface consumers use the same sealed SDK and Linux target.
