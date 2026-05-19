"""Interop packaging lowering acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_lowering_bridge_emission import (
    check_mixed_image_package_lowering_bridge_emission_case,
)

_EXPORTED_CASE_NAMES = [
    "check_mixed_image_package_lowering_bridge_emission_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
