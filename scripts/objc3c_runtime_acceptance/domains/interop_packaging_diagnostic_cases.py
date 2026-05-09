"""Interop packaging diagnostic acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_diagnostic_import_version import (
    check_import_version_feature_claim_diagnostics_case,
)

_EXPORTED_CASE_NAMES = [
    "check_import_version_feature_claim_diagnostics_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
