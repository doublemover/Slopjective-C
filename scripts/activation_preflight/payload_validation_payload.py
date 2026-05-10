"""JSON payload validation for activation preflight checks."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Sequence

from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.payload_validation_constants import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
)
from scripts.activation_preflight.payload_validation_freshness import (
    validate_freshness_entry,
    validate_freshness_payload,
)
from scripts.activation_preflight.payload_validation_inputs import validate_payload_input_provenance
from scripts.activation_preflight.payload_validation_triggers import (
    validate_actionable_statuses,
    validate_active_trigger_ids,
    validate_open_blockers_payload,
    validate_trigger_rows,
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

    header_error = validate_payload_header_and_inputs(
        payload,
        expected_issues_path=expected_issues_path,
        expected_milestones_path=expected_milestones_path,
        expected_catalog_path=expected_catalog_path,
        expected_open_blockers_path=expected_open_blockers_path,
        expected_t4_overlay_path=expected_t4_overlay_path,
    )
    if header_error is not None:
        return None, header_error

    _, actionable_status_error = validate_actionable_statuses(
        payload,
        expected_actionable_statuses=expected_actionable_statuses,
    )
    if actionable_status_error is not None:
        return None, actionable_status_error

    freshness_error = validate_freshness_payload(
        payload.get("freshness"),
        expected_issues_max_age_seconds=expected_issues_max_age_seconds,
        expected_milestones_max_age_seconds=expected_milestones_max_age_seconds,
    )
    if freshness_error is not None:
        return None, freshness_error

    gate_open, activation_required, payload_exit_code, queue_state, scalar_error = (
        validate_scalar_gate_fields(payload)
    )
    if scalar_error is not None:
        return None, scalar_error

    active_trigger_ids, active_trigger_error = validate_active_trigger_ids(payload)
    if active_trigger_error is not None or active_trigger_ids is None:
        return None, active_trigger_error

    trigger_rows_by_id, trigger_rows_error = validate_trigger_rows(
        payload,
        active_trigger_ids=active_trigger_ids,
    )
    if trigger_rows_error is not None or trigger_rows_by_id is None:
        return None, trigger_rows_error

    expected_activation_required = bool(active_trigger_ids)
    if activation_required is not expected_activation_required:
        return None, (
            "check_activation_triggers(json) activation reduction mismatch: "
            f"activation_required={activation_required!r} "
            f"expected={expected_activation_required!r}."
        )

    t4_new_scope_publish, t4_error = validate_t4_governance_overlay(payload)
    if t4_error is not None:
        return None, t4_error

    expected_gate_open = activation_required or t4_new_scope_publish
    if gate_open is not expected_gate_open:
        return None, (
            "check_activation_triggers(json) gate reduction mismatch: "
            f"gate_open={gate_open!r} expected={expected_gate_open!r}."
        )

    open_blockers_error = validate_open_blockers_payload(
        payload,
        active_trigger_ids=active_trigger_ids,
        trigger_rows_by_id=trigger_rows_by_id,
    )
    if open_blockers_error is not None:
        return None, open_blockers_error

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


def validate_payload_header_and_inputs(
    payload: dict[str, Any],
    *,
    expected_issues_path: Path,
    expected_milestones_path: Path,
    expected_catalog_path: Path,
    expected_open_blockers_path: Path | None,
    expected_t4_overlay_path: Path | None,
) -> str | None:
    mode = payload.get("mode")
    if mode != "offline-deterministic":
        return (
            "check_activation_triggers(json) missing deterministic "
            "mode 'offline-deterministic'."
        )

    inputs = payload.get("inputs")
    if not isinstance(inputs, dict):
        return "check_activation_triggers(json) missing object 'inputs'."

    return validate_payload_input_provenance(
        inputs,
        expected_issues_path=expected_issues_path,
        expected_milestones_path=expected_milestones_path,
        expected_catalog_path=expected_catalog_path,
        expected_open_blockers_path=expected_open_blockers_path,
        expected_t4_overlay_path=expected_t4_overlay_path,
    )


def validate_scalar_gate_fields(
    payload: dict[str, Any],
) -> tuple[bool | None, bool | None, int | None, str | None, str | None]:
    gate_open = payload.get("gate_open")
    if not isinstance(gate_open, bool):
        return None, None, None, None, "check_activation_triggers(json) missing boolean 'gate_open'."

    payload_exit_code = payload.get("exit_code")
    if (
        isinstance(payload_exit_code, bool)
        or not isinstance(payload_exit_code, int)
        or payload_exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN)
    ):
        return None, None, None, None, (
            "check_activation_triggers(json) missing deterministic integer "
            "'exit_code' in {0,1}."
        )

    queue_state = payload.get("queue_state")
    if not isinstance(queue_state, str) or not queue_state:
        return None, None, None, None, "check_activation_triggers(json) missing non-empty 'queue_state'."

    activation_required = payload.get("activation_required")
    if not isinstance(activation_required, bool):
        return None, None, None, None, (
            "check_activation_triggers(json) missing boolean 'activation_required'."
        )

    return gate_open, activation_required, payload_exit_code, queue_state, None


def validate_t4_governance_overlay(payload: dict[str, Any]) -> tuple[bool, str | None]:
    t4_overlay = payload.get("t4_governance_overlay")
    if not isinstance(t4_overlay, dict):
        return False, "check_activation_triggers(json) missing object 't4_governance_overlay'."

    t4_new_scope_publish = t4_overlay.get("new_scope_publish")
    if not isinstance(t4_new_scope_publish, bool):
        return False, (
            "check_activation_triggers(json) missing boolean "
            "'t4_governance_overlay.new_scope_publish'."
        )

    t4_source = t4_overlay.get("source")
    if not isinstance(t4_source, str) or not t4_source:
        return False, (
            "check_activation_triggers(json) missing non-empty string "
            "'t4_governance_overlay.source'."
        )

    return t4_new_scope_publish, None


__all__ = [
    "parse_activation_payload",
    "validate_freshness_entry",
    "validate_payload_header_and_inputs",
    "validate_scalar_gate_fields",
    "validate_t4_governance_overlay",
]
