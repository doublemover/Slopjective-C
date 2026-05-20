"""Block/ARC runtime ABI probe payload assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from ..c_api import (
    BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
    BLOCK_ARC_RUNTIME_ARC_MODEL,
    BLOCK_ARC_RUNTIME_BLOCK_MODEL,
    BLOCK_ARC_RUNTIME_DESCRIPTOR_MODEL,
    BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    BLOCK_ARC_RUNTIME_INVOKE_THUNK_MODEL,
)


def _expect_preserved_fields(
    payload: dict[str, Any],
    expected_fields: dict[str, object],
) -> None:
    for field, expected_value in expected_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )


def assert_block_arc_runtime_abi_payload(payload: dict[str, Any]) -> int:
    expected_string_fields = {
        "block_promote_symbol": "objc3_runtime_promote_block_i32",
        "block_invoke_symbol": "objc3_runtime_invoke_block_i32",
        "retain_symbol": "objc3_runtime_retain_i32",
        "release_symbol": "objc3_runtime_release_i32",
        "autorelease_symbol": "objc3_runtime_autorelease_i32",
        "autoreleasepool_push_symbol": "objc3_runtime_push_autoreleasepool_scope",
        "autoreleasepool_pop_symbol": "objc3_runtime_pop_autoreleasepool_scope",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "block_descriptor_model": BLOCK_ARC_RUNTIME_DESCRIPTOR_MODEL,
        "block_invoke_thunk_model": BLOCK_ARC_RUNTIME_INVOKE_THUNK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    }
    _expect_preserved_fields(payload, expected_string_fields)

    expected_integer_fields = {
        "abi_status": 0,
        "arc_status": 0,
        "retained": 77,
        "autoreleased": 77,
        "released": 77,
        "invoke_result": 17,
        "stale_invoke_result": 0,
        "copy_count": 1,
        "dispose_count": 1,
        "private_runtime_abi_ready": 1,
        "public_runtime_header_unchanged": 1,
        "deterministic": 1,
        "live_runtime_block_handle_count": 0,
        "block_promote_call_count": 1,
        "block_invoke_call_count": 2,
        "retain_call_count": 2,
        "release_call_count": 3,
        "autorelease_call_count": 1,
        "autoreleasepool_push_count": 1,
        "autoreleasepool_pop_count": 1,
        "current_property_read_count": 0,
        "current_property_write_count": 0,
        "current_property_exchange_count": 0,
        "weak_current_property_load_count": 0,
        "weak_current_property_store_count": 0,
        "last_descriptor_storage_size_bytes": 32,
        "last_descriptor_capture_count": 1,
        "last_descriptor_storage_word_count": 4,
        "last_descriptor_flags": 7,
        "last_invoke_plan_storage_word_count": 0,
        "last_promote_has_pointer_capture_storage": 1,
        "last_descriptor_parameter_count": 4,
        "last_descriptor_has_invoke": 1,
        "last_descriptor_has_copy_helper": 1,
        "last_descriptor_has_dispose_helper": 1,
        "last_descriptor_has_pointer_capture_storage": 1,
        "last_invoke_plan_has_descriptor": 0,
        "last_invoke_plan_has_invoke": 0,
        "last_invoke_plan_was_runnable": 0,
        "last_block_invoke_result": 17,
        "last_autorelease_value": 77,
        "arc_retain_call_count": 2,
        "arc_release_call_count": 3,
        "arc_autorelease_call_count": 1,
        "arc_autoreleasepool_push_count": 1,
        "arc_autoreleasepool_pop_count": 1,
    }
    _expect_preserved_fields(payload, expected_integer_fields)

    expect(
        isinstance(payload.get("last_descriptor_address"), int)
        and payload["last_descriptor_address"] > 0,
        "expected block ARC runtime ABI probe to publish a non-zero descriptor address",
    )
    expect(
        isinstance(payload.get("last_descriptor_invoke_address"), int)
        and payload["last_descriptor_invoke_address"] > 0,
        "expected block ARC runtime ABI probe to publish a non-zero descriptor invoke address",
    )

    handle = payload.get("handle")
    expect(
        isinstance(handle, int) and handle > 0,
        "expected block ARC runtime ABI probe to publish a live promoted block handle",
    )
    expect(
        payload.get("retain_handle_result") == handle
        and payload.get("release_handle_result") == handle
        and payload.get("final_release_result") == handle,
        "expected block ARC runtime ABI probe to preserve block handle retain/release traffic",
    )
    expect(
        payload.get("last_promoted_block_handle") == handle
        and payload.get("last_invoked_block_handle") == handle,
        "expected block ARC runtime ABI probe to preserve the last promoted and invoked block handle",
    )
    expect(
        payload.get("last_retain_value") == handle
        and payload.get("last_release_value") == handle,
        "expected block ARC runtime ABI probe to preserve the last ARC retain/release value",
    )

    return handle


__all__ = ["assert_block_arc_runtime_abi_payload"]
