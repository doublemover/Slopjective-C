# Linux release status

| Item                 | Current state                                                                                                                                                         |
| -------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Public download      | [Linux SDK v0.1.0-preview.1](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1)                                                              |
| Released source      | Source 2.300 / System 2.223 at [`8fe5c2fd8`](https://github.com/doublemover/Slopjective-C/commit/8fe5c2fd89008d1c090a9355c11bdda01ceb9309)                            |
| Preview matrix       | Native x86-64 and AArch64, both qualified on Debian 13 and Ubuntu 26.04.                                                                                              |
| Compiler ABI         | Extension ABI 24, with GNUstep 2.2 and pinned libobjc2. Rebuild Objective-C 3 objects, packages, and compiler-generated metadata with the preview SDK.                |
| Earlier SDK evidence | SDK 2.62 / extension ABI 21 is recorded in the [development history](../roadmap/linux_development_history.md).                                                        |
| Language support     | Follow the [capability schema](../../schemas/objc3-language-capabilities.json). The preview claim list remains empty until profile evidence and reviews are complete. |

## Platform delivery

| Platform                 | Delivery                                                                                                                                                                                          | Evidence                                                                                                                                                                                                                                                                                        |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Linux x86-64 and AArch64 | Published native preview archives with the patched LLVM 23.1.5 compiler, ELF/DWARF output, and GNUstep 2.2 plus libobjc2.                                                                         | [Release assets](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1) · [Exact archive and clean-install identities](preview-manifest.json)                                                                                                                              |
| Apple Silicon macOS      | The opt-in `macos-arm64-registration-experimental` route passed the native compiler, runtime, package, relocation, installed ARC and portable service checks, including ASan and UBSan consumers. | [Native package qualification](https://github.com/doublemover/Slopjective-C/actions/runs/45299864475) · [Verified two-package update](https://github.com/doublemover/Slopjective-C/actions/runs/45312431420) · [Experimental replay instructions](../runbooks/objc3c_darwin_arm64_bootstrap.md) |
| Production macOS arm64   | Unsupported. The experimental package records `support_truth: false`.                                                                                                                             | [Production capability matrix](../support/capability_matrix.md#macos-arm64-platform-support)                                                                                                                                                                                                    |

The Linux version is the exact base of the published patched compiler. Source
builds admit the pinned host LLVM toolchain. The experimental Apple Silicon
package uses that LLVM release as bootstrap and requires an arm64 host with
Xcode 16.4 build `16F6` and macOS SDK 15.5. These checks identify specific tool
combinations. [Linux host pins](../../toolchains/linux-hosts.json) · [Darwin package admission](../../scripts/objc3c_toolchain/darwin_sdk.py)

The capability matrix tracks the reviewed production profiles. The Linux
preview manifest defines the published Linux install scope. [Platform hardening runbook](../runbooks/objc3c_platform_hardening.md#8206-umbrella-closure-boundary)

## Release evidence

- The ordinary SDK, instrumented, coverage, native analysis, and clean install
  gates passed for [x86-64](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421)
  and [AArch64](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382).
- Both architectures passed fresh installs from the release downloads. All four
  source archives passed checksum verification. [Release rehearsal](https://github.com/doublemover/Slopjective-C/actions/runs/44903086638/attempts/2)
- The [retained job receipts](preview-qualification.json) record exact-attempt
  outcomes and elapsed time for the qualification jobs. The
  [published manifest](preview-manifest.json) records the install scopes and
  image identities.
- Report suspected vulnerabilities using the [security policy](../../SECURITY.md).

The [release guide](linux.md) gives the procedure. The
[issue evidence inventory](../support/linux_open_issue_review.json) records
individual issues with their implementation commits or evidence owners.
The [acceptance follow-ups](../roadmap/linux_acceptance_followups.md) describe
outstanding checks and the [open issue ledger](../roadmap/linux_open_issue_sprint.md)
records the development checkpoints. The [preview issue review](../roadmap/linux_preview_issue_review.md)
reconciles those checkpoints with the public release.
The [historical documentation index](../roadmap/linux_historical_documentation.md)
points to earlier command inventories.
