from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class LabelDefinition:
    name: str
    color: str
    description: str
