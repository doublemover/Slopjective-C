"""Evidence manifest validation rules."""

from __future__ import annotations

from typing import Any

from objc3c_shared.json_io import load_json_object as load_json

from .diagnostics import ValidationFailure
from .paths import ROOT
from .path_validation import require_contract_id, require_path
from .source_model import (
    EXPECTED_DISCLOSURE_MODES,
    EXPECTED_ESCALATION_TARGETS,
    EXPECTED_INTAKE_FAMILIES,
    EXPECTED_INTAKE_SURFACES,
    EXPECTED_PUBLISHABLE_TRUST_STATES,
    EXPECTED_QUARANTINE_TRUST_STATES,
    EXPECTED_REQUIRED_PROVENANCE_FIELDS,
    EXPECTED_TRUST_STATES,
)


def validate_intake_manifest(
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


def validate_quarantine_manifest(
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
