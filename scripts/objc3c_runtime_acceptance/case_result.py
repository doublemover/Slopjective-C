"""Runtime acceptance case result model."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class CaseResult:
    case_id: str
    probe: str
    fixture: str | None
    claim_class: str
    passed: bool
    summary: dict[str, Any]
