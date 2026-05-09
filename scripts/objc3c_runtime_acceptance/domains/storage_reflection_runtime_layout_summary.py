"""Summary helpers for storage/reflection layout runtime cases."""

from __future__ import annotations

from typing import Any

from .storage_reflection_runtime_layout_payload import (
    InstanceAllocationLayoutPayload,
    PropertyLayoutPayload,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


PROPERTY_LAYOUT_CASE_ID = "property-layout"
INSTANCE_ALLOCATION_LAYOUT_CASE_ID = "instance-allocation-layout-runtime"


def build_property_layout_summary(facts: PropertyLayoutPayload) -> dict[str, Any]:
    return storage_reflection_case_summary(
        PROPERTY_LAYOUT_CASE_ID,
        {
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": facts.first_alloc,
            "second_alloc": facts.second_alloc,
            "count_value_first": facts.payload["count_value_first"],
            "count_value_second": facts.payload["count_value_second"],
            "selector_table_entry_count": facts.selector_state.get("selector_table_entry_count"),
        },
    )


def build_instance_allocation_layout_summary(
    facts: InstanceAllocationLayoutPayload,
) -> dict[str, Any]:
    return storage_reflection_case_summary(
        INSTANCE_ALLOCATION_LAYOUT_CASE_ID,
        {
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": facts.first_alloc,
            "second_alloc": facts.second_alloc,
            "live_instance_count": facts.graph_state.get("live_instance_count"),
            "instance_size_bytes": facts.graph_state.get("last_allocated_instance_size_bytes"),
            "widget_property_accessor_count": facts.widget_entry.get("runtime_property_accessor_count"),
            "count_first_after_second": facts.payload["count_value_first_after_second"],
            "count_second_after": facts.payload["count_value_second_after"],
        },
    )


__all__ = [
    "build_instance_allocation_layout_summary",
    "build_property_layout_summary",
    "INSTANCE_ALLOCATION_LAYOUT_CASE_ID",
    "PROPERTY_LAYOUT_CASE_ID",
]
