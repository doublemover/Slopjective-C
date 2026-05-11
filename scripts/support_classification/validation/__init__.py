"""Support classification validation facade."""

from __future__ import annotations

from .aggregates import validate_evidence_families
from .aggregates import validate_public_claim_surfaces
from .aggregates import validate_support_classes
from .normalization import normalize_checked_in_path
from .normalization import require_existing_contract_path
from .normalization import require_list
from .normalization import require_string
from .orchestration import classify_surfaces
from .orchestration import validate_demotion_triggers
from .records import validate_surface_paths

__all__ = [
    "classify_surfaces",
    "normalize_checked_in_path",
    "require_existing_contract_path",
    "require_list",
    "require_string",
    "validate_demotion_triggers",
    "validate_evidence_families",
    "validate_public_claim_surfaces",
    "validate_support_classes",
    "validate_surface_paths",
]
