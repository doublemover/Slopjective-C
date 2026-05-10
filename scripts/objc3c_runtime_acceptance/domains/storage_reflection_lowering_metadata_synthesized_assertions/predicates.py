"""Synthesized accessor lowering metadata predicates."""

from __future__ import annotations

from typing import Any

from .catalog import SYNTHESIZED_LL_COUNT_FRAGMENTS
from .data import SurfaceValueExpectation


def synthesized_lowering_surface_is_mapping(surface: Any) -> bool:
    return isinstance(surface, dict)


def lowering_surface_field_matches(
    surface: dict[str, Any],
    expectation: SurfaceValueExpectation,
) -> bool:
    return surface.get(expectation.field) == expectation.expected


def lowering_surface_artifacts_match(surface: dict[str, Any]) -> bool:
    return (
        surface.get("object_artifact") == "module.obj"
        and surface.get("backend_artifact") == "module.ll"
    )


def lowering_requirements_match(surface: dict[str, Any]) -> bool:
    return (
        surface.get("requires_coupled_registration_manifest") is True
        and surface.get("requires_real_compile_output") is True
        and surface.get("requires_linked_runtime_probe") is True
    )


def synthesized_ll_count_fragments_match(synthesized_ll: str) -> bool:
    return all(fragment in synthesized_ll for fragment in SYNTHESIZED_LL_COUNT_FRAGMENTS)
