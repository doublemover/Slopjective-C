"""Freshness contract checks for activation preflight JSON payloads."""

from __future__ import annotations

from typing import Any


def validate_freshness_payload(
    freshness: Any,
    *,
    expected_issues_max_age_seconds: int | None,
    expected_milestones_max_age_seconds: int | None,
) -> str | None:
    if not isinstance(freshness, dict):
        return "check_activation_triggers(json) missing object 'freshness'."

    issues_freshness_error = validate_freshness_entry(
        "issues",
        freshness.get("issues"),
        expected_max_age_seconds=expected_issues_max_age_seconds,
    )
    if issues_freshness_error is not None:
        return issues_freshness_error

    return validate_freshness_entry(
        "milestones",
        freshness.get("milestones"),
        expected_max_age_seconds=expected_milestones_max_age_seconds,
    )


def validate_freshness_entry(
    label: str,
    entry: Any,
    *,
    expected_max_age_seconds: int | None,
) -> str | None:
    if not isinstance(entry, dict):
        return f"check_activation_triggers(json) freshness.{label} must be an object."

    requested = entry.get("requested")
    if not isinstance(requested, bool):
        return (
            "check_activation_triggers(json) freshness."
            f"{label}.requested must be boolean."
        )

    max_age_seconds = entry.get("max_age_seconds")
    if max_age_seconds is not None and (
        isinstance(max_age_seconds, bool)
        or not isinstance(max_age_seconds, int)
        or max_age_seconds < 0
    ):
        return (
            "check_activation_triggers(json) freshness."
            f"{label}.max_age_seconds must be null or non-negative integer."
        )

    generated_at_utc = entry.get("generated_at_utc")
    if generated_at_utc is not None and (
        not isinstance(generated_at_utc, str) or not generated_at_utc
    ):
        return (
            "check_activation_triggers(json) freshness."
            f"{label}.generated_at_utc must be null or non-empty string."
        )

    age_seconds = entry.get("age_seconds")
    if age_seconds is not None and (
        isinstance(age_seconds, bool) or not isinstance(age_seconds, int) or age_seconds < 0
    ):
        return (
            "check_activation_triggers(json) freshness."
            f"{label}.age_seconds must be null or non-negative integer."
        )

    fresh = entry.get("fresh")
    if fresh is not None and not isinstance(fresh, bool):
        return (
            "check_activation_triggers(json) freshness."
            f"{label}.fresh must be null or boolean."
        )

    if expected_max_age_seconds is None:
        if requested:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.requested={requested!r} expected=False."
            )
        if (
            max_age_seconds is not None
            or generated_at_utc is not None
            or age_seconds is not None
            or fresh is not None
        ):
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label} should use null metadata when request is omitted."
            )
        return None

    if not requested:
        return (
            "check_activation_triggers(json) freshness drift: "
            f"freshness.{label}.requested={requested!r} expected=True."
        )
    if max_age_seconds != expected_max_age_seconds:
        return (
            "check_activation_triggers(json) freshness drift: "
            f"freshness.{label}.max_age_seconds={max_age_seconds!r} "
            f"expected={expected_max_age_seconds!r}."
        )
    if not isinstance(generated_at_utc, str) or not generated_at_utc:
        return (
            "check_activation_triggers(json) freshness drift: "
            f"freshness.{label}.generated_at_utc must be a non-empty string "
            "when freshness is requested."
        )
    if isinstance(age_seconds, bool) or not isinstance(age_seconds, int) or age_seconds < 0:
        return (
            "check_activation_triggers(json) freshness drift: "
            f"freshness.{label}.age_seconds must be a non-negative integer "
            "when freshness is requested."
        )
    if fresh is not True:
        return (
            "check_activation_triggers(json) freshness drift: "
            f"freshness.{label}.fresh={fresh!r} expected=True."
        )
    return None
