# Linux release status

| Item                 | Current state                                                                                                                                                                                                                     |
| -------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Public download      | First preview pending. Published assets appear in [GitHub Releases](https://github.com/doublemover/Slopjective-C/releases).                                                                                                       |
| Source candidate     | Source 2.300 / System 2.223. The [Linux implementation PR](https://github.com/doublemover/Slopjective-C/pull/8278) and [delivery PR](https://github.com/doublemover/Slopjective-C/pull/8328) track integration and qualification. |
| Preview matrix       | Native x86-64 and AArch64. Each archive must pass clean installs on Debian 13 and Ubuntu 26.04.                                                                                                                                   |
| Compiler ABI         | Extension ABI 24 for the candidate, with GNUstep 2.2 and pinned libobjc2. Rebuild Objective-C 3 objects, packages, and compiler-generated metadata with the selected SDK.                                                         |
| Earlier SDK evidence | SDK 2.62 / extension ABI 21 is recorded in the [development history](../roadmap/linux_development_history.md).                                                                                                                    |
| Language support     | Follow the [capability schema](../../schemas/objc3-language-capabilities.json). The preview claim list remains empty until profile evidence and reviews are complete.                                                             |

## Acceptance and publication

- Finish the ordinary SDK, instrumented, coverage, native analysis, and fresh
  install gates for the exact release revision.
- Commit the approved distribution license and establish the project's private
  reporting route.
- Stage the draft on `main` using both successful qualification run IDs.
  Verify the native draft download rehearsals before publishing.
- Deploy and check the public site. Verify the release URLs and reconcile the
  receipts and workload evidence with the open issues.

The [release guide](linux.md) gives the procedure. The
[issue evidence inventory](../support/linux_open_issue_review.json) records
individual issues with their implementation commits or evidence owners.
The [acceptance follow-ups](../roadmap/linux_acceptance_followups.md) describe
outstanding checks and the [open issue ledger](../roadmap/linux_open_issue_sprint.md)
records the development checkpoints.
The [historical documentation index](../roadmap/linux_historical_documentation.md)
points to earlier command inventories.
