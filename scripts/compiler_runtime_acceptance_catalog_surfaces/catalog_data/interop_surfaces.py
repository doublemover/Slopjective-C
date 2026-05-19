"""Cross-module and package interop acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import SOURCE_CODE_AND_CASE_FIELDS, SOURCE_FIELD_AND_CASE_FIELDS, source_model_case_fields
from ..model import SurfaceRequirement


INTEROP_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_cross_module_package_interop_source_surface",
        RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_textual_binary_interface_parity_source_surface",
        RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_FIELD_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_mixed_image_interop_semantics_surface",
        RUNTIME_MIXED_IMAGE_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
        source_model_case_fields("language_profile_model"),
    ),
    SurfaceRequirement(
        "runtime_package_loading_module_identity_semantics_surface",
        RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
        source_model_case_fields("runtime_probe"),
    ),
    SurfaceRequirement(
        "runtime_c_cpp_swift_bridge_semantics_surface",
        RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID,
        source_model_case_fields("language_profile_model"),
    ),
    SurfaceRequirement(
        "runtime_import_version_feature_claim_diagnostics_surface",
        RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        source_model_case_fields("diagnostic_model"),
    ),
    SurfaceRequirement(
        "runtime_packaging_bridge_loader_artifact_surface",
        RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        source_model_case_fields("artifact_surface_model"),
    ),
    SurfaceRequirement(
        "runtime_mixed_image_package_lowering_bridge_emission_surface",
        RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        source_model_case_fields("lowering_model"),
    ),
    SurfaceRequirement(
        "runtime_cross_language_replay_import_surface_preservation_surface",
        RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
        source_model_case_fields("preservation_model"),
    ),
    SurfaceRequirement(
        "runtime_package_loader_bridge_abi_surface",
        RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
        source_model_case_fields("runtime_abi_model"),
    ),
    SurfaceRequirement(
        "runtime_package_loading_interop_implementation_surface",
        RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        source_model_case_fields("implementation_model"),
    ),
)

__all__ = ["INTEROP_SURFACES"]
