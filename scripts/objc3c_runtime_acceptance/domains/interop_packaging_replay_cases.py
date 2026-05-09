"""Interop packaging replay-preservation acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_replay_import_preservation import (
    check_cross_language_replay_import_surface_preservation_case,
)

_EXPORTED_CASE_NAMES = [
    "check_cross_language_replay_import_surface_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
