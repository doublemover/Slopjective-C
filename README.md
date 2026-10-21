# Objective-C 3.0

Slopjective-C develops a native Objective-C 3 compiler on a patched LLVM/Clang
SDK. `objc3c` is the public compiler. It uses Clang's parser, semantic analysis,
AST, ownership model, native CodeGen and linker driver. Linux x86-64 with the
GNUstep 2.2 ABI and pinned libobjc2 runtime is the active distribution target.

## Build and use

The authoritative versions are in
[`toolchains/llvm/manifest.json`](toolchains/llvm/manifest.json). The current
foundation is LLVM 23.1.3. Development requires Python 3.15.0, Node 26.12.0,
npm 12.2.1, CMake 4.4.4 and Ninja 1.13.2. A compiler cache is recommended.
Native SDK development also needs the system OpenSSL 3.5+, zlib and zstd
development headers.
PowerShell is not part of the Linux compiler build or qualification pipeline.

In the managed Linux development environment:

```sh
bash scripts/setup_cloud_environment.sh
source /workspace/.cloud-setup/Slopjective-C/activate.sh
npm run objc3c -- build-sdk --fetch
source out/toolchains/linux-sdk/activate.sh
objc3c tests/clang_foundation/installed_sdk/main.objc3 -O2 -g -o application
./application
```

`build-sdk` builds and tests the sealed LLVM fork, pinned native runtime and
frontend; installs an isolated candidate; qualifies the installed compiler,
embedding API, semantic interfaces, runtime and editor tools; creates a sealed
archive; and publishes a content-addressed installation. Logs and phase timings
live under `out/toolchains/linux-sdk-builds/`. Incremental Ninja builds and a
persistent compiler cache are reused. The product SDK and the bootstrap compiler
have separate roots; `OBJC3C_BOOTSTRAP_LLVM_ROOT` pins the latter.

The compiler accepts normal Clang compilation, linking, diagnostics and
Makefile dependency options. `.objc3` selects Objective-C 3 with ARC and Blocks.
Use `-fno-objc-arc` for explicit manual ownership, `.mm` for Objective-C++, and
`-c`, `-S`, `-E` or `-fsyntax-only` for individual compilation stages.

```sh
objc3c main.objc3 helpers.c -O2 -MMD -MF application.d -o application
objc3c library.m --objc3 --arc --emit-object library.o \
  --emit-interface Library.objc3i --module Library --package example.library
```

Semantic interface imports use locked module identities and can publish a
machine-readable dependency record. The same in-process frontend is available
through the installed C embedding API and `find_package(objc3c CONFIG)`:

```cmake
find_package(objc3c REQUIRED CONFIG)
target_link_libraries(my_embedding PRIVATE objc3c::Frontend)
```

Pass the installation through `CMAKE_PREFIX_PATH`. See the
[frontend API and interfaces](native/objc3c/src/clang_foundation/README.md) and
[LLVM foundation workflow](toolchains/llvm/README.md) for details.

## Install a release archive

An installation needs no checkout, Node, PowerShell or development Python.
On an apt-based Linux host, the release installer can provision its distribution
Python, decompressor, system headers and loader dependencies:

```sh
sh install-linux.sh --provision --archive objc3-sdk.tar.zst \
  --sha256 SHA256_FROM_RELEASE_RECEIPT --root /opt/objc3c
. /opt/objc3c/activate.sh
objc3c --version
```

Run provisioning as root; omit `--provision` when prerequisites are installed.
Release installers are shipped under `share/objc3c/` in the archive and alongside
CI release artifacts. Installations live under `sdks/<full-content-seal>`;
activation atomically switches `current`. Existing prefixes are verified before
reuse. The standalone verifier checks all file contents, modes and relative
symbolic links after extraction or relocation. Directory permissions are also
sealed; archives and activation files support root installation followed by
ordinary-user development, including under a restrictive installation umask.

The current binary qualification baseline is Debian 13, x86-64, glibc 2.41 and
libstdc++ with GLIBCXX_3.4.33. The shipped archive is also qualified on Ubuntu
26.04. Other distributions require their own installed archive execution
evidence before being declared supported. Windows, Darwin,
other architectures and older Linux runtimes are outside this milestone.

