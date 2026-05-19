"""Synthesized accessor lowering metadata assertion data shapes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class SurfaceValueExpectation:
    field: str
    expected: Any
    message: str


@dataclass(frozen=True)
class SynthesizedPropertyLoweringRow:
    owner_kind: str
    owner_name: str
    property_name: str
    synthesized: bool
    getter_helper: str
    setter_helper: str
