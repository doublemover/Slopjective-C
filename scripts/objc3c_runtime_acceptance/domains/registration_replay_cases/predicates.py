"""Registration replay assertion predicates."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any


def manifest_is_replay_ready(manifest: Mapping[str, Any]) -> bool:
    return manifest.get("ready_for_live_registration_discovery_replay") is True


def status_succeeded(payload: Mapping[str, Any], status_key: str) -> bool:
    return payload.get(status_key) == 0


__all__ = [
    "manifest_is_replay_ready",
    "status_succeeded",
]
