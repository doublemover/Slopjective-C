"""Block helper runtime assertions for Block/ARC execution cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


_BYREF_FORWARDING_SUMMARY_FIELDS = {
    "handle": "byref_forwarding_probe_handle",
    "second_handle": "byref_forwarding_second_probe_handle",
    "copy_count_after_promotion": "byref_forwarding_copy_count_after_promotion",
    "copy_count_after_second_promotion": "byref_forwarding_copy_count_after_second_promotion",
    "seed_stack_address": "byref_forwarding_seed_stack_address",
    "seed_forwarding_address_after_first_promotion": "byref_forwarding_seed_forwarding_address_after_first_promotion",
    "seed_forwarding_address_after_second_promotion": "byref_forwarding_seed_forwarding_address_after_second_promotion",
    "payload_stack_address": "byref_forwarding_payload_stack_address",
    "payload_forwarding_address_after_first_promotion": "byref_forwarding_payload_forwarding_address_after_first_promotion",
    "payload_forwarding_address_after_second_promotion": "byref_forwarding_payload_forwarding_address_after_second_promotion",
    "stack_seed_forwarding_updated": "byref_forwarding_stack_seed_forwarding_updated",
    "stack_payload_forwarding_updated": "byref_forwarding_stack_payload_forwarding_updated",
    "second_handle_shared_seed_forwarding": "byref_forwarding_second_handle_shared_seed_forwarding",
    "second_handle_shared_payload_forwarding": "byref_forwarding_second_handle_shared_payload_forwarding",
    "first_invoke_result": "byref_forwarding_first_invoke_result",
    "second_invoke_result": "byref_forwarding_second_invoke_result",
    "forwarded_stack_write_invoke_result": "byref_forwarding_forwarded_stack_write_invoke_result",
    "dispose_count_before_final_release": "byref_forwarding_dispose_count_before_final_release",
    "dispose_count_after_final_release": "byref_forwarding_dispose_count_after_final_release",
    "last_disposed_value": "byref_forwarding_last_disposed_value",
    "byref_destroy_count_after_final_release": "byref_forwarding_destroy_count_after_final_release",
    "final_release_result": "byref_forwarding_final_release_result",
    "invoke_after_release_result": "byref_forwarding_invoke_after_release_result",
    "second_final_release_result": "byref_forwarding_second_final_release_result",
    "second_invoke_after_release_result": "byref_forwarding_second_invoke_after_release_result",
    "byref_destroy_count_after_second_final_release": "byref_forwarding_destroy_count_after_second_final_release",
    "byref_destroyed_value_sum": "byref_forwarding_destroyed_value_sum",
}

_OWNED_CAPTURE_LIFETIME_SUMMARY_FIELDS = {
    "handle": "owned_capture_lifetime_probe_handle",
    "owned_capture_value": "owned_capture_lifetime_value",
    "initial_capture_found": "owned_capture_lifetime_initial_found",
    "initial_capture_retain_count": "owned_capture_lifetime_initial_retain_count",
    "copy_count_after_promotion": "owned_capture_lifetime_copy_count_after_promotion",
    "owned_capture_retain_count_after_promotion": "owned_capture_lifetime_retain_count_after_promotion",
    "last_retained_owned_capture": "owned_capture_lifetime_last_retained_value",
    "after_promotion_capture_found": "owned_capture_lifetime_after_promotion_found",
    "after_promotion_capture_retain_count": "owned_capture_lifetime_after_promotion_retain_count",
    "release_original_owner_result": "owned_capture_lifetime_release_original_owner_result",
    "after_original_release_capture_found": "owned_capture_lifetime_after_original_release_found",
    "after_original_release_capture_retain_count": "owned_capture_lifetime_after_original_release_retain_count",
    "invoke_result": "owned_capture_lifetime_invoke_result",
    "retain_block_result": "owned_capture_lifetime_retain_block_result",
    "release_retained_block_result": "owned_capture_lifetime_release_retained_block_result",
    "dispose_count_before_final_release": "owned_capture_lifetime_dispose_count_before_final_release",
    "owned_capture_release_count_before_final_release": "owned_capture_lifetime_release_count_before_final_release",
    "final_release_result": "owned_capture_lifetime_final_release_result",
    "dispose_count_after_final_release": "owned_capture_lifetime_dispose_count_after_final_release",
    "owned_capture_release_count_after_final_release": "owned_capture_lifetime_release_count_after_final_release",
    "last_released_owned_capture": "owned_capture_lifetime_last_released_value",
    "after_final_release_capture_found": "owned_capture_lifetime_after_final_release_found",
    "after_final_release_capture_retain_count": "owned_capture_lifetime_after_final_release_retain_count",
    "post_release_callback_count": "owned_capture_lifetime_post_release_callback_count",
    "invoke_after_release_result": "owned_capture_lifetime_invoke_after_release_result",
}

_OPTIONAL_BYREF_FORWARDING_FIELD_PREFIX_PAIRS = (
    ("stack_forwarding", "heap_forwarding"),
    ("stack_forwarded", "heap_forwarded"),
    ("stack_byref_forwarding", "heap_byref_forwarding"),
    ("stack_byref_forwarded", "heap_byref_forwarded"),
)


def _is_optional_byref_forwarding_field(field: str) -> bool:
    return any(
        field.startswith(prefix)
        for prefixes in _OPTIONAL_BYREF_FORWARDING_FIELD_PREFIX_PAIRS
        for prefix in prefixes
    )


def byref_forwarding_probe_summary(payload: dict[str, Any]) -> dict[str, Any]:
    summary = {
        summary_field: payload.get(payload_field)
        for payload_field, summary_field in _BYREF_FORWARDING_SUMMARY_FIELDS.items()
    }
    for field in sorted(payload):
        if _is_optional_byref_forwarding_field(field):
            summary[f"byref_forwarding_{field}"] = payload.get(field)
    return summary


def owned_capture_lifetime_probe_summary(
    payload: dict[str, Any],
) -> dict[str, Any]:
    return {
        summary_field: payload.get(payload_field)
        for payload_field, summary_field in _OWNED_CAPTURE_LIFETIME_SUMMARY_FIELDS.items()
    }


def _expect_optional_stack_heap_forwarding_fields(
    payload: dict[str, Any],
    *,
    label: str,
) -> None:
    for stack_prefix, heap_prefix in _OPTIONAL_BYREF_FORWARDING_FIELD_PREFIX_PAIRS:
        stack_suffixes = {
            field.removeprefix(stack_prefix)
            for field in payload
            if field.startswith(stack_prefix)
        }
        heap_suffixes = {
            field.removeprefix(heap_prefix)
            for field in payload
            if field.startswith(heap_prefix)
        }
        missing_heap = sorted(stack_suffixes - heap_suffixes)
        missing_stack = sorted(heap_suffixes - stack_suffixes)
        expect(
            not missing_heap and not missing_stack,
            (
                f"expected {label} to publish balanced stack/heap forwarding fields; "
                f"missing heap suffixes {missing_heap!r}, missing stack suffixes {missing_stack!r}"
            ),
        )
        for suffix in sorted(stack_suffixes & heap_suffixes):
            stack_field = f"{stack_prefix}{suffix}"
            heap_field = f"{heap_prefix}{suffix}"
            expect(
                payload.get(stack_field) is not None
                and payload.get(heap_field) is not None,
                (
                    f"expected {label} to publish non-null stack/heap forwarding "
                    f"fields {stack_field!r} and {heap_field!r}"
                ),
            )


def assert_byref_runtime_fixture(
    returncode: int,
    copy_dispose_surface: dict[str, Any],
) -> None:
    expect(
        returncode == 14,
        f"expected byref runtime positive fixture to exit 14, saw {returncode}",
    )
    expect(
        copy_dispose_surface.get("copy_helper_required_sites") == 1
        and copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected byref runtime positive fixture to require copy/dispose helpers",
    )


def assert_owned_runtime_fixture(
    returncode: int,
    copy_dispose_surface: dict[str, Any],
) -> None:
    expect(
        returncode == 11,
        f"expected owned object capture runtime positive fixture to exit 11, saw {returncode}",
    )
    expect(
        copy_dispose_surface.get("copy_helper_required_sites") == 1
        and copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned runtime positive fixture to require copy/dispose helpers",
    )


def assert_nonowning_runtime_fixture(
    returncode: int,
    copy_dispose_surface: dict[str, Any],
) -> None:
    expect(
        returncode == 9,
        f"expected non-owning object capture runtime positive fixture to exit 9, saw {returncode}",
    )
    expect(
        copy_dispose_surface.get("copy_helper_required_sites") == 0
        and copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning runtime positive fixture to elide copy/dispose helpers",
    )


def assert_byref_forwarding_probe_payload(payload: dict[str, Any]) -> None:
    handle = payload.get("handle")
    second_handle = payload.get("second_handle")
    expect(
        isinstance(handle, int) and handle > 0,
        "expected byref forwarding probe to publish a positive runtime block handle",
    )
    expect(
        isinstance(second_handle, int) and second_handle > 0,
        "expected byref forwarding probe to publish a positive second runtime block handle",
    )
    expect(
        payload.get("copy_count_after_promotion") == 1,
        "expected byref forwarding probe to execute one copy helper during promotion",
    )
    expect(
        payload.get("copy_count_after_second_promotion") == 2,
        "expected byref forwarding probe to execute one copy helper per promoted block",
    )
    expect(
        payload.get("stack_seed_forwarding_updated") == 1
        and payload.get("stack_payload_forwarding_updated") == 1,
        "expected byref forwarding probe to update stack cells to heap forwarding cells",
    )
    expect(
        payload.get("seed_stack_address")
        != payload.get("seed_forwarding_address_after_first_promotion")
        and payload.get("payload_stack_address")
        != payload.get("payload_forwarding_address_after_first_promotion"),
        "expected byref forwarding probe to publish distinct stack and heap forwarding cell addresses",
    )
    expect(
        payload.get("second_handle_shared_seed_forwarding") == 1
        and payload.get("second_handle_shared_payload_forwarding") == 1,
        "expected byref forwarding probe to share heap forwarding cells across promoted blocks",
    )
    expect(
        payload.get("seed_forwarding_address_after_first_promotion")
        == payload.get("seed_forwarding_address_after_second_promotion")
        and payload.get("payload_forwarding_address_after_first_promotion")
        == payload.get("payload_forwarding_address_after_second_promotion"),
        "expected byref forwarding probe to keep stable forwarding addresses across promotions",
    )
    expect(
        payload.get("first_invoke_result") == 23
        and payload.get("second_invoke_result") == 25,
        "expected byref forwarding probe to preserve runtime-owned forwarded cell state across invokes",
    )
    expect(
        payload.get("forwarded_stack_write_invoke_result") == 42,
        "expected byref forwarding probe to route stack-forwarded writes through the shared heap cell",
    )
    expect(
        payload.get("dispose_count_before_final_release") == 0,
        "expected byref forwarding probe to defer dispose helper execution until final release",
    )
    expect(
        payload.get("dispose_count_after_final_release") == 1,
        "expected byref forwarding probe to execute dispose helper once on final release",
    )
    expect(
        payload.get("last_disposed_value") == 11,
        "expected byref forwarding probe to dispose the original payload cell",
    )
    expect(
        payload.get("byref_destroy_count_after_final_release") == 0,
        "expected byref forwarding probe to keep shared heap byref cells alive while another block retains them",
    )
    expect(
        payload.get("final_release_result") == handle,
        "expected byref forwarding probe final release to return the released block handle",
    )
    expect(
        payload.get("invoke_after_release_result") == 0,
        "expected byref forwarding probe to reject post-release invocation",
    )
    expect(
        payload.get("second_final_release_result") == second_handle
        and payload.get("second_invoke_after_release_result") == 0,
        "expected byref forwarding probe to release the second block handle and reject stale invocation",
    )
    expect(
        payload.get("byref_destroy_count_after_second_final_release") == 2
        and payload.get("byref_destroyed_value_sum") == 42,
        "expected byref forwarding probe to destroy the shared seed and payload heap cells exactly once",
    )
    _expect_optional_stack_heap_forwarding_fields(
        payload,
        label="byref forwarding probe",
    )


def assert_owned_capture_lifetime_probe_payload(payload: dict[str, Any]) -> None:
    handle = payload.get("handle")
    owned_capture = payload.get("owned_capture_value")
    expect(
        isinstance(handle, int) and handle > 0,
        "expected owned-capture lifetime probe to publish a positive runtime block handle",
    )
    expect(
        isinstance(owned_capture, int) and owned_capture > 0,
        "expected owned-capture lifetime probe to publish a positive captured runtime object",
    )
    expect(
        payload.get("initial_capture_found") == 1
        and payload.get("initial_capture_retain_count") == 1,
        "expected owned-capture lifetime probe to start with one live local object owner",
    )
    expect(
        payload.get("copy_count_after_promotion") == 1
        and payload.get("owned_capture_retain_count_after_promotion") == 1,
        "expected owned-capture lifetime probe to retain the captured object exactly once during block promotion",
    )
    expect(
        payload.get("last_retained_owned_capture") == owned_capture
        and payload.get("after_promotion_capture_found") == 1
        and payload.get("after_promotion_capture_retain_count") == 2,
        "expected owned-capture lifetime probe to keep the promoted capture live with local plus block ownership",
    )
    expect(
        payload.get("release_original_owner_result") == owned_capture
        and payload.get("after_original_release_capture_found") == 1
        and payload.get("after_original_release_capture_retain_count") == 1,
        "expected owned-capture lifetime probe to keep the capture live after releasing the original owner",
    )
    expect(
        payload.get("invoke_result") == 110,
        "expected owned-capture lifetime probe to invoke through the runtime-owned captured object after local release",
    )
    expect(
        payload.get("retain_block_result") == handle
        and payload.get("release_retained_block_result") == handle,
        "expected owned-capture lifetime probe to retain and release the block handle without disposing captures early",
    )
    expect(
        payload.get("dispose_count_before_final_release") == 0
        and payload.get("owned_capture_release_count_before_final_release") == 0,
        "expected owned-capture lifetime probe to defer dispose and captured-object release until final block release",
    )
    expect(
        payload.get("final_release_result") == handle
        and payload.get("dispose_count_after_final_release") == 1
        and payload.get("owned_capture_release_count_after_final_release") == 1,
        "expected owned-capture lifetime probe to dispose the block and release the captured object on final release",
    )
    expect(
        payload.get("last_released_owned_capture") == owned_capture
        and payload.get("after_final_release_capture_found") == 0
        and payload.get("after_final_release_capture_retain_count") == 0,
        "expected owned-capture lifetime probe to destroy the captured object after final block release",
    )
    expect(
        payload.get("post_release_callback_count") == 0
        and payload.get("invoke_after_release_result") == 0,
        "expected owned-capture lifetime probe to reject stale post-release block invocation without calling the thunk",
    )


__all__ = [
    "assert_byref_forwarding_probe_payload",
    "assert_owned_capture_lifetime_probe_payload",
    "assert_byref_runtime_fixture",
    "byref_forwarding_probe_summary",
    "owned_capture_lifetime_probe_summary",
    "assert_nonowning_runtime_fixture",
    "assert_owned_runtime_fixture",
]
