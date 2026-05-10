"""Readiness runner contract checks and exit reduction."""

from __future__ import annotations

import json
from typing import Any, Sequence

from .constants import EXIT_BLOCKED, EXIT_BOOTSTRAPPABLE, EXIT_RUNNER_ERROR
from .models import CommandResult


def parse_checker_payload(result: CommandResult) -> tuple[dict[str, Any] | None, str | None]:
    if result.exit_code not in (EXIT_BOOTSTRAPPABLE, EXIT_BLOCKED):
        return None, (
            "check_bootstrap_readiness(json) returned unexpected exit code "
            f"{result.exit_code}."
        )

    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        return (
            None,
            (
                "check_bootstrap_readiness(json) emitted invalid JSON: "
                f"{exc.msg} at {exc.lineno}:{exc.colno}."
            ),
        )

    if not isinstance(payload, dict):
        return None, "check_bootstrap_readiness(json) output root must be an object."

    def parse_count(field_name: str) -> tuple[int | None, str | None]:
        raw = payload.get(field_name)
        if isinstance(raw, bool) or not isinstance(raw, int) or raw < 0:
            return None, (
                "check_bootstrap_readiness(json) missing non-negative integer "
                f"{field_name!r}."
            )
        return raw, None

    issues_open_count, issues_error = parse_count("issues_open_count")
    if issues_error is not None:
        return None, issues_error
    milestones_open_count, milestones_error = parse_count("milestones_open_count")
    if milestones_error is not None:
        return None, milestones_error
    catalog_open_task_count, catalog_error = parse_count("catalog_open_task_count")
    if catalog_error is not None:
        return None, catalog_error
    blockers_open_count, blockers_error = parse_count("blockers_open_count")
    if blockers_error is not None:
        return None, blockers_error

    readiness_state = payload.get("readiness_state")
    if readiness_state not in ("bootstrappable", "blocked"):
        return None, (
            "check_bootstrap_readiness(json) missing deterministic 'readiness_state' "
            "in {'bootstrappable','blocked'}."
        )

    intake_recommendation = payload.get("intake_recommendation")
    if intake_recommendation not in ("go", "hold"):
        return None, (
            "check_bootstrap_readiness(json) missing deterministic "
            "'intake_recommendation' in {'go','hold'}."
        )

    blocking_dimensions = payload.get("blocking_dimensions")
    if not isinstance(blocking_dimensions, list) or not all(
        isinstance(item, str) and item for item in blocking_dimensions
    ):
        return None, (
            "check_bootstrap_readiness(json) missing non-empty string list "
            "'blocking_dimensions'."
        )
    if len(set(blocking_dimensions)) != len(blocking_dimensions):
        return None, (
            "check_bootstrap_readiness(json) has duplicate entries in "
            "'blocking_dimensions'."
        )

    expected_blocking_dimensions = [
        field_name
        for field_name, count in (
            ("issues_open_count", issues_open_count),
            ("milestones_open_count", milestones_open_count),
            ("catalog_open_task_count", catalog_open_task_count),
            ("blockers_open_count", blockers_open_count),
        )
        if count > 0
    ]
    if blocking_dimensions != expected_blocking_dimensions:
        return None, (
            "check_bootstrap_readiness(json) blocking_dimensions drift: "
            f"blocking_dimensions={blocking_dimensions!r} "
            f"expected={expected_blocking_dimensions!r}."
        )

    expected_readiness_state = (
        "bootstrappable" if not expected_blocking_dimensions else "blocked"
    )
    if readiness_state != expected_readiness_state:
        return None, (
            "check_bootstrap_readiness(json) readiness reduction mismatch: "
            f"readiness_state={readiness_state!r} expected={expected_readiness_state!r}."
        )

    expected_intake_recommendation = "go" if readiness_state == "bootstrappable" else "hold"
    if intake_recommendation != expected_intake_recommendation:
        return None, (
            "check_bootstrap_readiness(json) recommendation drift: "
            f"intake_recommendation={intake_recommendation!r} "
            f"expected={expected_intake_recommendation!r}."
        )

    expected_exit = (
        EXIT_BOOTSTRAPPABLE
        if readiness_state == "bootstrappable"
        else EXIT_BLOCKED
    )
    if result.exit_code != expected_exit:
        return None, (
            "check_bootstrap_readiness(json) readiness/exit mismatch: "
            f"readiness_state={readiness_state!r} exit={result.exit_code}."
        )

    return payload, None


def check_markdown_consistency(
    markdown_result: CommandResult,
    *,
    checker_payload: dict[str, Any],
) -> str | None:
    if markdown_result.exit_code not in (EXIT_BOOTSTRAPPABLE, EXIT_BLOCKED):
        return (
            "check_bootstrap_readiness(markdown) returned unexpected exit code "
            f"{markdown_result.exit_code}."
        )

    expected_exit = (
        EXIT_BOOTSTRAPPABLE
        if checker_payload["readiness_state"] == "bootstrappable"
        else EXIT_BLOCKED
    )
    if markdown_result.exit_code != expected_exit:
        return (
            "check_bootstrap_readiness(markdown) readiness/exit mismatch: "
            f"readiness_state={checker_payload['readiness_state']!r} "
            f"exit={markdown_result.exit_code}."
        )

    expected_lines = (
        "# Bootstrap Readiness",
        "| Metric | Value |",
        "| --- | --- |",
        f"| issues_open_count | `{checker_payload['issues_open_count']}` |",
        f"| milestones_open_count | `{checker_payload['milestones_open_count']}` |",
        f"| catalog_open_task_count | `{checker_payload['catalog_open_task_count']}` |",
        f"| blockers_open_count | `{checker_payload['blockers_open_count']}` |",
        f"| readiness_state | `{checker_payload['readiness_state']}` |",
        f"| intake_recommendation | `{checker_payload['intake_recommendation']}` |",
    )
    for expected_line in expected_lines:
        if expected_line in markdown_result.stdout:
            continue
        return (
            "check_bootstrap_readiness(markdown) missing deterministic line "
            f"{expected_line!r}."
        )
    return None


def determine_final_exit(
    *,
    checker_payload: dict[str, Any] | None,
    errors: Sequence[str],
) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"
    if checker_payload is None:
        return EXIT_RUNNER_ERROR, "runner-error"

    readiness_state = checker_payload.get("readiness_state")
    if readiness_state == "blocked":
        return EXIT_BLOCKED, "blocked-readiness"
    if readiness_state == "bootstrappable":
        return EXIT_BOOTSTRAPPABLE, "ok"
    return EXIT_RUNNER_ERROR, "runner-error"
