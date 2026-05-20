"""Object Model runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.object_model_cases import (
    check_canonical_dispatch_case,
    check_canonical_sample_set_case,
    check_dispatch_lookup_runtime_probe_case,
    check_live_dispatch_fast_path_case,
    check_metaclass_graph_root_class_case,
    check_method_cache_slow_path_probe_case,
    check_realization_lookup_reflection_runtime_case,
    check_runtime_object_foundation_protocol_category_case,
    check_runtime_library_case,
    check_typed_dispatch_abi_probe_case,
)
from objc3c_runtime_acceptance.domains.object_model_surface_cases import (
    build_runtime_category_attachment_merged_dispatch_surface,
    build_runtime_class_metaclass_protocol_realization_surface,
    build_runtime_cross_module_realized_metadata_replay_preservation_surface,
    build_runtime_dispatch_table_reflection_record_lowering_surface,
    build_runtime_object_model_abi_query_surface,
    build_runtime_object_model_realization_source_surface,
    build_runtime_realization_lookup_reflection_implementation_surface,
    build_runtime_realization_lookup_semantics_surface,
    build_runtime_realization_lowering_reflection_artifact_surface,
    build_runtime_reflection_query_surface,
    build_runtime_reflection_visibility_coherence_diagnostics_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_object_model_realization_source_surface",
    "build_runtime_realization_lowering_reflection_artifact_surface",
    "build_runtime_dispatch_table_reflection_record_lowering_surface",
    "build_runtime_cross_module_realized_metadata_replay_preservation_surface",
    "build_runtime_object_model_abi_query_surface",
    "build_runtime_realization_lookup_reflection_implementation_surface",
    "build_runtime_reflection_query_surface",
    "build_runtime_realization_lookup_semantics_surface",
    "build_runtime_class_metaclass_protocol_realization_surface",
    "build_runtime_category_attachment_merged_dispatch_surface",
    "build_runtime_reflection_visibility_coherence_diagnostics_surface",
    "check_runtime_library_case",
    "check_dispatch_lookup_runtime_probe_case",
    "check_method_cache_slow_path_probe_case",
    "check_typed_dispatch_abi_probe_case",
    "check_canonical_dispatch_case",
    "check_metaclass_graph_root_class_case",
    "check_canonical_sample_set_case",
    "check_realization_lookup_reflection_runtime_case",
    "check_runtime_object_foundation_protocol_category_case",
    "check_live_dispatch_fast_path_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
