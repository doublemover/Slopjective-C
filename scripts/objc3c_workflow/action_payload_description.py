"""Single-action description payload owner for workflow registry requests."""

from __future__ import annotations

from .action_payload_enrichment import enrich_action_payload
from .registry_views import require_action_spec


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(require_action_spec(action))


__all__ = ["describe_action_payload"]
