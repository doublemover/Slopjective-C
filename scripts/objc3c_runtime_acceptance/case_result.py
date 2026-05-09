"""Runtime acceptance case result model."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


RUNTIME_ACCEPTANCE_CASE_PASS_STATUS = "PASS"
RUNTIME_ACCEPTANCE_CASE_FAIL_STATUS = "FAIL"


@dataclass(frozen=True)
class CaseResult:
    case_id: str
    probe: str
    fixture: str | None
    claim_class: str
    passed: bool
    summary: dict[str, Any]

    @property
    def status(self) -> str:
        if self.passed:
            return RUNTIME_ACCEPTANCE_CASE_PASS_STATUS
        return RUNTIME_ACCEPTANCE_CASE_FAIL_STATUS


__all__ = [
    "CaseResult",
    "RUNTIME_ACCEPTANCE_CASE_FAIL_STATUS",
    "RUNTIME_ACCEPTANCE_CASE_PASS_STATUS",
]
