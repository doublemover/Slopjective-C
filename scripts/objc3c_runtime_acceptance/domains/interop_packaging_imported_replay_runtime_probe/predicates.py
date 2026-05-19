"""Imported-runtime replay probe result predicates."""

from __future__ import annotations

from typing import Any


def payload_value_matches(
    payload: dict[str, Any],
    key: str,
    expected: Any,
) -> bool:
    return payload.get(key) == expected


def payload_value_at_least(
    payload: dict[str, Any],
    key: str,
    minimum: int,
    *,
    default: int,
) -> bool:
    return payload.get(key, default) >= minimum


def payload_status_flag_is_set(
    payload: dict[str, Any],
    status_key: str,
    flag_key: str,
) -> bool:
    return payload.get(status_key) == 0 and payload.get(flag_key) == 1


def payload_method_is_resolved(
    payload: dict[str, Any],
    status_key: str,
    found_key: str,
    resolved_key: str,
) -> bool:
    return (
        payload.get(status_key) == 0
        and payload.get(found_key) == 1
        and payload.get(resolved_key) == 1
    )


def replay_image_count_matches_link_plan(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
) -> bool:
    return payload.get("post_replay_registered_image_count") == link_plan.get(
        "module_image_count"
    )


def startup_dispatch_value_survived(
    payload: dict[str, Any],
    key: str,
    startup_value: int,
    expected: int,
) -> bool:
    return payload.get(key) == startup_value == expected


__all__ = [
    "payload_method_is_resolved",
    "payload_status_flag_is_set",
    "payload_value_at_least",
    "payload_value_matches",
    "replay_image_count_matches_link_plan",
    "startup_dispatch_value_survived",
]