The installed sample combines semantic interface imports, managed objects and
Blocks, typed errors and `defer` cleanup. It uses the bounded native migration
intrinsics described below. Both provider and consumer are built from installed
source outside the checkout; no precompiled provider is needed:

```sh
python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/build.py" \
  --output-dir ./managed-sample --optimization O2
```

The build writes fresh MRC and ARC semantic interfaces, a locked v2 consumer
dependency record and `build-report.json`. Repeated builds reuse compilation
stages only when source, transitive header, SDK, flags and output hashes match.
Reuse and dependency records bind the concrete source and installation paths.
Private provider edits can retain the consumer's interface identity; API edits
invalidate its compilation. Every invocation links and runs the application.
Full SDK verification occurs before and after building, with its time reported
separately from compilation and linking.

Copy `share/objc3c/examples/managed/src` into a working directory and pass
`--source-dir ./my-source` to edit the example. Keep `--output-dir` separate from
both sources and the SDK. Run the installed invalidation and identity suite with:

```sh
python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/qualify.py" \
  --output-dir ./module-qualification
```

This is an installed example build graph; a general package manager and public
language module syntax remain future work. The
[source-module acceptance report](docs/support/linux_source_module_acceptance.json)
records the qualified invalidation cases and their timing scope.

## Offline source-package graphs

`objc3c-build` extends the installed example into explicit local package graphs:

```sh
cp -R "$OBJC3C_SDK_ROOT/share/objc3c/examples/packages" ./packages
objc3c-build --workspace ./packages/workspace.json \
  --output-dir ./package-build --optimization O2
```

Packages carry exact identity locks and public/private dependency edges. The
builder resolves a shared dependency once, emits locked native interfaces,
checks transitive visibility, reuses content-verified compilation, and relinks
and runs the application. It retains a build report and a content-identified
package installation inventory. The same operation is exposed through
`npm run objc3c -- build-packages`.

The v3 package contract requires explicit language, standard and ownership locks.
It supports Objective-C C17 and C++20/C++23 in Objective-C++ or plain C++ mode,
with exact mode matching across every dependency. Packages use local sources;
remote fetching, mode adaptation and public module syntax remain separate work.
See the [source-package contract](docs/roadmap/linux_source_packages.md).
The [package SDK acceptance receipt](docs/support/linux_source_graph_acceptance.json)
identifies the archive, code hashes and installed/fresh-distribution evidence.

## Language status

[`schemas/objc3-language-capabilities.json`](schemas/objc3-language-capabilities.json)
is the authority for public extensions; its generated
[Clang capability report](docs/support/clang_foundation_capabilities.json)
and compiler feature queries must agree. Public `defer { ... }` is qualified for
Linux x86-64, GNUstep 2.2, compatibility profile, with ARC and MRC. Cleanup runs
in reverse registration order on returns, structured exits and native exception
unwinding.

```objc
#include <stdio.h>
int main(void) {
  FILE *file = tmpfile();
  if (!file) return 1;
  defer { fclose(file); }
  fputs("Objective-C 3 on Linux\n", file);
  return 0;
}
```

The [public carrier contract](docs/roadmap/linux_public_carriers.md) defines
`Optional(T)`, `Result(T, E)`, constructors, tag queries and exhaustive `match`.
The [synchronous error contract](docs/roadmap/linux_public_errors.md) adds
`throws(E)`, checked calls and typed handlers. Native Clang implements their
qualified ownership, ABI and cleanup operations. Objective-C++ ARC locals support
explicit `copyValue` and `moveValue` construction for direct strong, weak
and unsafe object payloads and strong Block payloads. Copies preserve their
source; mutable strong and weak moves clear its payload while preserving the
carrier tag. Moving a const strong payload copies it. Inactive arms are never
read, and native ARC cleanup applies on normal exits and exception unwinding.
Objective-C and Objective-C++ ARC locals also support implicit copies and
assignment of these direct carriers. Assignment preserves the selected RHS
before evaluating the destination and releasing its old active payload, so
self-assignment, overlapping aliases and Result arm changes are safe. Copies
preserve the source, including const sources. Objective-C++ assignment returns
the destination lvalue and cleans up its snapshot if evaluating that destination
throws. The installed
`share/objc3c/examples/native/carrier-transfer.mm` witness exercises these
rules together with typed-error optionalization and `defer` at O0 and O2.
`share/objc3c/examples/native/carrier-assignment.m` qualifies implicit copying,
replacement, weak zeroing, Blocks, sequencing and native exceptions in both
Objective-C and Objective-C++ at O0 and O2.

