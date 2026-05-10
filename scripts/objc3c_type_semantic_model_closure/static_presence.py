from __future__ import annotations

from objc3c_tooling.validation import contains_all
from objc3c_type_semantic_model_closure.paths import FRONTEND_ARTIFACTS
from objc3c_type_semantic_model_closure.paths import IR_EMITTER
from objc3c_type_semantic_model_closure.paths import LOWERING_CONTRACT
from objc3c_type_semantic_model_closure.paths import RUNTIME_IMPORT_SURFACE
from objc3c_type_semantic_model_closure.paths import RUNTIME_IMPORT_SURFACE_HEADER
from objc3c_type_semantic_model_closure.paths import SEMA_CONTRACT
from objc3c_type_semantic_model_closure.paths import SEMANTIC_PASSES
from objc3c_type_semantic_model_closure.paths import read
from objc3c_type_semantic_model_closure.positive import REPLAY_KEY_SEGMENTS

STATIC_FIELD_TOKENS = [
    "object_pointer_semantic_sites",
    "protocol_composition_semantic_sites",
    "generic_suffix_semantic_sites",
    "nullability_suffix_semantic_sites",
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
    "canonical_type_entries",
    "canonical_object_type_entries",
    "canonical_protocol_qualified_entries",
    "canonical_nullable_entries",
    "canonical_implicitly_unwrapped_entries",
    "canonical_invalid_type_entries",
]

SEMANTIC_PASS_TOKENS = [
    "BuildTypeSystemTypeSemanticModelSummary",
    "type_annotation_surface_summary.object_pointer_type_sites",
    "protocol_qualified_object_type_summary.protocol_composition_sites",
    "type_annotation_surface_summary.generic_suffix_sites",
    "generic_metadata_abi_summary.generic_metadata_abi_sites",
    "type_annotation_surface_summary.nullability_suffix_sites",
    "nullability_flow_warning_precision_summary.nullability_flow_sites",
    "type_annotation_surface_summary.invalid_generic_suffix_sites",
    "type_annotation_surface_summary.invalid_nullability_suffix_sites",
    "protocol_qualified_object_type_summary.contract_violation_sites",
    "param_canonical_types",
    "return_canonical_type",
    "canonical_type",
    "IsUnsafeNullableToNonnullFlow",
    "IsCompatibleCanonicalSemanticType",
    "ValidateProtocolCompositionIdentifierBindings",
    "ResolveProtocolQualifiedMessageRequirement",
    "ResolveConcreteOwnerPropertyAccessor",
    "MakeSemanticTypeFromPropertyInfo",
    "object_pointer_type_name",
    "Objc3InterfaceGenericDefinition",
    "BuildInterfaceGenericDefinitions",
    "ValidateInterfaceGenericSpecializations",
    "AreGenericSpecializationsVarianceAssignmentCompatible",
    "ValidateProtocolQualifiedGenericArgument",
    "SubstituteGenericReceiverType",
    "ExtractGenericArgumentSpecialization",
    "generic_parameter_variance_source_order",
    "generic_arguments_source_order",
    "SupportsPointerParamTypeDeclarator",
    "optional_methods_by_key",
]


def compile_static_presence() -> dict[str, dict[str, bool]]:
    runtime_import_surface_text = read(RUNTIME_IMPORT_SURFACE)
    runtime_import_surface_header_text = read(RUNTIME_IMPORT_SURFACE_HEADER)
    return {
        "sema_contract_fields": contains_all(read(SEMA_CONTRACT), STATIC_FIELD_TOKENS),
        "semantic_pass_sources_and_replay_key": contains_all(
            read(SEMANTIC_PASSES),
            STATIC_FIELD_TOKENS + SEMANTIC_PASS_TOKENS + REPLAY_KEY_SEGMENTS,
        ),
        "artifact_json_fields": contains_all(
            read(FRONTEND_ARTIFACTS),
            STATIC_FIELD_TOKENS
            + [
                "semantic_canonical_type_metadata",
                "return_canonical_type",
                "param_canonical_types",
                "object_pointer_type_name",
                "generic_parameter_variance_source_order",
            ],
        ),
        "runtime_import_surface_generic_contract": contains_all(
            runtime_import_surface_text + runtime_import_surface_header_text,
            [
                "objc_type_system_generic_contract_preservation",
                "PopulateImportedTypeSystemGenericContractPreservation",
                "type_system_generic_contract_preservation_present",
                "type_system_protocol_qualified_generic_argument_count",
            ],
        ),
        "runtime_import_surface_nullability_contract": contains_all(
            runtime_import_surface_text + runtime_import_surface_header_text,
            [
                "objc_type_system_nullability_contract_preservation",
                "PopulateImportedTypeSystemNullabilityContractPreservation",
                "type_system_nullability_contract_preservation_present",
                "type_system_unspecified_nullability_entry_count",
            ],
        ),
        "runtime_import_surface_protocol_contract": contains_all(
            runtime_import_surface_text + runtime_import_surface_header_text,
            [
                "objc_type_system_protocol_contract_preservation",
                "PopulateImportedTypeSystemProtocolContractPreservation",
                "type_system_protocol_contract_preservation_present",
                "type_system_protocol_required_method_count",
            ],
        ),
        "lowering_contract_runtime_surface_present": contains_all(read(LOWERING_CONTRACT), ["Lowering", "runtime"]),
        "ir_emitter_runtime_surface_present": contains_all(read(IR_EMITTER), ["Objc3", "Emit"]),
    }
