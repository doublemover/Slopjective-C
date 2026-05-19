"""Probe result parsing and assertions for runnable block/ARC validation."""

from __future__ import annotations

from objc3c_tooling.probe_output import parse_json_output

from .assertions import expect


def parse_runtime_abi_probe_payload(result: object) -> dict[str, object]:
    return parse_json_output(result, "packaged block ARC runtime ABI probe")


def parse_byref_forwarding_probe_payload(result: object) -> dict[str, object]:
    return parse_json_output(result, "packaged block byref forwarding probe")


def assert_runtime_abi_probe_payload(payload: dict[str, object]) -> None:
    expect(payload.get("abi_status") == 0, "expected packaged block ARC ABI probe status to succeed")
    expect(payload.get("arc_status") == 0, "expected packaged ARC debug state snapshot to succeed")
    expect(payload.get("invoke_result") == 17, "expected packaged block ARC ABI probe to preserve invoke_result 17")
    expect(payload.get("block_promote_call_count") == 1, "expected packaged block ARC ABI probe to preserve one promote call")
    expect(payload.get("block_invoke_call_count") == 1, "expected packaged block ARC ABI probe to preserve one invoke call")
    expect(payload.get("retain_call_count") == 2, "expected packaged block ARC ABI probe to preserve two retain calls")
    expect(payload.get("release_call_count") == 3, "expected packaged block ARC ABI probe to preserve three release calls")
    expect(payload.get("autorelease_call_count") == 1, "expected packaged block ARC ABI probe to preserve one autorelease call")
    expect(payload.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected packaged block ARC ABI probe to preserve the ARC debug state snapshot symbol")


def assert_byref_forwarding_probe_payload(payload: dict[str, object]) -> None:
    expect(payload.get("handle", 0) > 0, "expected packaged byref forwarding probe to publish a positive handle")
    expect(payload.get("copy_count_after_promotion") == 1, "expected packaged byref forwarding probe to preserve one copy helper call")
    expect(payload.get("first_invoke_result") == 23, "expected packaged byref forwarding probe to preserve first invoke result 23")
    expect(payload.get("second_invoke_result") == 25, "expected packaged byref forwarding probe to preserve second invoke result 25")
    expect(payload.get("dispose_count_before_final_release") == 0, "expected packaged byref forwarding probe to defer dispose before final release")
    expect(payload.get("dispose_count_after_final_release") == 1, "expected packaged byref forwarding probe to execute dispose on final release")
    expect(payload.get("last_disposed_value") == 11, "expected packaged byref forwarding probe to preserve the disposed payload")
    expect(payload.get("invoke_after_release_result") == 0, "expected packaged byref forwarding probe to reject invoke after final release")


__all__ = [
    "assert_byref_forwarding_probe_payload",
    "assert_runtime_abi_probe_payload",
    "parse_byref_forwarding_probe_payload",
    "parse_runtime_abi_probe_payload",
]
