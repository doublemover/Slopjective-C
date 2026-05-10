"""Catalog surface requirement model."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class SurfaceRequirement:
    key: str
    contract_id: str
    required_fields: tuple[str, ...] = ()


__all__ = ["SurfaceRequirement"]
