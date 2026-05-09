"""Workflow action payload assembly."""

from __future__ import annotations

from dataclasses import asdict

from .action_payload_capability_truth import action_capability_truth_fields
from .action_payload_field_owners import action_payload_owner_fields
from .action_payload_public_fields import public_action_fields
from .action_payload_schema_fields import action_schema_fields
from .action_spec import ActionSpec


def shared_action_payload_fields(action: str) -> dict[str, object]:
    return {
        **public_action_fields(action),
        **action_schema_fields(),
        **action_capability_truth_fields(action),
        **action_payload_owner_fields(action),
    }


def build_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload.update(shared_action_payload_fields(spec.action))
    return payload


__all__ = ["build_action_payload", "shared_action_payload_fields"]
