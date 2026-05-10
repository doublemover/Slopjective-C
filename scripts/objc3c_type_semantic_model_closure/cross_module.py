from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.json_io import load_json_any as load_json


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


def compile_cross_module_generic_contract_summary(root: Path, provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(root / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
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


def write_drifted_generic_contract_surface(root: Path, tmp_root: Path, provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    provider_surface_path = root / provider_surface_path_value
    surface = load_json(provider_surface_path)
    preservation = surface.get("objc_type_system_generic_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit generic preservation")
    preservation["generic_variance_annotation_count"] = 0
    preservation["replay_key"] = str(preservation.get("replay_key", "")).replace(
        "variance_annotations=1",
        "variance_annotations=0",
    )
    drift_dir = tmp_root / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "generic-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path


def compile_cross_module_nullability_contract_summary(root: Path, provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(root / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
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


def write_drifted_nullability_contract_surface(root: Path, tmp_root: Path, provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    surface = load_json(root / provider_surface_path_value)
    preservation = surface.get("objc_type_system_nullability_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit nullability preservation")
    preservation["unspecified_nullability_entry_count"] = max(
        0,
        int(preservation.get("unspecified_nullability_entry_count", 0)) - 1,
    )
    drift_dir = tmp_root / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "nullability-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path


def compile_cross_module_protocol_contract_summary(root: Path, provider_run: dict[str, Any], consumer_run: dict[str, Any]) -> dict[str, bool]:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    provider_surface = load_json(root / provider_surface_path_value) if isinstance(provider_surface_path_value, str) else None
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


def write_drifted_protocol_contract_surface(root: Path, tmp_root: Path, provider_run: dict[str, Any]) -> Path:
    provider_surface_path_value = provider_run.get("runtime_import_surface_path")
    if not isinstance(provider_surface_path_value, str):
        raise RuntimeError("provider did not emit a runtime import surface")
    surface = load_json(root / provider_surface_path_value)
    preservation = surface.get("objc_type_system_protocol_contract_preservation")
    if not isinstance(preservation, dict):
        raise RuntimeError("provider runtime import surface did not emit protocol preservation")
    preservation["protocol_required_method_count"] = max(
        0,
        int(preservation.get("protocol_required_method_count", 0)) - 1,
    )
    drift_dir = tmp_root / "drifted-surfaces"
    drift_dir.mkdir(parents=True, exist_ok=True)
    drift_path = drift_dir / "protocol-contract-drift.runtime-import-surface.json"
    drift_path.write_text(canonical_json(surface), encoding="utf-8")
    return drift_path
