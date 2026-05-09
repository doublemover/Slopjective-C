"""Object Model runtime acceptance surface case aggregator."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.object_model_surface_class_cases import (
    build_runtime_category_attachment_merged_dispatch_surface,
    build_runtime_class_metaclass_protocol_realization_surface,
    build_runtime_reflection_visibility_coherence_diagnostics_surface,
)
from objc3c_runtime_acceptance.domains.object_model_surface_query_cases import (
    build_runtime_object_model_abi_query_surface,
    build_runtime_realization_lookup_reflection_implementation_surface,
    build_runtime_realization_lookup_semantics_surface,
    build_runtime_reflection_query_surface,
)
from objc3c_runtime_acceptance.domains.object_model_surface_realization_cases import (
    build_runtime_cross_module_realized_metadata_replay_preservation_surface,
    build_runtime_dispatch_table_reflection_record_lowering_surface,
    build_runtime_object_model_realization_source_surface,
    build_runtime_realization_lowering_reflection_artifact_surface,
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
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
