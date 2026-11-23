# Linux release status

| Item                 | Current state                                                                                                                                                         |
| -------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Public download      | [Linux SDK v0.1.0-preview.1](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1)                                                              |
| Released source      | Source 2.300 / System 2.223 at [`8fe5c2fd8`](https://github.com/doublemover/Slopjective-C/commit/8fe5c2fd89008d1c090a9355c11bdda01ceb9309)                            |
| Preview matrix       | Native x86-64 and AArch64, both qualified on Debian 13 and Ubuntu 26.04.                                                                                              |
| Compiler ABI         | Extension ABI 24, with GNUstep 2.2 and pinned libobjc2. Rebuild Objective-C 3 objects, packages, and compiler-generated metadata with the preview SDK.                |
| Earlier SDK evidence | SDK 2.62 / extension ABI 21 is recorded in the [development history](../roadmap/linux_development_history.md).                                                        |
| Language support     | Follow the [capability schema](../../schemas/objc3-language-capabilities.json). The preview claim list remains empty until profile evidence and reviews are complete. |

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
