"""Storage/reflection layout lowering contract assertions."""

from __future__ import annotations

from typing import cast

from objc3c_runtime_acceptance.expectation_matching import expect

from .catalog import ACCESSOR_LAYOUT_COUNT_EXPECTATIONS
from .catalog import ACCESSOR_LAYOUT_FIELD_EXPECTATIONS
from .catalog import ACCESSOR_LAYOUT_SURFACE_TYPE_MESSAGE
from .catalog import IVAR_LAYOUT_COUNT_EXPECTATIONS
from .catalog import IVAR_LAYOUT_FIELD_EXPECTATIONS
from .catalog import IVAR_LAYOUT_SURFACE_TYPE_MESSAGE
from .catalog import PROPERTY_SOURCE_LINK_EXPECTATIONS
from .catalog import SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS
from .catalog import SYNTHESIZED_ACCESSOR_FIELD_EXPECTATIONS
from .catalog import SYNTHESIZED_ACCESSOR_SURFACE_TYPE_MESSAGE
from .data import ManifestSurface
from .data import SurfaceValueExpectation
from .payloads import accessor_layout_surface_payload
from .payloads import ivar_layout_surface_payload
from .payloads import property_source_surface_payload
from .payloads import synthesized_accessor_surface_payload
from .predicates import surface_field_matches
from .predicates import surface_is_mapping


def assert_property_source_surface_links(manifest: ManifestSurface) -> None:
    property_source_surface = cast(
        ManifestSurface,
        property_source_surface_payload(manifest),
    )
    _expect_surface_fields(
        property_source_surface,
        PROPERTY_SOURCE_LINK_EXPECTATIONS,
    )


def assert_accessor_layout_surface(manifest: ManifestSurface) -> ManifestSurface:
    accessor_layout_surface = accessor_layout_surface_payload(manifest)
    expect(
        surface_is_mapping(accessor_layout_surface),
        ACCESSOR_LAYOUT_SURFACE_TYPE_MESSAGE,
    )
    accessor_layout_surface = cast(ManifestSurface, accessor_layout_surface)
    _expect_surface_fields(
        accessor_layout_surface,
        ACCESSOR_LAYOUT_FIELD_EXPECTATIONS,
    )
    _assert_accessor_layout_counts(accessor_layout_surface)
    return accessor_layout_surface


def assert_ivar_layout_surface(manifest: ManifestSurface) -> ManifestSurface:
    ivar_layout_surface = ivar_layout_surface_payload(manifest)
    expect(
        surface_is_mapping(ivar_layout_surface),
        IVAR_LAYOUT_SURFACE_TYPE_MESSAGE,
    )
    ivar_layout_surface = cast(ManifestSurface, ivar_layout_surface)
    _expect_surface_fields(
        ivar_layout_surface,
        IVAR_LAYOUT_FIELD_EXPECTATIONS,
    )
    _assert_ivar_layout_counts(ivar_layout_surface)
    return ivar_layout_surface


def assert_executable_synthesized_accessor_surface(
    manifest: ManifestSurface,
) -> ManifestSurface:
    synthesized_accessor_surface = synthesized_accessor_surface_payload(manifest)
    expect(
        surface_is_mapping(synthesized_accessor_surface),
        SYNTHESIZED_ACCESSOR_SURFACE_TYPE_MESSAGE,
    )
    synthesized_accessor_surface = cast(ManifestSurface, synthesized_accessor_surface)
    _expect_surface_fields(
        synthesized_accessor_surface,
        SYNTHESIZED_ACCESSOR_FIELD_EXPECTATIONS,
    )
    _assert_synthesized_accessor_counts(synthesized_accessor_surface)
    return synthesized_accessor_surface


def _expect_surface_fields(
    surface: ManifestSurface,
    expectations: tuple[SurfaceValueExpectation, ...],
) -> None:
    for expectation in expectations:
        expect(
            surface_field_matches(surface, expectation),
            expectation.message,
        )


def _assert_accessor_layout_counts(accessor_layout_surface: ManifestSurface) -> None:
    _expect_surface_fields(
        accessor_layout_surface,
        ACCESSOR_LAYOUT_COUNT_EXPECTATIONS,
    )


def _assert_ivar_layout_counts(ivar_layout_surface: ManifestSurface) -> None:
    _expect_surface_fields(
        ivar_layout_surface,
        IVAR_LAYOUT_COUNT_EXPECTATIONS,
    )


def _assert_synthesized_accessor_counts(surface: ManifestSurface) -> None:
    _expect_surface_fields(
        surface,
        SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS,
    )


__all__ = [
    "assert_accessor_layout_surface",
    "assert_executable_synthesized_accessor_surface",
    "assert_ivar_layout_surface",
    "assert_property_source_surface_links",
]
