# Darwin arm64 bootstrap investigation

macOS arm64 support is blocked under [#8229](https://github.com/doublemover/Slopjective-C/issues/8229).
The current compiler, runtime, packaging and release qualification target
Linux x86-64 and AArch64. The runtime profile verifier rejects `macos-arm64`
until an owned Objective-C 3 runtime and native qualification exist.

## Hosted bootstrap evidence

The first hosted Apple Silicon run completed the pinned bootstrap probe with
39 recorded commands and native static and dynamic Mach-O execution after
relocation. The receipt records `bootstrap-verified`. Objective-C 3 support
requires the runtime and SDK qualification below. [Hosted probe](https://github.com/doublemover/Slopjective-C/actions/runs/45281294132)

The [Darwin arm64 bootstrap probe](../../.github/workflows/darwin-bootstrap.yml)
runs on relevant pull request changes and supports manual dispatch. It selects
the standard `macos-15` Apple Silicon runner, Xcode 16.4 build `16F6`, macOS SDK
15.5, and native CPython 3.15.0. The job verifies the LLVM archive checksum and
calls the same probe used for a local replay. The `darwin-arm64-bootstrap-*`
artifact retains the host, download, and probe diagnostics on failure and the
receipt and fixture artifacts on success. The result records C toolchain
prerequisites with `support_truth: false`.

## Native bootstrap probe

Run this prerequisite check on an Apple Silicon Mac with macOS 15 or later,
native arm64 Python 3.11 or later, and a selected Xcode or Command Line Tools
installation with macOS SDK 15 or later. Use a writable directory on the
filesystem intended for the SDK. The probe records the selected developer
directory, Apple Clang, Apple linker and tools, SDK version, filesystem case
sensitivity, and the exact LLVM and Clang identities. The initial deployment
target for the C probe is macOS 15.0.

Use `zstd` and the upstream
[`LLVM-23.1.5-macOS-ARM64.tar.zst`](https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.5/LLVM-23.1.5-macOS-ARM64.tar.zst)
bootstrap archive. Check the archive's SHA-256 before extracting it.

```sh
printf '%s  %s\n' \
  503de30ef90311e0b143623801b19af2250adac6d69faed65ad0051170202e3b \
  LLVM-23.1.5-macOS-ARM64.tar.zst | shasum -a 256 -c
mkdir llvm-darwin-bootstrap
zstd --long=30 -dc LLVM-23.1.5-macOS-ARM64.tar.zst | tar -xf - -C llvm-darwin-bootstrap --strip-components=1
python3 -B -m scripts.objc3c_toolchain.darwin_bootstrap \
  --llvm-root "$PWD/llvm-darwin-bootstrap" --output-dir "$PWD/darwin-bootstrap-result"
```

The probe checks that the bootstrap tools contain an arm64 slice. It then
emits a C fixture object with `llc`, creates a Darwin archive and a Mach-O
dylib, verifies DWARF and a dSYM, and runs both static- and dynamic-linked
consumers after moving their tree to a path containing a space. It records
the load paths and rejects an unexpected library, including Apple `libobjc`.
Retain `darwin-bootstrap-result/report.json` with the generated artifacts. The
receipt records the checkout revision and probe digest. It covers bootstrap
capability and has `support_truth: false`.

## Experimental Objective-C 3 registration

The `macos-arm64-registration-experimental` build uses a Darwin-only compiler
patch on top of the locked LLVM source. It emits one Mach-O initializer per
translation unit that defines a root class. The initializer hands that unit's
class table to the owned runtime. Class sends use the metaclass method table,
and instance sends use the class method table through the live object's ISA.
Both lookups match the selector and exact no-argument `int` encoding. The compiler
driver reserves `-fobjc-runtime=gnustep-2.99` for Objective-C 3 on macOS arm64
and rejects ARC and Apple runtime linking for this profile.

From the same checkout and Xcode configuration as the bootstrap probe, put
CMake and Ninja on `PATH` and run the explicit experimental qualification.

```sh
python3 -B -m scripts.objc3c_toolchain.darwin_registration \
  --profile macos-arm64-registration-experimental --fetch \
  --bootstrap-report "$PWD/darwin-bootstrap-result/report.json" \
  --output-dir "$PWD/darwin-registration-result"
```

The command builds a native patched Clang with an AArch64 backend and limits
the compile pool to the host CPU and memory budget. It selects the active
Xcode libc++ headers and SDK link stub, then compiles, links and runs a
`std::hash<std::string>` probe before building Clang. The compiler build and
the native C++ service fixtures share that header and runtime pairing. The
receipts bind the bootstrap toolchain, compiler inputs, runtime source,
fixtures and outputs. The
native fixtures cover a stateless root class with Objective-C 3 `defer`, two
live instances, same-named class and instance methods, a nil send and nested
`self` dispatch. The provider's sibling class registers from the same archive
member. A late-loaded dylib adds a class to the same runtime, and the consumer
checks both class and instance sends through it. Both executables run after
moving the install tree. The controls check a missing runtime at link and
load, the prior GNUstep driver guard, a subclass, an instance ivar, a
parameterized method, ARC, Apple runtime linking, and a wrong architecture.

The qualifier also compiles the current concrete collection and text service
sources with the patched compiler and libc++. The existing map-count and text
boundary regressions run from the relocated tree with active checks. Their
`portable_services` receipt binds the source files, arm64 Mach-O objects,
executables and system C++ library dependencies.

Pull requests that change the experimental sources select the same workflow
step automatically. Manual dispatch selects it with `runtime_registration`.
The workflow passes `--package-sdk` to seal that same compiler and runtime,
then installs the archive into a fresh relocated root after removing the build
tree. The installed compiler recompiles the packaged consumers before the
runner checks their native execution and debug information.
An x86-64 replay built the modified driver and CodeGen translation units with
the preview toolchain. They emitted arm64 Mach-O provider and plugin objects
with startup initializers and owned runtime symbols. DWARF verified. The
class method lowers its deferred result to `42`, and the same-named instance
method returns `17`. The compiler diagnosed subclasses, instance ivars, and
parameterized methods. The allocation and lookup controls passed the separate
native Linux C runtime replay with ASan and UBSan.

Native execution evidence for this step is pending. The receipt has
`support_truth: false`, and the release profile still requires SDK packaging,
installation and native execution qualification. Loaded experimental libraries
must remain resident for the lifetime of their registered class records.

The first hosted runtime build reached `clang-tblgen` and failed at link.
The compiler used upstream LLVM's bundled libc++ headers, which requested
`std::__1::__hash_memory` from the selected Xcode SDK libc++. The repaired
preflight checks the matching Xcode headers and SDK runtime before configuration.
[Failed hosted attempt](https://github.com/doublemover/Slopjective-C/actions/runs/45289344512/job/138093814031)

### Instance lifetime and the next ABI boundary

The experimental MRC entry points `objc3_darwin_allocate_v1` and
`objc3_darwin_deallocate_v1` allocate and reclaim one ISA header with private
registry bookkeeping. Allocation admits the exact registered class pointer
and its stateless, zero-size compiler metadata. Dispatch recognizes only
registered class objects and live instances. The owner keeps an instance live
for the entire send, including nested sends and calls from other threads.
Deallocation removes the live registry entry and releases the allocation.

Superclass support needs dependency-ordered registration across images,
inherited lookup, and explicit super dispatch. A stateful ABI must resolve
the compiler's relative instance-size and ivar-offset records, validate
alignment and overflow, and initialize the resulting storage before it becomes
visible to a sender. Native controls need separately linked subclasses,
cross-image parent registration, and invalid or stale layout records.

ARC requires retain and release operations with a defined finalization path,
weak-slot synchronization, autorelease pools, and exception-aware cleanup.
Those operations need ABI and compiler lowering tests alongside the Mach-O
loader tests. They also need native lifetime evidence for instances and Blocks
that cross image and thread boundaries.

## Evidence required for a port

- A native Apple Silicon host or a GitHub Actions `macos-15` arm64 runner,
  access to the pinned LLVM source and bootstrap dependencies, and permission to
  run and retain a full native qualification. The hosted route needs repository
  Actions workflow dispatch and artifact upload access. Local qualification
  needs no repository administration permission. The bootstrap runner starts
  with Xcode 16.4 and macOS SDK 15.5 on the
  [Apple Silicon macOS 15 runner image](https://github.com/actions/runner-images/blob/main/images/macos/macos-15-arm64-Readme.md).
- A single owned Objective-C 3 runtime ABI on Mach-O. The current GNUstep 2.2
  runtime and ARC, exceptions, async and carrier paths have Linux-specific
  gates. The released driver rejects GNUstep 2.2 for Mach-O. Its GNUstep
  registration emits ELF section boundaries, COMDAT groups and an ELF
  constructor section. The separate experimental compiler handles stateless root
  classes and instances with ordinary no-argument `int` methods. Inheritance,
  instance storage, properties, protocols, categories, ARC, exceptions and
  cross-image runtime lifecycle still need a design and native workloads. Inspect the linked
  dependency closure for accidental Apple ObjC2 use. [Driver guard](https://github.com/llvm/llvm-project/blob/392dfff97d29b59d77e5cac6d4b6ef2585fe7117/clang/lib/Driver/ToolChains/Clang.cpp#L8628)
  · [Registration lowering](https://github.com/llvm/llvm-project/blob/392dfff97d29b59d77e5cac6d4b6ef2585fe7117/clang/lib/CodeGen/CGObjCGNU.cpp#L1518)
- A native patched Clang and runtime build plan, a Mach-O distribution layout
  and debug inspection, and an install/update/rollback/uninstall path with
  versioned receipts. Run the public compiler and package commands from clean
  installs, including hosts with case-insensitive paths and restrictive
  installation permissions.
- Native negative cases for unsupported SDK/tool combinations, wrong
  architecture, missing runtime, invalid loader path, mismatched debug data
  and failed installation. Bind the results to the exact build, package and
  install used for the positive runs. Review that evidence before changing
  `platform.darwin-arm64.unsupported`.
