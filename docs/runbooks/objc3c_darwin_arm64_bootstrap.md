# Darwin arm64 bootstrap investigation

macOS arm64 support is blocked under [#8229](https://github.com/doublemover/Slopjective-C/issues/8229).
The current compiler, runtime, packaging and release qualification target
Linux x86-64 and AArch64. The runtime profile verifier rejects `macos-arm64`
until an owned Objective-C 3 runtime and native qualification exist.

## Native bootstrap probe

Run this prerequisite check on an Apple Silicon Mac with macOS 15 or later,
native arm64 Python 3.11 or later, and a selected Xcode or Command Line Tools
installation with macOS SDK 15 or later. Use a writable directory on the
filesystem intended for the SDK. The probe records the selected developer
directory, Apple Clang, Apple linker and tools, SDK version, filesystem case
sensitivity, and the exact LLVM and Clang identities. The initial deployment
target for the C probe is macOS 15.0.

Use the upstream
[`LLVM-23.1.5-macOS-ARM64.tar.xz`](https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.5/LLVM-23.1.5-macOS-ARM64.tar.xz)
bootstrap archive. Check the archive's SHA-256 before extracting it.

```sh
printf '%s  %s\n' \
  aba592b4a15e8f90c634c5367b51ec8418b8bcd2829865db331a17dc9418dcff \
  LLVM-23.1.5-macOS-ARM64.tar.xz | shasum -a 256 -c
mkdir llvm-darwin-bootstrap
tar -xf LLVM-23.1.5-macOS-ARM64.tar.xz -C llvm-darwin-bootstrap --strip-components=1
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

## Evidence required for a port

- A native Apple Silicon host or a GitHub Actions `macos-15` arm64 runner,
  access to the pinned LLVM source and bootstrap dependencies, and permission to
  run and retain a full native qualification. The hosted route needs repository
  Actions workflow dispatch and artifact upload access. Local qualification
  needs no repository administration permission. The first hosted probe can use
  Xcode 16.4 and macOS SDK 15.5 on the
  [Apple Silicon macOS 15 runner image](https://github.com/actions/runner-images/blob/main/images/macos/macos-15-arm64-Readme.md).
- A single owned Objective-C 3 runtime ABI on Mach-O. The current GNUstep 2.2
  runtime and ARC, exceptions, async and carrier paths have Linux-specific
  gates. GNUstep module registration on Linux uses ELF section boundary
  symbols. The Darwin port needs Mach-O sections and matching runtime
  registration. Qualify these paths with native workloads and inspect the
  linked dependency closure for accidental Apple ObjC2 use.
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
