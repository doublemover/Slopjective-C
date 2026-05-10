"""Importable compiler/runtime acceptance catalog surface package."""

from __future__ import annotations

from . import contract_ids as _contract_ids
from .catalog import AUTHORITATIVE_CHILD_REPORT_CONTRACTS, COMMON_SURFACES
from .contract_ids import *
from .model import SurfaceRequirement

__all__ = [
    *_contract_ids.__all__,
    "AUTHORITATIVE_CHILD_REPORT_CONTRACTS",
    "COMMON_SURFACES",
    "SurfaceRequirement",
]
