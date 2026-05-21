from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.compiler import nested_semantic_model
from objc3c_type_semantic_model_closure.reporting import SUMMARY_FIELDS

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


def compile_generic_method_substitution_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_interfaces = canonical_metadata.get("interfaces", []) if isinstance(canonical_metadata, dict) else []
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    semantic_box = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticBox"), None)
    semantic_vault = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticVault"), None)
    consume_generic_method = next(
        (entry for entry in canonical_functions if entry.get("name") == "consumeGenericMethod"),
        None,
    )
    methods = semantic_vault.get("methods", []) if isinstance(semantic_vault, dict) else []
    peek_method = next((entry for entry in methods if entry.get("selector") == "peek"), None)
    param_types = (
        consume_generic_method.get("param_canonical_types", [])
        if isinstance(consume_generic_method, dict)
        else []
    )
    return_type = peek_method.get("return_canonical_type", {}) if isinstance(peek_method, dict) else {}
    return {
        "generic_method_substitution_positive_fixture_compiles": run["exit_code"] == 0,
        "generic_method_substitution_positive_manifest_emitted": run["manifest_path"] is not None,
        "generic_method_substitution_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "generic_method_substitution_positive_has_no_diagnostics": run["diagnostics"] == [],
        "generic_method_substitution_positive_manifest_ready": bool(
            model and model.get("ready_for_lowering_and_runtime")
        ),
        "generic_method_substitution_method_return_keeps_type_parameter": return_type.get("canonical_spelling") == "T"
        and return_type.get("object_pointer_type_name") == "T",
        "generic_method_substitution_receiver_metadata_preserves_concrete_argument": len(param_types) == 1
        and (param_types[0] or {}).get("generic_arguments_source_order") == ["SemanticBox*"],
        "generic_method_substitution_target_property_metadata_available": isinstance(semantic_box, dict)
        and any(property_entry.get("name") == "title" for property_entry in semantic_box.get("properties", [])),
    }


def compile_generic_function_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    canonical_metadata = manifest.get("semantic_canonical_type_metadata") if isinstance(manifest, dict) else None
    canonical_functions = canonical_metadata.get("functions", []) if isinstance(canonical_metadata, dict) else []
    canonical_interfaces = canonical_metadata.get("interfaces", []) if isinstance(canonical_metadata, dict) else []
    generic_identity = next(
        (entry for entry in canonical_functions if entry.get("name") == "genericIdentity"),
        None,
    )
    consume_generic = next(
        (entry for entry in canonical_functions if entry.get("name") == "consumeGenericFunction"),
        None,
    )
    semantic_box = next((entry for entry in canonical_interfaces if entry.get("name") == "SemanticBox"), None)
    identity_params = generic_identity.get("param_canonical_types", []) if isinstance(generic_identity, dict) else []
    identity_return = generic_identity.get("return_canonical_type", {}) if isinstance(generic_identity, dict) else {}
    consume_params = consume_generic.get("param_canonical_types", []) if isinstance(consume_generic, dict) else []
    return {
        "generic_function_positive_fixture_compiles": run["exit_code"] == 0,
        "generic_function_positive_manifest_emitted": run["manifest_path"] is not None,
        "generic_function_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "generic_function_positive_has_no_diagnostics": run["diagnostics"] == [],
        "generic_function_positive_manifest_ready": bool(
            model and model.get("ready_for_lowering_and_runtime")
        ),
        "generic_function_metadata_preserves_type_parameter": isinstance(generic_identity, dict)
        and generic_identity.get("generic_parameter_names_source_order") == ["T"]
        and generic_identity.get("generic_parameter_constraints_lexicographic") == [["Persistable"]],
        "generic_function_param_and_return_keep_type_parameter": len(identity_params) == 1
        and (identity_params[0] or {}).get("canonical_spelling") == "T"
        and identity_return.get("canonical_spelling") == "T",
        "generic_function_call_argument_preserves_concrete_type": len(consume_params) == 1
        and (consume_params[0] or {}).get("object_pointer_type_name") == "SemanticBox",
        "generic_function_target_property_metadata_available": isinstance(semantic_box, dict)
        and any(property_entry.get("name") == "title" for property_entry in semantic_box.get("properties", [])),
    }


def compile_protocol_category_positive_summary(run: dict[str, Any]) -> dict[str, bool]:
    manifest = run.get("manifest")
    model = nested_semantic_model(manifest)
    categories = manifest.get("categories", []) if isinstance(manifest, dict) else []
    interfaces = manifest.get("interfaces", []) if isinstance(manifest, dict) else []
    tracing_interface = next(
        (
            entry
            for entry in categories
            if entry.get("record_kind") == "interface"
            and entry.get("class_name") == "Widget"
            and entry.get("category_name") == "Tracing"
        ),
        None,
    )
    tracing_implementation = next(
        (
            entry
            for entry in categories
            if entry.get("record_kind") == "implementation"
            and entry.get("class_name") == "Widget"
            and entry.get("category_name") == "Tracing"
        ),
        None,
    )
    auxiliary_interface = next(
        (
            entry
            for entry in categories
            if entry.get("record_kind") == "interface"
            and entry.get("class_name") == "Widget"
            and entry.get("category_name") == "Auxiliary"
        ),
        None,
    )
    widget_interface = next((entry for entry in interfaces if entry.get("name") == "Widget"), None)
    derived_interface = next((entry for entry in interfaces if entry.get("name") == "Derived"), None)
    return {
        "protocol_category_positive_fixture_compiles": run["exit_code"] == 0,
        "protocol_category_positive_manifest_emitted": run["manifest_path"] is not None,
        "protocol_category_positive_llvm_ir_emitted": run["llvm_ir_path"] is not None,
        "protocol_category_positive_manifest_ready": bool(model and model.get("ready_for_lowering_and_runtime")),
        "protocol_category_positive_contract_violations_zero": bool(
            model and all(int(model.get(field, -1)) == 0 for field in ZERO_FIELDS)
        ),
        "protocol_category_interface_adopts_protocol": isinstance(tracing_interface, dict)
        and tracing_interface.get("adopted_protocols") == ["protocol:Tracer"],
        "protocol_category_interface_methods_preserved": isinstance(tracing_interface, dict)
        and int(tracing_interface.get("method_count", -1)) == 2,
        "protocol_category_implementation_pair_preserved": isinstance(tracing_implementation, dict)
        and int(tracing_implementation.get("method_count", -1)) == 2,
        "protocol_category_non_protocol_category_preserved": isinstance(auxiliary_interface, dict)
        and auxiliary_interface.get("adopted_protocols") == []
        and int(auxiliary_interface.get("method_count", -1)) == 1,
        "protocol_category_base_class_protocol_adoption_preserved": isinstance(widget_interface, dict)
        and widget_interface.get("adopted_protocols") == ["protocol:Worker"],
        "protocol_category_inherited_class_protocol_adoption_preserved": isinstance(derived_interface, dict)
        and derived_interface.get("adopted_protocols") == ["protocol:Tracer"],
    }
