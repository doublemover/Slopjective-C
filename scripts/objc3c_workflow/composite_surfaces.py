"""Child report surface ownership for composite workflow reports."""

from __future__ import annotations

from collections.abc import Sequence

from .actions.validation_timing import load_surface_from_report
from .composite_surface_acceptance_keys import ACCEPTANCE_COMPOSITE_SURFACE_KEYS
from .composite_surface_block_storage_keys import BLOCK_STORAGE_COMPOSITE_SURFACE_KEYS
from .composite_surface_claim_keys import CLAIM_COMPOSITE_SURFACE_KEYS
from .composite_surface_error_keys import ERROR_COMPOSITE_SURFACE_KEYS
from .composite_surface_interop_keys import INTEROP_COMPOSITE_SURFACE_KEYS
from .composite_surface_metaprogramming_keys import (
    METAPROGRAMMING_COMPOSITE_SURFACE_KEYS,
)
from .composite_surface_reflection_keys import REFLECTION_COMPOSITE_SURFACE_KEYS

COMPOSITE_SURFACE_KEYS = (
    *ERROR_COMPOSITE_SURFACE_KEYS,
    *ACCEPTANCE_COMPOSITE_SURFACE_KEYS,
    *BLOCK_STORAGE_COMPOSITE_SURFACE_KEYS,
    *CLAIM_COMPOSITE_SURFACE_KEYS,
    *METAPROGRAMMING_COMPOSITE_SURFACE_KEYS,
    *INTEROP_COMPOSITE_SURFACE_KEYS,
    *REFLECTION_COMPOSITE_SURFACE_KEYS,
)


def attach_child_surfaces(
    payload: dict[str, object],
    steps: Sequence[dict[str, object]],
) -> None:
    for surface_key in COMPOSITE_SURFACE_KEYS:
        surface = load_surface_from_report(steps, surface_key)
        if surface is not None:
            payload[surface_key] = surface


__all__ = ["COMPOSITE_SURFACE_KEYS", "attach_child_surfaces"]
