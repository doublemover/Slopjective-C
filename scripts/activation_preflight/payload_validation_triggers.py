"""Trigger and open-blocker checks for activation preflight JSON payloads."""

from __future__ import annotations

from typing import Any, Sequence

from scripts.activation_preflight.payload_validation_constants import OPEN_BLOCKERS_TRIGGER_ID


def validate_actionable_statuses(
    payload: dict[str, Any],
    *,
    expected_actionable_statuses: Sequence[str],
) -> tuple[list[str] | None, str | None]:
    actionable_statuses = payload.get("actionable_statuses")
    if not isinstance(actionable_statuses, list) or not all(
        isinstance(status, str) and status for status in actionable_statuses
    ):
        return None, (
            "check_activation_triggers(json) missing non-empty string list "
            "'actionable_statuses'."
        )
    if len(set(actionable_statuses)) != len(actionable_statuses):
        return None, (
            "check_activation_triggers(json) has duplicate entries in "
            "'actionable_statuses'."
        )
    if tuple(actionable_statuses) != tuple(expected_actionable_statuses):
        return None, (
            "check_activation_triggers(json) actionable status drift: "
            f"actionable_statuses={actionable_statuses!r} "
            f"expected={list(expected_actionable_statuses)!r}."
        )
    return actionable_statuses, None


def validate_active_trigger_ids(payload: dict[str, Any]) -> tuple[list[str] | None, str | None]:
    active_trigger_ids = payload.get("active_trigger_ids")
    if not isinstance(active_trigger_ids, list) or not all(
        isinstance(item, str) for item in active_trigger_ids
    ):
        return None, "check_activation_triggers(json) missing string list 'active_trigger_ids'."
    if len(set(active_trigger_ids)) != len(active_trigger_ids):
        return None, "check_activation_triggers(json) has duplicate entries in 'active_trigger_ids'."
    return active_trigger_ids, None


def validate_trigger_rows(
    payload: dict[str, Any],
    *,
    active_trigger_ids: list[str],
) -> tuple[dict[str, tuple[int, bool]] | None, str | None]:
    trigger_rows = payload.get("triggers")
    if not isinstance(trigger_rows, list):
        return None, "check_activation_triggers(json) missing list 'triggers'."
    fired_trigger_ids: list[str] = []
    trigger_rows_by_id: dict[str, tuple[int, bool]] = {}
    for index, row in enumerate(trigger_rows):
        if not isinstance(row, dict):
            return None, f"check_activation_triggers(json) trigger row {index} must be an object."

        row_id = row.get("id")
        if not isinstance(row_id, str) or not row_id:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{index} missing non-empty string 'id'."
            )
        if row_id in trigger_rows_by_id:
            return None, (
                "check_activation_triggers(json) has duplicate trigger row id "
                f"{row_id!r}."
            )

        row_condition = row.get("condition")
        if not isinstance(row_condition, str) or not row_condition:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing non-empty string 'condition'."
            )

        row_count = row.get("count")
        if isinstance(row_count, bool) or not isinstance(row_count, int) or row_count < 0:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing non-negative integer 'count'."
            )

        row_fired = row.get("fired")
        if not isinstance(row_fired, bool):
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing boolean 'fired'."
            )

        trigger_rows_by_id[row_id] = (row_count, row_fired)
        if row_fired:
            fired_trigger_ids.append(row_id)

    if active_trigger_ids != fired_trigger_ids:
        return None, (
            "check_activation_triggers(json) active trigger ordering drift: "
            f"active_trigger_ids={active_trigger_ids!r} fired_ids={fired_trigger_ids!r}."
        )

    return trigger_rows_by_id, None


def validate_open_blockers_payload(
    payload: dict[str, Any],
    *,
    active_trigger_ids: list[str],
    trigger_rows_by_id: dict[str, tuple[int, bool]],
) -> str | None:
    open_blockers = payload.get("open_blockers")
    if not isinstance(open_blockers, dict):
        return "check_activation_triggers(json) missing object 'open_blockers'."

    open_blocker_count = open_blockers.get("count")
    if (
        isinstance(open_blocker_count, bool)
        or not isinstance(open_blocker_count, int)
        or open_blocker_count < 0
    ):
        return (
            "check_activation_triggers(json) missing non-negative integer "
            "'open_blockers.count'."
        )

    open_blockers_trigger_id = open_blockers.get("trigger_id")
    if open_blockers_trigger_id != OPEN_BLOCKERS_TRIGGER_ID:
        return (
            "check_activation_triggers(json) missing deterministic "
            f"'open_blockers.trigger_id={OPEN_BLOCKERS_TRIGGER_ID}'."
        )

    open_blockers_trigger_fired = open_blockers.get("trigger_fired")
    if not isinstance(open_blockers_trigger_fired, bool):
        return (
            "check_activation_triggers(json) missing boolean "
            "'open_blockers.trigger_fired'."
        )

    expected_open_blockers_trigger = open_blocker_count > 0
    if open_blockers_trigger_fired is not expected_open_blockers_trigger:
        return (
            "check_activation_triggers(json) inconsistent open blocker trigger state: "
            f"count={open_blocker_count} trigger_fired={open_blockers_trigger_fired}."
        )

    has_open_blockers_trigger = OPEN_BLOCKERS_TRIGGER_ID in active_trigger_ids
    if has_open_blockers_trigger is not expected_open_blockers_trigger:
        return (
            "check_activation_triggers(json) open blocker trigger mismatch: "
            f"active_trigger_ids contains {OPEN_BLOCKERS_TRIGGER_ID!r}="
            f"{has_open_blockers_trigger} while count={open_blocker_count}."
        )

    if OPEN_BLOCKERS_TRIGGER_ID not in trigger_rows_by_id:
        return (
            "check_activation_triggers(json) missing trigger row "
            f"{OPEN_BLOCKERS_TRIGGER_ID!r}."
        )
    blockers_row_count, blockers_row_fired = trigger_rows_by_id[OPEN_BLOCKERS_TRIGGER_ID]
    if blockers_row_count != open_blocker_count or blockers_row_fired is not open_blockers_trigger_fired:
        return (
            "check_activation_triggers(json) open blocker row drift: "
            f"trigger_row=(count={blockers_row_count}, fired={blockers_row_fired}) "
            f"payload=(count={open_blocker_count}, fired={open_blockers_trigger_fired})."
        )

    return None
