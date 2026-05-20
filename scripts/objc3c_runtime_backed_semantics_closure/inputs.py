from __future__ import annotations

from objc3c_runtime_backed_semantics_closure.paths import LOWERING_CONTRACT_CPP
from objc3c_runtime_backed_semantics_closure.paths import ROOT
from objc3c_runtime_backed_semantics_closure.paths import STATIC_ANALYSIS

LIVE_ERROR_RUNTIME_SURFACE_FLAG = "--objc3-enable-live-error-runtime-surface"

POSITIVE_FIXTURES = {
    "block_arc_autorelease_return": ROOT / "tests" / "tooling" / "fixtures" / "native" / "arc_block_autorelease_return_positive.objc3",
    "arc_weak_autoreleasepool": ROOT / "tests" / "tooling" / "fixtures" / "native" / "reference_counting_weak_autoreleasepool_positive.objc3",
    "error_runtime_bridge_helper": ROOT / "tests" / "tooling" / "fixtures" / "native" / "error_runtime_bridge_helper_positive.objc3",
    "live_error_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_error_runtime_integration_positive.objc3",
    "live_continuation_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_continuation_runtime_integration_positive.objc3",
    "live_task_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_task_runtime_and_executor_implementation_positive.objc3",
    "live_actor_mailbox_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_actor_mailbox_runtime_positive.objc3",
}

POSITIVE_FIXTURE_EXTRA_ARGS = {
    "error_runtime_bridge_helper": [LIVE_ERROR_RUNTIME_SURFACE_FLAG],
    "live_error_runtime": [LIVE_ERROR_RUNTIME_SURFACE_FLAG],
}

POSITIVE_FIXTURE_IR_CALL_TOKENS = {
    "block_arc_autorelease_return": [
        "call void @__objc3_block_copy_helper_",
        "call i32 @objc3_runtime_promote_block_i32",
        "call void @__objc3_block_dispose_helper_",
        "define internal i32 @__objc3_block_invoke_",
        "@__objc3_block_desc_",
        "call i32 @objc3_runtime_retain_i32",
        "call i32 @objc3_runtime_release_i32",
        "call i32 @objc3_runtime_autorelease_i32",
    ],
    "arc_weak_autoreleasepool": [
        "call void @objc3_runtime_push_autoreleasepool_scope()",
        "call void @objc3_runtime_pop_autoreleasepool_scope()",
        "call i32 @objc3_runtime_read_current_property_i32()",
        "call i32 @objc3_runtime_exchange_current_property_i32",
        "call i32 @objc3_runtime_load_weak_current_property_i32()",
        "call void @objc3_runtime_store_weak_current_property_i32",
        "call i32 @objc3_runtime_retain_i32",
        "call i32 @objc3_runtime_release_i32",
        "call i32 @objc3_runtime_autorelease_i32",
    ],
    "error_runtime_bridge_helper": [
        "call void @objc3_runtime_store_thrown_error_i32",
        "call i32 @objc3_runtime_load_thrown_error_i32",
        "call i32 @objc3_runtime_bridge_status_error_i32",
        "call i32 @objc3_runtime_bridge_nserror_error_i32",
        "call i32 @objc3_runtime_catch_matches_error_i32",
    ],
    "live_error_runtime": [
        "call void @objc3_runtime_store_thrown_error_i32",
        "call i32 @objc3_runtime_load_thrown_error_i32",
        "call i32 @objc3_runtime_bridge_status_error_i32",
        "call i32 @objc3_runtime_catch_matches_error_i32",
    ],
    "live_continuation_runtime": [
        "call i32 @objc3_runtime_allocate_async_continuation_i32",
        "call i32 @objc3_runtime_handoff_async_continuation_to_executor_i32",
        "call i32 @objc3_runtime_resume_async_continuation_i32",
    ],
    "live_task_runtime": [
        "call i32 @objc3_runtime_spawn_task_i32",
        "call i32 @objc3_runtime_enter_task_group_scope_i32",
        "call i32 @objc3_runtime_add_task_group_task_i32",
        "call i32 @objc3_runtime_wait_task_group_next_i32",
        "call i32 @objc3_runtime_cancel_task_group_i32",
        "call i32 @objc3_runtime_task_is_cancelled_i32",
        "call i32 @objc3_runtime_task_on_cancel_i32",
        "call i32 @objc3_runtime_executor_hop_i32",
    ],
    "live_actor_mailbox_runtime": [
        "call i32 @objc3_runtime_actor_mailbox_enqueue_i32",
        "call i32 @objc3_runtime_actor_mailbox_drain_next_i32",
        "call i32 @objc3_runtime_actor_record_replay_proof_i32",
        "call i32 @objc3_runtime_actor_record_race_guard_i32",
        "call i32 @objc3_runtime_actor_enter_nonisolated_i32",
    ],
}

