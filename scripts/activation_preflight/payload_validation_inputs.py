"""Input provenance checks for activation preflight JSON payloads."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def validate_payload_input_provenance(
    inputs: dict[str, Any],
    *,
    expected_issues_path: Path,
    expected_milestones_path: Path,
    expected_catalog_path: Path,
    expected_open_blockers_path: Path | None,
    expected_t4_overlay_path: Path | None,
) -> str | None:
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
        return (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.issues_json={issues_input!r} expected={expected_issues_display_path!r}."
        )

    milestones_input = inputs.get("milestones_json")
    if milestones_input != expected_milestones_display_path:
        return (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.milestones_json={milestones_input!r} "
            f"expected={expected_milestones_display_path!r}."
        )

    catalog_input = inputs.get("catalog_json")
    if catalog_input != expected_catalog_display_path:
        return (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.catalog_json={catalog_input!r} expected={expected_catalog_display_path!r}."
        )

    open_blockers_input = inputs.get("open_blockers_json")
    if open_blockers_input != expected_open_blockers_display_path:
        return (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.open_blockers_json={open_blockers_input!r} "
            f"expected={expected_open_blockers_display_path!r}."
        )

    t4_overlay_input = inputs.get("t4_governance_overlay_json")
    if t4_overlay_input != expected_t4_overlay_display_path:
        return (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.t4_governance_overlay_json={t4_overlay_input!r} "
            f"expected={expected_t4_overlay_display_path!r}."
        )

    return None
