"""Payload capture for synthesized accessor runtime acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class SynthesizedAccessorRuntimePayload:
    payload: dict[str, Any]
    registration_state: Any
    selector_state: Any
    count_entry: Any
    set_count_entry: Any
    enabled_entry: Any
    set_enabled_entry: Any
    value_entry: Any
    set_value_entry: Any


def capture_synthesized_accessor_runtime_payload(
    payload: dict[str, Any],
) -> SynthesizedAccessorRuntimePayload:
    return SynthesizedAccessorRuntimePayload(
        payload=payload,
        registration_state=payload.get("registration_state", {}),
        selector_state=payload.get("selector_table_state", {}),
        count_entry=payload.get("count_entry", {}),
        set_count_entry=payload.get("set_count_entry", {}),
        enabled_entry=payload.get("enabled_entry", {}),
        set_enabled_entry=payload.get("set_enabled_entry", {}),
        value_entry=payload.get("value_entry", {}),
        set_value_entry=payload.get("set_value_entry", {}),
    )


__all__ = [
    "SynthesizedAccessorRuntimePayload",
    "capture_synthesized_accessor_runtime_payload",
]
