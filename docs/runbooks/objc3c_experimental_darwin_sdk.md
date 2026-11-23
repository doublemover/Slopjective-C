# Experimental Objective-C 3 SDK on Apple Silicon

The experimental SDK packages the patched Apple Silicon Clang driver and the
private Objective-C 3 registration runtime. The profile covers stateless
Objective-C root classes and no-argument `int` methods under manual reference
counting. The package receipt carries `support_truth: false`. [Registration
profile](../../toolchains/llvm/experimental/darwin-registration.json)

Run the producer on native Apple Silicon with macOS 15.0 or later, Xcode 16.4
build `16F6`, the macOS 15.5 SDK, and the pinned LLVM 23.1.5 ARM64 bootstrap.
Select the active Xcode with `DEVELOPER_DIR` if needed. Use the existing
bootstrap instructions to verify its LLVM archive and produce the bootstrap
receipt. The producer also requires Python 3.15, CMake and Ninja. Installation
and consumers require native arm64 Python 3.11 or later and the same Xcode tool
and SDK identities. [Bootstrap instructions](objc3c_darwin_arm64_bootstrap.md)

## Build and package

Pass `--package-sdk` to the experimental registration producer. Output must be
fresh. The producer assembles the SDK while its verified native compiler is
available, then qualifies the archive from an isolated, relocated installation.

```sh
python3 -B -m scripts.objc3c_toolchain.darwin_registration \
  --profile macos-arm64-registration-experimental --fetch --package-sdk \
  --bootstrap-report "$PWD/darwin-bootstrap-result/report.json" \
  --output-dir "$PWD/darwin-registration-result"
```

The archive and detached `package.json` are under
`darwin-registration-result/sdk/package`. The archive embeds a relative-path
inventory with file permissions and SHA-256 digests, compiler source and
configuration identity, bootstrap identity, and the private runtime ABI. Its
contents are the native Clang driver, `llvm-ar`, `llvm-dwarfdump`, Clang resource
headers and arm64 builtins, both private runtime libraries, the registration
example, and licenses. The selected Xcode and its SDK remain host prerequisites.

## Install and replay

Read the expected archive SHA-256 from the trusted package receipt. The install
command checks it, admits the sealed native payload, and publishes a fresh prefix
only after validation. It creates the prefix inside an existing parent directory.
Use a different path for each installation.

```sh
package="$PWD/darwin-registration-result/sdk/package"
archive="$package/objc3-darwin-sdk.tar.gz"
archive_sha=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["archive_sha256"])' "$package/package.json")

python3 -B -m scripts.objc3c_toolchain.darwin_sdk install \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --prefix "$PWD/Objective-C 3 Experimental SDK"

python3 -B -m scripts.objc3c_toolchain.darwin_sdk qualify \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --output-dir "$PWD/darwin-sdk-replay"
```

`qualify` installs into another fresh prefix whose path contains spaces, then
uses the installed Clang driver and resource directory to compile the packaged
source example. It checks Mach-O arm64 objects, the Darwin archive, DWARF, private
runtime install names and relative load paths. It links and executes static and
shared-runtime consumers, including a late-loaded fixture. It also checks that
ARC and x86-64 compilation and missing-runtime linking produce the expected
diagnostics. Successful evidence lives in `qualification.json`, together with
exact commands and native exit statuses.

Local tests cover the archive format, relocation structure, manifest and content
hashes, path admission, wrong-architecture files, and host prerequisites. The
cold Apple Silicon compiler build, package, install, and execution remain the
platform acceptance gate. [Package owner](../../scripts/objc3c_toolchain/darwin_sdk.py)
