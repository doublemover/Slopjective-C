"""Runtime acceptance case exports for performance and ad hoc callers."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.block_arc import (
    check_arc_property_helper_case,
)
from objc3c_runtime_acceptance.domains.object_model import (
    check_live_dispatch_fast_path_case,
    check_realization_lookup_reflection_runtime_case,
)
from objc3c_runtime_acceptance.domains.registration import (
    check_installation_lifecycle_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection import (
    check_storage_ownership_reflection_case,
)
from objc3c_runtime_acceptance.domains.stdlib_runtime_cases import (
    check_stdlib_concurrency_runtime_probe_case,
    check_stdlib_core_runtime_probe_case,
)
from objc3c_runtime_acceptance.native_binaries import ensure_native_binaries
from objc3c_runtime_acceptance.native_binaries import find_clangxx

__all__ = [
    "check_arc_property_helper_case",
    "check_installation_lifecycle_case",
    "check_live_dispatch_fast_path_case",
    "check_realization_lookup_reflection_runtime_case",
    "check_storage_ownership_reflection_case",
    "check_stdlib_concurrency_runtime_probe_case",
    "check_stdlib_core_runtime_probe_case",
    "ensure_native_binaries",
    "find_clangxx",
]
