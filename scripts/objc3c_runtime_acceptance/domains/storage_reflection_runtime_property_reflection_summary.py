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
            "value_property_attribute_count": facts.value_property.get("attribute_count"),
            "value_property_is_strong": facts.value_property.get("is_strong"),
            "value_property_has_custom_accessors": (
                facts.value_property.get("has_custom_getter") == 1
                and facts.value_property.get("has_custom_setter") == 1
            ),
            "count_property_runtime_setter": facts.count_property.get("has_runtime_setter"),
            "count_property_is_assign": facts.count_property.get("is_assign"),
            "token_property_is_readonly": facts.token_property.get("is_readonly"),
        },
    )


__all__ = ["PROPERTY_REFLECTION_CASE_ID", "build_property_reflection_summary"]
