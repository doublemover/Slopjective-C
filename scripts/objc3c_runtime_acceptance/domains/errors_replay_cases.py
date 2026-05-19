"""Error-handling replay acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_replay_cross_module_case import (
    check_cross_module_error_metadata_replay_preservation_case,
)


_EXPORTED_CASE_NAMES = [
    "check_cross_module_error_metadata_replay_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
