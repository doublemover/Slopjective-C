from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.conformance import compile_conformance_checks
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_generic_contract_summary
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_nullability_contract_summary
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_protocol_contract_summary
from objc3c_type_semantic_model_closure.negative import compile_negative_summary
from objc3c_type_semantic_model_closure.paths import IR_EMITTER
from objc3c_type_semantic_model_closure.paths import LOWERING_CONTRACT
from objc3c_type_semantic_model_closure.paths import ROOT
from objc3c_type_semantic_model_closure.positive import compile_generic_variance_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_nested_generic_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_protocol_generic_positive_summary
from objc3c_type_semantic_model_closure.static_presence import compile_static_presence


def _diagnostic_text(run: dict[str, Any]) -> str:
    return (
        str(run["stderr"])
        + str(run["stdout"])
        + " ".join(str(diag.get("message", "")) for diag in run["diagnostics"])
    )


def compile_summary_checks(
    runs: dict[str, dict[str, Any]],
) -> tuple[dict[str, Any], dict[str, bool], dict[str, Any]]:
    positive_run = runs["positive_run"]
    nested_generic_positive_run = runs["nested_generic_positive_run"]
    generic_variance_positive_run = runs["generic_variance_positive_run"]
    protocol_generic_positive_run = runs["protocol_generic_positive_run"]
    cross_module_generic_consumer_run = runs["cross_module_generic_consumer_run"]
    cross_module_generic_drift_run = runs["cross_module_generic_drift_run"]
    cross_module_nullability_consumer_run = runs["cross_module_nullability_consumer_run"]
    cross_module_nullability_drift_run = runs["cross_module_nullability_drift_run"]
    cross_module_protocol_drift_run = runs["cross_module_protocol_drift_run"]

    model, positive_checks = compile_positive_summary(positive_run)
    nested_generic_positive_checks = compile_nested_generic_positive_summary(nested_generic_positive_run)
    generic_variance_positive_checks = compile_generic_variance_positive_summary(generic_variance_positive_run)
    protocol_generic_positive_checks = compile_protocol_generic_positive_summary(protocol_generic_positive_run)
    cross_module_generic_checks = compile_cross_module_generic_contract_summary(
        ROOT,
        protocol_generic_positive_run,
        cross_module_generic_consumer_run,
    )
    cross_module_nullability_checks = compile_cross_module_nullability_contract_summary(
        ROOT,
        positive_run,
        cross_module_nullability_consumer_run,
    )
    cross_module_protocol_checks = compile_cross_module_protocol_contract_summary(
        ROOT,
        positive_run,
        cross_module_nullability_consumer_run,
    )
    negative_checks = compile_negative_summary(
        negative_run=runs["negative_run"],
        nullability_negative_run=runs["nullability_negative_run"],
        protocol_method_nullability_negative_run=runs["protocol_method_nullability_negative_run"],
        protocol_property_nullability_negative_run=runs["protocol_property_nullability_negative_run"],
        protocol_optional_required_conflict_negative_run=runs["protocol_optional_required_conflict_negative_run"],
        unknown_protocol_composition_negative_run=runs["unknown_protocol_composition_negative_run"],
        protocol_qualified_unknown_message_negative_run=runs["protocol_qualified_unknown_message_negative_run"],
        typed_object_receiver_unknown_message_negative_run=runs["typed_object_receiver_unknown_message_negative_run"],
        generic_constraint_violation_negative_run=runs["generic_constraint_violation_negative_run"],
        generic_substitution_unknown_message_negative_run=runs["generic_substitution_unknown_message_negative_run"],
        nested_generic_constraint_violation_negative_run=runs["nested_generic_constraint_violation_negative_run"],
        generic_invariant_assignment_negative_run=runs["generic_invariant_assignment_negative_run"],
        protocol_generic_unknown_protocol_negative_run=runs["protocol_generic_unknown_protocol_negative_run"],
    )

    static_presence = compile_static_presence()
    checks = {
        **positive_checks,
        **nested_generic_positive_checks,
        **generic_variance_positive_checks,
        **protocol_generic_positive_checks,
        **cross_module_generic_checks,
        "cross_module_generic_contract_drift_fails_closed": cross_module_generic_drift_run["exit_code"] != 0,
        "cross_module_generic_contract_drift_reports_variance_loss": "type-system generic contract preservation dropped variance annotations"
        in _diagnostic_text(cross_module_generic_drift_run),
        **cross_module_nullability_checks,
        "cross_module_nullability_contract_drift_fails_closed": cross_module_nullability_drift_run["exit_code"] != 0,
        "cross_module_nullability_contract_drift_reports_entry_loss": "type-system nullability contract preservation dropped canonical nullability entries"
        in _diagnostic_text(cross_module_nullability_drift_run),
        **cross_module_protocol_checks,
        "cross_module_protocol_contract_drift_fails_closed": cross_module_protocol_drift_run["exit_code"] != 0,
        "cross_module_protocol_contract_drift_reports_requirement_loss": "type-system protocol contract preservation dropped requirement or inheritance entries"
        in _diagnostic_text(cross_module_protocol_drift_run),
        **negative_checks,
        **compile_conformance_checks(),
        "static_sema_contract_fields_present": all(static_presence["sema_contract_fields"].values()),
        "static_semantic_pass_sources_present": all(static_presence["semantic_pass_sources_and_replay_key"].values()),
        "static_artifact_json_fields_present": all(static_presence["artifact_json_fields"].values()),
        "static_runtime_import_surface_generic_contract_present": all(static_presence["runtime_import_surface_generic_contract"].values()),
        "static_runtime_import_surface_nullability_contract_present": all(static_presence["runtime_import_surface_nullability_contract"].values()),
        "static_runtime_import_surface_protocol_contract_present": all(static_presence["runtime_import_surface_protocol_contract"].values()),
        "runtime_lowering_and_ir_source_refs_exist": LOWERING_CONTRACT.is_file() and IR_EMITTER.is_file(),
    }
    return model, checks, static_presence


__all__ = ["compile_summary_checks"]
