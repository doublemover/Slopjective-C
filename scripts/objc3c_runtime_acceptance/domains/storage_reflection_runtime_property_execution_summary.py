"""Summary helpers for the storage/reflection property execution case."""

from __future__ import annotations

from typing import Any

from .storage_reflection_runtime_property_execution_payload import (
    PropertyExecutionPayload,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


PROPERTY_EXECUTION_CASE_ID = "property-execution"


def build_property_execution_summary(
    facts: PropertyExecutionPayload,
) -> dict[str, Any]:
    return storage_reflection_case_summary(
        PROPERTY_EXECUTION_CASE_ID,
        {
            "base_count_value": facts.payload.get("base_count_value"),
            "count_value": facts.payload.get("count_value"),
            "enabled_value": facts.payload.get("enabled_value"),
            "value_result": facts.payload.get("value_result"),
            "set_token_result": facts.payload.get("set_token_result"),
            "runtime_property_accessor_count": facts.widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": facts.registry_state.get("slot_backed_property_count"),
            "base_count_inherited": facts.base_count_property.get("inherited"),
            "count_dispatch_kind": facts.count_dispatch.get("last_implementation_kind"),
            "value_dispatch_kind": facts.value_dispatch.get("last_implementation_kind"),
            "set_token_dispatch_kind": facts.set_token_dispatch.get("last_implementation_kind"),
            "token_dispatch_kind": facts.token_dispatch.get("last_implementation_kind"),
        },
    )


__all__ = ["PROPERTY_EXECUTION_CASE_ID", "build_property_execution_summary"]
