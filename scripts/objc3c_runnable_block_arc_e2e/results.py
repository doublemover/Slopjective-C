"""Probe result parsing and assertions for runnable block/ARC validation."""

from __future__ import annotations

from objc3c_tooling.probe_output import parse_json_output

from .assertions import expect


_BYREF_FORWARDING_SUMMARY_FIELDS = (
    "handle",
    "second_handle",
    "copy_count_after_promotion",
    "copy_count_after_second_promotion",
    "seed_stack_address",
    "seed_forwarding_address_after_first_promotion",
    "seed_forwarding_address_after_second_promotion",
    "payload_stack_address",
    "payload_forwarding_address_after_first_promotion",
    "payload_forwarding_address_after_second_promotion",
    "stack_seed_forwarding_updated",
    "stack_payload_forwarding_updated",
    "second_handle_shared_seed_forwarding",
    "second_handle_shared_payload_forwarding",
    "first_invoke_result",
    "second_invoke_result",
    "forwarded_stack_write_invoke_result",
    "dispose_count_before_final_release",
    "dispose_count_after_final_release",
    "last_disposed_value",
    "byref_destroy_count_after_final_release",
    "final_release_result",
    "invoke_after_release_result",
    "second_final_release_result",
    "second_invoke_after_release_result",
    "byref_destroy_count_after_second_final_release",
    "byref_destroyed_value_sum",
)

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


def byref_forwarding_probe_summary(payload: dict[str, object]) -> dict[str, object]:
    summary = {
        field: payload.get(field)
        for field in _BYREF_FORWARDING_SUMMARY_FIELDS
    }
    for field in sorted(payload):
        if _is_optional_byref_forwarding_field(field):
            summary[field] = payload.get(field)
    return summary


def _expect_optional_stack_heap_forwarding_fields(
    payload: dict[str, object],
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


def parse_runtime_abi_probe_payload(result: object) -> dict[str, object]:
    return parse_json_output(result, "packaged block ARC runtime ABI probe")


def parse_byref_forwarding_probe_payload(result: object) -> dict[str, object]:
    return parse_json_output(result, "packaged block byref forwarding probe")


def assert_runtime_abi_probe_payload(payload: dict[str, object]) -> None:
    expect(payload.get("abi_status") == 0, "expected packaged block ARC ABI probe status to succeed")
    expect(payload.get("arc_status") == 0, "expected packaged ARC debug state snapshot to succeed")
    expect(payload.get("invoke_result") == 17, "expected packaged block ARC ABI probe to preserve invoke_result 17")
    expect(payload.get("stale_invoke_result") == 0, "expected packaged block ARC ABI probe to reject stale post-release invoke")
    expect(payload.get("block_promote_call_count") == 1, "expected packaged block ARC ABI probe to preserve one promote call")
    expect(payload.get("block_invoke_call_count") == 2, "expected packaged block ARC ABI probe to preserve successful plus fail-closed invoke calls")
    expect(payload.get("last_descriptor_has_invoke") == 1, "expected packaged block ARC ABI probe to preserve descriptor invoke proof")
    expect(payload.get("last_descriptor_flags") == 7, "expected packaged block ARC ABI probe to preserve descriptor flags")
    expect(payload.get("retain_call_count") == 2, "expected packaged block ARC ABI probe to preserve two retain calls")
    expect(payload.get("release_call_count") == 3, "expected packaged block ARC ABI probe to preserve three release calls")
    expect(payload.get("autorelease_call_count") == 1, "expected packaged block ARC ABI probe to preserve one autorelease call")
    expect(payload.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected packaged block ARC ABI probe to preserve the ARC debug state snapshot symbol")


def assert_byref_forwarding_probe_payload(payload: dict[str, object]) -> None:
    handle = payload.get("handle")
    second_handle = payload.get("second_handle")
    expect(
        isinstance(handle, int) and handle > 0,
        "expected packaged byref forwarding probe to publish a positive handle",
    )
    expect(
        isinstance(second_handle, int) and second_handle > 0,
        "expected packaged byref forwarding probe to publish a positive second handle",
    )
    expect(payload.get("copy_count_after_promotion") == 1, "expected packaged byref forwarding probe to preserve one copy helper call")
    expect(payload.get("copy_count_after_second_promotion") == 2, "expected packaged byref forwarding probe to execute one copy helper per promoted block")
    expect(
        payload.get("stack_seed_forwarding_updated") == 1
        and payload.get("stack_payload_forwarding_updated") == 1,
        "expected packaged byref forwarding probe to update stack cells to heap forwarding cells",
    )
    expect(
        payload.get("seed_stack_address")
        != payload.get("seed_forwarding_address_after_first_promotion")
        and payload.get("payload_stack_address")
        != payload.get("payload_forwarding_address_after_first_promotion"),
        "expected packaged byref forwarding probe to publish distinct stack and heap forwarding cell addresses",
    )
    expect(
        payload.get("second_handle_shared_seed_forwarding") == 1
        and payload.get("second_handle_shared_payload_forwarding") == 1,
        "expected packaged byref forwarding probe to share heap forwarding cells across promoted blocks",
    )
    expect(
        payload.get("seed_forwarding_address_after_first_promotion")
        == payload.get("seed_forwarding_address_after_second_promotion")
        and payload.get("payload_forwarding_address_after_first_promotion")
        == payload.get("payload_forwarding_address_after_second_promotion"),
        "expected packaged byref forwarding probe to keep stable forwarding addresses across promotions",
    )
    expect(payload.get("first_invoke_result") == 23, "expected packaged byref forwarding probe to preserve first invoke result 23")
    expect(payload.get("second_invoke_result") == 25, "expected packaged byref forwarding probe to preserve second invoke result 25")
    expect(payload.get("forwarded_stack_write_invoke_result") == 42, "expected packaged byref forwarding probe to route stack-forwarded writes through shared heap state")
    expect(payload.get("dispose_count_before_final_release") == 0, "expected packaged byref forwarding probe to defer dispose before final release")
    expect(payload.get("dispose_count_after_final_release") == 1, "expected packaged byref forwarding probe to execute dispose on final release")
    expect(payload.get("last_disposed_value") == 11, "expected packaged byref forwarding probe to preserve the disposed payload")
    expect(payload.get("byref_destroy_count_after_final_release") == 0, "expected packaged byref forwarding probe to keep shared heap byref cells alive while another block retains them")
    expect(
        payload.get("final_release_result") == handle,
        "expected packaged byref forwarding probe final release to return the released handle",
    )
    expect(payload.get("invoke_after_release_result") == 0, "expected packaged byref forwarding probe to reject invoke after final release")
    expect(
        payload.get("second_final_release_result") == second_handle
        and payload.get("second_invoke_after_release_result") == 0,
        "expected packaged byref forwarding probe to release the second block handle and reject stale invocation",
    )
    expect(
        payload.get("byref_destroy_count_after_second_final_release") == 2
        and payload.get("byref_destroyed_value_sum") == 42,
        "expected packaged byref forwarding probe to destroy the shared seed and payload heap cells exactly once",
    )
    _expect_optional_stack_heap_forwarding_fields(
        payload,
        label="packaged byref forwarding probe",
    )


__all__ = [
    "assert_byref_forwarding_probe_payload",
    "assert_runtime_abi_probe_payload",
    "byref_forwarding_probe_summary",
    "parse_byref_forwarding_probe_payload",
    "parse_runtime_abi_probe_payload",
]
