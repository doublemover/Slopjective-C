"""JSON payload validation for activation preflight checks."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.payload_validation_constants import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
    OPEN_BLOCKERS_TRIGGER_ID,
)


def parse_activation_payload(
    result: CommandResult,
    *,
    expected_issues_path: Path,
    expected_milestones_path: Path,
    expected_catalog_path: Path,
    expected_open_blockers_path: Path | None,
    expected_t4_overlay_path: Path | None,
    expected_actionable_statuses: Sequence[str],
    expected_issues_max_age_seconds: int | None,
    expected_milestones_max_age_seconds: int | None,
) -> tuple[dict[str, Any] | None, str | None]:
    if result.exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN):
        return None, (
            "check_activation_triggers(json) returned unexpected exit code "
            f"{result.exit_code}."
        )

    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        return (
            None,
            (
                "check_activation_triggers(json) emitted invalid JSON: "
                f"{exc.msg} at {exc.lineno}:{exc.colno}."
            ),
        )

    if not isinstance(payload, dict):
        return None, "check_activation_triggers(json) output root must be an object."

    mode = payload.get("mode")
    if mode != "offline-deterministic":
        return None, (
            "check_activation_triggers(json) missing deterministic "
            "mode 'offline-deterministic'."
        )

    inputs = payload.get("inputs")
    if not isinstance(inputs, dict):
        return None, "check_activation_triggers(json) missing object 'inputs'."

    expected_issues_display_path = display_path(expected_issues_path)
    expected_milestones_display_path = display_path(expected_milestones_path)
    expected_catalog_display_path = display_path(expected_catalog_path)
    expected_open_blockers_display_path = (
        display_path(expected_open_blockers_path)
        if expected_open_blockers_path is not None
        else None
    )
    expected_t4_overlay_display_path = (
        display_path(expected_t4_overlay_path)
        if expected_t4_overlay_path is not None
        else None
    )

    issues_input = inputs.get("issues_json")
    if issues_input != expected_issues_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.issues_json={issues_input!r} expected={expected_issues_display_path!r}."
        )

    milestones_input = inputs.get("milestones_json")
    if milestones_input != expected_milestones_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.milestones_json={milestones_input!r} "
            f"expected={expected_milestones_display_path!r}."
        )

    catalog_input = inputs.get("catalog_json")
    if catalog_input != expected_catalog_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.catalog_json={catalog_input!r} expected={expected_catalog_display_path!r}."
        )

    open_blockers_input = inputs.get("open_blockers_json")
    if open_blockers_input != expected_open_blockers_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.open_blockers_json={open_blockers_input!r} "
            f"expected={expected_open_blockers_display_path!r}."
        )

    t4_overlay_input = inputs.get("t4_governance_overlay_json")
    if t4_overlay_input != expected_t4_overlay_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.t4_governance_overlay_json={t4_overlay_input!r} "
            f"expected={expected_t4_overlay_display_path!r}."
        )

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

    freshness = payload.get("freshness")
    if not isinstance(freshness, dict):
        return None, "check_activation_triggers(json) missing object 'freshness'."

    issues_freshness_error = validate_freshness_entry(
        "issues",
        freshness.get("issues"),
        expected_max_age_seconds=expected_issues_max_age_seconds,
    )
    if issues_freshness_error is not None:
        return None, issues_freshness_error

    milestones_freshness_error = validate_freshness_entry(
        "milestones",
        freshness.get("milestones"),
        expected_max_age_seconds=expected_milestones_max_age_seconds,
    )
    if milestones_freshness_error is not None:
        return None, milestones_freshness_error

    gate_open = payload.get("gate_open")
    if not isinstance(gate_open, bool):
        return None, "check_activation_triggers(json) missing boolean 'gate_open'."

    payload_exit_code = payload.get("exit_code")
    if (
        isinstance(payload_exit_code, bool)
        or not isinstance(payload_exit_code, int)
        or payload_exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN)
    ):
        return None, (
            "check_activation_triggers(json) missing deterministic integer "
            "'exit_code' in {0,1}."
        )

    queue_state = payload.get("queue_state")
    if not isinstance(queue_state, str) or not queue_state:
        return None, "check_activation_triggers(json) missing non-empty 'queue_state'."

    activation_required = payload.get("activation_required")
    if not isinstance(activation_required, bool):
        return None, "check_activation_triggers(json) missing boolean 'activation_required'."

    active_trigger_ids = payload.get("active_trigger_ids")
    if not isinstance(active_trigger_ids, list) or not all(
        isinstance(item, str) for item in active_trigger_ids
    ):
        return None, "check_activation_triggers(json) missing string list 'active_trigger_ids'."
    if len(set(active_trigger_ids)) != len(active_trigger_ids):
        return None, "check_activation_triggers(json) has duplicate entries in 'active_trigger_ids'."

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

    expected_activation_required = bool(active_trigger_ids)
    if activation_required is not expected_activation_required:
        return None, (
            "check_activation_triggers(json) activation reduction mismatch: "
            f"activation_required={activation_required!r} "
            f"expected={expected_activation_required!r}."
        )

    t4_overlay = payload.get("t4_governance_overlay")
    if not isinstance(t4_overlay, dict):
        return None, "check_activation_triggers(json) missing object 't4_governance_overlay'."

    t4_new_scope_publish = t4_overlay.get("new_scope_publish")
    if not isinstance(t4_new_scope_publish, bool):
        return None, (
            "check_activation_triggers(json) missing boolean "
            "'t4_governance_overlay.new_scope_publish'."
        )

    t4_source = t4_overlay.get("source")
    if not isinstance(t4_source, str) or not t4_source:
        return None, (
            "check_activation_triggers(json) missing non-empty string "
            "'t4_governance_overlay.source'."
        )

    expected_gate_open = activation_required or t4_new_scope_publish
    if gate_open is not expected_gate_open:
        return None, (
            "check_activation_triggers(json) gate reduction mismatch: "
            f"gate_open={gate_open!r} expected={expected_gate_open!r}."
        )

    open_blockers = payload.get("open_blockers")
    if not isinstance(open_blockers, dict):
        return None, "check_activation_triggers(json) missing object 'open_blockers'."

    open_blocker_count = open_blockers.get("count")
    if (
        isinstance(open_blocker_count, bool)
        or not isinstance(open_blocker_count, int)
        or open_blocker_count < 0
    ):
        return None, (
            "check_activation_triggers(json) missing non-negative integer "
            "'open_blockers.count'."
        )

    open_blockers_trigger_id = open_blockers.get("trigger_id")
    if open_blockers_trigger_id != OPEN_BLOCKERS_TRIGGER_ID:
        return None, (
            "check_activation_triggers(json) missing deterministic "
            f"'open_blockers.trigger_id={OPEN_BLOCKERS_TRIGGER_ID}'."
        )

    open_blockers_trigger_fired = open_blockers.get("trigger_fired")
    if not isinstance(open_blockers_trigger_fired, bool):
        return None, (
            "check_activation_triggers(json) missing boolean "
            "'open_blockers.trigger_fired'."
        )

    expected_open_blockers_trigger = open_blocker_count > 0
    if open_blockers_trigger_fired is not expected_open_blockers_trigger:
        return None, (
            "check_activation_triggers(json) inconsistent open blocker trigger state: "
            f"count={open_blocker_count} trigger_fired={open_blockers_trigger_fired}."
        )

    has_open_blockers_trigger = OPEN_BLOCKERS_TRIGGER_ID in active_trigger_ids
    if has_open_blockers_trigger is not expected_open_blockers_trigger:
        return None, (
            "check_activation_triggers(json) open blocker trigger mismatch: "
            f"active_trigger_ids contains {OPEN_BLOCKERS_TRIGGER_ID!r}="
            f"{has_open_blockers_trigger} while count={open_blocker_count}."
        )

    if OPEN_BLOCKERS_TRIGGER_ID not in trigger_rows_by_id:
        return None, (
            "check_activation_triggers(json) missing trigger row "
            f"{OPEN_BLOCKERS_TRIGGER_ID!r}."
        )
    blockers_row_count, blockers_row_fired = trigger_rows_by_id[OPEN_BLOCKERS_TRIGGER_ID]
    if blockers_row_count != open_blocker_count or blockers_row_fired is not open_blockers_trigger_fired:
        return None, (
            "check_activation_triggers(json) open blocker row drift: "
            f"trigger_row=(count={blockers_row_count}, fired={blockers_row_fired}) "
            f"payload=(count={open_blocker_count}, fired={open_blockers_trigger_fired})."
        )

    expected_queue_state = "dispatch-open" if gate_open else "idle"
    if queue_state != expected_queue_state:
        return None, (
            "check_activation_triggers(json) queue_state drift: "
            f"queue_state={queue_state!r} expected={expected_queue_state!r}."
        )

    expected_exit = EXIT_GATE_OPEN if gate_open else EXIT_GATE_CLOSED
    if payload_exit_code != expected_exit:
        return (
            None,
            (
                "check_activation_triggers(json) payload gate_open/exit mismatch: "
                f"gate_open={gate_open!r} payload_exit={payload_exit_code}."
            ),
        )

    if expected_exit != result.exit_code:
        return (
            None,
            (
                "check_activation_triggers(json) gate_open/exit mismatch: "
                f"gate_open={gate_open!r} exit={result.exit_code}."
            ),
        )

    return payload, None


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
