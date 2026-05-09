"""Typed public package bridge model."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class PackageBridgeSpec:
    package_bridge: str
    action: str
    summary: str
    audience: str
    category: str
    backend: str
    validation_tier: str
    guarantee_owner: str
    pass_through_args: bool
    mode: str
    runner_path: str
    public_entrypoint: str


__all__ = ["PackageBridgeSpec"]
