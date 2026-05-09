"""Summary helpers for storage/reflection property metadata runtime acceptance."""

from __future__ import annotations

from typing import Any

from .storage_reflection_runtime_property_reflection_payload import (
    PropertyReflectionPayload,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


PROPERTY_REFLECTION_CASE_ID = "property-reflection"


def build_property_reflection_summary(
    facts: PropertyReflectionPayload,
) -> dict[str, Any]:
    return storage_reflection_case_summary(
        PROPERTY_REFLECTION_CASE_ID,
        {
            "reflectable_property_count": facts.registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": facts.registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": facts.value_property.get("setter_available"),
            "count_property_runtime_setter": facts.count_property.get("has_runtime_setter"),
        },
    )


__all__ = ["PROPERTY_REFLECTION_CASE_ID", "build_property_reflection_summary"]
