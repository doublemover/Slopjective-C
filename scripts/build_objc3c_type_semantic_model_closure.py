from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.validation import contains_all
from objc3c_type_semantic_model_closure.compiler import diagnostic_matches
from objc3c_type_semantic_model_closure.compiler import nested_semantic_model
from objc3c_type_semantic_model_closure.compiler import run_compiler
from objc3c_type_semantic_model_closure.reporting import SUMMARY_FIELDS
from objc3c_type_semantic_model_closure.reporting import expected_report_outputs
from objc3c_type_semantic_model_closure.reporting import write_outputs

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "type-semantic-model-closure"
JSON_OUT = REPORT_DIR / "type_semantic_model_closure_summary.json"
MD_OUT = REPORT_DIR / "type_semantic_model_closure_summary.md"
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "type-semantic-model-closure"

CONTRACT_ID = "objc3c.semantic.type-semantic-model-closure.v1"
ISSUE = "#8013"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_model_closure_positive.objc3"
NESTED_GENERIC_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_nested_generic_positive.objc3"
GENERIC_VARIANCE_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_generic_variance_positive.objc3"
PROTOCOL_GENERIC_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_protocol_generic_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_duplicate_protocol_composition.objc3"
NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_nullable_to_nonnull_flow.objc3"
PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_method_nullability_conflict.objc3"
PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_property_nullability_conflict.objc3"
UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_unknown_protocol_composition.objc3"
PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_qualified_unknown_message.objc3"
TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_typed_object_receiver_unknown_message.objc3"
GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_constraint_violation.objc3"
GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_substitution_unknown_message.objc3"
NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_nested_generic_constraint_violation.objc3"
GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_invariant_assignment.objc3"
PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_generic_unknown_protocol.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-01.json"
CONFORMANCE_NESTED_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-12.json"
CONFORMANCE_GENERIC_VARIANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-13.json"
CONFORMANCE_PROTOCOL_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-15.json"
CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-17.json"
CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-18.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-02.json"
CONFORMANCE_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-03.json"
CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-04.json"
CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-05.json"
CONFORMANCE_UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-06.json"
CONFORMANCE_PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-07.json"
CONFORMANCE_TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-08.json"
CONFORMANCE_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-09.json"
CONFORMANCE_GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-10.json"
CONFORMANCE_NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-11.json"
CONFORMANCE_GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-14.json"
CONFORMANCE_PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-16.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
RUNTIME_IMPORT_SURFACE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
RUNTIME_IMPORT_SURFACE_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"

POSITIVE_MIN_COUNTS = {
    "optional_binding_sites": 2,
    "optional_binding_clause_sites": 2,
    "guard_binding_sites": 1,
    "optional_send_sites": 1,
    "nil_coalescing_sites": 1,
    "optional_propagation_sites": 1,
    "optional_flow_refinement_sites": 2,
    "guard_binding_exit_enforcement_sites": 1,
    "typed_keypath_literal_sites": 1,
    "typed_keypath_class_root_sites": 1,
    "object_pointer_semantic_sites": 7,
    "protocol_composition_semantic_sites": 4,
    "generic_suffix_semantic_sites": 4,
    "generic_erasure_semantic_sites": 4,
    "nullability_suffix_semantic_sites": 5,
    "nullability_semantic_sites": 5,
    "canonical_type_entries": 9,
    "canonical_object_type_entries": 7,
    "canonical_protocol_qualified_entries": 4,
    "canonical_generic_argument_entries": 4,
    "canonical_nullable_entries": 4,
    "canonical_implicitly_unwrapped_entries": 1,
}

ZERO_FIELDS = [
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
    "canonical_invalid_type_entries",
    "optional_binding_contract_violation_sites",
    "optional_send_contract_violation_sites",
    "optional_flow_contract_violation_sites",
    "typed_keypath_contract_violation_sites",
]

