"""Artifact and workflow report-surface validation rules."""

from __future__ import annotations

from objc3c_shared.json_io import load_json_object as load_json

from .diagnostics import ValidationFailure
from .paths import EXPECTED_ARTIFACT_ROOT, EXPECTED_REPORT_ROOT, ROOT
from .path_validation import require_contract_id


def validate_artifact_surface(required_paths: dict[str, str]) -> None:
    artifact_surface = load_json(ROOT / required_paths["artifact_surface"])
    require_contract_id(artifact_surface, "artifact_surface")
    if artifact_surface.get("artifact_root") != EXPECTED_ARTIFACT_ROOT:
        raise ValidationFailure("artifact surface artifact_root drifted")
    if artifact_surface.get("report_root") != EXPECTED_REPORT_ROOT:
        raise ValidationFailure("artifact surface report_root drifted")


def validate_workflow_surface(
    surface: dict[str, object],
    required_paths: dict[str, str],
) -> None:
    workflow_surface = load_json(ROOT / required_paths["workflow_surface"])
    require_contract_id(workflow_surface, "workflow_surface")
    if workflow_surface.get("package_bridge") != surface.get("package_bridge"):
        raise ValidationFailure("workflow surface package_bridge drifted")
    if workflow_surface.get("source_check_action") != surface.get("source_check_action"):
        raise ValidationFailure("workflow surface source_check_action drifted")
    if workflow_surface.get("validate_action") != "validate-external-validation":
        raise ValidationFailure("workflow surface validate_action drifted")
    required_actions = workflow_surface.get("required_actions")
    validate_child_actions = workflow_surface.get("validate_child_actions")
    required_child_reports = workflow_surface.get("required_child_reports")
    if not isinstance(required_actions, list) or not required_actions:
        raise ValidationFailure("workflow surface required_actions drifted")
    if not isinstance(validate_child_actions, list) or not validate_child_actions:
        raise ValidationFailure("workflow surface validate_child_actions drifted")
    if validate_child_actions != required_actions[: len(validate_child_actions)]:
        raise ValidationFailure("workflow surface child action ordering drifted")
    if not isinstance(required_child_reports, dict) or not required_child_reports:
        raise ValidationFailure("workflow surface required_child_reports drifted")
    for relative_path in required_child_reports:
        if not isinstance(relative_path, str) or not relative_path.startswith(
            EXPECTED_REPORT_ROOT
        ):
            raise ValidationFailure("workflow surface child report path drifted")
