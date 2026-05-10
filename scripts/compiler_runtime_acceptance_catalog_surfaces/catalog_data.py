"""Static runtime acceptance catalog surface definitions."""

from __future__ import annotations

from pathlib import Path

from .contract_ids import *
from .evidence_fields import (
    AUTHORITATIVE_PROBE_PATH_FIELD,
    SOURCE_CODE_AND_CASE_FIELDS,
    SOURCE_FIELD_AND_CASE_FIELDS,
    case_evidence_fields,
    probe_path_fields,
    source_model_case_fields,
)
from .model import SurfaceRequirement
from .surface_validation import (
    validate_authoritative_child_contracts,
    validate_surface_requirements,
)


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .catalog_data.authoritative_children import (  # noqa: E402
    AUTHORITATIVE_CHILD_REPORT_CONTRACTS,
)
from .catalog_data.surfaces import COMMON_SURFACES  # noqa: E402


validate_surface_requirements(COMMON_SURFACES)
validate_authoritative_child_contracts(AUTHORITATIVE_CHILD_REPORT_CONTRACTS, COMMON_SURFACES)

__all__ = [
    "AUTHORITATIVE_CHILD_REPORT_CONTRACTS",
    "COMMON_SURFACES",
]
