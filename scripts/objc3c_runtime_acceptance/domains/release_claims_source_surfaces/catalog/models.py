"""Release-claims source-surface catalog models."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from ..data import PayloadField, ReleaseClaimsSourceSurfaceData


PayloadFields = tuple[PayloadField, ...]
SurfacePaths = tuple[Any, ...]


@dataclass(frozen=True)
class ReleaseClaimGroup:
    contract_id: str
    case_ids: frozenset[str]
    pre_case_fields: PayloadFields
    fixture_paths: SurfacePaths
    post_fixture_fields: PayloadFields


__all__ = [
    "PayloadField",
    "PayloadFields",
    "ReleaseClaimGroup",
    "ReleaseClaimsSourceSurfaceData",
    "SurfacePaths",
]
