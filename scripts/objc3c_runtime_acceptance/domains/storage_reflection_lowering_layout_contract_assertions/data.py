"""Storage/reflection layout lowering contract assertion data shapes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


ManifestSurface = dict[str, Any]


@dataclass(frozen=True)
class SurfaceValueExpectation:
    field: str
    expected: Any
    message: str
    identity: bool = False
