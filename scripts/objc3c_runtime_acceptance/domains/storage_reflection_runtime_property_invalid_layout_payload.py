"""Payload capture for malformed property/ivar layout runtime acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class PropertyInvalidLayoutPayload:
    payload: dict[str, Any]
    image_walk: dict[str, Any]
    class_entry: dict[str, Any]
    property_entry: dict[str, Any]
    registry_state: dict[str, Any]

    @property
    def registration_status(self) -> Any:
        return self.payload.get("registration_status")

    @property
    def strict_published_layout(self) -> Any:
        return self.payload.get("strict_published_layout")


def capture_property_invalid_layout_payload(
    payload: dict[str, Any],
) -> PropertyInvalidLayoutPayload:
    return PropertyInvalidLayoutPayload(
        payload=payload,
        image_walk=payload.get("image_walk", {}),
        class_entry=payload.get("class_entry", {}),
        property_entry=payload.get("property_entry", {}),
        registry_state=payload.get("registry_state", {}),
    )


__all__ = [
    "PropertyInvalidLayoutPayload",
    "capture_property_invalid_layout_payload",
]
