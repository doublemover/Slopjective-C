"""Runtime acceptance route model for workflow action metadata."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class RuntimeAcceptanceRoute:
    action: str
    suite: str
    title: str
    validation_tier: str
    guarantee_owner: str

    @property
    def target(self) -> str:
        return f"python:scripts/check_objc3c_runtime_acceptance.py --suite {self.suite}"

    def args(self) -> tuple[str, ...]:
        return ("--suite", self.suite)

    def command(self, runtime_acceptance_script: Path) -> list[str]:
        return [sys.executable, str(runtime_acceptance_script), *self.args()]
