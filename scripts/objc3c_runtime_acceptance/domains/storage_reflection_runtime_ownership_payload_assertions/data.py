"""Storage ownership reflection assertion data shapes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class StorageOwnershipReflectionFacts:
    payload: dict[str, Any]
    box_entry: dict[str, Any]
    implementation_surface: dict[str, Any]
    manifest_implementation_surface: dict[str, Any]


@dataclass(frozen=True)
class SurfaceFieldExpectation:
    field: str
    expected: Any
    message: str


@dataclass(frozen=True)
class LlTextExpectation:
    fragment: str
    message: str


__all__ = [
    "LlTextExpectation",
    "StorageOwnershipReflectionFacts",
    "SurfaceFieldExpectation",
]
