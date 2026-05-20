"""Payload models for Object Model metaclass sample runtime probes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class MetaclassGraphProbeFacts:
    graph_state: dict[str, Any]
    root_entry: dict[str, Any]
    widget_entry: dict[str, Any]
    root_class_state: dict[str, Any]
    widget_class_state: dict[str, Any]
    widget_known_class_state: dict[str, Any]
    widget_inherited_state: dict[str, Any]
    widget_own_state: dict[str, Any]
    root_shared_entry: dict[str, Any]
    widget_shared_entry: dict[str, Any]
    widget_inherited_entry: dict[str, Any]
    widget_own_entry: dict[str, Any]
    widget_super_entry: dict[str, Any]
    fail_closed_diagnostics: dict[str, Any]


@dataclass(frozen=True)
class CanonicalSampleProbeFacts:
    payload: dict[str, Any]
    widget_entry: dict[str, Any]
    worker_query: dict[str, Any]
    tracer_query: dict[str, Any]
    count_property: dict[str, Any]
    value_property: dict[str, Any]
    token_property: dict[str, Any]


def capture_canonical_sample_probe_facts(
    payload: dict[str, Any],
) -> CanonicalSampleProbeFacts:
    return CanonicalSampleProbeFacts(
        payload=payload,
        widget_entry=payload.get("widget_entry", {}),
        worker_query=payload.get("worker_query", {}),
        tracer_query=payload.get("tracer_query", {}),
        count_property=payload.get("count_property", {}),
        value_property=payload.get("value_property", {}),
        token_property=payload.get("token_property", {}),
    )


__all__ = [
    "CanonicalSampleProbeFacts",
    "MetaclassGraphProbeFacts",
    "capture_canonical_sample_probe_facts",
]
