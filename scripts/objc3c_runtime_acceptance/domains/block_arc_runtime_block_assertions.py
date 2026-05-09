"""Block helper runtime assertions for Block/ARC execution cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


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
    expect(
        isinstance(payload.get("handle"), int) and payload.get("handle", 0) > 0,
        "expected byref forwarding probe to publish a positive runtime block handle",
    )
    expect(
        payload.get("copy_count_after_promotion") == 1,
        "expected byref forwarding probe to execute one copy helper during promotion",
    )
    expect(
        payload.get("first_invoke_result") == 23
        and payload.get("second_invoke_result") == 25,
        "expected byref forwarding probe to preserve runtime-owned forwarded cell state across invokes",
    )
    expect(
        payload.get("dispose_count_before_final_release") == 0
        and payload.get("dispose_count_after_final_release") == 1,
        "expected byref forwarding probe to defer dispose helper execution until final release",
    )
    expect(
        payload.get("last_disposed_value") == 11,
        "expected byref forwarding probe to dispose the original owned capture payload",
    )
    expect(
        payload.get("final_release_result") == payload.get("handle")
        and payload.get("invoke_after_release_result") == 0,
        "expected byref forwarding probe to release the block handle and reject post-release invocation",
    )


__all__ = [
    "assert_byref_forwarding_probe_payload",
    "assert_byref_runtime_fixture",
    "assert_nonowning_runtime_fixture",
    "assert_owned_runtime_fixture",
]
