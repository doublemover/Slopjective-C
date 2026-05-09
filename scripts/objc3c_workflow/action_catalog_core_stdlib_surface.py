"""Core stdlib surface action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_STDLIB_SURFACE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-stdlib-surface": ActionSpec("check-stdlib-surface", "check the checked-in stdlib boundary contracts, canonical module inventory, package-name mapping, and lowering/import artifact contract", "python:scripts/check_stdlib_surface.py", validation_tier="repo", guarantee_owner="stdlib roots, canonical module inventory, package-name mapping, and lowering/import artifact contract stay checked in and coherent"),
}


__all__ = ["CORE_STDLIB_SURFACE_ACTION_SPECS"]
