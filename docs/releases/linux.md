# Linux SDK releases

The [Linux SDK v0.1.0-preview.1 release](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1)
provides binary archives for x86-64 and AArch64. Check the
[release support table](preview-support.md) for distributions and ABI minimums.
The release manifest records the source commit, archive digests, SDK seals,
compiler identities and qualification runs. For a compiler source build, see
the [maintainer workflow](../runbooks/objc3c_maintainer_workflows.md).

## Install and run

On the release page, download `install-linux.sh`, `install-sdk.py`,
`linux-release.json`, `SHA256SUMS`, and the archive for your architecture. Keep
the files together under their published names.
The installer supports a user-owned location and a system location.

```sh
(
  set -eu
  arch=$(uname -m)
  case "$arch" in
    x86_64|aarch64) ;;
    *) printf 'Unsupported Linux architecture\n' >&2; exit 1 ;;
  esac
  archive="objc3-sdk-v0.1.0-preview.1-linux-$arch.tar.zst"
  for file in install-linux.sh install-sdk.py linux-release.json "$archive"; do
    awk -v file="$file" '$2 == file { print }' SHA256SUMS | sha256sum --check --strict
  done
  checksum=$(awk -v archive="$archive" '$2 == archive { print $1 }' SHA256SUMS)
  sh install-linux.sh --archive "$archive" --sha256 "$checksum" --root "$HOME/.local/objc3c"
)
```

If prerequisites are missing, run the verified installer with `--provision` as
root and use `--root /opt/objc3c` for a system installation. The installer uses
the distribution's apt repositories to install its dependencies. An existing
installation root keeps its administrator-set permissions.

The current bootstrap checks its companion file and the archive checksum before
invoking apt and skips package setup once all prerequisites are installed. The
published preview bootstrap provisions packages first, so use the checksum
checks above before invoking its `--provision` option.

The current installer source checks the archive's glibc and system libstdc++
requirements before it activates an SDK. If the check fails, use a qualified
distribution or update its libstdc++ package. The published preview installer
predates this check. The [ABI admission replay](../roadmap/linux_installer_abi_acceptance.md)
uses the published x86-64 archive with the updated installer.

After activating the SDK, follow [Getting started](../tutorials/getting_started.md)
to compile your first program from a fresh working directory.

## Update, roll back, and remove

Use the same `--root` and run the verified installer with the new release
archive and checksum. It verifies the new generation before atomically changing
`current`. When a shell sources `activate.sh`, it selects one concrete SDK
generation. Source the script again to select the updated SDK in an existing
shell.

To roll back, run the installer with the earlier verified release archive and
checksum at the same root. Keep that archive and checksum for as long as you
need its generation. If verification or host admission fails, the selected
generation stays active.

To remove the SDK, close shells using it and remove the dedicated installation
directory chosen with `--root`. That directory contains `activate.sh`, the
`current` symlink, every SDK generation under `sdks/`, and the installation
lock. Dependencies installed through `--provision` are distribution packages
and can be used by other software.

## Symbolize application diagnostics

The SDK includes `bin/llvm-symbolizer` from its own LLVM build. After activating
the SDK, the symbolizer is available on `PATH`. To make the association explicit,
set `ASAN_SYMBOLIZER_PATH` or the debugger's `external_symbolizer_path` option to
`$OBJC3C_SDK_ROOT/bin/llvm-symbolizer`. The latter option works in
`UBSAN_OPTIONS` and `TSAN_OPTIONS`. Compile the application with `-g` for
source file and line information.

## License and source

The release includes the [GPLv2 project license](../../LICENSE).
Installed SDKs place the project license and bundled third-party notices
under `share/licenses/`. The release includes separate source archives for
Slopjective-C, LLVM, libobjc2, and robin-map. The Slopjective-C source archive
contains the patches, toolchain manifests, and build scripts used for the SDK.
Verify the source downloads with `SHA256SUMS` and check their revisions in
`linux-release.json`. To rebuild, use a Git checkout of the listed
Slopjective-C revision and follow the
[maintainer workflow](../runbooks/objc3c_maintainer_workflows.md). The source
preparation commands use Git metadata to verify the pinned dependencies.

## Release owner checklist

1. Finish native x86-64 and AArch64 qualification for the revision to ship.
   Check the terminal gates and clean Debian 13 and Ubuntu 26.04 installs.
2. Run **Stage Linux SDK release draft** from that commit on `main`, passing
   both qualification run IDs and a new preview version. The workflow verifies
   the source, archive hashes, seals, and installer pair before creating a
   draft. The release manifest preserves both distributions' installation
   receipts, image digests, and the exact qualification attempt's job outcomes
   and timings for each architecture. Bulk qualification artifacts expire seven
   days after upload. For a new qualification attempt, use
   **Re-run all jobs** or dispatch the same revision again.
3. Review the draft release notes, archive permissions, asset names, and
   published reporting route. Confirm the terms for distributing the combined
   SDK and provide the corresponding source required by the GPL for any
   GPL-covered binaries. Require passing draft-download rehearsals on both
   architectures and a passing source downloads check. Each native rehearsal
   matches the downloaded installer pair to the qualified archive's sealed
   inventory before executing it, then checks the active SDK's seal and source
   build against the release manifest. Resume a partial upload
   with the same version, revision, and qualification runs. The staging owner
   verifies the existing bundle and draft asset hashes, preserves completed
   assets, and retries only missing assets and confirmed empty upload placeholders.
4. Reconcile the release receipts with the [tracked Linux acceptance work](../roadmap/linux_acceptance_followups.md).
   Review workload observations under [#8291](https://github.com/doublemover/Slopjective-C/issues/8291).
   Publish after the owner has reviewed the release. Open the public release
   and verify the asset links and installation guide from a signed-out session.

Before making a conformance claim, assemble the matching
[profile evidence](../../spec/conformance/profile_release_evidence_checklist.md)
from tests executed against the shipped archive, including the required
independent review. The preview release manifest carries an empty profile claim list.

To verify the compiler feature queries, activate the installed SDK and run the
following from a checkout containing the release's capability catalog. Run it
on each native architecture. For an older release, pass `--authority` with
`schemas/objc3-language-capabilities.json` from the corresponding source tag.

```sh
python3 -m scripts.objc3c_toolchain.qualify_linux_capabilities \
  --sdk-root "$OBJC3C_SDK_ROOT" --output "linux-$(uname -m)-capability-queries.json"
```

The report records the SDK seal, compiler digest, catalog identity and each
advertised feature's witness references. Compare the seal with the release
manifest and review the referenced behavior evidence before publishing claims.
