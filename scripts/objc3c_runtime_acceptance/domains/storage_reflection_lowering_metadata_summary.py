"""Summary shaping for storage/reflection lowering metadata cases."""

from __future__ import annotations

from typing import Any


def build_accessor_storage_lowering_metadata_summary(
    synthesized_lowering_surface: dict[str, Any],
    arc_lowering_surface: dict[str, Any],
) -> dict[str, Any]:
    return {
        "synthesized_accessor_owner_entries": synthesized_lowering_surface.get(
            "synthesized_accessor_owner_entries"
        ),
        "synthesized_getter_entries": synthesized_lowering_surface.get(
            "synthesized_getter_entries"
        ),
        "synthesized_setter_entries": synthesized_lowering_surface.get(
            "synthesized_setter_entries"
        ),
        "strong_exchange_entries": synthesized_lowering_surface.get(
            "current_property_exchange_entries"
        ),
        "weak_store_entries": arc_lowering_surface.get(
            "weak_current_property_store_entries"
        ),
    }


__all__ = ["build_accessor_storage_lowering_metadata_summary"]
