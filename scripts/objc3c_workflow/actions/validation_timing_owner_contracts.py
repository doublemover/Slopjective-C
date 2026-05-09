"""Owner contract payloads for validation timing surfaces."""

from __future__ import annotations

from collections.abc import Mapping

VALIDATION_TIMING_REQUIRED_OWNER_KEYS = (
    "source_owner",
    "budget_owner",
    "report_owner",
    "profile_owner",
    "render_owner",
    "orchestration_owner",
    "hard_blocking_decision_owner",
)

VALIDATION_TIMING_OWNER_CONTRACT: dict[str, str] = {
    "source_owner": "validation_timing_changed_paths",
    "budget_owner": "validation_timing_budgets",
    "report_owner": "validation_timing_report_io",
    "profile_owner": "validation_timing_profile_rules",
    "render_owner": "validation_timing_markdown",
    "orchestration_owner": "validation_timing_orchestration",
    "hard_blocking_decision_owner": "validation_timing_budgets",
}


def validation_timing_owner_payload() -> dict[str, str]:
    return dict(VALIDATION_TIMING_OWNER_CONTRACT)


def require_validation_timing_owners(payload: Mapping[str, object]) -> None:
    owners = payload.get("owners")
    if not isinstance(owners, Mapping):
        raise ValueError("validation timing payload is missing owners")
    missing = [
        key
        for key in VALIDATION_TIMING_REQUIRED_OWNER_KEYS
        if not isinstance(owners.get(key), str) or not owners.get(key)
    ]
    if missing:
        raise ValueError(
            "validation timing payload is missing owner keys: "
            + ", ".join(sorted(missing))
        )
