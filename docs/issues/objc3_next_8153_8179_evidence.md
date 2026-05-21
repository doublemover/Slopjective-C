# Objective-C 3.0 Post-Cutover Issue Evidence: #8153-#8179

This page records durable repo evidence for the post-cutover issue index and
the late platform, application-sample, and release-channel documentation rows.
It is not a release publication log and does not replace GitHub issue state.

## Source Rules

- #8153 is the roadmap index. Local closure evidence must point at checked-in
  support truth, issue-specific fixtures, and replayable public commands.
- #8177 platform support claims come from checked-in platform/toolchain source
  truth, not generated reports or source-only compile success.
- #8178 application sample claims come from checked-in sample sources,
  workspaces, replay contracts, and public compile validation.
- #8179 release-channel docs may cite release notes and changelog policy, but
  package/release operation scripts and generated publication manifests remain
  owned by the release lane.

## Issue Evidence Matrix

| Issue | Local evidence | Public replay surface | Boundary |
| --- | --- | --- | --- |
| #8153 | `docs/support/capability_matrix.json`, `docs/support/evidence_map.json`, `tests/conformance/support_claim_runnable_evidence_catalog.json` | `python scripts/build_capability_support_docs.py --check` | Index evidence is source-backed support truth only; this page does not close GitHub issues by itself. |
| #8177 | `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`, `tests/tooling/fixtures/platform_support/source_truth_matrix.json`, `docs/runbooks/objc3c_platform_toolchain_support_matrix.md` | `npm run objc3c -- build-platform-support-matrix` | `windows-x64` is the only supported host row; Linux, macOS, sanitizer, and broad LLVM ranges remain fail-closed or reserved. |
| #8178 | `showcase/applicationFrameworkSamples/manifest.json`, sample `workspace.json` files, sample `replay-contract.json` files, `docs/tutorials/application-framework-samples.md` | `npm run objc3c -- validate-application-framework-samples` | Samples are package-aware showcase inputs and do not widen support beyond implemented capability rows. |
| #8179 | `docs/runbooks/objc3c_release_operations.md`, `docs/runbooks/objc3c_release_channel_operations.md`, `tests/tooling/fixtures/release_channel_operations_policy.json` | `npm run objc3c -- validate-release-operations` | This docs lane records source-derived release notes and changelog policy only; publication manifests and release operation scripts are release-lane proof. |

## Capability Rows Owned By This Slice

- `platform.windows-x64.tier1`
- `platform.linux-x64.unsupported`
- `platform.darwin-arm64.unsupported`
- `toolchain.llvm.current-probed-executable`
- `toolchain.package-bridge.minimum-host-tools`
- `toolchain.sanitizer.address`
- `toolchain.sanitizer.undefined`
- `applications.framework-samples.object-runtime-library`
- `applications.framework-samples.interop-adapter-library`
- `applications.framework-samples.stdlib-text-collections-cli`
- `applications.framework-samples.async-runtime-application`

Generated outputs under `tmp/`, `checked_outputs/`, and package/release artifact
roots may demonstrate replay, but they are not source truth for this issue
evidence page.
