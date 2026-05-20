"""Unified concurrency runtime ABI surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_surface_support import (
    authoritative_case_ids,
)

from ..c_api import (
    PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
    UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
    UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
    UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
    UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
    UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
)
from ..runtime_contract_concurrency import (
    ACTOR_RUNTIME_ABI_PROBE,
    CONTINUATION_RUNTIME_ABI_PROBE,
    RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
    TASK_RUNTIME_ABI_PROBE,
)


def build_runtime_unified_concurrency_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "unified_concurrency_source_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "async_task_actor_normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "unified_concurrency_lowering_metadata_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_unified_concurrency_runtime_abi_boundary": (
            PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
        ),
        "async_continuation_state_snapshot_symbol": (
            "objc3_runtime_copy_async_continuation_state_for_testing"
        ),
        "task_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_task_runtime_state_for_testing"
        ),
        "actor_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_actor_runtime_state_for_testing"
        ),
        "runtime_abi_boundary_model": UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
        "continuation_runtime_model": UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
        "task_runtime_model": UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
        "actor_runtime_model": UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
        "fail_closed_model": UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
        "required_task_scheduler_snapshot_fields": [
            "scheduler_enqueue_count",
            "scheduler_dequeue_count",
            "last_scheduled_task_handle",
            "last_scheduled_executor_tag",
            "last_dequeued_task_handle",
            "last_dequeued_executor_tag",
            "last_executor_queue_depth",
            "max_executor_queue_depth",
            "scheduler_sequence",
            "deadlock_guard_passed",
            "race_guard_passed",
        ],
        "requires_pre_drain_scheduler_snapshot": True,
        "requires_post_drain_scheduler_snapshot": True,
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"unified-concurrency-runtime-abi"},
        ),
        "authoritative_probe_paths": [
            CONTINUATION_RUNTIME_ABI_PROBE,
            TASK_RUNTIME_ABI_PROBE,
            ACTOR_RUNTIME_ABI_PROBE,
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = ["build_runtime_unified_concurrency_runtime_abi_surface"]
