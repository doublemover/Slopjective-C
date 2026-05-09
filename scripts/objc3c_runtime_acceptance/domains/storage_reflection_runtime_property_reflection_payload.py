"""Payload capture for storage/reflection property metadata runtime acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class PropertyReflectionPayload:
    payload: dict[str, Any]
    widget_entry: Any
    token_property: Any
    value_property: Any
    count_property: Any
    missing_property: Any
    missing_class_property: Any
    registry_after_count: Any


def capture_property_reflection_payload(
    payload: dict[str, Any],
) -> PropertyReflectionPayload:
    return PropertyReflectionPayload(
        payload=payload,
        widget_entry=payload.get("widget_entry", {}),
        token_property=payload.get("token_property", {}),
        value_property=payload.get("value_property", {}),
        count_property=payload.get("count_property", {}),
        missing_property=payload.get("missing_property", {}),
        missing_class_property=payload.get("missing_class_property", {}),
        registry_after_count=payload.get("registry_state_after_count", {}),
    )


__all__ = [
    "PropertyReflectionPayload",
    "capture_property_reflection_payload",
]
