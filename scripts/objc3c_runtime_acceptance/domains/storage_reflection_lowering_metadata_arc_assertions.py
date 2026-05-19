"""ARC accessor metadata assertions for storage/reflection lowering."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_property_assertions import (
    expect_property_lowering,
)
from objc3c_runtime_acceptance.expectation_matching import expect


def assert_arc_accessor_lowering_metadata_surface(
    arc_manifest: dict[str, Any],
    arc_ll: str,
) -> dict[str, Any]:
    arc_lowering_surface = arc_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    _assert_arc_accessor_counts(arc_lowering_surface)
    expect(
        "exchange_current_property_calls=1" in arc_ll
        and "weak_load_current_property_calls=1" in arc_ll
        and "weak_store_current_property_calls=1" in arc_ll,
        "expected ARC property interaction fixture LLVM IR to agree with the published helper-lowering counts",
    )
    _assert_arc_property_lowering_rows(arc_manifest)
    return arc_lowering_surface


def _assert_arc_accessor_counts(surface: dict[str, Any]) -> None:
    expect(
        surface.get("synthesized_accessor_owner_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered property owners",
    )
    expect(
        surface.get("synthesized_getter_entries") == 2
        and surface.get("synthesized_setter_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered getters and setters",
    )
    expect(
        surface.get("current_property_read_entries") == 1,
        "expected ARC property interaction fixture to publish one plain/strong getter read entry",
    )
    expect(
        surface.get("current_property_write_entries") == 0,
        "expected ARC property interaction fixture to publish zero plain write entries",
    )
    expect(
        surface.get("current_property_exchange_entries") == 1,
        "expected ARC property interaction fixture to publish one strong exchange entry",
    )
    expect(
        surface.get("weak_current_property_load_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-load entry",
    )
    expect(
        surface.get("weak_current_property_store_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-store entry",
    )


def _assert_arc_property_lowering_rows(arc_manifest: dict[str, Any]) -> None:
    expect_property_lowering(
        arc_manifest,
        "class-interface",
        "ArcBox",
        "currentValue",
        False,
        "",
        "",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "currentValue",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "weakValue",
        True,
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    )


__all__ = ["assert_arc_accessor_lowering_metadata_surface"]