Linux ARC functions now pass and return these direct carriers by value under
extension ABI revision 10. The [recursive carrier contract](docs/roadmap/linux_recursive_carriers.md)
extends ownership through nested Optional/Result values, managed match snapshots
and optionalization of managed results. Each argument gets a caller-owned snapshot when it is
evaluated; that snapshot survives later argument evaluation and is destroyed at
the full-expression boundary, including exception exits. Results initialize
fresh caller-owned storage. Separate provider/consumer witnesses cover ordinary
functions, callbacks, C++ members and virtual adjustment thunks at O0, O2 and
ThinLTO, plus PCH and semantic module imports. Run the installed suite with:

```sh
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-carrier-calls.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./carrier-call-evidence
```

Objective-C messages and Block invocation now use the same ownership rules for
the [runtime call subset](docs/roadmap/linux_runtime_call_abi.md). Versioned type
encodings distinguish strong, weak and unsafe storage. Installed witnesses cover
class, instance, protocol and superclass dispatch, nil receivers, heap
Blocks, native exceptions and runtime reflection. Run their nine configurations
with `share/objc3c/examples/native/qualify-runtime-carrier-calls.py`, using the same
`--sdk-root` and `--output-dir` options as the function suite.

The [managed typed-error contract](docs/roadmap/linux_managed_typed_errors.md)
composes those calls with synchronous typed errors, propagating and forced try,
catch recovery and native unwinding. Failed assignment preserves its destination;
callee cleanup destroys completed results when a native exception interrupts
return. Run `share/objc3c/examples/native/qualify-managed-throws.py` with the same
SDK/output options for nine executions and eighteen forced-failure checks.

The [managed call contract](docs/roadmap/linux_managed_call_abi.md) defines the
ownership and target bounds. [Owned Block captures](docs/roadmap/linux_owned_block_captures.md) now preserve
active-arm lifetime through stack/heap closures and exceptional copying. The [managed record contract](docs/roadmap/linux_managed_records.md) extends
active-arm ownership through enclosing C and C++ records and fixed-size record
arrays. Ordinary synchronous functions use callee-owned record snapshots and a
common C/C++ indirect ABI. The [heap storage contract](docs/roadmap/linux_heap_storage.md)
extends ownership into ivars, nonatomic properties, whole-record runtime calls
and shared Block captures, including constructor rollback. Volatile carriers
and managed MRC transfers remain outside the qualified contract. GNUstep 2.2 supports [true direct dispatch](docs/roadmap/linux_direct_dispatch.md),
including lazy class initialization and managed typed-error calls.

The [C++ interface contract](docs/roadmap/linux_cxx_interfaces.md) extends semantic
interchange to bounded global APIs and public data records, with exact language,
standard, ownership and linkage identity. C++20 and C++23 consumers cover C/C++
linkage, overloads, owned records, source imports and ThinLTO.

The [public module surface contract](docs/roadmap/linux_public_module_surfaces.md)
selects exported roots, rejects private type leaks and preserves dependent build
reuse across private implementation edits.

The [named SPI contract](docs/roadmap/linux_module_spi.md) preserves privileged
module partitions and exact-provider grants through native imports, source
builds and relocated immutable packages. Re-exports retain public visibility
without forwarding privileges.

The [module-owned naming contract](docs/roadmap/linux_module_owned_names.md)
binds native functions, globals, classes, protocols and categories to their
module and package identity. Independent providers can reuse private source
names. Canonical interfaces, ownership projections, immutable packages and the
Linux debugger preserve those bindings.

The [structured runtime API](docs/roadmap/linux_structured_runtime.md) provides
public C task groups and serial actor mailboxes, including nested cancellation,
completion polling, logical hop facts and cleanup-aware joins. Native worker
execution and instrumented ASan, LeakSanitizer, UBSan and ThreadSanitizer checks
cover O0/O2 behavior.

