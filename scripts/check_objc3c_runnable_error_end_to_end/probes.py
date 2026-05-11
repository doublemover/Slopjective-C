"""Packaged runnable error probe payload validation."""

from __future__ import annotations

from .assertions import expect

EXPECTED_PROBE_INTEGER_FIELDS = {
    "rc": 54,
    "store_call_count": 1,
    "load_call_count": 1,
    "status_bridge_call_count": 1,
    "nserror_bridge_call_count": 0,
    "catch_match_call_count": 1,
    "last_stored_error_value": 45,
    "last_loaded_error_value": 45,
    "last_status_bridge_status_value": 5,
    "last_status_bridge_error_value": 45,
    "last_catch_match_kind": 1,
    "last_catch_match_is_catch_all": 0,
    "last_catch_match_result": 1,
}


def assert_probe_payload(probe_payload: dict[str, object]) -> None:
    expect(
        probe_payload.get("status") == 0,
        "expected packaged error runtime probe to copy the bridge-state snapshot successfully",
    )
    for field, expected_value in EXPECTED_PROBE_INTEGER_FIELDS.items():
        expect(
            probe_payload.get(field) == expected_value,
            f"expected packaged error runtime probe to preserve {field}",
        )
    expect(
        probe_payload.get("last_catch_kind_name") == "nserror",
        "expected packaged error runtime probe to preserve the NSError catch-kind label",
    )


__all__ = ["EXPECTED_PROBE_INTEGER_FIELDS", "assert_probe_payload"]
