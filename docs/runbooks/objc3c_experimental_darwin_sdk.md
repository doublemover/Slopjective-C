# Experimental Objective-C 3 SDK on Apple Silicon

The experimental SDK packages the patched Apple Silicon Clang driver and the
private Objective-C 3 registration runtime. The profile covers class, instance,
and `super` dispatch for `int` methods with up to two signed `int` arguments.
A root class can declare one fixed `int` field. The lifetime checks exercise
manual allocation and deallocation as well as ARC strong references, zeroing
weak references, and thread-local autorelease pools. The package receipt carries
`support_truth: false`. [Registration
profile](../../toolchains/llvm/experimental/darwin-registration.json)

Run the producer on native Apple Silicon with macOS 15.0 or later, Xcode 16.4
build `16F6`, the macOS 15.5 SDK, and the pinned LLVM 23.1.5 ARM64 bootstrap.
Select the active Xcode with `DEVELOPER_DIR` if needed. Use the existing
bootstrap instructions to verify its LLVM archive and produce the bootstrap
receipt. The producer also requires Python 3.15, CMake and Ninja. Installation
and consumers require native arm64 Python 3.11 or later and the same Xcode tool
and SDK identities. [Bootstrap instructions](objc3c_darwin_arm64_bootstrap.md)

## Build and package

The `darwin build-sdk` command builds the experimental registration profile and
packages the SDK. Output must be fresh. The producer assembles the SDK while its
verified native compiler is available, then qualifies the archive from an
isolated, relocated installation.

```sh
python3 -B -m scripts.objc3c_toolchain darwin build-sdk \
  --fetch \
  --bootstrap-report "$PWD/darwin-bootstrap-result/report.json" \
  --output-dir "$PWD/darwin-registration-result"
```

The archive and detached `package.json` are under
`darwin-registration-result/sdk/package`. The archive embeds a relative-path
inventory with file permissions and SHA-256 digests, compiler source and
configuration identity, bootstrap identity, and the private runtime ABI. Its
contents are the native Clang driver, `llvm-ar`, `llvm-dwarfdump`, Clang resource
headers and arm64 builtins, both private runtime libraries, the registration
and ARC examples, and licenses. The portable collections and text static
library ships with three public headers under `include/objc3c/runtime` and a C++ example
under `share/objc3c/examples/collections-text`. The selected Xcode and its SDK
remain host prerequisites.

## Install and replay

Read the expected archive SHA-256 from the trusted package receipt. The install
command checks it, admits the sealed native payload, and publishes a fresh prefix
only after validation. It creates the prefix inside an existing parent directory.
Use a different path for each installation.

```sh
package="$PWD/darwin-registration-result/sdk/package"
archive="$package/objc3-darwin-sdk.tar.gz"
archive_sha=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["archive_sha256"])' "$package/package.json")

python3 -B -m scripts.objc3c_toolchain darwin install \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --prefix "$PWD/Objective-C 3 Experimental SDK"

python3 -B -m scripts.objc3c_toolchain darwin qualify \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --output-dir "$PWD/darwin-sdk-replay"
```

`qualify` installs into another fresh prefix whose path contains spaces, then
uses the installed Clang driver and resource directory to compile the packaged
source example. It checks Mach-O arm64 objects, the Darwin archive, DWARF, private
runtime install names and relative load paths. It links and executes static and
shared-runtime consumers, including a late-loaded fixture. An optimized ARC
consumer confirms that dyld loads the owned runtime from the installed SDK.
The installed C++ example exercises map replacement and Unicode scalar slice
boundaries. ARC with Objective-C exceptions, x86-64 compilation, and
missing-runtime linking produce the expected diagnostics. Successful evidence
lives in `qualification.json`, with exact commands and native exit statuses.

Local tests cover the archive format, relocation structure, manifest and content
hashes, path admission, wrong-architecture files, and host prerequisites. The
cold Apple Silicon compiler build, package, install, and execution remain the
platform acceptance gate. [Package owner](../../scripts/objc3c_toolchain/darwin_sdk.py)

## Managed SDK root

Use `--root` to keep a stable `current` SDK path. A repeat command verifies and
reuses the admitted archive. A command with a different admitted package
switches `current` to the new seal and reclaims the previous SDK. Complete jobs
using the previous SDK before switching. Use a dedicated root with an existing
writable parent.

```sh
root="$PWD/Managed Objective-C 3 SDK"
python3 -B -m scripts.objc3c_toolchain darwin install \
  --archive "$archive" --archive-sha256 "$archive_sha" --root "$root"
"$root/current/bin/clang" --version
```

Retain the `seal_sha256` from the install result. Pass that exact seal when
removing the managed SDK. Uninstall verifies the payload and leaves unrelated
root files in place. Edited or additional payload files cause rejection.

```sh
python3 -B -m scripts.objc3c_toolchain darwin uninstall \
  --root "$root" --seal-sha256 "$installed_seal"
```