The [resumable task bridge](docs/roadmap/linux_task_coroutines.md) connects native
C++20/23 and Objective-C++ coroutine frames to the task ABI. Awaiting queues the
continuation on the parent's executor; cancellation, abandonment, retained values,
Block captures and captured Objective-C exceptions preserve native cleanup.
Worker callbacks drain autoreleased temporaries.

The [task destination bridge](docs/roadmap/linux_task_hops.md) adds actor-bound
starts, queued executor and actor hops, and inherited child destinations. Service
ABI 3 requires rebuilding runtime clients, coroutine providers and packages.

The [native async compiler stage](docs/roadmap/linux_native_async.md) lowers
logical-result free functions and native awaits into Objective-C++20/23 task
frames, including nested private children, deferred cleanup and source-free
package imports. It uses the reserved `__objc3_async` and `__objc3_await` spellings.
Extension ABI 14 requires every object, PCH, semantic interface and package to be
rebuilt. Public async/await and compiler actor isolation remain reserved.

The [public availability contract](docs/roadmap/linux_api_availability.md) keeps
native deprecation, unavailability and platform metadata in semantic identities
and preserves their diagnostics in source-free consumers. Public metadata edits
invalidate dependent compilations.

The [ownership projection workflow](docs/roadmap/linux_ownership_projections.md)
connects closed ARC and MRC C APIs through explicit native return ownership.
Consumer-mode interfaces bind their original producer contract; source graphs and
immutable packages record the actual provider mode and preserve exact mode locks.
Interface schema 2, source manifests v4 and binary packages v3 require regeneration.

The [immutable package workflow](docs/roadmap/linux_immutable_packages.md) publishes
verified native dependency closures into local content-addressed stores. Consumers
compile against relocated installations after producer sources are removed, with
exact SDK, language, standard and ownership locks.

Managed payload generalization, async/await and actors require further
implementation and qualification. Private compiler fixtures do not establish
additional public capabilities. Generic callable metadata
and interfaces have explicit supported subsets; they do not establish complete
language-evolution readiness.

The former custom parser, `fn` module dialect and its historical capability
inventory no longer describe the public compiler. Historical roadmap and
support reports remain in the repository as records of earlier work.

## Validation and development

```sh
python -m scripts.objc3c_toolchain check-host-tools --runtime
python -m scripts.objc3c_native_validation
python -m pytest -q tests/tooling/test_toolchain_package.py
python -m scripts.objc3c_toolchain.generate_features --check
python -m scripts.measure_objc3c_linux --sdk "$OBJC3C_SDK_ROOT" --report tmp/benchmarks/development.json
python -m scripts.measure_objc3c_linux --sdk "$OBJC3C_SDK_ROOT" \
  --budget-report docs/support/linux_public_core_latency.json --report tmp/benchmarks/regression.json
```

Native validation checks O0/O2 behavior, ABI, ownership, cleanup, expected
failures, installed interface consumers and editor tools. Evidence records bind
source, compiler, runtime, system headers and executable contents and reject
changes during validation. The
[Linux SDK acceptance receipt](docs/support/linux_task_hops_acceptance.json)
records the qualified archive and fresh-host checks; each new SDK has its own
qualification and release receipt. Runtime timings must be distinguished from
compile and wrapper timings when comparing performance.
The [current latency report](docs/support/linux_task_hops_latency.json) records
warm command measurements and regression budgets for its AMD CPU model and quota.
The [historical paired report](docs/support/linux_development_latency.json) retains
the earlier Intel measurements; cross-CPU speedup comparisons are inapplicable.
Use a separate output report when checking those budgets.

The [forward development milestones](docs/roadmap/linux_forward_development.md)
identify the remaining public-language, module, concurrency and architecture work.

The [managed array contract](docs/roadmap/linux_managed_arrays.md) adds local
multidimensional carrier arrays, initialization failure cleanup, and weak/unsafe
Block payloads, introduced in ABI 9. Its installed qualification also checks reverse
partial destruction, imports, PCH, ThinLTO and rejected storage boundaries.