POSITIVE_FIXTURE_IMPORT_SURFACE_EXPECTATIONS = {
    "block_arc_autorelease_return": {
        "objc_runtime_block_ownership_artifact_preservation": {
            "booleans": {
                "runtime_import_artifact_ready": True,
                "separate_compilation_preservation_ready": True,
                "runtime_support_library_link_wiring_ready": True,
                "arc_cleanup_preservation_ready": True,
            },
            "minimums": {
                "local_block_literal_sites": 1,
                "local_copy_helper_required_sites": 1,
                "local_dispose_helper_required_sites": 1,
                "local_copy_helper_symbolized_sites": 1,
                "local_dispose_helper_symbolized_sites": 1,
                "local_escape_to_heap_sites": 1,
                "local_arc_autorelease_insertion_sites": 1,
            },
        },
    },
    "arc_weak_autoreleasepool": {
        "objc_runtime_block_ownership_artifact_preservation": {
            "booleans": {
                "runtime_import_artifact_ready": True,
                "separate_compilation_preservation_ready": True,
                "runtime_support_library_link_wiring_ready": True,
                "arc_cleanup_preservation_ready": True,
            },
            "minimums": {
                "local_arc_retain_insertion_sites": 1,
                "local_arc_release_insertion_sites": 1,
                "local_autoreleasepool_scope_sites": 1,
            },
        },
    },
    "error_runtime_bridge_helper": {
        "objc_error_handling_result_and_bridging_artifact_replay": {
            "booleans": {
                "runtime_import_artifact_ready": True,
                "binary_artifact_replay_ready": True,
                "separate_compilation_replay_ready": True,
            },
            "replay_key_minimums": {
                "throws_replay_key": {
                    "throws_propagation_sites": 1,
                },
                "ns_error_replay_key": {
                    "ns_error_bridging_sites": 1,
                    "ns_error_out_parameter_sites": 1,
                },
            },
        },
    },
    "live_error_runtime": {
        "objc_error_handling_result_and_bridging_artifact_replay": {
            "booleans": {
                "runtime_import_artifact_ready": True,
                "binary_artifact_replay_ready": True,
                "separate_compilation_replay_ready": True,
            },
            "replay_key_minimums": {
                "throws_replay_key": {
                    "throws_propagation_sites": 1,
                },
                "ns_error_replay_key": {
                    "ns_error_bridging_sites": 1,
                    "ns_error_out_parameter_sites": 1,
                },
            },
        },
    },
    "live_actor_mailbox_runtime": {
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface": {
            "booleans": {
                "actor_mailbox_runtime_ready": True,
            },
            "minimums": {
                "actor_interface_sites": 1,
                "actor_method_sites": 1,
                "actor_metadata_record_sites": 1,
                "nonisolated_entry_sites": 1,
                "executor_affinity_sites": 1,
                "actor_hop_artifact_sites": 1,
                "actor_isolation_thunk_sites": 1,
                "replay_proof_dependency_sites": 1,
                "race_guard_dependency_sites": 1,
                "task_handoff_sites": 1,
            },
        },
    },
}

NEGATIVE_FIXTURES = {
    "escaping_block_actor_handoff": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "escaping_block_with_handoff_rejected.objc3",
        "codes": ["O3S294"],
    },
    "throw_without_throws_or_catch": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "throw_requires_throws_or_catch_negative.objc3",
        "codes": ["O3S274"],
    },
    "rethrow_without_throws_or_local_handler": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "rethrow_requires_throws_or_local_handler_negative.objc3",
        "codes": ["O3S284"],
    },
    "throwing_call_without_try": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "throwing_call_requires_try_negative.objc3",
        "codes": ["O3S341"],
    },
    "task_group_without_scope": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "task_group_without_scope_rejected.objc3",
        "codes": ["O3S228"],
    },
    "actor_hop_without_async": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "actor_hop_without_async_rejected.objc3",
        "codes": ["O3S289"],
    },
    "non_actor_actor_hop": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "non_actor_actor_hop_rejected.objc3",
        "codes": ["O3S342"],
    },
    "non_actor_non_sendable_crossing": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "non_actor_non_sendable_crossing_rejected.objc3",
        "codes": ["O3S343"],
    },
    "non_actor_actor_isolated_body": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "non_actor_actor_isolated_body_rejected.objc3",
        "codes": ["O3S344"],
    },
    "nonisolated_actor_isolated_body": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "nonisolated_actor_isolated_body_rejected.objc3",
        "codes": ["O3S345"],
    },
}

DURABLE_REPLAY_DIRS = [
    "validation_block_literal_capture_contract",
    "validation_block_abi_invoke_trampoline_contract",
    "validation_block_storage_escape_contract",
    "validation_block_copy_dispose_contract",
    "validation_arc_diagnostics_fixit_contract",
    "validation_error_diagnostics_recovery_contract",
    "validation_ns_error_bridging_contract",
    "validation_result_like_lowering_contract",
    "validation_async_continuation_contract",
    "validation_await_lowering_suspension_state_contract",
    "validation_actor_isolation_sendability_contract",
    "validation_task_runtime_interop_cancellation_contract",
    "validation_concurrency_replay_contract",
]

