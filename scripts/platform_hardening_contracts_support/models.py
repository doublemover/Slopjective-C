"""Typed models for platform-hardening contract helpers."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class ToolProbe:
    command: tuple[str, ...]
    available: bool
    exit_code: int
    headline: str

    def as_json(self) -> dict[str, Any]:
        return {
            "command": list(self.command),
            "available": self.available,
            "exit_code": self.exit_code,
            "headline": self.headline,
        }


@dataclass(frozen=True)
class HostSnapshot:
    os: str
    arch: str
    system: str
    machine: str

    def as_json(self) -> dict[str, str]:
        return {"os": self.os, "arch": self.arch}


__all__ = [
    "HostSnapshot",
    "ToolProbe",
]
