"""Synthesized accessor metadata assertions for storage/reflection lowering."""

from __future__ import annotations

from typing import Any, cast

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_property_assertions import (
    expect_property_lowering,
)
from objc3c_runtime_acceptance.expectation_matching import expect

from .catalog import LOWERING_REQUIREMENTS_MESSAGE
from .catalog import LOWERING_SURFACE_ARTIFACT_MESSAGE
from .catalog import LOWERING_SURFACE_METADATA_EXPECTATIONS
from .catalog import LOWERING_SURFACE_TYPE_MESSAGE
from .catalog import LLVM_COUNT_FRAGMENT_MESSAGE
from .catalog import SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS
from .catalog import SYNTHESIZED_ACCESSOR_SYMBOL_EXPECTATIONS
from .catalog import SYNTHESIZED_PROPERTY_LOWERING_ROWS
from .payloads import property_lowering_row_payload
from .payloads import synthesized_lowering_surface_payload
from .predicates import lowering_requirements_match
from .predicates import lowering_surface_artifacts_match
from .predicates import lowering_surface_field_matches
from .predicates import synthesized_ll_count_fragments_match
from .predicates import synthesized_lowering_surface_is_mapping


def assert_synthesized_accessor_lowering_metadata_surface(
    synthesized_manifest: dict[str, Any],
    synthesized_ll: str,
) -> dict[str, Any]:
    synthesized_lowering_surface = synthesized_lowering_surface_payload(
        synthesized_manifest
    )
    expect(
        synthesized_lowering_surface_is_mapping(synthesized_lowering_surface),
        LOWERING_SURFACE_TYPE_MESSAGE,
    )
    synthesized_lowering_surface = cast(
        dict[str, Any],
        synthesized_lowering_surface,
    )
    _assert_synthesized_lowering_surface_metadata(synthesized_lowering_surface)
    _assert_synthesized_accessor_counts(synthesized_lowering_surface)
    _assert_synthesized_accessor_symbols(synthesized_lowering_surface)
    expect(
        synthesized_ll_count_fragments_match(synthesized_ll),
        LLVM_COUNT_FRAGMENT_MESSAGE,
    )
    _assert_synthesized_property_lowering_rows(synthesized_manifest)
    return synthesized_lowering_surface


def _assert_synthesized_lowering_surface_metadata(surface: dict[str, Any]) -> None:
    for expectation in LOWERING_SURFACE_METADATA_EXPECTATIONS[:3]:
        expect(
            lowering_surface_field_matches(surface, expectation),
            expectation.message,
        )
    expect(
        lowering_surface_artifacts_match(surface),
        LOWERING_SURFACE_ARTIFACT_MESSAGE,
    )
    for expectation in LOWERING_SURFACE_METADATA_EXPECTATIONS[3:]:
        expect(
            lowering_surface_field_matches(surface, expectation),
            expectation.message,
        )
    expect(
        lowering_requirements_match(surface),
        LOWERING_REQUIREMENTS_MESSAGE,
    )


def _assert_synthesized_accessor_counts(surface: dict[str, Any]) -> None:
    for expectation in SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS:
        expect(
            lowering_surface_field_matches(surface, expectation),
            expectation.message,
        )


def _assert_synthesized_accessor_symbols(surface: dict[str, Any]) -> None:
    for expectation in SYNTHESIZED_ACCESSOR_SYMBOL_EXPECTATIONS:
        expect(
            lowering_surface_field_matches(surface, expectation),
            expectation.message,
        )


def _assert_synthesized_property_lowering_rows(
    synthesized_manifest: dict[str, Any],
) -> None:
    for row in SYNTHESIZED_PROPERTY_LOWERING_ROWS:
        expect_property_lowering(
            synthesized_manifest,
            *property_lowering_row_payload(row),
        )


__all__ = ["assert_synthesized_accessor_lowering_metadata_surface"]
