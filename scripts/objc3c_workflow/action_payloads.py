"""Machine-readable action payloads for the objc3c workflow registry."""

from __future__ import annotations

from .action_payload_builder import build_action_payload
from .action_registry_payload import build_registry_payload
from .action_spec import ActionSpec
from .registry_views import action_specs, require_action_spec


def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    return build_action_payload(spec)


def list_actions_payload() -> dict[str, object]:
    return build_registry_payload(action_specs())


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(require_action_spec(action))
