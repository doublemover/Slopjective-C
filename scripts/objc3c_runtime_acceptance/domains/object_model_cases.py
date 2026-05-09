"""Object Model linked-runtime acceptance case surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.object_model_dispatch_cases import (
    check_canonical_dispatch_case,
)
from objc3c_runtime_acceptance.domains.object_model_fast_path_cases import (
    check_live_dispatch_fast_path_case,
)
from objc3c_runtime_acceptance.domains.object_model_lookup_cases import (
    check_realization_lookup_reflection_runtime_case,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_cases import (
    check_canonical_sample_set_case,
    check_metaclass_graph_root_class_case,
)
from objc3c_runtime_acceptance.domains.object_model_runtime_library_cases import (
    check_runtime_library_case,
)

_EXPORTED_CASE_NAMES = [
    "check_runtime_library_case",
    "check_canonical_dispatch_case",
    "check_metaclass_graph_root_class_case",
    "check_canonical_sample_set_case",
    "check_realization_lookup_reflection_runtime_case",
    "check_live_dispatch_fast_path_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
