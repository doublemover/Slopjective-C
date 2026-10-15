"""Typed models for platform-hardening contract helpers."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .constants import PLATFORM_IDENTITY_CONTRACTS


def platform_id_for_host(system: str, machine: str) -> str:
    normalized_system = system.lower()
    normalized_machine = machine.lower()
    for platform_id, identity in PLATFORM_IDENTITY_CONTRACTS.items():
        host_systems = {str(item) for item in identity["host_systems"]}
        host_machines = {str(item) for item in identity["host_machines"]}
        if normalized_system in host_systems and normalized_machine in host_machines:
            return platform_id
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
