# Linux SDK releases

The [Linux SDK v0.1.0-preview.1 release](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1)
provides binary archives for x86-64 and AArch64. Check the
[release support table](preview-support.md) for distributions and ABI minimums.
The release manifest records the source commit, archive digests, SDK seals,
compiler identities and qualification runs. For a compiler source build, see
the [maintainer workflow](../runbooks/objc3c_maintainer_workflows.md).

## Install and run

On the release page, download `install-linux.sh`, `install-sdk.py`,
`SHA256SUMS`, and the archive for your architecture. Keep them in one directory.
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
  for file in install-linux.sh install-sdk.py "$archive"; do
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

After activating the SDK, follow [Getting started](../tutorials/getting_started.md)
to compile your first program from a fresh working directory.

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
