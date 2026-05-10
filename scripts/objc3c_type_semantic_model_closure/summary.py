from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.compiler import run_compiler
from objc3c_type_semantic_model_closure.conformance import compile_conformance_checks
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_generic_contract_summary
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_nullability_contract_summary
from objc3c_type_semantic_model_closure.cross_module import compile_cross_module_protocol_contract_summary
from objc3c_type_semantic_model_closure.cross_module import write_drifted_generic_contract_surface
from objc3c_type_semantic_model_closure.cross_module import write_drifted_nullability_contract_surface
from objc3c_type_semantic_model_closure.cross_module import write_drifted_protocol_contract_surface
from objc3c_type_semantic_model_closure.negative import compile_negative_summary
from objc3c_type_semantic_model_closure.paths import COMPILER
from objc3c_type_semantic_model_closure.paths import CONTRACT_ID
from objc3c_type_semantic_model_closure.paths import GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_VARIANCE_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import IR_EMITTER
from objc3c_type_semantic_model_closure.paths import ISSUE
from objc3c_type_semantic_model_closure.paths import LOWERING_CONTRACT
from objc3c_type_semantic_model_closure.paths import NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import ROOT
from objc3c_type_semantic_model_closure.paths import SOURCE_TRUTH_PATHS
from objc3c_type_semantic_model_closure.paths import TMP_ROOT
from objc3c_type_semantic_model_closure.paths import TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import rel
from objc3c_type_semantic_model_closure.positive import POSITIVE_MIN_COUNTS
from objc3c_type_semantic_model_closure.positive import REPLAY_KEY_SEGMENTS
from objc3c_type_semantic_model_closure.positive import ZERO_FIELDS
from objc3c_type_semantic_model_closure.positive import compile_generic_variance_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_nested_generic_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_positive_summary
from objc3c_type_semantic_model_closure.positive import compile_protocol_generic_positive_summary
from objc3c_type_semantic_model_closure.reporting import SUMMARY_FIELDS
from objc3c_type_semantic_model_closure.static_presence import compile_static_presence


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(ROOT, COMPILER, POSITIVE_FIXTURE, TMP_ROOT / "positive")
    nested_generic_positive_run = run_compiler(ROOT, COMPILER, NESTED_GENERIC_POSITIVE_FIXTURE, TMP_ROOT / "positive-nested-generic")
    generic_variance_positive_run = run_compiler(ROOT, COMPILER, GENERIC_VARIANCE_POSITIVE_FIXTURE, TMP_ROOT / "positive-generic-variance")
    protocol_generic_positive_run = run_compiler(ROOT, COMPILER, PROTOCOL_GENERIC_POSITIVE_FIXTURE, TMP_ROOT / "positive-protocol-generic")
    cross_module_nullability_drift_surface = write_drifted_nullability_contract_surface(ROOT, TMP_ROOT, positive_run)
    cross_module_protocol_drift_surface = write_drifted_protocol_contract_surface(ROOT, TMP_ROOT, positive_run)
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
    cross_module_generic_drift_surface = write_drifted_generic_contract_surface(ROOT, TMP_ROOT, protocol_generic_positive_run)
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
        ROOT,
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
        ROOT,
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
        ROOT,
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

    static_presence = compile_static_presence()

    negative_checks = compile_negative_summary(
        negative_run=negative_run,
        nullability_negative_run=nullability_negative_run,
        protocol_method_nullability_negative_run=protocol_method_nullability_negative_run,
        protocol_property_nullability_negative_run=protocol_property_nullability_negative_run,
        unknown_protocol_composition_negative_run=unknown_protocol_composition_negative_run,
        protocol_qualified_unknown_message_negative_run=protocol_qualified_unknown_message_negative_run,
        typed_object_receiver_unknown_message_negative_run=typed_object_receiver_unknown_message_negative_run,
        generic_constraint_violation_negative_run=generic_constraint_violation_negative_run,
        generic_substitution_unknown_message_negative_run=generic_substitution_unknown_message_negative_run,
        nested_generic_constraint_violation_negative_run=nested_generic_constraint_violation_negative_run,
        generic_invariant_assignment_negative_run=generic_invariant_assignment_negative_run,
        protocol_generic_unknown_protocol_negative_run=protocol_generic_unknown_protocol_negative_run,
    )

    conformance_checks = compile_conformance_checks()

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
        "source_truth_paths": [rel(path) for path in SOURCE_TRUTH_PATHS],
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
