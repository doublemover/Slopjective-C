"""ARC fixture assertions for Block/ARC runtime execution cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


def _expect_arc_fixture_returncode(
    returncode: int,
    expected_returncode: int,
    fixture_label: str,
) -> None:
    expect(
        returncode == expected_returncode,
        f"expected {fixture_label} to exit {expected_returncode}, saw {returncode}",
    )


def _expect_retain_release_insertions(
    sema_pass_manager: dict[str, Any],
    expected_count: int,
    message: str,
) -> None:
    expect(
        sema_pass_manager.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == expected_count
        and sema_pass_manager.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == expected_count,
        message,
    )


def assert_arc_mode_runtime_fixture(
    returncode: int,
    sema_pass_manager: dict[str, Any],
) -> None:
    _expect_arc_fixture_returncode(
        returncode,
        17,
        "ARC mode runtime positive fixture",
    )
    _expect_retain_release_insertions(
        sema_pass_manager,
        8,
        "expected ARC mode runtime positive fixture to preserve eight retain and eight release insertions",
    )


def assert_arc_inference_runtime_fixture(
    returncode: int,
    sema_pass_manager: dict[str, Any],
) -> None:
    _expect_arc_fixture_returncode(
        returncode,
        17,
        "ARC inference runtime positive fixture",
    )
    _expect_retain_release_insertions(
        sema_pass_manager,
        8,
        "expected ARC inference runtime positive fixture to preserve eight retain and eight release insertions",
    )


def assert_arc_cleanup_scope_runtime_fixture(
    returncode: int,
    sema_pass_manager: dict[str, Any],
) -> None:
    _expect_arc_fixture_returncode(
        returncode,
        9,
        "ARC cleanup scope runtime positive fixture",
    )
    _expect_retain_release_insertions(
        sema_pass_manager,
        1,
        "expected ARC cleanup scope runtime positive fixture to preserve one retain/release cleanup pair",
    )


def assert_arc_implicit_cleanup_runtime_fixture(
    returncode: int,
    sema_pass_manager: dict[str, Any],
) -> None:
    _expect_arc_fixture_returncode(
        returncode,
        0,
        "ARC implicit cleanup runtime positive fixture",
    )
    _expect_retain_release_insertions(
        sema_pass_manager,
        1,
        "expected ARC implicit cleanup runtime positive fixture to preserve one retain/release cleanup pair",
    )


__all__ = [
    "assert_arc_cleanup_scope_runtime_fixture",
    "assert_arc_implicit_cleanup_runtime_fixture",
    "assert_arc_inference_runtime_fixture",
    "assert_arc_mode_runtime_fixture",
]
