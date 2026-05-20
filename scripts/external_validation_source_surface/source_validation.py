"""Source-surface and source-family validation orchestration."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .diagnostics import ValidationFailure
from .evidence_validation import validate_intake_manifest, validate_quarantine_manifest
from .path_validation import require_exact_list, require_exact_path, require_path
from .paths import EXPECTED_REQUIRED_PATHS
from .report_validation import (
    validate_artifact_surface,
    validate_claim_gate_contracts,
    validate_workflow_surface,
)
from .source_model import (
    EXPECTED_FAMILY_IDS,
    EXPECTED_SOURCE_FAMILY_PATHS,
    SCHEMA_VERSION,
    SOURCE_SURFACE_CONTRACT_ID,
    SourceSurfaceValidation,
)


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

    intake_entry_summaries = validate_intake_manifest(required_paths, checked_paths)
    quarantine_entry_summaries = validate_quarantine_manifest(required_paths, checked_paths)
    validate_artifact_surface(required_paths)
    validate_claim_gate_contracts(required_paths)
    validate_workflow_surface(surface, required_paths)
    family_summaries = validate_families(surface, checked_paths)

    checked_paths.append(repo_rel(source_surface_path))
    return SourceSurfaceValidation(
        required_paths=required_paths,
        checked_roots=checked_roots,
        family_summaries=family_summaries,
        intake_entry_summaries=intake_entry_summaries,
        quarantine_entry_summaries=quarantine_entry_summaries,
        checked_paths=sorted(set(checked_paths)),
    )


def validate_families(
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
