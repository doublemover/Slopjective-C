"""Concurrency runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.concurrency_actor_cross_module_cases import (
    check_cross_module_concurrency_actor_artifact_preservation_case,
)
from objc3c_runtime_acceptance.domains.concurrency_live_runtime_cases import (
    check_live_unified_concurrency_runtime_implementation_case,
)
from objc3c_runtime_acceptance.domains.concurrency_lowering_metadata_cases import (
    check_unified_concurrency_lowering_metadata_surface_case,
)
from objc3c_runtime_acceptance.domains.concurrency_normalization_cases import (
    check_async_task_actor_normalization_completion_case,
)
from objc3c_runtime_acceptance.domains.concurrency_runtime_abi_cases import (
    check_unified_concurrency_runtime_abi_case,
)
from objc3c_runtime_acceptance.domains.concurrency_source_manifest_cases import (
    check_unified_concurrency_runtime_architecture_case,
)
from objc3c_runtime_acceptance.domains.concurrency_surfaces import (
    build_runtime_async_task_actor_normalization_completion_surface,
    build_runtime_unified_concurrency_lowering_metadata_surface,
    build_runtime_unified_concurrency_runtime_abi_surface,
    build_runtime_unified_concurrency_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_unified_concurrency_source_surface",
    "build_runtime_async_task_actor_normalization_completion_surface",
    "build_runtime_unified_concurrency_lowering_metadata_surface",
    "build_runtime_unified_concurrency_runtime_abi_surface",
    "check_unified_concurrency_runtime_architecture_case",
    "check_async_task_actor_normalization_completion_case",
    "check_unified_concurrency_lowering_metadata_surface_case",
    "check_unified_concurrency_runtime_abi_case",
    "check_live_unified_concurrency_runtime_implementation_case",
    "check_cross_module_concurrency_actor_artifact_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
