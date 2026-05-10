"""Synthesized accessor lowering metadata payload shaping."""

from __future__ import annotations

from typing import Any

from .catalog import LOWERING_SURFACE_KEY
from .data import SynthesizedPropertyLoweringRow


def synthesized_lowering_surface_payload(
    synthesized_manifest: dict[str, Any],
) -> Any:
    return synthesized_manifest.get(LOWERING_SURFACE_KEY, {})


def property_lowering_row_payload(
    row: SynthesizedPropertyLoweringRow,
) -> tuple[str, str, str, bool, str, str]:
    return (
        row.owner_kind,
        row.owner_name,
        row.property_name,
        row.synthesized,
        row.getter_helper,
        row.setter_helper,
    )
