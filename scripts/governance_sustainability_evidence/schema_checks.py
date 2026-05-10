"""Contract and schema checks for governance sustainability evidence."""

from __future__ import annotations

from typing import Any, Mapping

from .contracts import BLOCKER_METADATA, SUPPORT_STATE
from .model import JsonObject
from .paths import SELF_GENERATED_REPORTS


def generated_report_failure(path: str, payload: Mapping[str, Any]) -> str | None:
    if path in SELF_GENERATED_REPORTS:
        return None
    if not payload:
        return f"missing generated report {path}"
    if payload.get("status") not in (None, "PASS") or payload.get("ok") is False:
        return f"generated report not passing: {path}"
    return None


def release_blockers(
    report_payloads: Mapping[str, JsonObject],
    budget_enforcement: Mapping[str, Any],
    integration: Mapping[str, Any],
) -> list[str]:
    failures = [
        failure
        for path, payload in report_payloads.items()
        for failure in [generated_report_failure(path, payload)]
        if failure is not None
    ]
    if budget_enforcement.get("status") != "PASS":
        failures.append("budget enforcement is not PASS")
    if integration.get("status") != "PASS":
        failures.append("integration summary is not PASS")
    return failures


def measured_budget_state(budget_inventory: Mapping[str, Any]) -> JsonObject:
    measured = (
        budget_inventory.get("measured", {})
        if isinstance(budget_inventory.get("measured"), dict)
        else {}
    )
    return {
        "package_bridge_count": measured.get("package_bridge_count"),
        "package_bridge_budget": measured.get("package_bridge_budget"),
        "public_workflow_action_count": measured.get("public_workflow_action_count"),
        "live_check_script_count": measured.get("live_check_script_count"),
    }


def claim_audit(
    failures: list[str],
    budget_inventory: Mapping[str, Any],
) -> JsonObject:
    return {
        "support_state": SUPPORT_STATE,
        "release_blockers": failures,
        "demoted_or_out_of_scope_claims": [
            "hosted community infrastructure",
            "hosted registry moderation",
            "prose-only governance approval",
        ],
        "blocker_metadata": BLOCKER_METADATA,
        "measured_budget_state": measured_budget_state(budget_inventory),
    }


__all__ = [
    "claim_audit",
    "generated_report_failure",
    "measured_budget_state",
    "release_blockers",
]
