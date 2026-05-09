"""Error-handling source acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_source_catch_filter_case import (
    check_catch_filter_finalization_source_case,
)
from objc3c_runtime_acceptance.domains.errors_source_execution_case import (
    check_error_execution_cleanup_source_case,
)


_EXPORTED_CASE_NAMES = [
    "check_error_execution_cleanup_source_case",
    "check_catch_filter_finalization_source_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
