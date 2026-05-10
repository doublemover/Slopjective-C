from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from activation_triggers.model import CONTRACT_ID
from activation_triggers.model import EXIT_GATE_CLOSED
from activation_triggers.model import EXIT_GATE_OPEN
from activation_triggers.model import OPEN_BLOCKERS_TRIGGER_ID
from activation_triggers.model import TRIGGER_ORDER
from activation_triggers.model import utc_now_string


def build_freshness_entry(*, requested: bool) -> dict[str, Any]:
    if not requested:
        return {
            "requested": False,
            "max_age_seconds": None,
            "generated_at_utc": None,
            "age_seconds": None,
            "fresh": None,
        }
    return {
        "requested": True,
        "max_age_seconds": None,
        "generated_at_utc": utc_now_string(),
        "age_seconds": 0,
        "fresh": True,
    }


def build_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    t4_overlay_path: Path | None,
    actionable_statuses: Sequence[str],
    issues_count: int,
    milestones_count: int,
    actionable_count: int,
    open_blocker_count: int,
    t4_new_scope_publish: bool,
    t4_source: str,
    issues_max_age_seconds: int | None,
    milestones_max_age_seconds: int | None,
) -> dict[str, Any]:
    trigger_rows = []
    active_trigger_ids: list[str] = []
    for trigger_id, condition in TRIGGER_ORDER:
        count = {
            "T1-ISSUES": issues_count,
            "T2-MILESTONES": milestones_count,
            "T3-ACTIONABLE-ROWS": actionable_count,
            "T5-OPEN-BLOCKERS": open_blocker_count,
        }[trigger_id]
        fired = count > 0
        trigger_rows.append(
            {"id": trigger_id, "condition": condition, "count": count, "fired": fired}
        )
        if fired:
            active_trigger_ids.append(trigger_id)

    activation_required = bool(active_trigger_ids)
    gate_open = activation_required or t4_new_scope_publish
    queue_state = "dispatch-open" if gate_open else "idle"
    exit_code = EXIT_GATE_OPEN if gate_open else EXIT_GATE_CLOSED

    issues_freshness = build_freshness_entry(requested=issues_max_age_seconds is not None)
    issues_freshness["max_age_seconds"] = issues_max_age_seconds
    milestones_freshness = build_freshness_entry(requested=milestones_max_age_seconds is not None)
    milestones_freshness["max_age_seconds"] = milestones_max_age_seconds

    return {
        "mode": "offline-deterministic",
        "contract_id": CONTRACT_ID,
        "fail_closed": True,
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": display_path(open_blockers_path) if open_blockers_path is not None else None,
            "t4_governance_overlay_json": display_path(t4_overlay_path) if t4_overlay_path is not None else None,
        },
        "actionable_statuses": list(actionable_statuses),
        "trigger_order": [trigger_id for trigger_id, _ in TRIGGER_ORDER],
        "freshness": {
            "issues": issues_freshness,
            "milestones": milestones_freshness,
        },
        "triggers": trigger_rows,
        "active_trigger_ids": active_trigger_ids,
        "activation_required": activation_required,
        "open_blockers": {
            "count": open_blocker_count,
            "trigger_id": OPEN_BLOCKERS_TRIGGER_ID,
            "trigger_fired": open_blocker_count > 0,
        },
        "t4_governance_overlay": {
            "new_scope_publish": t4_new_scope_publish,
            "source": t4_source,
        },
        "gate_open": gate_open,
        "queue_state": queue_state,
        "exit_code": exit_code,
    }
