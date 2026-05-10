"""Runtime acceptance catalog surface facade."""

from __future__ import annotations

from .catalog_data import AUTHORITATIVE_CHILD_REPORT_CONTRACTS, COMMON_SURFACES
from .contract_ids import *
from .model import SurfaceRequirement


__all__ = [
    "AUTHORITATIVE_CHILD_REPORT_CONTRACTS",
    "COMMON_SURFACES",
]
