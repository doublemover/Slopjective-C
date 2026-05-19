"""Summary for malformed property/ivar layout runtime acceptance."""

from __future__ import annotations

from .storage_reflection_owner_contracts import storage_reflection_case_summary
from .storage_reflection_runtime_property_invalid_layout_payload import (
    PropertyInvalidLayoutPayload,
)


PROPERTY_INVALID_LAYOUT_CASE_ID = "property-ivar-invalid-layout-runtime"


def build_property_invalid_layout_summary(
    facts: PropertyInvalidLayoutPayload,
) -> dict[str, object]:
    return storage_reflection_case_summary(
        PROPERTY_INVALID_LAYOUT_CASE_ID,
        {
            "fail_closed_behavior": (
                "RuntimePropertyIvarDescriptorHasStrictPublishedLayout rejects "
                "the disagreed published layout; registration remains OK, the "
                "class realizes, and the malformed property is neither "
                "reflectable nor slot-backed"
            ),
            "registration_status": facts.registration_status,
            "strict_published_layout": facts.strict_published_layout,
            "class_realized": facts.class_entry.get("found") == 1,
            "runtime_property_accessor_count": facts.class_entry.get(
                "runtime_property_accessor_count"
            ),
            "property_reflection_found": facts.property_entry.get("found"),
            "property_lookup_found": facts.registry_state.get("last_query_found"),
            "reflectable_property_count": facts.registry_state.get(
                "reflectable_property_count"
            ),
            "slot_backed_property_count": facts.registry_state.get(
                "slot_backed_property_count"
            ),
        },
    )


__all__ = [
    "PROPERTY_INVALID_LAYOUT_CASE_ID",
    "build_property_invalid_layout_summary",
]
