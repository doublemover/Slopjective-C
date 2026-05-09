"""Public bridge payload serialization."""

from __future__ import annotations

from dataclasses import asdict

from .public_bridge_registry import PACKAGE_BRIDGES


def describe_package_bridge_payload(script_name: str) -> dict[str, object]:
    return asdict(PACKAGE_BRIDGES[script_name])


__all__ = ["describe_package_bridge_payload"]
