"""Captured payload sections for storage/reflection layout runtime probes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .storage_reflection_runtime_layout_artifacts import (
    InstanceAllocationLayoutArtifacts,
    PropertyLayoutArtifacts,
)


@dataclass(frozen=True)
class PropertyLayoutPayload:
    payload: dict[str, Any]
    ll_text: str
    registration_state: Any
    selector_state: Any
    count_entry: Any
    set_count_entry: Any
    first_alloc: int
    second_alloc: int


@dataclass(frozen=True)
class InstanceAllocationLayoutPayload:
    payload: dict[str, Any]
    ll_text: str
    manifest: dict[str, Any]
    registration_state: Any
    selector_state: Any
    graph_state: Any
    widget_entry: Any
    count_entry: Any
    set_count_entry: Any
    first_alloc: int
    second_alloc: int


def capture_property_layout_payload(
    artifacts: PropertyLayoutArtifacts,
) -> PropertyLayoutPayload:
    payload = artifacts.payload
    return PropertyLayoutPayload(
        payload=payload,
        ll_text=artifacts.ll_text,
        registration_state=payload.get("registration_state", {}),
        selector_state=payload.get("selector_table_state", {}),
        count_entry=payload.get("count_entry", {}),
        set_count_entry=payload.get("set_count_entry", {}),
        first_alloc=int(payload.get("first_alloc", 0)),
        second_alloc=int(payload.get("second_alloc", 0)),
    )


def capture_instance_allocation_layout_payload(
    artifacts: InstanceAllocationLayoutArtifacts,
) -> InstanceAllocationLayoutPayload:
    payload = artifacts.payload
    return InstanceAllocationLayoutPayload(
        payload=payload,
        ll_text=artifacts.ll_text,
        manifest=artifacts.manifest,
        registration_state=payload.get("registration_state", {}),
        selector_state=payload.get("selector_table_state", {}),
        graph_state=payload.get("graph_state", {}),
        widget_entry=payload.get("widget_entry", {}),
        count_entry=payload.get("count_entry", {}),
        set_count_entry=payload.get("set_count_entry", {}),
        first_alloc=int(payload.get("first_alloc", 0)),
        second_alloc=int(payload.get("second_alloc", 0)),
    )


__all__ = [
    "InstanceAllocationLayoutPayload",
    "PropertyLayoutPayload",
    "capture_instance_allocation_layout_payload",
    "capture_property_layout_payload",
]
