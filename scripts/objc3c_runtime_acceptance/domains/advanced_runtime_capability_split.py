"""Public advanced-runtime capability split contract for #8155."""

from __future__ import annotations

from typing import Any

ADVANCED_RUNTIME_SPLIT_ISSUE = 8155
ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID = "language.advanced-runtime-closure"
ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE = (
    "scripts/objc3c_runtime_acceptance/domains/advanced_runtime_capability_split.py"
)

ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS: tuple[dict[str, str], ...] = (
    {
        "capability_id": "language.blocks.escape-capture-legality",
        "support_claim": "objc3c.behavior.language.blocks.escape-capture-legality",
        "behavior_fixture": "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
    },
    {
        "capability_id": "runtime.blocks.copy-dispose-invoke",
        "support_claim": "objc3c.behavior.runtime.blocks.copy-dispose-invoke",
        "behavior_fixture": "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
    },
    {
        "capability_id": "runtime.blocks.byref-forwarding",
        "support_claim": "objc3c.behavior.runtime.blocks.byref-forwarding",
        "behavior_fixture": "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
    },
    {
        "capability_id": "language.arc-cleanup.integration",
        "support_claim": "objc3c.behavior.arc-cleanup.integration",
        "behavior_fixture": "tests/tooling/fixtures/native/arc_cleanup_source_construct_order_positive.objc3",
    },
    {
        "capability_id": "language.errors.try-catch-semantics",
        "support_claim": "objc3c.behavior.errors.try-catch-semantics",
        "behavior_fixture": "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
    },
    {
        "capability_id": "compiler.lowering.error-unwind-cleanup",
        "support_claim": "objc3c.behavior.lowering.error-unwind-cleanup",
        "behavior_fixture": "scripts/objc3c_runtime_acceptance/domains/errors_lowering_throw_catch_case.py",
    },
    {
        "capability_id": "runtime.errors.nserror-status-bridge",
        "support_claim": "objc3c.behavior.runtime.error-nserror-status-bridge",
        "behavior_fixture": "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
    },
    {
        "capability_id": "runtime.errors.live-bridge-cleanup",
        "support_claim": "objc3c.behavior.runtime.error-live-bridge-cleanup",
        "behavior_fixture": "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
    },
    {
        "capability_id": "runtime.concurrency.async-actors",
        "support_claim": "objc3c.behavior.runtime.concurrency-async-actors",
        "behavior_fixture": "tests/native/runtime/concurrency/actor_executor_contract.objc3",
    },
    {
        "capability_id": "runtime.concurrency.task-continuation-lifecycle",
        "support_claim": "objc3c.behavior.runtime.concurrency-task-continuation-lifecycle",
        "behavior_fixture": "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
    },
    {
        "capability_id": "runtime.concurrency.actor-mailbox-isolation",
        "support_claim": "objc3c.behavior.runtime.concurrency-actor-mailbox-isolation",
        "behavior_fixture": "tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.property-behavior-semantics",
        "support_claim": "objc3c.behavior.language.metaprogramming.property-behavior-semantics",
        "behavior_fixture": "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.derive-expansion-inventory",
        "support_claim": "objc3c.behavior.language.metaprogramming.derive-expansion-inventory",
        "behavior_fixture": "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.macro-safety-sandbox-determinism",
        "support_claim": "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism",
        "behavior_fixture": "tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3",
    },
    {
        "capability_id": "runtime.metaprogramming.host-cache-boundary",
        "support_claim": "objc3c.behavior.runtime.metaprogramming.host-cache-boundary",
        "behavior_fixture": "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
    },
    {
        "capability_id": "runtime.interop.package-loader-bridge",
        "support_claim": "objc3c.behavior.runtime.interop.package-loader-bridge",
        "behavior_fixture": "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
    },
    {
        "capability_id": "runtime.interop.mixed-image-replay",
        "support_claim": "objc3c.behavior.runtime.interop.mixed-image-replay",
        "behavior_fixture": "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3",
    },
)