REPLAY_KEY_SEGMENTS = [
    "bindings=",
    "guards=",
    "optional-sends=",
    "coalescing=",
    "propagation=",
    "refinement=",
    "guard-exit=",
    "keypaths=",
    "keypath-class=",
    "object-pointers=",
    "protocol-compositions=",
    "generic-suffixes=",
    "generic-erasure=",
    "nullability-suffixes=",
    "nullability=",
    "canonical-types=",
    "canonical-object-types=",
    "canonical-protocol-qualified=",
    "canonical-generics=",
    "canonical-nullable=",
    "canonical-iuo=",
    "canonical-invalid=",
    "invalid-generic-suffixes=",
    "invalid-nullability-suffixes=",
    "invalid-protocol-compositions=",
]

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


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def compile_positive_summary(run: dict[str, Any]) -> tuple[dict[str, Any] | None, dict[str, bool]]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    if not model:
        return None, {"manifest_has_type_semantic_model": False}
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    canonical_interfaces = canonical_metadata.get("interfaces", []) if isinstance(canonical_metadata, dict) else []
    choose_function = next((entry for entry in canonical_functions if entry.get("name") == "choose"), None)
    consume_box_function = next((entry for entry in canonical_functions if entry.get("name") == "consumeBox"), None)
    semantic_box = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticBox"), None)
    replay_key = str(model.get("replay_key", ""))
    checks = {
        "positive_fixture_compiles": run["exit_code"] == 0,
        "positive_manifest_emitted": run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "manifest_has_type_semantic_model": True,
        "manifest_has_canonical_type_metadata": isinstance(canonical_metadata, dict),
        "canonical_function_metadata_publishes_replay_keys": isinstance(choose_function, dict)
        and bool((choose_function.get("return_canonical_type") or {}).get("replay_key"))
        and len(choose_function.get("param_canonical_types", [])) == 3
        and all(bool((param or {}).get("replay_key")) for param in choose_function.get("param_canonical_types", [])),
        "canonical_named_object_pointer_metadata_publishes_owner": isinstance(consume_box_function, dict)
        and any(
            (param or {}).get("object_pointer_type_name") == "SemanticBox"
            and "object-name=SemanticBox" in str((param or {}).get("replay_key", ""))
            for param in consume_box_function.get("param_canonical_types", [])
        ),
        "canonical_interface_property_metadata_publishes_replay_keys": isinstance(semantic_box, dict)
        and any(
            property_entry.get("name") == "title"
            and bool((property_entry.get("canonical_type") or {}).get("replay_key"))
            for property_entry in semantic_box.get("properties", [])
        ),
        "all_summary_fields_emitted": all(field in model for field in SUMMARY_FIELDS),
        "ready_for_lowering_and_runtime": bool(model.get("ready_for_lowering_and_runtime")),
        "deterministic": bool(model.get("deterministic")),
        "minimum_positive_counts_observed": all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_contract_violations_zero": all(int(model.get(field, -1)) == 0 for field in ZERO_FIELDS),
        "replay_key_covers_typed_surfaces": all(segment in replay_key for segment in REPLAY_KEY_SEGMENTS),
        "protocol_generic_nullability_counts_bound_to_object_pointers": int(model.get("protocol_composition_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2))
        and int(model.get("generic_suffix_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2))
        and int(model.get("nullability_suffix_semantic_sites", -1)) <= int(model.get("object_pointer_semantic_sites", -2)),
    }
    return model, checks


def compile_nested_generic_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    consume_nested = next((entry for entry in canonical_functions if entry.get("name") == "consumeNestedGeneric"), None)
    param_types = consume_nested.get("param_canonical_types", []) if isinstance(consume_nested, dict) else []
    nested_param = param_types[0] if param_types else {}
    replay_key = str((nested_param or {}).get("replay_key", ""))
    canonical_spelling = str((nested_param or {}).get("canonical_spelling", ""))
    generic_args = (nested_param or {}).get("generic_arguments_source_order", [])
    return {
        "nested_generic_positive_fixture_compiles": run["exit_code"] == 0,
        "nested_generic_positive_manifest_emitted": run["manifest_path"] is not None,
        "nested_generic_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "nested_generic_positive_manifest_ready": bool(model and model.get("ready_for_lowering_and_runtime")),
        "nested_generic_positive_contract_violations_zero": bool(
            model and all(int(model.get(field, -1)) == 0 for field in ZERO_FIELDS)
        ),
        "nested_generic_positive_param_metadata_publishes_owner": (nested_param or {}).get("object_pointer_type_name") == "SemanticEnvelope",
        "nested_generic_positive_param_metadata_preserves_nested_argument": generic_args == ["SemanticVault<SemanticBox*>*"],
        "nested_generic_positive_param_metadata_publishes_replay_key": "object-name=SemanticEnvelope" in replay_key
        and "generics=SemanticVault<SemanticBox*>*" in replay_key,
        "nested_generic_positive_param_metadata_publishes_canonical_spelling": canonical_spelling == "SemanticEnvelope<SemanticVault<SemanticBox*>*>*",
    }


def compile_generic_variance_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_interfaces = canonical_metadata.get("interfaces", []) if isinstance(canonical_metadata, dict) else []
    semantic_vault = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticVault"), None)
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    accept_covariant = next((entry for entry in canonical_functions if entry.get("name") == "acceptCovariant"), None)
    consume_covariant = next((entry for entry in canonical_functions if entry.get("name") == "consumeCovariant"), None)
    accept_param_types = accept_covariant.get("param_canonical_types", []) if isinstance(accept_covariant, dict) else []
    consume_param_types = consume_covariant.get("param_canonical_types", []) if isinstance(consume_covariant, dict) else []
    return {
        "generic_variance_positive_fixture_compiles": run["exit_code"] == 0,
        "generic_variance_positive_manifest_emitted": run["manifest_path"] is not None,
        "generic_variance_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "generic_variance_interface_metadata_preserves_parameter_name": isinstance(semantic_vault, dict)
        and semantic_vault.get("generic_parameter_names_source_order") == ["T"],
        "generic_variance_interface_metadata_preserves_covariance": isinstance(semantic_vault, dict)
        and semantic_vault.get("generic_parameter_variance_source_order") == ["__covariant"],
        "generic_variance_interface_metadata_preserves_protocol_adoption": isinstance(semantic_vault, dict)
        and semantic_vault.get("adopted_protocols_lexicographic") == [],
        "generic_variance_assignment_param_metadata_preserves_erased_target": len(accept_param_types) == 1
        and (accept_param_types[0] or {}).get("generic_arguments_source_order") == ["SemanticBase*"],
        "generic_variance_assignment_param_metadata_preserves_concrete_value": len(consume_param_types) == 1
        and (consume_param_types[0] or {}).get("generic_arguments_source_order") == ["SemanticBox*"],
    }


def compile_protocol_generic_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_interfaces = canonical_metadata.get("interfaces", []) if isinstance(canonical_metadata, dict) else []
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    semantic_box = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticBox"), None)
    accept_protocol = next((entry for entry in canonical_functions if entry.get("name") == "acceptProtocolVault"), None)
    consume_concrete = next((entry for entry in canonical_functions if entry.get("name") == "consumeConcreteVault"), None)
    accept_param_types = accept_protocol.get("param_canonical_types", []) if isinstance(accept_protocol, dict) else []
    consume_param_types = consume_concrete.get("param_canonical_types", []) if isinstance(consume_concrete, dict) else []
    return {
        "protocol_generic_positive_fixture_compiles": run["exit_code"] == 0,
        "protocol_generic_positive_manifest_emitted": run["manifest_path"] is not None,
        "protocol_generic_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "protocol_generic_positive_manifest_ready": bool(model and model.get("ready_for_lowering_and_runtime")),
        "protocol_generic_positive_contract_violations_zero": bool(
            model and all(int(model.get(field, -1)) == 0 for field in ZERO_FIELDS)
        ),
        "protocol_generic_argument_metadata_preserves_id_protocol": len(accept_param_types) == 1
        and (accept_param_types[0] or {}).get("generic_arguments_source_order") == ["id<Persistable>"],
        "protocol_generic_argument_metadata_preserves_concrete_argument": len(consume_param_types) == 1
        and (consume_param_types[0] or {}).get("generic_arguments_source_order") == ["SemanticBox*"],
        "protocol_generic_argument_replay_key_preserves_id_protocol": len(accept_param_types) == 1
        and "generics=id<Persistable>" in str((accept_param_types[0] or {}).get("replay_key", "")),
        "protocol_generic_covariant_value_adopts_constraint_protocol": isinstance(semantic_box, dict)
        and semantic_box.get("adopted_protocols_lexicographic") == ["Persistable"],
    }


def find_imported_runtime_metadata_semantic_rules(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if "imported_type_system_generic_contract_module_count" in node:
            return node
        for value in node.values():
            found = find_imported_runtime_metadata_semantic_rules(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_imported_runtime_metadata_semantic_rules(value)
            if found is not None:
                return found
    return None


def compile_cross_module_generic_contract_summary(provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(ROOT / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
    preservation = (
        provider_surface.get("objc_type_system_generic_contract_preservation")
        if isinstance(provider_surface, dict)
        else None
    )
    imported_rules = find_imported_runtime_metadata_semantic_rules(consumer_run.get("manifest"))
    return {
        "cross_module_provider_fixture_compiles": provider_run["exit_code"] == 0,
        "cross_module_provider_runtime_import_surface_emitted": isinstance(provider_surface, dict),
        "cross_module_provider_generic_contract_preservation_emitted": isinstance(preservation, dict),
        "cross_module_provider_generic_contract_ready": isinstance(preservation, dict)
        and preservation.get("ready") is True
        and preservation.get("deterministic") is True,
        "cross_module_provider_generic_contract_preserves_interface_count": isinstance(preservation, dict)
        and int(preservation.get("generic_interface_count", -1)) == 1,
        "cross_module_provider_generic_contract_preserves_parameter_variance": isinstance(preservation, dict)
        and int(preservation.get("generic_parameter_count", -1)) == 1
        and int(preservation.get("generic_variance_annotation_count", -1)) == 1,
        "cross_module_provider_generic_contract_preserves_protocol_qualified_argument": isinstance(preservation, dict)
        and int(preservation.get("protocol_qualified_generic_argument_count", -1)) == 1,
        "cross_module_provider_generic_contract_preserves_interface_payload": isinstance(preservation, dict)
        and any(
            entry.get("name") == "SemanticVault"
            and entry.get("generic_parameter_names_source_order") == ["T"]
            and entry.get("generic_parameter_variance_source_order") == ["__covariant"]
            for entry in preservation.get("generic_contract_interfaces", [])
        ),
        "cross_module_consumer_imports_generic_contract_surface": consumer_run["exit_code"] == 0
        and isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_type_system_generic_contract_module_count", 0)) == 1,
        "cross_module_consumer_imported_generic_counts_match_provider": isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_generic_interface_count", -1)) == 1
        and int(imported_rules.get("imported_generic_parameter_count", -1)) == 1
        and int(imported_rules.get("imported_generic_variance_annotation_count", -1)) == 1
        and int(imported_rules.get("imported_protocol_qualified_generic_argument_count", -1)) == 1,
        "cross_module_consumer_type_surface_landed": isinstance(imported_rules, dict)
        and imported_rules.get("imported_type_system_type_surface_landed") is True
        and imported_rules.get("ready") is True,
        "cross_module_consumer_replay_key_covers_imported_generic_contract": isinstance(imported_rules, dict)
        and "imported_type_system_generic_contract_module_count=1" in str(imported_rules.get("replay_key", ""))
        and "imported_protocol_qualified_generic_argument_count=1" in str(imported_rules.get("replay_key", "")),
    }


def write_drifted_generic_contract_surface(provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    provider_surface_path = ROOT / provider_surface_path_value
    surface = load_json(provider_surface_path)
    preservation = surface.get("objc_type_system_generic_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit generic preservation")
    preservation["generic_variance_annotation_count"] = 0
    preservation["replay_key"] = str(preservation.get("replay_key", "")).replace(
        "variance_annotations=1",
        "variance_annotations=0",
    )
    drift_dir = TMP_ROOT / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "generic-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path


def compile_cross_module_nullability_contract_summary(provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(ROOT / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
    preservation = (
        provider_surface.get("objc_type_system_nullability_contract_preservation")
        if isinstance(provider_surface, dict)
        else None
    )
    imported_rules = find_imported_runtime_metadata_semantic_rules(consumer_run.get("manifest"))
    provider_canonical_count = int(preservation.get("canonical_type_count", -1)) if isinstance(preservation, dict) else -1
    provider_nullable_count = int(preservation.get("nullable_entry_count", -1)) if isinstance(preservation, dict) else -1
    provider_unspecified_count = int(preservation.get("unspecified_nullability_entry_count", -1)) if isinstance(preservation, dict) else -1
    return {
        "cross_module_provider_nullability_contract_preservation_emitted": isinstance(preservation, dict),
        "cross_module_provider_nullability_contract_ready": isinstance(preservation, dict)
        and preservation.get("ready") is True
        and preservation.get("deterministic") is True,
        "cross_module_provider_nullability_counts_are_complete": isinstance(preservation, dict)
        and provider_canonical_count
        == int(preservation.get("nullable_entry_count", -2))
        + int(preservation.get("nonnull_entry_count", -2))
        + int(preservation.get("implicitly_unwrapped_entry_count", -2))
        + int(preservation.get("null_resettable_entry_count", -2))
        + int(preservation.get("unspecified_nullability_entry_count", -2)),
        "cross_module_provider_nullability_preserves_nullable_and_unspecified": provider_nullable_count > 0
        and provider_unspecified_count > 0,
        "cross_module_consumer_imports_nullability_contract_surface": consumer_run["exit_code"] == 0
        and isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_type_system_nullability_contract_module_count", 0)) == 1,
        "cross_module_consumer_imported_nullability_counts_match_provider": isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_nullability_canonical_type_count", -1)) == provider_canonical_count
        and int(imported_rules.get("imported_nullable_entry_count", -1)) == provider_nullable_count
        and int(imported_rules.get("imported_unspecified_nullability_entry_count", -1)) == provider_unspecified_count,
        "cross_module_consumer_nullability_replay_key_covers_imported_contract": isinstance(imported_rules, dict)
        and "imported_type_system_nullability_contract_module_count=1" in str(imported_rules.get("replay_key", ""))
        and "imported_nullable_entry_count=" in str(imported_rules.get("replay_key", "")),
    }


def write_drifted_nullability_contract_surface(provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    surface = load_json(ROOT / provider_surface_path_value)
    preservation = surface.get("objc_type_system_nullability_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit nullability preservation")
    preservation["unspecified_nullability_entry_count"] = max(
        0,
        int(preservation.get("unspecified_nullability_entry_count", 0)) - 1,
    )
    drift_dir = TMP_ROOT / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "nullability-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path


def compile_cross_module_protocol_contract_summary(provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(ROOT / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
    preservation = (
        provider_surface.get("objc_type_system_protocol_contract_preservation")
        if isinstance(provider_surface, dict)
        else None
    )
    imported_rules = find_imported_runtime_metadata_semantic_rules(consumer_run.get("manifest"))
    provider_protocol_count = int(preservation.get("protocol_decl_count", -1)) if isinstance(preservation, dict) else -1
    provider_inheritance_count = int(preservation.get("protocol_inheritance_edge_count", -1)) if isinstance(preservation, dict) else -1
    provider_required_method_count = int(preservation.get("protocol_required_method_count", -1)) if isinstance(preservation, dict) else -1
    provider_optional_method_count = int(preservation.get("protocol_optional_method_count", -1)) if isinstance(preservation, dict) else -1
    provider_required_property_count = int(preservation.get("protocol_required_property_count", -1)) if isinstance(preservation, dict) else -1
    provider_optional_property_count = int(preservation.get("protocol_optional_property_count", -1)) if isinstance(preservation, dict) else -1
    return {
        "cross_module_provider_protocol_contract_preservation_emitted": isinstance(preservation, dict),
        "cross_module_provider_protocol_contract_ready": isinstance(preservation, dict)
        and preservation.get("ready") is True
        and preservation.get("deterministic") is True,
        "cross_module_provider_protocol_contract_preserves_requirement_partitions": isinstance(preservation, dict)
        and provider_protocol_count == 1
        and provider_required_method_count == 1
        and provider_optional_method_count == 0
        and provider_required_property_count == 1
        and provider_optional_property_count == 1,
        "cross_module_provider_protocol_contract_preserves_inheritance_and_adoption_edges": isinstance(preservation, dict)
        and provider_inheritance_count == 0
        and int(preservation.get("class_protocol_adoption_count", -1)) == 0,
        "cross_module_provider_protocol_contract_preserves_protocol_payload": isinstance(preservation, dict)
        and any(
            entry.get("name") == "SemanticValue"
            and entry.get("required_method_count") == 1
            and entry.get("optional_property_count") == 1
            for entry in preservation.get("protocol_contract_protocols", [])
        ),
        "cross_module_consumer_imports_protocol_contract_surface": consumer_run["exit_code"] == 0
        and isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_type_system_protocol_contract_module_count", 0)) == 1,
        "cross_module_consumer_imported_protocol_counts_match_provider": isinstance(imported_rules, dict)
        and int(imported_rules.get("imported_protocol_decl_count", -1)) == provider_protocol_count
        and int(imported_rules.get("imported_protocol_required_method_count", -1)) == provider_required_method_count
        and int(imported_rules.get("imported_protocol_optional_method_count", -1)) == provider_optional_method_count
        and int(imported_rules.get("imported_class_protocol_adoption_count", -1)) == int(preservation.get("class_protocol_adoption_count", -2) if isinstance(preservation, dict) else -2),
        "cross_module_consumer_protocol_replay_key_covers_imported_contract": isinstance(imported_rules, dict)
        and "imported_type_system_protocol_contract_module_count=1" in str(imported_rules.get("replay_key", ""))
        and "imported_protocol_required_method_count=1" in str(imported_rules.get("replay_key", "")),
    }


def write_drifted_protocol_contract_surface(provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    surface = load_json(ROOT / provider_surface_path_value)
    preservation = surface.get("objc_type_system_protocol_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit protocol preservation")
    preservation["protocol_required_method_count"] = max(
        0,
        int(preservation.get("protocol_required_method_count", 0)) - 1,
    )
    drift_dir = TMP_ROOT / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "protocol-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(ROOT, COMPILER, POSITIVE_FIXTURE, TMP_ROOT / "positive")
    nested_generic_positive_run = run_compiler(ROOT, COMPILER, NESTED_GENERIC_POSITIVE_FIXTURE, TMP_ROOT / "positive-nested-generic")
    generic_variance_positive_run = run_compiler(ROOT, COMPILER, GENERIC_VARIANCE_POSITIVE_FIXTURE, TMP_ROOT / "positive-generic-variance")
    protocol_generic_positive_run = run_compiler(ROOT, COMPILER, PROTOCOL_GENERIC_POSITIVE_FIXTURE, TMP_ROOT / "positive-protocol-generic")
    cross_module_nullability_drift_surface = write_drifted_nullability_contract_surface(positive_run)
    cross_module_protocol_drift_surface = write_drifted_protocol_contract_surface(positive_run)
    cross_module_nullability_consumer_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-cross-module-nullability-consumer",
        [
            "--objc3-import-runtime-surface",
            str(TMP_ROOT / "positive" / "module.runtime-import-surface.json"),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
    )
    cross_module_nullability_drift_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "negative-cross-module-nullability-drift",
        [
            "--objc3-import-runtime-surface",
            str(cross_module_nullability_drift_surface),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
    )
    cross_module_protocol_drift_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "negative-cross-module-protocol-drift",
        [
            "--objc3-import-runtime-surface",
            str(cross_module_protocol_drift_surface),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
    )
    cross_module_generic_drift_surface = write_drifted_generic_contract_surface(protocol_generic_positive_run)
    cross_module_generic_consumer_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-cross-module-generic-consumer",
        [
            "--objc3-import-runtime-surface",
            str(TMP_ROOT / "positive-protocol-generic" / "module.runtime-import-surface.json"),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
    )
    cross_module_generic_drift_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "negative-cross-module-generic-drift",
        [
            "--objc3-import-runtime-surface",
            str(cross_module_generic_drift_surface),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
    )
    negative_run = run_compiler(ROOT, COMPILER, NEGATIVE_FIXTURE, TMP_ROOT / "negative-duplicate-protocol")
    nullability_negative_run = run_compiler(ROOT, COMPILER, NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-nullability-flow")
    protocol_method_nullability_negative_run = run_compiler(ROOT, COMPILER, PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-method-nullability")
    protocol_property_nullability_negative_run = run_compiler(ROOT, COMPILER, PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-property-nullability")
    unknown_protocol_composition_negative_run = run_compiler(ROOT, COMPILER, UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-unknown-protocol-composition")
    protocol_qualified_unknown_message_negative_run = run_compiler(ROOT, COMPILER, PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-qualified-unknown-message")
    typed_object_receiver_unknown_message_negative_run = run_compiler(ROOT, COMPILER, TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-typed-object-receiver-unknown-message")
    generic_constraint_violation_negative_run = run_compiler(ROOT, COMPILER, GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-constraint-violation")
    generic_substitution_unknown_message_negative_run = run_compiler(ROOT, COMPILER, GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-substitution-unknown-message")
    nested_generic_constraint_violation_negative_run = run_compiler(ROOT, COMPILER, NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-nested-generic-constraint-violation")
    generic_invariant_assignment_negative_run = run_compiler(ROOT, COMPILER, GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-invariant-assignment")
    protocol_generic_unknown_protocol_negative_run = run_compiler(ROOT, COMPILER, PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-generic-unknown-protocol")
    model, positive_checks = compile_positive_summary(positive_run)
    nested_generic_positive_checks = compile_nested_generic_positive_summary(nested_generic_positive_run)
    generic_variance_positive_checks = compile_generic_variance_positive_summary(generic_variance_positive_run)
    protocol_generic_positive_checks = compile_protocol_generic_positive_summary(protocol_generic_positive_run)
    cross_module_generic_checks = compile_cross_module_generic_contract_summary(
        protocol_generic_positive_run,
        cross_module_generic_consumer_run,
    )
    cross_module_generic_drift_checks = {
        "cross_module_generic_contract_drift_fails_closed": cross_module_generic_drift_run["exit_code"] != 0,
        "cross_module_generic_contract_drift_reports_variance_loss": "type-system generic contract preservation dropped variance annotations"
        in (
            cross_module_generic_drift_run["stderr"]
            + cross_module_generic_drift_run["stdout"]
            + " ".join(str(diag.get("message", "")) for diag in cross_module_generic_drift_run["diagnostics"])
        ),
    }
    cross_module_nullability_checks = compile_cross_module_nullability_contract_summary(
        positive_run,
        cross_module_nullability_consumer_run,
    )
    cross_module_nullability_drift_checks = {
        "cross_module_nullability_contract_drift_fails_closed": cross_module_nullability_drift_run["exit_code"] != 0,
        "cross_module_nullability_contract_drift_reports_entry_loss": "type-system nullability contract preservation dropped canonical nullability entries"
        in (
            cross_module_nullability_drift_run["stderr"]
            + cross_module_nullability_drift_run["stdout"]
            + " ".join(str(diag.get("message", "")) for diag in cross_module_nullability_drift_run["diagnostics"])
        ),
    }
    cross_module_protocol_checks = compile_cross_module_protocol_contract_summary(
        positive_run,
        cross_module_nullability_consumer_run,
    )
    cross_module_protocol_drift_checks = {
        "cross_module_protocol_contract_drift_fails_closed": cross_module_protocol_drift_run["exit_code"] != 0,
        "cross_module_protocol_contract_drift_reports_requirement_loss": "type-system protocol contract preservation dropped requirement or inheritance entries"
        in (
            cross_module_protocol_drift_run["stderr"]
            + cross_module_protocol_drift_run["stdout"]
            + " ".join(str(diag.get("message", "")) for diag in cross_module_protocol_drift_run["diagnostics"])
        ),
    }

    sema_contract_text = read(SEMA_CONTRACT)
    semantic_passes_text = read(SEMANTIC_PASSES)
    artifacts_text = read(FRONTEND_ARTIFACTS)
    runtime_import_surface_text = read(RUNTIME_IMPORT_SURFACE)
    runtime_import_surface_header_text = read(RUNTIME_IMPORT_SURFACE_HEADER)
    lowering_text = read(LOWERING_CONTRACT)
    ir_text = read(IR_EMITTER)
    manifest_text = read(SEMANTIC_MANIFEST)
    readme_text = read(SEMANTIC_README)
    stress_manifest_text = read(STRESS_MANIFEST)
    conformance_positive = load_json(CONFORMANCE_POSITIVE)
    conformance_nested_generic_positive = load_json(CONFORMANCE_NESTED_GENERIC_POSITIVE)
    conformance_generic_variance_positive = load_json(CONFORMANCE_GENERIC_VARIANCE_POSITIVE)
    conformance_protocol_generic_positive = load_json(CONFORMANCE_PROTOCOL_GENERIC_POSITIVE)
    conformance_cross_module_generic_positive = load_json(CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE)
    conformance_cross_module_protocol_positive = load_json(CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE)
    conformance_negative = load_json(CONFORMANCE_NEGATIVE)
    conformance_nullability_negative = load_json(CONFORMANCE_NULLABILITY_NEGATIVE)
    conformance_protocol_method_nullability_negative = load_json(CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE)
    conformance_protocol_property_nullability_negative = load_json(CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE)
    conformance_unknown_protocol_composition_negative = load_json(CONFORMANCE_UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE)
    conformance_protocol_qualified_unknown_message_negative = load_json(CONFORMANCE_PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE)
    conformance_typed_object_receiver_unknown_message_negative = load_json(CONFORMANCE_TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE)
    conformance_generic_constraint_violation_negative = load_json(CONFORMANCE_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE)
    conformance_generic_substitution_unknown_message_negative = load_json(CONFORMANCE_GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE)
    conformance_nested_generic_constraint_violation_negative = load_json(CONFORMANCE_NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE)
    conformance_generic_invariant_assignment_negative = load_json(CONFORMANCE_GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE)
    conformance_protocol_generic_unknown_protocol_negative = load_json(CONFORMANCE_PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE)

    static_presence = {
        "sema_contract_fields": contains_all(sema_contract_text, STATIC_FIELD_TOKENS),
        "semantic_pass_sources_and_replay_key": contains_all(semantic_passes_text, STATIC_FIELD_TOKENS + SEMANTIC_PASS_TOKENS + REPLAY_KEY_SEGMENTS),
        "artifact_json_fields": contains_all(artifacts_text, STATIC_FIELD_TOKENS + ["semantic_canonical_type_metadata", "return_canonical_type", "param_canonical_types", "object_pointer_type_name", "generic_parameter_variance_source_order"]),
        "runtime_import_surface_generic_contract": contains_all(runtime_import_surface_text + runtime_import_surface_header_text, ["objc_type_system_generic_contract_preservation", "PopulateImportedTypeSystemGenericContractPreservation", "type_system_generic_contract_preservation_present", "type_system_protocol_qualified_generic_argument_count"]),
        "runtime_import_surface_nullability_contract": contains_all(runtime_import_surface_text + runtime_import_surface_header_text, ["objc_type_system_nullability_contract_preservation", "PopulateImportedTypeSystemNullabilityContractPreservation", "type_system_nullability_contract_preservation_present", "type_system_unspecified_nullability_entry_count"]),
        "runtime_import_surface_protocol_contract": contains_all(runtime_import_surface_text + runtime_import_surface_header_text, ["objc_type_system_protocol_contract_preservation", "PopulateImportedTypeSystemProtocolContractPreservation", "type_system_protocol_contract_preservation_present", "type_system_protocol_required_method_count"]),
        "lowering_contract_runtime_surface_present": contains_all(lowering_text, ["Lowering", "runtime"]),
        "ir_emitter_runtime_surface_present": contains_all(ir_text, ["Objc3", "Emit"]),
    }

    source_truth_paths = [
        POSITIVE_FIXTURE,
        NESTED_GENERIC_POSITIVE_FIXTURE,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        PROTOCOL_GENERIC_POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        NULLABILITY_NEGATIVE_FIXTURE,
        PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE,
        PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE,
        UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE,
        PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
        TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
        GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE,
        GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
        NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE,
        GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE,
        PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE,
        CONFORMANCE_POSITIVE,
        CONFORMANCE_NESTED_GENERIC_POSITIVE,
        CONFORMANCE_GENERIC_VARIANCE_POSITIVE,
        CONFORMANCE_PROTOCOL_GENERIC_POSITIVE,
        CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE,
        CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE,
        CONFORMANCE_NEGATIVE,
        CONFORMANCE_NULLABILITY_NEGATIVE,
        CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE,
        CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE,
        CONFORMANCE_UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE,
        CONFORMANCE_PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE,
        CONFORMANCE_TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE,
        CONFORMANCE_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE,
        CONFORMANCE_GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE,
        CONFORMANCE_NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE,
        CONFORMANCE_GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE,
        CONFORMANCE_PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE,
        SEMANTIC_MANIFEST,
        SEMANTIC_README,
        STRESS_MANIFEST,
        SEMA_CONTRACT,
        SEMANTIC_PASSES,
        FRONTEND_ARTIFACTS,
        RUNTIME_IMPORT_SURFACE,
        RUNTIME_IMPORT_SURFACE_HEADER,
        LOWERING_CONTRACT,
        IR_EMITTER,
    ]
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in source_truth_paths)

    negative_checks = {
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_duplicate_protocol_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S206", 7, 21),
        "nullability_negative_fixture_fails_closed": nullability_negative_run["exit_code"] != 0,
        "nullability_negative_diagnostics_json_emitted": nullability_negative_run["diagnostics_path"] is not None,
        "nullable_to_nonnull_diagnostic_observed": diagnostic_matches(nullability_negative_run["diagnostics"], "O3S227", 9, 23),
        "protocol_method_nullability_negative_fixture_fails_closed": protocol_method_nullability_negative_run["exit_code"] != 0,
        "protocol_method_nullability_negative_diagnostics_json_emitted": protocol_method_nullability_negative_run["diagnostics_path"] is not None,
        "protocol_method_nullability_conflict_diagnostic_observed": diagnostic_matches(protocol_method_nullability_negative_run["diagnostics"], "O3S218", 9, 1),
        "protocol_property_nullability_negative_fixture_fails_closed": protocol_property_nullability_negative_run["exit_code"] != 0,
        "protocol_property_nullability_negative_diagnostics_json_emitted": protocol_property_nullability_negative_run["diagnostics_path"] is not None,
        "protocol_property_nullability_conflict_diagnostic_observed": diagnostic_matches(protocol_property_nullability_negative_run["diagnostics"], "O3S218", 9, 1),
        "unknown_protocol_composition_negative_fixture_fails_closed": unknown_protocol_composition_negative_run["exit_code"] != 0,
        "unknown_protocol_composition_negative_diagnostics_json_emitted": unknown_protocol_composition_negative_run["diagnostics_path"] is not None,
        "unknown_protocol_composition_diagnostic_observed": diagnostic_matches(unknown_protocol_composition_negative_run["diagnostics"], "O3S206", 4, 21),
        "protocol_qualified_unknown_message_negative_fixture_fails_closed": protocol_qualified_unknown_message_negative_run["exit_code"] != 0,
        "protocol_qualified_unknown_message_negative_diagnostics_json_emitted": protocol_qualified_unknown_message_negative_run["diagnostics_path"] is not None,
        "protocol_qualified_unknown_message_diagnostic_observed": diagnostic_matches(protocol_qualified_unknown_message_negative_run["diagnostics"], "O3S216", 18, 18),
        "typed_object_receiver_unknown_message_negative_fixture_fails_closed": typed_object_receiver_unknown_message_negative_run["exit_code"] != 0,
        "typed_object_receiver_unknown_message_negative_diagnostics_json_emitted": typed_object_receiver_unknown_message_negative_run["diagnostics_path"] is not None,
        "typed_object_receiver_unknown_message_diagnostic_observed": diagnostic_matches(typed_object_receiver_unknown_message_negative_run["diagnostics"], "O3S216", 18, 18),
        "generic_constraint_violation_negative_fixture_fails_closed": generic_constraint_violation_negative_run["exit_code"] != 0,
        "generic_constraint_violation_negative_diagnostics_json_emitted": generic_constraint_violation_negative_run["diagnostics_path"] is not None,
        "generic_constraint_violation_diagnostic_observed": diagnostic_matches(generic_constraint_violation_negative_run["diagnostics"], "O3S206", 29, 50),
        "generic_substitution_unknown_message_negative_fixture_fails_closed": generic_substitution_unknown_message_negative_run["exit_code"] != 0,
        "generic_substitution_unknown_message_negative_diagnostics_json_emitted": generic_substitution_unknown_message_negative_run["diagnostics_path"] is not None,
        "generic_substitution_unknown_message_diagnostic_observed": diagnostic_matches(generic_substitution_unknown_message_negative_run["diagnostics"], "O3S216", 23, 18),
        "nested_generic_constraint_violation_negative_fixture_fails_closed": nested_generic_constraint_violation_negative_run["exit_code"] != 0,
        "nested_generic_constraint_violation_negative_diagnostics_json_emitted": nested_generic_constraint_violation_negative_run["diagnostics_path"] is not None,
        "nested_generic_constraint_violation_diagnostic_observed": diagnostic_matches(nested_generic_constraint_violation_negative_run["diagnostics"], "O3S206", 33, 12),
        "generic_invariant_assignment_negative_fixture_fails_closed": generic_invariant_assignment_negative_run["exit_code"] != 0,
        "generic_invariant_assignment_negative_diagnostics_json_emitted": generic_invariant_assignment_negative_run["diagnostics_path"] is not None,
        "generic_invariant_assignment_diagnostic_observed": diagnostic_matches(generic_invariant_assignment_negative_run["diagnostics"], "O3S206", 30, 26),
        "protocol_generic_unknown_protocol_negative_fixture_fails_closed": protocol_generic_unknown_protocol_negative_run["exit_code"] != 0,
        "protocol_generic_unknown_protocol_negative_diagnostics_json_emitted": protocol_generic_unknown_protocol_negative_run["diagnostics_path"] is not None,
        "protocol_generic_unknown_protocol_diagnostic_observed": diagnostic_matches(protocol_generic_unknown_protocol_negative_run["diagnostics"], "O3S206", 10, 12),
    }

    conformance_checks = {
        "semantic_manifest_indexes_typ_8013_01": "TYP-8013-01.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_02": "TYP-8013-02.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_03": "TYP-8013-03.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_04": "TYP-8013-04.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_05": "TYP-8013-05.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_06": "TYP-8013-06.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_07": "TYP-8013-07.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_08": "TYP-8013-08.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_09": "TYP-8013-09.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_10": "TYP-8013-10.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_11": "TYP-8013-11.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_12": "TYP-8013-12.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_13": "TYP-8013-13.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_14": "TYP-8013-14.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_15": "TYP-8013-15.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_16": "TYP-8013-16.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_17": "TYP-8013-17.json" in manifest_text,
        "semantic_manifest_indexes_typ_8013_18": "TYP-8013-18.json" in manifest_text,
        "semantic_readme_mentions_issue_8013": "#8013" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_nested_generic_positive_fixture": rel(NESTED_GENERIC_POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_generic_variance_positive_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_protocol_generic_positive_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_nullability_negative_fixture": rel(NULLABILITY_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_protocol_method_nullability_negative_fixture": rel(PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_protocol_property_nullability_negative_fixture": rel(PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_unknown_protocol_composition_negative_fixture": rel(UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_protocol_qualified_unknown_message_negative_fixture": rel(PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_typed_object_receiver_unknown_message_negative_fixture": rel(TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_generic_constraint_violation_negative_fixture": rel(GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_generic_substitution_unknown_message_negative_fixture": rel(GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_nested_generic_constraint_violation_negative_fixture": rel(NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_generic_invariant_assignment_negative_fixture": rel(GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_protocol_generic_unknown_protocol_negative_fixture": rel(PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "nested_generic_positive_conformance_references_fixture": rel(NESTED_GENERIC_POSITIVE_FIXTURE) in conformance_nested_generic_positive.get("references", []),
        "generic_variance_positive_conformance_references_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE) in conformance_generic_variance_positive.get("references", []),
        "protocol_generic_positive_conformance_references_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE) in conformance_protocol_generic_positive.get("references", []),
        "cross_module_generic_positive_conformance_references_provider_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE) in conformance_cross_module_generic_positive.get("references", []),
        "cross_module_generic_positive_conformance_references_consumer_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE) in conformance_cross_module_generic_positive.get("references", []),
        "cross_module_generic_positive_conformance_references_runtime_import_surface": rel(RUNTIME_IMPORT_SURFACE) in conformance_cross_module_generic_positive.get("references", []),
        "cross_module_protocol_positive_conformance_references_provider_fixture": rel(POSITIVE_FIXTURE) in conformance_cross_module_protocol_positive.get("references", []),
        "cross_module_protocol_positive_conformance_references_consumer_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE) in conformance_cross_module_protocol_positive.get("references", []),
        "cross_module_protocol_positive_conformance_references_runtime_import_surface": rel(RUNTIME_IMPORT_SURFACE) in conformance_cross_module_protocol_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "nullability_negative_conformance_references_fixture": rel(NULLABILITY_NEGATIVE_FIXTURE) in conformance_nullability_negative.get("references", []),
        "protocol_method_nullability_negative_conformance_references_fixture": rel(PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE) in conformance_protocol_method_nullability_negative.get("references", []),
        "protocol_property_nullability_negative_conformance_references_fixture": rel(PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE) in conformance_protocol_property_nullability_negative.get("references", []),
        "unknown_protocol_composition_negative_conformance_references_fixture": rel(UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE) in conformance_unknown_protocol_composition_negative.get("references", []),
        "protocol_qualified_unknown_message_negative_conformance_references_fixture": rel(PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in conformance_protocol_qualified_unknown_message_negative.get("references", []),
        "typed_object_receiver_unknown_message_negative_conformance_references_fixture": rel(TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in conformance_typed_object_receiver_unknown_message_negative.get("references", []),
        "generic_constraint_violation_negative_conformance_references_fixture": rel(GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE) in conformance_generic_constraint_violation_negative.get("references", []),
        "generic_substitution_unknown_message_negative_conformance_references_fixture": rel(GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE) in conformance_generic_substitution_unknown_message_negative.get("references", []),
        "nested_generic_constraint_violation_negative_conformance_references_fixture": rel(NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE) in conformance_nested_generic_constraint_violation_negative.get("references", []),
        "generic_invariant_assignment_negative_conformance_references_fixture": rel(GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE) in conformance_generic_invariant_assignment_negative.get("references", []),
        "protocol_generic_unknown_protocol_negative_conformance_references_fixture": rel(PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE) in conformance_protocol_generic_unknown_protocol_negative.get("references", []),
        "negative_conformance_expects_o3s206_location": conformance_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 7, "column": 21}],
        "nullability_negative_conformance_expects_o3s227_location": conformance_nullability_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S227", "line": 9, "column": 23}],
        "protocol_method_nullability_negative_conformance_expects_o3s218_location": conformance_protocol_method_nullability_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S218", "line": 9, "column": 1}],
        "protocol_property_nullability_negative_conformance_expects_o3s218_location": conformance_protocol_property_nullability_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S218", "line": 9, "column": 1}],
        "unknown_protocol_composition_negative_conformance_expects_o3s206_location": conformance_unknown_protocol_composition_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 4, "column": 21}],
        "protocol_qualified_unknown_message_negative_conformance_expects_o3s216_location": conformance_protocol_qualified_unknown_message_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S216", "line": 18, "column": 18}],
        "typed_object_receiver_unknown_message_negative_conformance_expects_o3s216_location": conformance_typed_object_receiver_unknown_message_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S216", "line": 18, "column": 18}],
        "generic_constraint_violation_negative_conformance_expects_o3s206_location": conformance_generic_constraint_violation_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 29, "column": 50}],
        "generic_substitution_unknown_message_negative_conformance_expects_o3s216_location": conformance_generic_substitution_unknown_message_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S216", "line": 23, "column": 18}],
        "nested_generic_constraint_violation_negative_conformance_expects_o3s206_location": conformance_nested_generic_constraint_violation_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 33, "column": 12}],
        "generic_invariant_assignment_negative_conformance_expects_o3s206_location": conformance_generic_invariant_assignment_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 30, "column": 26}],
        "protocol_generic_unknown_protocol_negative_conformance_expects_o3s206_location": conformance_protocol_generic_unknown_protocol_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S206", "line": 10, "column": 12}],
        "stress_manifest_compiles_positive_fixture": rel(POSITIVE_FIXTURE) in stress_manifest_text,
        "stress_manifest_compiles_nested_generic_positive_fixture": rel(NESTED_GENERIC_POSITIVE_FIXTURE) in stress_manifest_text,
        "stress_manifest_compiles_generic_variance_positive_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE) in stress_manifest_text,
        "stress_manifest_compiles_protocol_generic_positive_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE) in stress_manifest_text,
        "no_tmp_source_truth": no_tmp_source_truth,
    }

    checks = {
        **positive_checks,
        **nested_generic_positive_checks,
        **generic_variance_positive_checks,
        **protocol_generic_positive_checks,
        **cross_module_generic_checks,
        **cross_module_generic_drift_checks,
        **cross_module_nullability_checks,
        **cross_module_nullability_drift_checks,
        **cross_module_protocol_checks,
        **cross_module_protocol_drift_checks,
        **negative_checks,
        **conformance_checks,
        "static_sema_contract_fields_present": all(static_presence["sema_contract_fields"].values()),
        "static_semantic_pass_sources_present": all(static_presence["semantic_pass_sources_and_replay_key"].values()),
        "static_artifact_json_fields_present": all(static_presence["artifact_json_fields"].values()),
        "static_runtime_import_surface_generic_contract_present": all(static_presence["runtime_import_surface_generic_contract"].values()),
        "static_runtime_import_surface_nullability_contract_present": all(static_presence["runtime_import_surface_nullability_contract"].values()),
        "static_runtime_import_surface_protocol_contract_present": all(static_presence["runtime_import_surface_protocol_contract"].values()),
        "runtime_lowering_and_ir_source_refs_exist": LOWERING_CONTRACT.is_file() and IR_EMITTER.is_file(),
    }
    status = "PASS" if all(checks.values()) else "FAIL"

    return {
        "contract_id": CONTRACT_ID,
        "issue": ISSUE,
        "status": status,
        "checks": checks,
        "static_presence": static_presence,
        "source_truth_paths": [rel(path) for path in source_truth_paths],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "nested_generic_positive_fixture": rel(NESTED_GENERIC_POSITIVE_FIXTURE),
        "generic_variance_positive_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE),
        "protocol_generic_positive_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE),
        "cross_module_generic_provider_fixture": rel(PROTOCOL_GENERIC_POSITIVE_FIXTURE),
        "cross_module_generic_consumer_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE),
        "cross_module_protocol_provider_fixture": rel(POSITIVE_FIXTURE),
        "cross_module_protocol_consumer_fixture": rel(GENERIC_VARIANCE_POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "nullability_negative_fixture": rel(NULLABILITY_NEGATIVE_FIXTURE),
        "protocol_method_nullability_negative_fixture": rel(PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE),
        "protocol_property_nullability_negative_fixture": rel(PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE),
        "unknown_protocol_composition_negative_fixture": rel(UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE),
        "protocol_qualified_unknown_message_negative_fixture": rel(PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "typed_object_receiver_unknown_message_negative_fixture": rel(TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "generic_constraint_violation_negative_fixture": rel(GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE),
        "generic_substitution_unknown_message_negative_fixture": rel(GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "nested_generic_constraint_violation_negative_fixture": rel(NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE),
        "generic_invariant_assignment_negative_fixture": rel(GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE),
        "protocol_generic_unknown_protocol_negative_fixture": rel(PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE),
        "positive_compile": {key: value for key, value in positive_run.items() if key != "manifest"},
        "nested_generic_positive_compile": {key: value for key, value in nested_generic_positive_run.items() if key != "manifest"},
        "generic_variance_positive_compile": {key: value for key, value in generic_variance_positive_run.items() if key != "manifest"},
        "protocol_generic_positive_compile": {key: value for key, value in protocol_generic_positive_run.items() if key != "manifest"},
        "cross_module_generic_consumer_compile": {key: value for key, value in cross_module_generic_consumer_run.items() if key != "manifest"},
        "cross_module_generic_drift_compile": {key: value for key, value in cross_module_generic_drift_run.items() if key != "manifest"},
        "cross_module_nullability_consumer_compile": {key: value for key, value in cross_module_nullability_consumer_run.items() if key != "manifest"},
        "cross_module_nullability_drift_compile": {key: value for key, value in cross_module_nullability_drift_run.items() if key != "manifest"},
        "cross_module_protocol_consumer_compile": {key: value for key, value in cross_module_nullability_consumer_run.items() if key != "manifest"},
        "cross_module_protocol_drift_compile": {key: value for key, value in cross_module_protocol_drift_run.items() if key != "manifest"},
        "negative_compile": {key: value for key, value in negative_run.items() if key != "manifest"},
        "nullability_negative_compile": {key: value for key, value in nullability_negative_run.items() if key != "manifest"},
        "protocol_method_nullability_negative_compile": {key: value for key, value in protocol_method_nullability_negative_run.items() if key != "manifest"},
        "protocol_property_nullability_negative_compile": {key: value for key, value in protocol_property_nullability_negative_run.items() if key != "manifest"},
        "unknown_protocol_composition_negative_compile": {key: value for key, value in unknown_protocol_composition_negative_run.items() if key != "manifest"},
        "protocol_qualified_unknown_message_negative_compile": {key: value for key, value in protocol_qualified_unknown_message_negative_run.items() if key != "manifest"},
        "typed_object_receiver_unknown_message_negative_compile": {key: value for key, value in typed_object_receiver_unknown_message_negative_run.items() if key != "manifest"},
        "generic_constraint_violation_negative_compile": {key: value for key, value in generic_constraint_violation_negative_run.items() if key != "manifest"},
        "generic_substitution_unknown_message_negative_compile": {key: value for key, value in generic_substitution_unknown_message_negative_run.items() if key != "manifest"},
        "nested_generic_constraint_violation_negative_compile": {key: value for key, value in nested_generic_constraint_violation_negative_run.items() if key != "manifest"},
        "generic_invariant_assignment_negative_compile": {key: value for key, value in generic_invariant_assignment_negative_run.items() if key != "manifest"},
        "protocol_generic_unknown_protocol_negative_compile": {key: value for key, value in protocol_generic_unknown_protocol_negative_run.items() if key != "manifest"},
        "type_semantic_model": model,
        "required_summary_fields": SUMMARY_FIELDS,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "zero_contract_violation_fields": ZERO_FIELDS,
        "required_replay_key_segments": REPLAY_KEY_SEGMENTS,
        "validation_commands": [
            "python scripts/build_objc3c_type_semantic_model_closure.py --check",
            "python -m pytest tests/tooling/test_build_objc3c_type_semantic_model_closure.py",
            "npm run objc3c -- test-execution-replay",
            "npm run objc3c -- test-lowering-runtime-stress",
            "npm run objc3c -- test-full",
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    add_check_argument(parser)
    args = parser.parse_args()
    summary = build_summary()
    expected_json, expected_md = expected_report_outputs(summary)
    if args.check:
        if not JSON_OUT.is_file() or JSON_OUT.read_text(encoding="utf-8") != expected_json:
            raise SystemExit(f"{rel(JSON_OUT)} is stale; run this script without --check")
        if not MD_OUT.is_file() or MD_OUT.read_text(encoding="utf-8") != expected_md:
            raise SystemExit(f"{rel(MD_OUT)} is stale; run this script without --check")
        if summary["status"] != "PASS":
            raise SystemExit("type semantic model closure summary failed")
        print(f"status: {summary['status']}")
        print(f"summary_path: {rel(JSON_OUT)}")
        return 0
    write_outputs(summary, json_path=JSON_OUT, markdown_path=MD_OUT)
    print(f"wrote: {rel(JSON_OUT)}")
    print(f"wrote: {rel(MD_OUT)}")
    print(f"status: {summary['status']}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