If activation returns `cleanup_error`, the new SDK is active. Repeating that
install retries cleanup of the previously admitted SDK.

## Re-admit a retained native package receipt

Use the checkout revision recorded by the workflow and download its native
evidence and terminal receipt. Supply the expected byte digest of the
terminal receipt from the reviewed evidence record. The command replays the
native checks and compares the result with the pinned receipt. A matching
result carries `admission: experimental-package-verified` and
`support_truth: false`.

```sh
python3 -B -m scripts.objc3c_toolchain.darwin_evidence \
  --evidence "$native_evidence" --revision "$tested_revision" \
  --outcome success --mode "$package_mode" \
  --terminal-receipt "$terminal_receipt" --terminal-sha256 "$terminal_sha256"
```

Choose `package` for the ordinary SDK or `package-sanitizers` for the optional
consumer sanitizer package.

## Replay managed installation on Apple Silicon

While both workflow artifacts are retained, pass a JSON pin to the registered
`darwin-bootstrap.yml` workflow. The pin contains four positive integers
`run_id`, `run_attempt`, `evidence_artifact_id`, and `terminal_artifact_id`.
It also contains the exact 40-character `revision`, the GitHub-reported
archive hashes `evidence_zip_sha256` and `terminal_zip_sha256`, and the
`terminal_sha256` of the extracted `darwin-terminal.json` receipt. Use
lowercase hex values without the `sha256:` prefix.

```sh
gh workflow run darwin-bootstrap.yml --ref "$replay_ref" -f candidate="$candidate"
```

The replay job checks that the exact native and terminal jobs passed on the
producing attempt. It verifies both ZIP files and replays the terminal verifier
from the recorded producer revision before installing the SDK. It exercises
install, repeat install, an installed ARC consumer, and verified uninstall
without rebuilding LLVM. Supply `-f replacement="$replacement"` with a second
independently pinned package of a different seal to exercise hard cutover and
reclamation. The `darwin-managed-replay` artifact records both revisions, the
admitted package hashes, native commands and lifecycle outcomes.
[Replay workflow](../../.github/workflows/darwin-managed-replay.yml)

## Optional ASan and UBSan consumers

Set `runtime_sanitizers: true` in a manual run of **Darwin arm64 bootstrap probe**
to select runtime registration, SDK packaging, and the native sanitizer check.
Both workflow inputs default to `false`. Set `runtime_registration: true` with
`runtime_sanitizers: false` for the ordinary experimental SDK package.
[Workflow](../../.github/workflows/darwin-bootstrap.yml)

```sh
gh workflow run darwin-bootstrap.yml \
  -f runtime_registration=true -f runtime_sanitizers=true
```

On the same native Apple Silicon setup, pass `--sdk-sanitizers` to
`darwin build-sdk` and use a fresh output directory.

```sh
python3 -B -m scripts.objc3c_toolchain darwin build-sdk \
  --fetch --sdk-sanitizers \
  --bootstrap-report "$PWD/darwin-bootstrap-result/report.json" \
  --output-dir "$PWD/darwin-registration-sanitizers"
```

Install and qualify the optional archive using its trusted package receipt. The
sealed archive selects the sanitizer checks during qualification.

```sh
package="$PWD/darwin-registration-sanitizers/sdk/package"
archive="$package/objc3-darwin-sdk.tar.gz"
archive_sha=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["archive_sha256"])' "$package/package.json")

python3 -B -m scripts.objc3c_toolchain darwin install \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --prefix "$PWD/Objective-C 3 Experimental Sanitizer SDK"

python3 -B -m scripts.objc3c_toolchain darwin qualify \
  --archive "$archive" --archive-sha256 "$archive_sha" \
  --output-dir "$PWD/darwin-sdk-sanitizer-replay"
```

The manifest seals `libclang_rt.asan_osx_dynamic.dylib` and
`libclang_rt.ubsan_osx_dynamic.dylib` under `lib/clang/23/lib/darwin`. The
`consumer_sanitizers` descriptor binds both SHA-256 digests to the pinned
upstream LLVM 23.1.5 macOS ARM64 archive. The installed compiler builds the
packaged stateless Objective-C 3 fixture separately in plain, ASan, and UBSan
modes. The witness exercises allocation, dispatch, and cleanup under manual
reference counting. Instrumentation covers the consumer object, which links
the SDK's private runtime archive. [Witness source](../../tests/clang_foundation/darwin_sanitizers/stateless.m)

Qualification requires successful native execution of all three arm64 Mach-O
binaries on Apple Silicon from a fresh, relocated SDK. The evidence binds the
compiler-generated sanitizer and dispatch imports, relative library search
paths, and the exact installed compiler-rt dylib loaded by each sanitized
process. Local replay writes `darwin-sdk-sanitizer-replay/qualification.json`.
The workflow retains the native receipts and a terminal receipt in
`package-sanitizers` mode. The optional profile remains experimental with
`support_truth: false`. [Terminal verifier](../../scripts/objc3c_toolchain/darwin_evidence.py)
