"""Registration and startup-ordering runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.registration_lifecycle_cases import (
    check_installation_lifecycle_case,
)
from objc3c_runtime_acceptance.domains.registration_replay_cases import (
    check_multi_image_registration_reset_replay_case,
)
from objc3c_runtime_acceptance.domains.registration_surfaces import (
    build_runtime_installation_abi_surface,
    build_runtime_loader_lifecycle_surface,
    build_runtime_multi_image_startup_ordering_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_multi_image_startup_ordering_source_surface",
    "build_runtime_installation_abi_surface",
    "build_runtime_loader_lifecycle_surface",
    "check_installation_lifecycle_case",
    "check_multi_image_registration_reset_replay_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
