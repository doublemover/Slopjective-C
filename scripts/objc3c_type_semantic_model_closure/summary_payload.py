from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.paths import CONTRACT_ID
from objc3c_type_semantic_model_closure.paths import GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_VARIANCE_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import ISSUE
from objc3c_type_semantic_model_closure.paths import NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import SOURCE_TRUTH_PATHS
from objc3c_type_semantic_model_closure.paths import TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import rel
from objc3c_type_semantic_model_closure.positive import POSITIVE_MIN_COUNTS
from objc3c_type_semantic_model_closure.positive import REPLAY_KEY_SEGMENTS
from objc3c_type_semantic_model_closure.positive import ZERO_FIELDS
from objc3c_type_semantic_model_closure.reporting import SUMMARY_FIELDS


def _without_manifest(run: dict[str, Any]) -> dict[str, Any]:
    return {key: value for key, value in run.items() if key != "manifest"}


def build_summary_payload(
    *,
    runs: dict[str, dict[str, Any]],
    model: dict[str, Any],
    checks: dict[str, bool],
    static_presence: dict[str, Any],
    status: str,
) -> dict[str, Any]:
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
        "protocol_optional_required_conflict_negative_fixture": rel(PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE),
        "unknown_protocol_composition_negative_fixture": rel(UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE),
        "protocol_qualified_unknown_message_negative_fixture": rel(PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "typed_object_receiver_unknown_message_negative_fixture": rel(TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "generic_constraint_violation_negative_fixture": rel(GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE),
        "generic_substitution_unknown_message_negative_fixture": rel(GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE),
        "nested_generic_constraint_violation_negative_fixture": rel(NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE),
        "generic_invariant_assignment_negative_fixture": rel(GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE),
        "protocol_generic_unknown_protocol_negative_fixture": rel(PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE),
        "positive_compile": _without_manifest(runs["positive_run"]),
        "nested_generic_positive_compile": _without_manifest(runs["nested_generic_positive_run"]),
        "generic_variance_positive_compile": _without_manifest(runs["generic_variance_positive_run"]),
        "protocol_generic_positive_compile": _without_manifest(runs["protocol_generic_positive_run"]),
        "cross_module_generic_consumer_compile": _without_manifest(runs["cross_module_generic_consumer_run"]),
        "cross_module_generic_drift_compile": _without_manifest(runs["cross_module_generic_drift_run"]),
        "cross_module_nullability_consumer_compile": _without_manifest(runs["cross_module_nullability_consumer_run"]),
        "cross_module_nullability_drift_compile": _without_manifest(runs["cross_module_nullability_drift_run"]),
        "cross_module_protocol_consumer_compile": _without_manifest(runs["cross_module_nullability_consumer_run"]),
        "cross_module_protocol_drift_compile": _without_manifest(runs["cross_module_protocol_drift_run"]),
        "negative_compile": _without_manifest(runs["negative_run"]),
        "nullability_negative_compile": _without_manifest(runs["nullability_negative_run"]),
        "protocol_method_nullability_negative_compile": _without_manifest(runs["protocol_method_nullability_negative_run"]),
        "protocol_property_nullability_negative_compile": _without_manifest(runs["protocol_property_nullability_negative_run"]),
        "protocol_optional_required_conflict_negative_compile": _without_manifest(runs["protocol_optional_required_conflict_negative_run"]),
        "unknown_protocol_composition_negative_compile": _without_manifest(runs["unknown_protocol_composition_negative_run"]),
        "protocol_qualified_unknown_message_negative_compile": _without_manifest(runs["protocol_qualified_unknown_message_negative_run"]),
        "typed_object_receiver_unknown_message_negative_compile": _without_manifest(runs["typed_object_receiver_unknown_message_negative_run"]),
        "generic_constraint_violation_negative_compile": _without_manifest(runs["generic_constraint_violation_negative_run"]),
        "generic_substitution_unknown_message_negative_compile": _without_manifest(runs["generic_substitution_unknown_message_negative_run"]),
        "nested_generic_constraint_violation_negative_compile": _without_manifest(runs["nested_generic_constraint_violation_negative_run"]),
        "generic_invariant_assignment_negative_compile": _without_manifest(runs["generic_invariant_assignment_negative_run"]),
        "protocol_generic_unknown_protocol_negative_compile": _without_manifest(runs["protocol_generic_unknown_protocol_negative_run"]),
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


__all__ = ["build_summary_payload"]
