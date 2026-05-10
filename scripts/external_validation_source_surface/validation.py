"""Public validation surface for the external validation source contract."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .diagnostics import ValidationFailure
from .path_validation import (
    require_contract_id,
    require_exact_list,
    require_exact_path,
    require_path,
)
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
from .source_validation import validate_source_surface

__all__ = [
    "Any",
    "EXPECTED_ARTIFACT_ROOT",
    "EXPECTED_CONTRACT_IDS",
    "EXPECTED_DISCLOSURE_MODES",
    "EXPECTED_ESCALATION_TARGETS",
    "EXPECTED_FAMILY_IDS",
    "EXPECTED_INTAKE_FAMILIES",
    "EXPECTED_INTAKE_SURFACES",
    "EXPECTED_PUBLISHABLE_TRUST_STATES",
    "EXPECTED_QUARANTINE_TRUST_STATES",
    "EXPECTED_REPORT_ROOT",
    "EXPECTED_REQUIRED_PATHS",
    "EXPECTED_REQUIRED_PROVENANCE_FIELDS",
    "EXPECTED_SOURCE_FAMILY_PATHS",
    "EXPECTED_TRUST_STATES",
    "Path",
    "ROOT",
    "SCHEMA_VERSION",
    "SOURCE_SURFACE_CONTRACT_ID",
    "SourceSurfaceValidation",
    "ValidationFailure",
    "load_json",
    "repo_rel",
    "require_contract_id",
    "require_exact_list",
    "require_exact_path",
    "require_path",
    "validate_source_surface",
]