HELPER_SYMBOLS = [
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_store_thrown_error_i32",
    "objc3_runtime_load_thrown_error_i32",
    "objc3_runtime_bridge_status_error_i32",
    "objc3_runtime_bridge_nserror_error_i32",
    "objc3_runtime_catch_matches_error_i32",
    "objc3_runtime_allocate_async_continuation_i32",
    "objc3_runtime_handoff_async_continuation_to_executor_i32",
    "objc3_runtime_resume_async_continuation_i32",
    "objc3_runtime_spawn_task_i32",
    "objc3_runtime_enter_task_group_scope_i32",
    "objc3_runtime_add_task_group_task_i32",
    "objc3_runtime_wait_task_group_next_i32",
    "objc3_runtime_cancel_task_group_i32",
    "objc3_runtime_task_is_cancelled_i32",
    "objc3_runtime_task_on_cancel_i32",
    "objc3_runtime_executor_hop_i32",
    "objc3_runtime_actor_enter_isolation_thunk_i32",
    "objc3_runtime_actor_enter_nonisolated_i32",
    "objc3_runtime_actor_hop_to_executor_i32",
    "objc3_runtime_actor_record_replay_proof_i32",
    "objc3_runtime_actor_record_race_guard_i32",
    "objc3_runtime_actor_bind_executor_i32",
    "objc3_runtime_actor_mailbox_enqueue_i32",
    "objc3_runtime_actor_mailbox_drain_next_i32",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_promote_block_i32",
    "objc3_runtime_invoke_block_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
]

REQUIRED_IR_TOKENS = [
    "runtime_backed_semantics_closure = contract=objc3c.runtime.backed.semantics.closure.v1",
    "runtime_block_byref_forwarding_heap_promotion_ownership_interop",
    "runtime_block_allocation_copy_dispose_invoke_support",
    "__objc3_block_copy_helper_",
    "__objc3_block_dispose_helper_",
    "runnable_block_execution_matrix",
    "arc_automatic_insertions",
    "arc_cleanup_weak_lifetime_hooks",
    "arc_block_autorelease_return_lowering",
    "runnable_arc_closeout",
    "error_handling_error_runtime_bridge_helper",
    "error_handling_live_error_runtime_integration",
    "concurrency_live_continuation_runtime_integration",
    "concurrency_live_task_runtime_integration",
    "concurrency_task_runtime_hardening",
    "actor_lowering_metadata_contract",
    "concurrency_replay_race_guard_lowering",
]

SOURCE_TOKENS = {
    "lowering_contract_header": {
        ROOT / "native" / "objc3c" / "src" / "lower" / "contracts" / "block_runtime_helper_contracts.h": [
            "kObjc3RuntimeBackedSemanticsClosureContractId",
            "kObjc3RuntimeBackedSemanticsClosureBlockModel",
            "kObjc3RuntimeBackedSemanticsClosureArcModel",
            "kObjc3RuntimeBackedSemanticsClosureErrorModel",
            "kObjc3RuntimeBackedSemanticsClosureConcurrencyModel",
            "Objc3RuntimeBackedSemanticsClosureSummary",
        ],
    },
    "lowering_contract_cpp": {
        LOWERING_CONTRACT_CPP: [
            "Objc3RuntimeBackedSemanticsClosureSummary()",
            "runtime_helper_count=36",
            "kObjc3RuntimeBackedSemanticsClosureFailClosedModel",
        ],
    },
    "ir_emitter": {
        ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_module_metadata_publication_runtime_gates.cpp": [
            "runtime_backed_semantics_closure",
            "Objc3RuntimeBackedSemanticsClosureSummary()",
            "Objc3RunnableArcCloseoutSummary()",
        ],
    },
    "sema_pass_manager": {
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_block_equivalence.cpp": [
            "IsEquivalentBlockStorageEscapeSemanticsSummary",
            "IsEquivalentBlockCopyDisposeSemanticsSummary",
        ],
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_dispatch_ownership_equivalence.cpp": [
            "IsEquivalentRetainReleaseOperationSummary",
            "IsEquivalentWeakUnownedSemanticsSummary",
        ],
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_type_summary_equivalence.cpp": [
            "IsEquivalentThrowsPropagationSummary",
            "IsEquivalentAsyncContinuationSummary",
            "IsEquivalentTaskRuntimeCancellationSummary",
            "IsEquivalentActorIsolationSendabilitySummary",
        ],
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_control_flow_equivalence.cpp": [
            "IsEquivalentConcurrencyReplayRaceGuardSummary",
        ],
    },
    "semantic_passes": {
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_body_validation_entrypoints.inc": [
            "ResolveObjc3GlobalInitializerValues",
        ],
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_generic_protocol_message_validation_block_captures.inc": [
            "ResolveBlockCaptureSemanticType",
        ],
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_task_runtime_summary_builders.inc": [
            "BuildTaskRuntimeCancellationSummaryFromIntegrationSurface",
        ],
    },
    "static_analysis": {
        STATIC_ANALYSIS: [
            "BlockAlwaysReturns",
        ],
    },
}
