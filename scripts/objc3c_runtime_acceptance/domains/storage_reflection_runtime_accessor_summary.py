"""Summary helpers for synthesized accessor runtime acceptance."""

from __future__ import annotations

from typing import Any

from .storage_reflection_runtime_accessor_payload import (
    SynthesizedAccessorRuntimePayload,
)


def build_synthesized_accessor_runtime_summary(
    facts: SynthesizedAccessorRuntimePayload,
) -> dict[str, Any]:
    return {
        "widget_instance": facts.payload["widget_instance"],
        "count_value": facts.payload["count_value"],
        "enabled_value": facts.payload["enabled_value"],
        "value_result": facts.payload["value_result"],
        "selector_table_entry_count": facts.selector_state.get("selector_table_entry_count"),
    }


__all__ = ["build_synthesized_accessor_runtime_summary"]