ADVANCED_RUNTIME_FEATURE_TAXONOMY: tuple[dict[str, object], ...] = (
    {
        "feature_id": "blocks.escape-capture-legality",
        "family": "blocks",
        "public_status": "implemented",
        "capability_id": "language.blocks.escape-capture-legality",
        "support_claim": "objc3c.behavior.language.blocks.escape-capture-legality",
        "evidence": (
            "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
        ),
    },
    {
        "feature_id": "blocks.copy-dispose-invoke",
        "family": "blocks",
        "public_status": "implemented",
        "capability_id": "runtime.blocks.copy-dispose-invoke",
        "support_claim": "objc3c.behavior.runtime.blocks.copy-dispose-invoke",
        "evidence": (
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
        ),
    },
    {
        "feature_id": "blocks.byref-forwarding",
        "family": "blocks",
        "public_status": "implemented",
        "capability_id": "runtime.blocks.byref-forwarding",
        "support_claim": "objc3c.behavior.runtime.blocks.byref-forwarding",
        "evidence": (
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        ),
    },
    {
        "feature_id": "blocks.full-language-closure",
        "family": "blocks",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.blocks.full-language-closure",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "blocks.conflicting-owned-capture",
        "family": "blocks",
        "public_status": "rejected",
        "diagnostic_behavior": "conflicting escaping block capture ownership is rejected by checked negative fixture diagnostics",
        "evidence": (
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
        ),
    },
    {
        "feature_id": "blocks.helper-abi",
        "family": "blocks",
        "public_status": "internal",
        "evidence": ("native/objc3c/src/runtime/blocks/block_runtime_api.cpp",),
    },
    {
        "feature_id": "arc.cleanup-integration",
        "family": "arc",
        "public_status": "implemented",
        "capability_id": "language.arc-cleanup.integration",
        "support_claim": "objc3c.behavior.arc-cleanup.integration",
        "evidence": (
            "tests/tooling/fixtures/native/arc_cleanup_source_construct_order_positive.objc3",
        ),
    },
    {
        "feature_id": "arc.full-automation",
        "family": "arc",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.arc.full-automation",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "errors.try-catch-semantics",
        "family": "errors",
        "public_status": "implemented",
        "capability_id": "language.errors.try-catch-semantics",
        "support_claim": "objc3c.behavior.errors.try-catch-semantics",
        "evidence": (
            "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        ),
    },
    {
        "feature_id": "errors.unwind-cleanup-lowering",
        "family": "errors",
        "public_status": "implemented",
        "capability_id": "compiler.lowering.error-unwind-cleanup",
        "support_claim": "objc3c.behavior.lowering.error-unwind-cleanup",
        "evidence": (
            "scripts/objc3c_runtime_acceptance/domains/errors_lowering_throw_catch_case.py",
        ),
    },
    {
        "feature_id": "errors.nserror-status-bridge",
        "family": "errors",
        "public_status": "implemented",
        "capability_id": "runtime.errors.nserror-status-bridge",
        "support_claim": "objc3c.behavior.runtime.error-nserror-status-bridge",
        "evidence": (
            "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
        ),
    },
    {
        "feature_id": "errors.live-bridge-cleanup",
        "family": "errors",
        "public_status": "implemented",
        "capability_id": "runtime.errors.live-bridge-cleanup",
        "support_claim": "objc3c.behavior.runtime.error-live-bridge-cleanup",
        "evidence": (
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
        ),
    },
    {
        "feature_id": "errors.generalized-foreign-exception-abi",
        "family": "errors",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.errors.generalized-foreign-exception-abi",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "errors.throwing-call-without-try",
        "family": "errors",
        "public_status": "rejected",
        "diagnostic_behavior": "throwing calls outside a try/local handler fail closed in semantic diagnostics",
        "evidence": (
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
        ),
    },
    {
        "feature_id": "errors.bridge-helper-state",
        "family": "errors",
        "public_status": "internal",
        "evidence": ("native/objc3c/src/runtime/errors/error_bridge.cpp",),
    },
    {
        "feature_id": "concurrency.async-actors",
        "family": "concurrency",
        "public_status": "implemented",
        "capability_id": "runtime.concurrency.async-actors",
        "support_claim": "objc3c.behavior.runtime.concurrency-async-actors",
        "evidence": ("tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",),
    },
    {
        "feature_id": "concurrency.task-continuation-lifecycle",
        "family": "concurrency",
        "public_status": "implemented",
        "capability_id": "runtime.concurrency.task-continuation-lifecycle",
        "support_claim": "objc3c.behavior.runtime.concurrency-task-continuation-lifecycle",
        "evidence": (
            "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
        ),
    },
    {
        "feature_id": "concurrency.actor-mailbox-isolation",
        "family": "concurrency",
        "public_status": "implemented",
        "capability_id": "runtime.concurrency.actor-mailbox-isolation",
        "support_claim": "objc3c.behavior.runtime.concurrency-actor-mailbox-isolation",
        "evidence": ("tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp",),
    },
    {
        "feature_id": "concurrency.broad-async-actor-closure",
        "family": "concurrency",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.concurrency.broad-async-actor-closure",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "concurrency.non-async-task-runtime-use",
        "family": "concurrency",
        "public_status": "rejected",
        "diagnostic_behavior": "task runtime helpers are rejected from non-async source contexts",
        "evidence": ("tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",),
    },
    {
        "feature_id": "concurrency.executor-helper-state",
        "family": "concurrency",
        "public_status": "internal",
        "evidence": ("native/objc3c/src/runtime/concurrency/executor.cpp",),
    },
    {
        "feature_id": "property.behavior-semantics",
        "family": "property",
        "public_status": "implemented",
        "capability_id": "language.metaprogramming.property-behavior-semantics",
        "support_claim": "objc3c.behavior.language.metaprogramming.property-behavior-semantics",
        "evidence": (
            "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        ),
    },
    {
        "feature_id": "property.unsupported-combinations",
        "family": "property",
        "public_status": "rejected",
        "diagnostic_behavior": "unsupported property behavior combinations fail closed before runtime materialization",
        "evidence": (
            "tests/tooling/fixtures/native/property_behavior_legality_negative_unsupported.objc3",
        ),
    },
    {
        "feature_id": "metaprogramming.derive-expansion-inventory",
        "family": "metaprogramming",
        "public_status": "implemented",
        "capability_id": "language.metaprogramming.derive-expansion-inventory",
        "support_claim": "objc3c.behavior.language.metaprogramming.derive-expansion-inventory",
        "evidence": (
            "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        ),
    },
    {
        "feature_id": "metaprogramming.macro-safety-sandbox-determinism",
        "family": "metaprogramming",
        "public_status": "implemented",
        "capability_id": "language.metaprogramming.macro-safety-sandbox-determinism",
        "support_claim": "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism",
        "evidence": ("tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3",),
    },
    {
        "feature_id": "metaprogramming.host-cache-boundary",
        "family": "metaprogramming",
        "public_status": "implemented",
        "capability_id": "runtime.metaprogramming.host-cache-boundary",
        "support_claim": "objc3c.behavior.runtime.metaprogramming.host-cache-boundary",
        "evidence": ("tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",),
    },
    {
        "feature_id": "metaprogramming.arbitrary-macro-ecosystem",
        "family": "metaprogramming",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.metaprogramming.arbitrary-macro-ecosystem",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "interop.package-loader-bridge",
        "family": "interop",
        "public_status": "implemented",
        "capability_id": "runtime.interop.package-loader-bridge",
        "support_claim": "objc3c.behavior.runtime.interop.package-loader-bridge",
        "evidence": ("tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",),
    },
    {
        "feature_id": "interop.mixed-image-replay",
        "family": "interop",
        "public_status": "implemented",
        "capability_id": "runtime.interop.mixed-image-replay",
        "support_claim": "objc3c.behavior.runtime.interop.mixed-image-replay",
        "evidence": ("tests/tooling/runtime/import_module_execution_matrix_probe.cpp",),
    },
    {
        "feature_id": "interop.broad-runtime-closure",
        "family": "interop",
        "public_status": "reserved",
        "reserved_boundary_id": "runtime.interop.broad-runtime-closure",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "evidence": ("docs/support/hard_cutover_capability_truth.md",),
    },
    {
        "feature_id": "interop.conflicting-bridge-metadata",
        "family": "interop",
        "public_status": "rejected",
        "diagnostic_behavior": "conflicting ObjC++/Swift bridge metadata is rejected by recovery diagnostics",
        "evidence": (
            "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
        ),
    },
)

ADVANCED_RUNTIME_IMPLEMENTED_SUPPORT_CONTRACTS: tuple[dict[str, object], ...] = (
    {
        "contract_id": "objc3c.advanced-runtime.public-support.blocks-escape-capture-legality.v1",
        "capability_id": "language.blocks.escape-capture-legality",
        "support_claim": "objc3c.behavior.language.blocks.escape-capture-legality",
        "public_command": "npm run objc3c -- test-runtime-acceptance-block-arc",
        "contract_scope": "block capture ownership legality, escaping capture rejection, and byref capture diagnostics",
        "source_truth": (
            "native/objc3c/src/parse/objc3_parser_core_block_literal_capture_list.inc",
            "native/objc3c/src/sema/objc3_sema_pass_manager_async_block_message_validation_block_capture.inc",
            "tests/tooling/fixtures/block_arc_closure/boundary_inventory.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
            "tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json",
            "scripts/objc3c_runtime_acceptance/domains/block_arc_surface_ownership_transfer.py",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.blocks.full-language-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.blocks-copy-dispose-invoke.v1",
        "capability_id": "runtime.blocks.copy-dispose-invoke",
        "support_claim": "objc3c.behavior.runtime.blocks.copy-dispose-invoke",
        "public_command": "npm run objc3c -- test-runtime-acceptance-block-arc",
        "contract_scope": "runtime block promotion, copy/dispose helper traffic, invoke thunk behavior, and stale-handle rejection",
        "source_truth": (
            "native/objc3c/src/runtime/blocks/block_runtime_api.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifact_runtime_block_manifest.cpp",
            "scripts/objc3c_runtime_acceptance/domains/block_arc_runtime_cases.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_owned_capture_lifetime_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_negative.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.blocks.full-language-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.blocks-byref-forwarding.v1",
        "capability_id": "runtime.blocks.byref-forwarding",
        "support_claim": "objc3c.behavior.runtime.blocks.byref-forwarding",
        "public_command": "npm run objc3c -- test-runtime-acceptance-block-arc",
        "contract_scope": "byref cell forwarding, heap promotion, duplicate binding rejection, and missing identifier rejection",
        "source_truth": (
            "native/objc3c/src/runtime/blocks/block_runtime_api.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifact_runtime_block_ownership_manifest.cpp",
            "scripts/objc3c_runtime_acceptance/domains/block_arc_runtime_abi_cases.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/execution/positive/byref_capture_argument_materialization.objc3",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/byref_capture_missing_identifier.objc3",
            "tests/tooling/fixtures/native/execution/negative/byref_capture_duplicate_binding.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.blocks.full-language-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.arc-cleanup-integration.v1",
        "capability_id": "language.arc-cleanup.integration",
        "support_claim": "objc3c.behavior.arc-cleanup.integration",
        "public_command": "npm run objc3c -- test-runtime-acceptance-arc-cleanup-integration",
        "contract_scope": "ARC cleanup ordering across errors, async cleanup, property interaction, and block escape failures",
        "source_truth": (
            "native/objc3c/src/runtime/memory/arc.cpp",
            "native/objc3c/src/ir/objc3_ir_message_send_emission.cpp",
            "tests/tooling/fixtures/arc_cleanup_integration/owner_contract.json",
            "scripts/objc3c_runtime_acceptance/suite_catalog.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/arc_cleanup_source_construct_order_positive.objc3",
            "tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3",
            "tests/tooling/fixtures/native/async_cleanup_integration_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
            "tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.arc.full-automation",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.errors-try-catch-semantics.v1",
        "capability_id": "language.errors.try-catch-semantics",
        "support_claim": "objc3c.behavior.errors.try-catch-semantics",
        "public_command": "npm run objc3c -- validate-error-conformance",
        "contract_scope": "try/catch semantics, local handler requirement, rethrow diagnostics, and fail-closed unsupported throw contexts",
        "source_truth": (
            "native/objc3c/src/artifacts/objc3_frontend_error_semantic_json.cpp",
            "native/objc3c/src/lower/contracts/error_handling_throws_unwind_contracts.h",
            "scripts/objc3c_runtime_acceptance/domains/errors_semantic_try_catch_case.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
            "tests/tooling/fixtures/error_runtime_closure/error_propagation_unwind_cleanup_semantic_model.json",
            "scripts/objc3c_runtime_acceptance/domains/errors_semantic_try_catch_case.py",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
            "tests/tooling/fixtures/native/rethrow_requires_throws_or_local_handler_negative.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.errors.generalized-foreign-exception-abi",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.error-unwind-cleanup.v1",
        "capability_id": "compiler.lowering.error-unwind-cleanup",
        "support_claim": "objc3c.behavior.lowering.error-unwind-cleanup",
        "public_command": "npm run objc3c -- validate-error-conformance",
        "contract_scope": "error unwind cleanup lowering, cleanup resume metadata, autoreleasepool unwind probes, and bridged cleanup ordering",
        "source_truth": (
            "native/objc3c/src/artifacts/objc3_frontend_artifact_error_lowering_plan.cpp",
            "native/objc3c/src/lower/contracts/error_handling_lowering_contracts.h",
            "tests/tooling/fixtures/error_runtime_closure/error_propagation_unwind_cleanup_semantic_model.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3",
            "scripts/objc3c_runtime_acceptance/domains/errors_lowering_throw_catch_case.py",
            "tests/tooling/runtime/cleanup_unwind_autoreleasepool_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
            "tests/tooling/fixtures/native/rethrow_requires_throws_or_local_handler_negative.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.errors.generalized-foreign-exception-abi",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.errors-nserror-status-bridge.v1",
        "capability_id": "runtime.errors.nserror-status-bridge",
        "support_claim": "objc3c.behavior.runtime.error-nserror-status-bridge",
        "public_command": "npm run objc3c -- validate-error-conformance",
        "contract_scope": "NSError/status bridge helper metadata, bridge-state runtime helper behavior, and missing-out-parameter diagnostics",
        "source_truth": (
            "native/objc3c/src/runtime/errors/error_bridge_state.cpp",
            "native/objc3c/src/runtime/errors/error_bridge_snapshot_contracts.h",
            "tests/tooling/fixtures/error_runtime_closure/bridged_error_cross_module_compatibility_policy.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
            "tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.errors.generalized-foreign-exception-abi",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.error-live-bridge-cleanup.v1",
        "capability_id": "runtime.errors.live-bridge-cleanup",
        "support_claim": "objc3c.behavior.runtime.error-live-bridge-cleanup",
        "public_command": "npm run objc3c -- validate-error-conformance",
        "contract_scope": "live NSError/status bridge cleanup, runtime bridge helpers, and throwing-call fail-closed diagnostics",
        "source_truth": (
            "native/objc3c/src/runtime/errors/error_bridge.cpp",
            "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
            "scripts/check_error_runtime_closure_semantics_bridge.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
            "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.errors.generalized-foreign-exception-abi",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.concurrency-async-actors.v1",
        "capability_id": "runtime.concurrency.async-actors",
        "support_claim": "objc3c.behavior.runtime.concurrency-async-actors",
        "public_command": "npm run objc3c -- validate-concurrency-conformance",
        "contract_scope": "async actor runtime executor binding, actor mailbox scheduling, live task runtime integration, and invalid actor-handle rejection",
        "source_truth": (
            "native/objc3c/src/runtime/concurrency/actor_isolation.cpp",
            "native/objc3c/src/runtime/concurrency/executor.cpp",
            "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
        ),
        "positive_evidence": (
            "tests/native/runtime/concurrency/actor_executor_contract.objc3",
            "scripts/objc3c_runtime_acceptance/domains/concurrency_live_runtime_cases.py",
            "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/actor_nonisolated_executor_rejected.objc3",
            "tests/tooling/fixtures/native/non_actor_actor_hop_rejected.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.concurrency.broad-async-actor-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.concurrency-task-continuation-lifecycle.v1",
        "capability_id": "runtime.concurrency.task-continuation-lifecycle",
        "support_claim": "objc3c.behavior.runtime.concurrency-task-continuation-lifecycle",
        "public_command": "npm run objc3c -- validate-concurrency-conformance",
        "contract_scope": "task continuation allocation, resume/handoff lifecycle, executor implementation, and non-async rejection",
        "source_truth": (
            "native/objc3c/src/runtime/concurrency/continuation_state.cpp",
            "native/objc3c/src/runtime/concurrency/task_lifecycle.cpp",
            "tests/tooling/fixtures/concurrency_runtime_closure/task_continuation_lifecycle_contract.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
            "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",
            "tests/tooling/fixtures/native/task_group_without_scope_rejected.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.concurrency.broad-async-actor-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.concurrency-actor-mailbox-isolation.v1",
        "capability_id": "runtime.concurrency.actor-mailbox-isolation",
        "support_claim": "objc3c.behavior.runtime.concurrency-actor-mailbox-isolation",
        "public_command": "npm run objc3c -- validate-concurrency-conformance",
        "contract_scope": "actor mailbox enqueue/drain, executor isolation, and invalid actor-hop rejection",
        "source_truth": (
            "native/objc3c/src/runtime/concurrency/actor_isolation.cpp",
            "native/objc3c/src/runtime/concurrency/actor_mailbox.cpp",
            "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3",
            "tests/native/runtime/concurrency/actor_executor_contract.objc3",
            "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
            "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/actor_nonisolated_executor_rejected.objc3",
            "tests/tooling/fixtures/native/non_actor_actor_hop_rejected.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.concurrency.broad-async-actor-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.metaprogramming-property-behavior-semantics.v1",
        "capability_id": "language.metaprogramming.property-behavior-semantics",
        "support_claim": "objc3c.behavior.language.metaprogramming.property-behavior-semantics",
        "public_command": "npm run objc3c -- test-runtime-acceptance",
        "contract_scope": "property behavior semantic legality and runtime materialization boundary without unsupported combinations",
        "source_truth": (
            "native/objc3c/src/artifacts/objc3_frontend_metaprogramming_semantic_json_property_behavior.inc",
            "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json",
            "scripts/objc3c_runtime_acceptance/domains/metaprogramming_derive_property_cases.py",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
            "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/property_behavior_legality_negative_unsupported.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_negative_nonobject.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.metaprogramming.arbitrary-macro-ecosystem",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.metaprogramming-derive-expansion-inventory.v1",
        "capability_id": "language.metaprogramming.derive-expansion-inventory",
        "support_claim": "objc3c.behavior.language.metaprogramming.derive-expansion-inventory",
        "public_command": "npm run objc3c -- validate-metaprogramming-conformance",
        "contract_scope": "derive expansion inventory, selector conflict rejection, and checked macro public-surface metadata",
        "source_truth": (
            "native/objc3c/src/sema/objc3_semantic_passes_protocol_metaprogramming_helpers.inc",
            "native/objc3c/src/artifacts/objc3_frontend_metaprogramming_semantic_json_derive_inventory.inc",
            "tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
            "tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json",
            "native/objc3c/src/sema/objc3_semantic_passes_protocol_metaprogramming_helpers.inc",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/derive_expansion_inventory_negative_unsupported.objc3",
            "tests/tooling/fixtures/native/derive_expansion_inventory_negative_selector_conflict.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.metaprogramming.arbitrary-macro-ecosystem",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.metaprogramming-macro-safety-sandbox-determinism.v1",
        "capability_id": "language.metaprogramming.macro-safety-sandbox-determinism",
        "support_claim": "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism",
        "public_command": "npm run objc3c -- validate-metaprogramming-conformance",
        "contract_scope": "macro package provenance, sandbox metadata, deterministic cache key checks, and invalid package rejection",
        "source_truth": (
            "native/objc3c/src/parse/objc3_parser_attributes_callable_bridge_macro_profile_publication_cache_sandbox.inc",
            "native/objc3c/src/artifacts/objc3_frontend_metaprogramming_semantic_json_macro_safety.inc",
            "tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3",
            "tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json",
            "tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_package.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.metaprogramming.arbitrary-macro-ecosystem",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.metaprogramming-host-cache-boundary.v1",
        "capability_id": "runtime.metaprogramming.host-cache-boundary",
        "support_claim": "objc3c.behavior.runtime.metaprogramming.host-cache-boundary",
        "public_command": "npm run objc3c -- test-runtime-acceptance",
        "contract_scope": "macro host process cache boundary, cross-module cache replay, and cache metadata fail-closed checks",
        "source_truth": (
            "native/objc3c/src/driver/objc3_driver_metaprogramming_cache_publication.cpp",
            "native/objc3c/src/runtime/metadata/runtime_snapshot_helpers.cpp",
            "tests/tooling/fixtures/metaprogramming_interop_closure/lowering_runtime_artifact_contract.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_cache_key.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_sandbox_policy.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.metaprogramming.arbitrary-macro-ecosystem",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.interop-package-loader-bridge.v1",
        "capability_id": "runtime.interop.package-loader-bridge",
        "support_claim": "objc3c.behavior.runtime.interop.package-loader-bridge",
        "public_command": "npm run objc3c -- validate-interop-conformance",
        "contract_scope": "package loader bridge metadata, header bridge generation, and tampered bridge rejection",
        "source_truth": (
            "native/objc3c/src/artifacts/interop/interop_bridge_artifacts.cpp",
            "native/objc3c/src/driver/objc3_driver_interop_artifact_publication.cpp",
            "tests/tooling/fixtures/metaprogramming_interop_closure/interop_runtime_ownership_abi_policy.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
            "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3",
            "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",
            "tests/tooling/runtime/header_module_bridge_generation_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/runtime/package_loader_fail_closed_diagnostics_probe.cpp",
            "tests/tooling/fixtures/runtime_import_interop_bridge_metadata/tampered_bridge_surface.json",
            "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.interop.broad-runtime-closure",
        ),
    },
    {
        "contract_id": "objc3c.advanced-runtime.public-support.interop-mixed-image-replay.v1",
        "capability_id": "runtime.interop.mixed-image-replay",
        "support_claim": "objc3c.behavior.runtime.interop.mixed-image-replay",
        "public_command": "npm run objc3c -- validate-interop-conformance",
        "contract_scope": "mixed-image runtime registration replay, package provider/consumer proof, and duplicate declaration rejection",
        "source_truth": (
            "native/objc3c/src/runtime/images/registration_api.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.h",
            "tests/tooling/fixtures/metaprogramming_interop_closure/packaged_interop_proof_contract.json",
        ),
        "positive_evidence": (
            "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3",
            "tests/tooling/fixtures/native/runtime_packaging_provider.objc3",
            "tests/tooling/runtime/import_module_execution_matrix_probe.cpp",
            "tests/tooling/runtime/multi_image_registration_reset_replay_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
            "tests/tooling/fixtures/native/missing_replay_proof_rejected.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
        ),
        "reserved_non_public_capabilities": (
            "runtime.interop.broad-runtime-closure",
        ),
    },
)

ADVANCED_RUNTIME_RESERVED_BOUNDARIES: tuple[dict[str, str], ...] = (
    {
        "boundary_id": "runtime.blocks.full-language-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Only explicit capture legality, copy/dispose/invoke, and byref forwarding rows are public; broader block language closure remains unavailable.",
    },
    {
        "boundary_id": "runtime.arc.full-automation",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "ARC cleanup integration is public only for the checked helper-backed rows; broad ARC ownership automation is not claimed.",
    },
    {
        "boundary_id": "runtime.errors.generalized-foreign-exception-abi",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "NSError/status bridge cleanup has executable evidence; generalized foreign exception ABI support remains unclaimed.",
    },
    {
        "boundary_id": "runtime.concurrency.broad-async-actor-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Task continuation and actor mailbox rows are public; scheduler fairness, distributed actors, and broad async ABI closure remain reserved.",
    },
    {
        "boundary_id": "runtime.metaprogramming.arbitrary-macro-ecosystem",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Checked derive inventory, property behavior semantics, and host/cache boundaries are public; arbitrary third-party macro expansion remains unavailable.",
    },
    {
        "boundary_id": "runtime.interop.broad-runtime-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Package-loader bridge and mixed-image replay rows are public; broad interop closure and public ABI widening remain reserved.",
    },
)


def build_advanced_runtime_capability_split_contract() -> dict[str, Any]:
    return {
        "issue": ADVANCED_RUNTIME_SPLIT_ISSUE,
        "reserved_umbrella": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "source": ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE,
        "implemented_rows": list(ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS),
        "implemented_support_contracts": list(
            ADVANCED_RUNTIME_IMPLEMENTED_SUPPORT_CONTRACTS
        ),
        "feature_taxonomy": list(ADVANCED_RUNTIME_FEATURE_TAXONOMY),
        "reserved_boundaries": list(ADVANCED_RUNTIME_RESERVED_BOUNDARIES),
    }


__all__ = [
    "ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE",
    "ADVANCED_RUNTIME_FEATURE_TAXONOMY",
    "ADVANCED_RUNTIME_IMPLEMENTED_SUPPORT_CONTRACTS",
    "ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS",
    "ADVANCED_RUNTIME_RESERVED_BOUNDARIES",
    "ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID",
    "ADVANCED_RUNTIME_SPLIT_ISSUE",
    "build_advanced_runtime_capability_split_contract",
]
