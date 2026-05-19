"""Storage ownership reflection assertion predicates."""

from __future__ import annotations

from typing import Any

from .data import LlTextExpectation
from .data import StorageOwnershipReflectionFacts
from .data import SurfaceFieldExpectation


def box_reflection_payload_found(facts: StorageOwnershipReflectionFacts) -> bool:
    return facts.box_entry.get("found") == 1


def box_runtime_accessor_count_satisfies(
    facts: StorageOwnershipReflectionFacts,
) -> bool:
    return facts.box_entry.get("runtime_property_accessor_count", 0) >= 5


def box_instance_layout_satisfies(facts: StorageOwnershipReflectionFacts) -> bool:
    return facts.box_entry.get("runtime_instance_size_bytes", 0) >= 40


def surface_field_matches(
    surface: dict[str, Any],
    expectation: SurfaceFieldExpectation,
) -> bool:
    if isinstance(expectation.expected, bool):
        return surface.get(expectation.field) is expectation.expected
    return surface.get(expectation.field) == expectation.expected


def ll_text_contains_expectation(
    ll_text: str,
    expectation: LlTextExpectation,
) -> bool:
    return expectation.fragment in ll_text


__all__ = [
    "box_instance_layout_satisfies",
    "box_reflection_payload_found",
    "box_runtime_accessor_count_satisfies",
    "ll_text_contains_expectation",
    "surface_field_matches",
]
