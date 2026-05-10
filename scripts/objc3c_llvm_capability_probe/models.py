"""Typed payload helpers for LLVM capability probe reports."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ExecutableProbe:
    role: str
    path: str
    found: bool
    version_exit_code: int
    version_duration_ms: float
    version_headline: str
    diagnostic: str | None = None

    def as_payload(self) -> dict[str, object]:
        payload: dict[str, object] = {
            "role": self.role,
            "path": self.path,
            "found": self.found,
            "version_exit_code": self.version_exit_code,
            "version_duration_ms": self.version_duration_ms,
            "version_headline": self.version_headline,
        }
        if self.diagnostic is not None:
            payload["diagnostic"] = self.diagnostic
        return payload
