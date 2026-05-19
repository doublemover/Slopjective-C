"""Storage/reflection source-surface models."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


PayloadField = tuple[str, Any]


@dataclass(frozen=True)
class StorageReflectionSourceSurfaceDefinition:
    contract_id: str
    case_ids: frozenset[str]
    pre_case_fields: tuple[PayloadField, ...]
    fixture_paths: tuple[Any, ...]
    probe_paths: tuple[Any, ...]
    post_probe_fields: tuple[PayloadField, ...]


__all__ = [
    "PayloadField",
    "StorageReflectionSourceSurfaceDefinition",
]
