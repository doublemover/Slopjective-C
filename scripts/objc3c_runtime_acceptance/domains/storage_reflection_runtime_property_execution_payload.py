"""Payload sections captured by the storage/reflection property execution probe."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class PropertyExecutionPayload:
    payload: dict[str, Any]
    widget_entry: Any
    registry_state: Any
    base_count_property: Any
    count_property: Any
    enabled_property: Any
    value_property: Any
    token_property: Any
    base_count_method: Any
    count_method: Any
    enabled_method: Any
    value_method: Any
    set_token_method: Any
    token_method: Any
    set_base_count_dispatch: Any
    base_count_dispatch: Any
    set_count_dispatch: Any
    count_dispatch: Any
    set_enabled_dispatch: Any
    enabled_dispatch: Any
    set_value_dispatch: Any
    value_dispatch: Any
    set_token_dispatch: Any
    token_dispatch: Any


def capture_property_execution_payload(
    payload: dict[str, Any],
) -> PropertyExecutionPayload:
    return PropertyExecutionPayload(
        payload=payload,
        widget_entry=payload.get("widget_entry", {}),
        registry_state=payload.get("registry_state", {}),
        base_count_property=payload.get("base_count_property", {}),
        count_property=payload.get("count_property", {}),
        enabled_property=payload.get("enabled_property", {}),
        value_property=payload.get("value_property", {}),
        token_property=payload.get("token_property", {}),
        base_count_method=payload.get("base_count_method", {}),
        count_method=payload.get("count_method", {}),
        enabled_method=payload.get("enabled_method", {}),
        value_method=payload.get("value_method", {}),
        set_token_method=payload.get("set_token_method", {}),
        token_method=payload.get("token_method", {}),
        set_base_count_dispatch=payload.get("set_base_count_dispatch", {}),
        base_count_dispatch=payload.get("base_count_dispatch", {}),
        set_count_dispatch=payload.get("set_count_dispatch", {}),
        count_dispatch=payload.get("count_dispatch", {}),
        set_enabled_dispatch=payload.get("set_enabled_dispatch", {}),
        enabled_dispatch=payload.get("enabled_dispatch", {}),
        set_value_dispatch=payload.get("set_value_dispatch", {}),
        value_dispatch=payload.get("value_dispatch", {}),
        set_token_dispatch=payload.get("set_token_dispatch", {}),
        token_dispatch=payload.get("token_dispatch", {}),
    )


__all__ = [
    "PropertyExecutionPayload",
    "capture_property_execution_payload",
]
