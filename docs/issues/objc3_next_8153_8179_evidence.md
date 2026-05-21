# Objective-C 3.0 Post-Cutover Issue Evidence: #8153-#8179

This page records durable repo evidence for the post-cutover issue index and
the late developer-workflow, package, platform, application-sample, and
release-channel documentation rows.
It is not a release publication log and does not replace GitHub issue state.

## Source Rules

- #8153 is the roadmap index. Local closure evidence must point at checked-in
  support truth, issue-specific fixtures, and replayable public commands.
- #8156/#8161/#8162 stdlib evidence is owned by dedicated `objc3.text` and
  `objc3.collections` contracts, runtime probes, module manifests, and
  fail-closed negative fixtures.
- #8157/#8169/#8170/#8171/#8172/#8178 developer-product workflow evidence is
  routed through checked-in contracts, capability rows, evidence-map rows, and
  public `npm run objc3c -- <action>` commands.
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
| #8156 | `stdlib/core_architecture.json`, `stdlib/modules/objc3.text/module.json`, `stdlib/modules/objc3.collections/module.json`, `tests/conformance/support_claim_runnable_evidence_catalog.json` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Runtime-backed stdlib claims are limited to checked `objc3.text` and `objc3.collections` helper families; generic containers, Foundation bridging, and literal syntax remain separately reserved. |
| #8161 | `tests/tooling/fixtures/stdlib_collections/runtime_backed_collection_claims_contract.json`, `tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp`, `native/objc3c/src/runtime/stdlib/collections_runtime_contract.h` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Collections support covers concrete i32 arrays, slices, aggregation, mutable map insert/update, sets, and deterministic iterator guards; dictionary literals, generic key/value typing, non-i32 hashing, and map iteration remain outside the claim. |
| #8162 | `tests/tooling/fixtures/stdlib_text/runtime_backed_text_claims_contract.json`, `tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp`, `native/objc3c/src/runtime/stdlib/text_runtime_contract.h` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Text support covers runtime-owned UTF-8 records, byte/unit counts, concatenation, prefix helpers, and fail-closed status reporting; Unicode scalar iteration, normalization, formatting, interpolation, mutation, and Foundation bridging remain outside the claim. |
| #8157 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/developer_experience_completion_contract.json`, `tests/tooling/fixtures/developer_tooling/first_run_workflow_contract.json` | `npm run objc3c -- validate-getting-started`; `npm run objc3c -- validate-developer-tooling` | First-run product evidence covers onboarding, template, migration, diagnostic, and artifact-inspection routing; it does not claim a full IDE, full LSP, Objective-C 2 source acceptance, or debugger stepping. |
| #8169 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/diagnostic_quality_contract.json`, `tests/conformance/diagnostics/OBJ3-NEXT-016-*.json` | `npm run objc3c -- check-developer-diagnostic-quality`; `npm run objc3c -- validate-developer-tooling` | Diagnostic recovery and fix-it evidence must stay structured and deterministic; recovery cannot turn invalid programs into accepted programs. |
| #8170 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json`, `tests/tooling/fixtures/developer_tooling/formatter_rewrite_contract.json` | `npm run objc3c -- inspect-editor-tooling`; `npm run objc3c -- validate-developer-tooling` | Formatter/LSP/workspace support is limited to checked payloads; references, rename, semantic tokens, generalized code actions, and statement stepping remain fail-closed. |
| #8171 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/conformance/public_suite_manifest.json`, `tests/conformance/public_suite_package_replay_evidence.json` | `npm run objc3c -- validate-public-conformance-suite` | Public conformance truth comes from the checked stable suite manifest and replay package; external validation is corroborating evidence only. |
| #8172 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json`, `tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json`, `tests/tooling/fixtures/package_ecosystem/install_distribution_credibility_contract.json` | `npm run objc3c -- validate-package-manager-model`; `npm run objc3c -- validate-package-mirror`; `npm run objc3c -- validate-package-install-distribution --from-nothing` | Package manager support is local lock/manifest/mirror/registry metadata plus deterministic install/update/uninstall receipts only; hosted registry and network dependency resolution remain unsupported. |
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
- `diagnostics.parser-sema.recovery-fixits`
- `tooling.editor.formatter-lsp-workspace`
- `tooling.developer-experience.first-run-product-path`
- `conformance.public.stable-suite-manifest`
- `ecosystem.package-manager.local-registry`
- `stdlib.text.string-view-runtime-shape`
- `stdlib.text.byte-span-runtime-shape`
- `stdlib.collections.array-slice-runtime-shape`
- `stdlib.collections.array-aggregate-runtime-shape`
- `stdlib.collections.map-entry-runtime-shape`
- `stdlib.collections.set-iteration-runtime-shape`

Generated outputs under `tmp/`, `checked_outputs/`, and package/release artifact
roots may demonstrate replay, but they are not source truth for this issue
evidence page.
