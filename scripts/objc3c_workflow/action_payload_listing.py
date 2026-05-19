"""Action-list payload owner for workflow registry requests."""

from __future__ import annotations

from .action_registry_payload import build_registry_payload
from .registry_views import action_specs


def list_actions_payload() -> dict[str, object]:
    return build_registry_payload(action_specs())


__all__ = ["list_actions_payload"]
