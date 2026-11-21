# Linux SDK releases

The [GitHub Releases page](https://github.com/doublemover/Slopjective-C/releases)
is the public download location. Each published Linux SDK preview provides a
versioned release manifest with the source commit, archive digests, SDK seals,
compiler build identities, architecture, extension ABI, Linux ABI minimums, and
qualification run links. The release notes describe the supported feature set and rebuild
requirements. If there is no published preview, the source build is available
through the [maintainer workflow](../runbooks/objc3c_maintainer_workflows.md).
See [Linux release status](status.md) for the current candidate and open gates.

## Install and run

On the release page, download `install-linux.sh`, `install-sdk.py`,
`SHA256SUMS`, and the archive for your architecture. Keep them in one directory.
Use `uname -m` to choose the archive ending in `linux-x86_64.tar.zst` or
`linux-aarch64.tar.zst`. Check the release notes for the distributions qualified
for that specific archive.

Replace the example archive name below with the filename you downloaded:

```sh
(
  set -eu
  archive=objc3-sdk-v0.1.0-preview.1-linux-x86_64.tar.zst
  sha256sum --check --ignore-missing SHA256SUMS
  checksum=$(awk -v archive="$archive" '$2 == archive { print $1 }' SHA256SUMS)
  sh install-linux.sh --archive "$archive" --sha256 "$checksum" --root "$HOME/.local/objc3c"
)
```

After installation succeeds, activate the SDK and compile the included example.

```sh
. "$HOME/.local/objc3c/activate.sh"
objc3c --version
objc3c "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/main.objc3" -O2 -o application
./application
```

Use the verified installer with `--provision` as root if Debian or Ubuntu
prerequisites are missing. Use `--root /opt/objc3c` for a system installation.
When the installer creates an installation root it makes it traversable by
ordinary users. Administrators control access to an existing root.

## License and source

The draft bundle includes the [GPLv2 project license](../../LICENSE).
Installed SDKs place the project license and bundled third-party notices
under `share/licenses/`. The draft includes separate source archives for
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
   receipts and image digests for each architecture. Qualification artifacts
   expire seven days after upload. For a new qualification attempt, use
   **Re-run all jobs** or dispatch the same revision again.
3. Review the draft release notes, archive permissions, asset names, and
   published reporting route. Confirm the terms for distributing the combined
   SDK and provide the corresponding source required by the GPL for any
   GPL-covered binaries. Require passing draft-download rehearsals on both
   architectures and a passing source downloads check. A partial upload can
   resume with the same version, revision, and qualification runs.
4. Reconcile the release receipts with the [tracked Linux acceptance work](../roadmap/linux_acceptance_followups.md).
   Review workload observations under [#8291](https://github.com/doublemover/Slopjective-C/issues/8291).
   Publish after the owner has reviewed the release. Open the public release
   and verify the asset links and installation guide from a signed-out session.

Before making a conformance claim, assemble the matching
[profile evidence](../../spec/conformance/profile_release_evidence_checklist.md)
from tests executed against the shipped archive, including the required
independent review. The preview release manifest carries an empty profile claim list.
