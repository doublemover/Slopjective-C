"""Validation rules for the external validation source-surface contract."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .paths import EXPECTED_ARTIFACT_ROOT, EXPECTED_REPORT_ROOT, EXPECTED_REQUIRED_PATHS, ROOT
from .source_model import (
    EXPECTED_CONTRACT_IDS,
    EXPECTED_DISCLOSURE_MODES,
    EXPECTED_ESCALATION_TARGETS,
    EXPECTED_FAMILY_IDS,
    EXPECTED_INTAKE_FAMILIES,
    EXPECTED_INTAKE_SURFACES,
    EXPECTED_PUBLISHABLE_TRUST_STATES,
    EXPECTED_QUARANTINE_TRUST_STATES,
    EXPECTED_REQUIRED_PROVENANCE_FIELDS,
    EXPECTED_SOURCE_FAMILY_PATHS,
    EXPECTED_TRUST_STATES,
    SCHEMA_VERSION,
    SOURCE_SURFACE_CONTRACT_ID,
    SourceSurfaceValidation,
)


class ValidationFailure(RuntimeError):
    """Checked source-surface drift."""


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str:
    expected_path = EXPECTED_REQUIRED_PATHS[field_name]
    if source_surface.get(field_name) != expected_path:
        raise ValidationFailure(f"{field_name} drifted")
    return expected_path


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...]:
    if source_surface.get(field_name) != list(expected_items):
        raise ValidationFailure(f"{field_name} drifted")
    return expected_items


def require_contract_id(payload: dict[str, object], field_name: str) -> bool:
    expected_contract_id = EXPECTED_CONTRACT_IDS[field_name]
    if payload.get("contract_id") != expected_contract_id:
        raise ValidationFailure(f"{field_name} contract_id drifted")
    if payload.get("schema_version") != SCHEMA_VERSION:
        raise ValidationFailure(f"{field_name} schema_version drifted")
    return True


def validate_source_surface(
    source_surface_path: Path,
    *,
    expected_roots: tuple[str, ...],
) -> SourceSurfaceValidation:
    if not source_surface_path.is_file():
        raise ValidationFailure(f"missing source surface contract: {repo_rel(source_surface_path)}")

    surface = load_json(source_surface_path)
    if surface.get("contract_id") != SOURCE_SURFACE_CONTRACT_ID:
        raise ValidationFailure("contract_id drifted")
    if surface.get("schema_version") != SCHEMA_VERSION:
        raise ValidationFailure("schema_version drifted")

    checked_roots = require_exact_list(surface, "checked_in_roots", expected_roots)

    required_paths: dict[str, str] = {}
    checked_paths: list[str] = []
    for field_name in EXPECTED_REQUIRED_PATHS:
        relative_path = require_exact_path(surface, field_name)
        required_paths[field_name] = relative_path
        require_path(relative_path, kind=field_name)
        checked_paths.append(relative_path)
    for root in checked_roots:
        require_path(root, kind="checked-in root")
        checked_paths.append(root)

    intake_entry_summaries = _validate_intake_manifest(required_paths, checked_paths)
    quarantine_entry_summaries = _validate_quarantine_manifest(required_paths, checked_paths)
    _validate_artifact_surface(required_paths)
    _validate_workflow_surface(surface, required_paths)
    family_summaries = _validate_families(surface, checked_paths)

    checked_paths.append(repo_rel(source_surface_path))
    return SourceSurfaceValidation(
        required_paths=required_paths,
        checked_roots=checked_roots,
        family_summaries=family_summaries,
        intake_entry_summaries=intake_entry_summaries,
        quarantine_entry_summaries=quarantine_entry_summaries,
        checked_paths=sorted(set(checked_paths)),
    )


def _validate_intake_manifest(
    required_paths: dict[str, str],
    checked_paths: list[str],
) -> list[dict[str, Any]]:
    trust_policy = load_json(ROOT / required_paths["trust_policy"])
    require_contract_id(trust_policy, "trust_policy")
    if trust_policy.get("allowed_trust_states") != list(EXPECTED_TRUST_STATES):
        raise ValidationFailure("trust policy allowed_trust_states drifted")
    if trust_policy.get("publishable_trust_states") != list(EXPECTED_PUBLISHABLE_TRUST_STATES):
        raise ValidationFailure("trust policy publishable_trust_states drifted")
    if trust_policy.get("required_provenance_fields") != list(EXPECTED_REQUIRED_PROVENANCE_FIELDS):
        raise ValidationFailure("trust policy required_provenance_fields drifted")

    intake_manifest = load_json(ROOT / required_paths["intake_manifest"])
    require_contract_id(intake_manifest, "intake_manifest")
    entries = intake_manifest.get("entries")
    if not isinstance(entries, list) or len(entries) < 3:
        raise ValidationFailure("intake manifest entries drifted")

    allowed_trust_states = set(EXPECTED_TRUST_STATES)
    allowed_surfaces = set(EXPECTED_INTAKE_SURFACES)
    allowed_families = set(EXPECTED_INTAKE_FAMILIES)
    intake_entry_summaries: list[dict[str, Any]] = []
    for entry in entries:
        if not isinstance(entry, dict):
            raise ValidationFailure("intake manifest contains a non-object entry")
        summary_entry = _validate_intake_entry(
            entry,
            allowed_trust_states=allowed_trust_states,
            allowed_surfaces=allowed_surfaces,
            allowed_families=allowed_families,
            checked_paths=checked_paths,
        )
        intake_entry_summaries.append(summary_entry)
    return intake_entry_summaries


def _validate_intake_entry(
    entry: dict[str, object],
    *,
    allowed_trust_states: set[str],
    allowed_surfaces: set[str],
    allowed_families: set[str],
    checked_paths: list[str],
) -> dict[str, Any]:
    fixture_id = entry.get("fixture_id")
    trust_state = entry.get("trust_state")
    normalized_surface = entry.get("normalized_surface")
    family = entry.get("family")
    provenance = entry.get("provenance")
    replay_script = entry.get("replay_script")
    if not isinstance(fixture_id, str) or not fixture_id:
        raise ValidationFailure("intake manifest entry missing fixture_id")
    if trust_state not in allowed_trust_states:
        raise ValidationFailure(f"{fixture_id} references unknown trust_state")
    if normalized_surface not in allowed_surfaces:
        raise ValidationFailure(f"{fixture_id} references unknown normalized_surface")
    if family not in allowed_families:
        raise ValidationFailure(f"{fixture_id} references unknown family")
    if not isinstance(provenance, dict):
        raise ValidationFailure(f"{fixture_id} is missing provenance")
    for field_name in EXPECTED_REQUIRED_PROVENANCE_FIELDS:
        if not isinstance(provenance.get(field_name), str) or not provenance.get(field_name):
            raise ValidationFailure(f"{fixture_id} is missing provenance.{field_name}")
    if not isinstance(replay_script, str) or not replay_script:
        raise ValidationFailure(f"{fixture_id} is missing replay_script")
    require_path(replay_script, kind=f"{fixture_id} replay script")
    checked_paths.append(replay_script)
    summary_entry = {
        "fixture_id": fixture_id,
        "trust_state": trust_state,
        "normalized_surface": normalized_surface,
        "family": family,
        "replay_script": replay_script,
    }
    if normalized_surface == "conformance-case":
        normalized_case_path = entry.get("normalized_case_path")
        if not isinstance(normalized_case_path, str) or not normalized_case_path:
            raise ValidationFailure(f"{fixture_id} is missing normalized_case_path")
        require_path(normalized_case_path, kind=f"{fixture_id} normalized case")
        checked_paths.append(normalized_case_path)
        summary_entry["normalized_case_path"] = normalized_case_path
    else:
        normalized_contract_path = entry.get("normalized_contract_path")
        coverage_anchor = entry.get("coverage_anchor")
        if not isinstance(normalized_contract_path, str) or not normalized_contract_path:
            raise ValidationFailure(f"{fixture_id} is missing normalized_contract_path")
        if not isinstance(coverage_anchor, str) or not coverage_anchor:
            raise ValidationFailure(f"{fixture_id} is missing coverage_anchor")
        require_path(normalized_contract_path, kind=f"{fixture_id} normalized contract")
        require_path(coverage_anchor, kind=f"{fixture_id} coverage anchor")
        checked_paths.extend([normalized_contract_path, coverage_anchor])
        summary_entry["normalized_contract_path"] = normalized_contract_path
        summary_entry["coverage_anchor"] = coverage_anchor
    return summary_entry


def _validate_quarantine_manifest(
    required_paths: dict[str, str],
    checked_paths: list[str],
) -> list[dict[str, Any]]:
    quarantine_manifest = load_json(ROOT / required_paths["quarantine_manifest"])
    require_contract_id(quarantine_manifest, "quarantine_manifest")
    quarantine_entries = quarantine_manifest.get("entries")
    if not isinstance(quarantine_entries, list) or len(quarantine_entries) < 3:
        raise ValidationFailure("quarantine manifest entries drifted")

    allowed_disclosure_modes = set(EXPECTED_DISCLOSURE_MODES)
    allowed_escalation_targets = set(EXPECTED_ESCALATION_TARGETS)
    quarantine_entry_summaries: list[dict[str, Any]] = []
    for entry in quarantine_entries:
        if not isinstance(entry, dict):
            raise ValidationFailure("quarantine manifest contains a non-object entry")
        quarantine_entry_summaries.append(
            _validate_quarantine_entry(
                entry,
                allowed_disclosure_modes=allowed_disclosure_modes,
                allowed_escalation_targets=allowed_escalation_targets,
                checked_paths=checked_paths,
            )
        )
    return quarantine_entry_summaries


def _validate_quarantine_entry(
    entry: dict[str, object],
    *,
    allowed_disclosure_modes: set[str],
    allowed_escalation_targets: set[str],
    checked_paths: list[str],
) -> dict[str, Any]:
    fixture_id = entry.get("fixture_id")
    trust_state = entry.get("trust_state")
    diagnostic_id = entry.get("diagnostic_id")
    escalation_target = entry.get("escalation_target")
    disclosure_compatibility = entry.get("disclosure_compatibility")
    reason = entry.get("reason")
    normalized_contract_path = entry.get("normalized_contract_path")
    replay_script = entry.get("replay_script")
    if not isinstance(fixture_id, str) or not fixture_id:
        raise ValidationFailure("quarantine manifest entry missing fixture_id")
    if trust_state not in set(EXPECTED_QUARANTINE_TRUST_STATES):
        raise ValidationFailure(f"{fixture_id} uses an invalid quarantine trust_state")
    if not isinstance(diagnostic_id, str) or not diagnostic_id.startswith("OBJC3-EXTERNAL-EVIDENCE-"):
        raise ValidationFailure(f"{fixture_id} is missing an external-evidence diagnostic_id")
    if escalation_target not in allowed_escalation_targets:
        raise ValidationFailure(f"{fixture_id} references unknown escalation_target")
    if disclosure_compatibility not in allowed_disclosure_modes:
        raise ValidationFailure(f"{fixture_id} references unknown disclosure_compatibility")
    if not isinstance(reason, str) or not reason:
        raise ValidationFailure(f"{fixture_id} is missing reason")
    if not isinstance(normalized_contract_path, str) or not normalized_contract_path:
        raise ValidationFailure(f"{fixture_id} is missing normalized_contract_path")
    if not isinstance(replay_script, str) or not replay_script:
        raise ValidationFailure(f"{fixture_id} is missing replay_script")
    require_path(normalized_contract_path, kind=f"{fixture_id} normalized contract")
    require_path(replay_script, kind=f"{fixture_id} replay script")
    checked_paths.extend([normalized_contract_path, replay_script])
    return {
        "fixture_id": fixture_id,
        "trust_state": trust_state,
        "diagnostic_id": diagnostic_id,
        "escalation_target": escalation_target,
        "disclosure_compatibility": disclosure_compatibility,
        "normalized_contract_path": normalized_contract_path,
        "replay_script": replay_script,
    }


def _validate_artifact_surface(required_paths: dict[str, str]) -> None:
    artifact_surface = load_json(ROOT / required_paths["artifact_surface"])
    require_contract_id(artifact_surface, "artifact_surface")
    if artifact_surface.get("artifact_root") != EXPECTED_ARTIFACT_ROOT:
        raise ValidationFailure("artifact surface artifact_root drifted")
    if artifact_surface.get("report_root") != EXPECTED_REPORT_ROOT:
        raise ValidationFailure("artifact surface report_root drifted")


def _validate_workflow_surface(
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


def _validate_families(
    surface: dict[str, object],
    checked_paths: list[str],
) -> list[dict[str, Any]]:
    families = surface.get("source_families")
    if not isinstance(families, list) or len(families) != len(EXPECTED_FAMILY_IDS):
        raise ValidationFailure("source_families drifted")

    family_summaries: list[dict[str, Any]] = []
    observed_family_ids: list[str] = []
    for family in families:
        if not isinstance(family, dict):
            raise ValidationFailure("source_families contains a non-object entry")
        family_id = family.get("family_id")
        coverage_goal = family.get("coverage_goal")
        source_paths = family.get("source_paths")
        if not isinstance(family_id, str) or not family_id:
            raise ValidationFailure("source_families entry missing family_id")
        if not isinstance(coverage_goal, str) or not coverage_goal:
            raise ValidationFailure(f"{family_id} is missing coverage_goal")
        if not isinstance(source_paths, list) or not source_paths:
            raise ValidationFailure(f"{family_id} is missing source_paths")
        expected_source_paths = EXPECTED_SOURCE_FAMILY_PATHS.get(family_id)
        if expected_source_paths is None or source_paths != list(expected_source_paths):
            raise ValidationFailure(f"{family_id} source_paths drifted")
        observed_family_ids.append(family_id)
        family_checked_paths: list[str] = []
        for source_path in source_paths:
            if not isinstance(source_path, str) or not source_path:
                raise ValidationFailure(f"{family_id} contains a non-string source path")
            require_path(source_path, kind=f"{family_id} source path")
            checked_paths.append(source_path)
            family_checked_paths.append(source_path)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(family_checked_paths),
                "source_paths": family_checked_paths,
            }
        )

    if observed_family_ids != list(EXPECTED_FAMILY_IDS):
        raise ValidationFailure("source_families inventory drifted")
    return family_summaries
