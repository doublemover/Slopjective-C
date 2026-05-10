"""Storage/reflection layout lowering contract predicates."""

from __future__ import annotations

from typing import Any

from .data import SurfaceValueExpectation


def surface_is_mapping(surface: Any) -> bool:
    return isinstance(surface, dict)


def surface_field_matches(
    surface: dict[str, Any],
    expectation: SurfaceValueExpectation,
) -> bool:
    actual = surface.get(expectation.field)
    if expectation.identity:
        return actual is expectation.expected
    return actual == expectation.expected
