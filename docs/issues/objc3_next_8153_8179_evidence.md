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
| #8153 | `docs/support/capability_matrix.json`, `docs/support/evidence_map.json`, `tests/conformance/support_claim_runnable_evidence_catalog.json` | `npm run objc3c -- validate-post-cutover-issue-evidence` | Index evidence is source-backed support truth only; this page does not close GitHub issues by itself. |
| #8154 | `docs/support/capability_matrix.json`, `tests/tooling/test_next_runtime_public_rows.py`, `tests/tooling/test_runtime_capability_public_split.py` | `npm run objc3c -- validate-runnable-object-model`; `npm run objc3c -- validate-object-model-conformance` | Object-model realization is limited to implemented runtime rows and checked probes; Objective-C 2 compatibility, foreign runtime mirroring, and private snapshot-only claims remain outside the public support claim. |
| #8155 | `scripts/check_objc3c_next_runtime_public_rows.py`, `tests/tooling/test_runtime_capability_public_split.py`, `docs/support/evidence_map.json` | `npm run objc3c -- test-runtime-acceptance-fast` | Advanced runtime closure is split into object-model and advanced-runtime public rows; broad "runtime complete" claims must fail unless each precise row has implemented support evidence. |
| #8156 | `stdlib/core_architecture.json`, `stdlib/modules/objc3.text/module.json`, `stdlib/modules/objc3.collections/module.json`, `tests/conformance/support_claim_runnable_evidence_catalog.json` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Runtime-backed stdlib claims are limited to checked `objc3.text` and `objc3.collections` helper families; generic containers, Foundation bridging, and literal syntax remain separately reserved. |
| #8158 | `docs/runbooks/objc3c_release_operations.md`, `tests/tooling/fixtures/release_operations/channel_operations_model.json`, `scripts/build_objc3c_update_manifest.py` | `npm run objc3c -- validate-release-operations`; `npm run objc3c -- validate-release-operations-end-to-end` | Packaging and release credibility source truth is checked-in policy, manifest builders, and channel contracts; generated release/update artifacts under `tmp/` are evidence outputs only. |
| #8159 | `tests/tooling/fixtures/runtime_performance/workload_replay_contract.json`, `tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json`, `tests/tooling/fixtures/runtime_performance/scale_scenario_contract.json` | `npm run objc3c -- validate-runtime-performance`; `npm run objc3c -- validate-performance-governance` | Performance, stress, fuzz, and scale claims are governed replay contracts; benchmark reports do not widen language support and generated results cannot become source truth. |
| #8160 | `tests/tooling/test_generic_support_claim_groundwork.py`, `tests/tooling/fixtures/native/type_semantic_model_closure_positive.objc3`, `native/objc3c/src/parse/objc3_type_generic_profiles.cpp` | `npm run objc3c -- test-behavior-matrix`; `npm run objc3c -- validate-conformance-corpus` | Public generics are limited to proven parser, semantic, metadata, and cross-module identity rows; higher-kinded types, inferred associated types, and generic collection ABI remain unsupported unless separately proven. |
| #8161 | `tests/tooling/fixtures/stdlib_collections/runtime_backed_collection_claims_contract.json`, `tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp`, `native/objc3c/src/runtime/stdlib/collections_runtime_contract.h` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Collections support covers concrete i32 arrays, slices, aggregation, mutable map insert/update, sets, and deterministic iterator guards; dictionary literals, generic key/value typing, non-i32 hashing, and map iteration remain outside the claim. |
| #8162 | `tests/tooling/fixtures/stdlib_text/runtime_backed_text_claims_contract.json`, `tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp`, `native/objc3c/src/runtime/stdlib/text_runtime_contract.h` | `npm run objc3c -- validate-stdlib-foundation`; `npm run objc3c -- test-runtime-acceptance-fast` | Text support covers runtime-owned UTF-8 records, byte/unit counts, concatenation, prefix helpers, and fail-closed status reporting; Unicode scalar iteration, normalization, formatting, interpolation, mutation, and Foundation bridging remain outside the claim. |
| #8163 | `tests/tooling/fixtures/module_cache/`, `tests/tooling/fixtures/module_interop_contracts/foundation_next_visibility_bridge_contract.json`, `scripts/check_objc3c_incremental_module_cache_consistency.py` | `npm run objc3c -- test-mixed-module-differential`; `npm run objc3c -- validate-module-interop-contracts` | Module support covers deterministic public import lookup, visibility/reexport checks, and rebuild metadata; hidden declarations, stale cache inputs, cycles, and ABI-incompatible imports fail closed. |
| #8164 | `tests/tooling/test_protocol_existential_support_claim_groundwork.py`, `tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp`, `native/objc3c/src/runtime/classes/protocol_conformance.cpp` | `npm run objc3c -- validate-object-model-conformance`; `npm run objc3c -- test-runtime-acceptance-fast` | Protocol existential and witness evidence is limited to public type-system rows and runtime conformance records; associated-type inference, Swift bridging, and Objective-C 2 protocol compatibility remain out of scope. |
| #8165 | `tests/tooling/fixtures/runtime_import_interop_bridge_metadata/valid_bridge_surface.json`, `tests/tooling/fixtures/runtime_import_interop_bridge_metadata/tampered_bridge_surface.json`, `scripts/check_runtime_import_interop_bridge_metadata.py` | `npm run objc3c -- validate-interop-conformance`; `npm run objc3c -- validate-runnable-interop` | Interop support covers checked bridge metadata preservation and mixed-image replay boundaries; unsupported Swift/C++/foreign-runtime behavior must stay rejected or reserved. |
| #8166 | `tests/tooling/fixtures/ownership_memory_model/support_claim_contract.json`, `tests/tooling/fixtures/ownership_memory_model/formal_model_contract.json`, `tests/tooling/test_ownership_memory_model_support_claim_groundwork.py` | `npm run objc3c -- test-lowering-runtime-stress`; `npm run objc3c -- validate-conformance-corpus` | Ownership and memory-model support is source-backed by semantic ownership contracts and runtime hook evidence; unsupported ARC qualifiers, invalid block captures, and unresolved lifetime claims stay explicit diagnostics. |
| #8167 | `tests/tooling/fixtures/stdlib_concurrency/public_concurrency_usability_claims_contract.json`, `stdlib/modules/objc3.concurrency/module.json`, `tests/tooling/runtime/stdlib_concurrency_runtime_probe.cpp` | `npm run objc3c -- validate-runnable-concurrency`; `npm run objc3c -- validate-concurrency-conformance` | Public concurrency support is limited to checked task, task-group, executor-hop, actor-mailbox, and cancellation rows; scheduler guarantees and unimplemented async patterns remain fail-closed. |
| #8168 | `tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json`, `tests/tooling/fixtures/metaprogramming_public_surface/macro_expansion_artifact_ownership_contract.json`, `docs/runbooks/objc3c_macro_metaprogramming_surface.md` | `npm run objc3c -- validate-runnable-metaprogramming`; `npm run objc3c -- validate-metaprogramming-conformance` | Macro and metaprogramming claims require deterministic expansion artifacts, provenance, sandbox, and trust-policy evidence; arbitrary host execution and unchecked package macros stay rejected. |
| #8157 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/developer_experience_completion_contract.json`, `tests/tooling/fixtures/developer_tooling/first_run_workflow_contract.json` | `npm run objc3c -- validate-getting-started`; `npm run objc3c -- validate-developer-tooling` | First-run product evidence covers onboarding, template, migration, diagnostic, and artifact-inspection routing; it does not claim a full IDE, full LSP, Objective-C 2 source acceptance, or debugger stepping. |
| #8169 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/diagnostic_quality_contract.json`, `tests/conformance/diagnostics/OBJ3-NEXT-016-PARSE-RECOVERY-01.json`, `tests/conformance/diagnostics/OBJ3-NEXT-016-SEMA-RECOVERY-01.json` | `npm run objc3c -- check-developer-diagnostic-quality`; `npm run objc3c -- validate-developer-tooling` | Diagnostic recovery and fix-it evidence must stay structured and deterministic; recovery cannot turn invalid programs into accepted programs. |
| #8170 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json`, `tests/tooling/fixtures/developer_tooling/formatter_rewrite_contract.json` | `npm run objc3c -- inspect-editor-tooling`; `npm run objc3c -- validate-developer-tooling` | Formatter/LSP/workspace support is limited to checked payloads; references, rename, semantic tokens, generalized code actions, and statement stepping remain fail-closed. |
| #8171 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/conformance/public_suite_manifest.json`, `tests/conformance/public_suite_package_replay_evidence.json` | `npm run objc3c -- validate-public-conformance-suite` | Public conformance truth comes from the checked stable suite manifest and replay package; external validation is corroborating evidence only. |
| #8172 | `tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json`, `tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json`, `tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json`, `tests/tooling/fixtures/package_ecosystem/install_distribution_credibility_contract.json` | `npm run objc3c -- validate-package-manager-model`; `npm run objc3c -- validate-package-mirror`; `npm run objc3c -- validate-package-install-distribution --from-nothing` | Package manager support is local lock/manifest/mirror/registry metadata plus deterministic install/update/uninstall receipts only; hosted registry and network dependency resolution remain unsupported. |
| #8173 | `tests/tooling/fixtures/abi_governance/source_of_truth_manifest.json`, `docs/runbooks/objc3c_abi_governance.md`, `scripts/check_objc3c_abi_governance.py` | `npm run objc3c -- validate-abi-governance`; `npm run objc3c -- check-release-abi-api-drift` | ABI stability is governed by checked symbol, manifest, and compatibility policy; release reports may summarize drift but cannot introduce ABI guarantees. |
| #8174 | `scripts/check_objc3c_public_runtime_reflection_api.py`, `tests/tooling/runtime/public_runtime_reflection_api_probe.cpp`, `docs/support/capability_matrix.json` | `npm run objc3c -- validate-public-runtime-reflection-api`; `npm run objc3c -- validate-runnable-storage-reflection` | Public reflection APIs expose supported class, protocol, property, selector, and storage metadata with lifetime-safe result ownership; private debug snapshots and `_for_testing` surfaces remain inaccessible. |
| #8175 | `tests/tooling/fixtures/semantic_optimization_pipeline/pipeline.json`, `tests/tooling/fixtures/codegen_optimization_direct_dispatch/policy.json`, `scripts/check_objc3c_semantic_optimization_pipeline.py` | `npm run objc3c -- validate-semantic-optimization-pipeline`; `npm run objc3c -- validate-codegen-optimization-policy` | Optimization support is semantic-preserving and pass-governed; reserved devirtualization, method inlining, and cache-aware dispatch remain skip/no-success rows until executable evidence proves them. |
| #8176 | `scripts/objc3c_runtime_debug_trace/payload.py`, `native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h`, `tests/tooling/fixtures/developer_tooling/runtime_debug_trace/contract.json` | `npm run objc3c -- trace-runtime-debug`; `npm run objc3c -- test-runtime-acceptance-diagnostics` | Debug trace support covers checked snapshot-backed runtime lanes and support handoff rows only; debugger stepping, private metadata, and unsupported trace domains remain reserved or rejected. |
| #8177 | `tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json`, `tests/tooling/fixtures/platform_support/source_truth_matrix.json`, `docs/runbooks/objc3c_platform_toolchain_support_matrix.md` | `npm run objc3c -- build-platform-support-matrix` | `windows-x64` is the only supported host row; Linux, macOS, sanitizer, and broad LLVM ranges remain fail-closed or reserved. |
| #8178 | `showcase/applicationFrameworkSamples/manifest.json`, `showcase/applicationFrameworkSamples/apps/workflowStdlibCLI/workspace.json`, `showcase/applicationFrameworkSamples/apps/workflowStdlibCLI/replay-contract.json`, `docs/tutorials/application-framework-samples.md` | `npm run objc3c -- validate-application-framework-samples` | Samples are package-aware showcase inputs and do not widen support beyond implemented capability rows. |
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
- `runtime.object-model.full-realization`
- `runtime.object-model.interface-method-table`
- `runtime.object-model.class-realization`
- `runtime.object-model.category-protocol-registration`
- `runtime.object-model.property-ivar-reflection`
- `runtime.object-model.registration-replay`
- `runtime.object-model.bounded-query-snapshots`
- `language.generics.protocol-qualified-arguments`
- `language.generics.callable-type-parameters`
- `language.generics.variance-specialization`
- `runtime.generics.cross-module-metadata`
- `modules.public-import-lookup`
- `modules.visibility-reexport-rebuild-contract`
- `runtime.modules.imported-runtime-packaging-replay`
- `language.protocols.protocol-qualified-existential-value-flow`
- `language.protocols.existential-witness-model`
- `runtime.interop.package-loader-bridge`
- `runtime.interop.mixed-image-replay`
- `language.ownership.memory-model`
- `stdlib.concurrency.runtime-backed-v1`
- `stdlib.concurrency.public-task-spawn-api`
- `stdlib.concurrency.public-task-group-cancellation-api`
- `stdlib.concurrency.public-executor-hop-api`
- `stdlib.concurrency.public-actor-mailbox-api`
- `runtime.concurrency.task-continuation-lifecycle`
- `runtime.concurrency.actor-mailbox-isolation`
- `language.metaprogramming.property-behavior-semantics`
- `language.metaprogramming.derive-expansion-inventory`
- `language.metaprogramming.macro-safety-sandbox-determinism`
- `runtime.metaprogramming.host-cache-boundary`
- `release.abi.stability-governance`
- `abi.governance.source-truth`
- `runtime.public-api.reflection`
- `compiler.optimization.semantic-preserving-pipeline`
- `compiler.optimization.semantic-pass-registry`
- `compiler.optimization.devirtualization`
- `compiler.optimization.method-inlining`
- `runtime.optimization.cache-aware-dispatch`
- `runtime.debug-trace.structured-inspection`
- `runtime.debug-trace.async-tasks`
- `runtime.debug-trace.error-unwind`
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
