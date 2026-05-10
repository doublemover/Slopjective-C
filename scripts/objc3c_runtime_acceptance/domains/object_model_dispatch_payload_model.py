"""Typed payload view for canonical object dispatch assertions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class CanonicalDispatchPayload:
    payload: dict[str, Any]
    worker_query: dict[str, Any]
    tracer_query: dict[str, Any]
    graph_state: dict[str, Any]
    widget_entry: dict[str, Any]
    method_state: dict[str, Any]
    inherited_state: dict[str, Any]
    traced_state: dict[str, Any]
    class_state: dict[str, Any]
    ignored_state: dict[str, Any]
    ignored_cached_state: dict[str, Any]
    selector_handles: dict[str, Any]
    selector_table_state: dict[str, Any]
    traced_selector_entry: dict[str, Any]
    inherited_selector_entry: dict[str, Any]
    class_selector_entry: dict[str, Any]
    ignored_selector_entry: dict[str, Any]
    traced_entry: dict[str, Any]
    inherited_entry: dict[str, Any]
    class_entry: dict[str, Any]
    ignored_entry: dict[str, Any]
    alloc_entry: dict[str, Any]
    init_entry: dict[str, Any]
    new_entry: dict[str, Any]


def capture_canonical_dispatch_payload(
    payload: dict[str, Any],
) -> CanonicalDispatchPayload:
    return CanonicalDispatchPayload(
        payload=payload,
        worker_query=payload.get("worker_query", {}),
        tracer_query=payload.get("tracer_query", {}),
        graph_state=payload.get("graph_state", {}),
        widget_entry=payload.get("widget_entry", {}),
        method_state=payload.get("method_state", {}),
        inherited_state=payload.get("inherited_state", {}),
        traced_state=payload.get("traced_state", {}),
        class_state=payload.get("class_state", {}),
        ignored_state=payload.get("ignored_state", {}),
        ignored_cached_state=payload.get("ignored_cached_state", {}),
        selector_handles=payload.get("selector_handles", {}),
        selector_table_state=payload.get("selector_table_state", {}),
        traced_selector_entry=payload.get("traced_selector_entry", {}),
        inherited_selector_entry=payload.get("inherited_selector_entry", {}),
        class_selector_entry=payload.get("class_selector_entry", {}),
        ignored_selector_entry=payload.get("ignored_selector_entry", {}),
        traced_entry=payload.get("traced_entry", {}),
        inherited_entry=payload.get("inherited_entry", {}),
        class_entry=payload.get("class_entry", {}),
        ignored_entry=payload.get("ignored_entry", {}),
        alloc_entry=payload.get("alloc_entry", {}),
        init_entry=payload.get("init_entry", {}),
        new_entry=payload.get("new_entry", {}),
    )


__all__ = [
    "CanonicalDispatchPayload",
    "capture_canonical_dispatch_payload",
]
