"""Typed payload helpers for LLVM capability probe reports."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ExecutableProbe:
    role: str
    path: str
    configured_path: str
    resolved_path: str
    shadowing_status: str
    found: bool
    version_exit_code: int
    version_duration_ms: float
    version_headline: str
    version: str
    vendor: str
    diagnostic: str | None = None

    def as_payload(self) -> dict[str, object]:
        payload: dict[str, object] = {
            "role": self.role,
            "path": self.path,
            "configured_path": self.configured_path,
            "resolved_path": self.resolved_path,
            "shadowing_status": self.shadowing_status,
            "found": self.found,
            "version_exit_code": self.version_exit_code,
            "version_duration_ms": self.version_duration_ms,
            "version_headline": self.version_headline,
            "version": self.version,
            "vendor": self.vendor,
        }
        if self.diagnostic is not None:
            payload["diagnostic"] = self.diagnostic
        return payload
