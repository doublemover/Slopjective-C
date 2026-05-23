"""Typed models for platform-hardening contract helpers."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


def platform_id_for_host(system: str, machine: str) -> str:
    normalized_system = system.lower()
    normalized_machine = machine.lower()
    if normalized_system == "windows" and normalized_machine in {"amd64", "x86_64"}:
        return "windows-x64"
    if normalized_system == "linux" and normalized_machine in {"amd64", "x86_64"}:
        return "linux-x64"
    if normalized_system == "darwin" and normalized_machine in {"arm64", "aarch64"}:
        return "darwin-arm64"
    return "unsupported"


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
        return {
            "os": self.os,
            "arch": self.arch,
            "system": self.system,
            "machine": self.machine,
            "platform_id": platform_id_for_host(self.system, self.machine),
        }


__all__ = [
    "HostSnapshot",
    "ToolProbe",
    "platform_id_for_host",
]
