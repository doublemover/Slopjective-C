"""Error-handling diagnostic acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_diagnostic_bridge_filter_case import (
    check_bridging_filter_unwind_compatibility_diagnostics_case,
)


_EXPORTED_CASE_NAMES = [
    "check_bridging_filter_unwind_compatibility_diagnostics_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
