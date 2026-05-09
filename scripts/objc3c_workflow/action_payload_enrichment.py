"""Single-action payload enrichment owner."""

from __future__ import annotations

from .action_payload_builder import build_action_payload
from .action_spec import ActionSpec


def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    return build_action_payload(spec)


__all__ = ["enrich_action_payload"]
