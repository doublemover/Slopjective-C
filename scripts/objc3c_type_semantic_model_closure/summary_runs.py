from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.compiler import run_compiler
from objc3c_type_semantic_model_closure.cross_module import write_drifted_generic_contract_surface
from objc3c_type_semantic_model_closure.cross_module import write_drifted_nullability_contract_surface
from objc3c_type_semantic_model_closure.cross_module import write_drifted_protocol_contract_surface
from objc3c_type_semantic_model_closure.paths import COMPILER
from objc3c_type_semantic_model_closure.paths import GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_VARIANCE_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NESTED_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_CATEGORY_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_POSITIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import ROOT
from objc3c_type_semantic_model_closure.paths import TMP_ROOT
from objc3c_type_semantic_model_closure.paths import TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE


def _runtime_import_args(surface_path: object) -> list[str]:
    return [
        "--objc3-import-runtime-surface",
        str(surface_path),
        "--objc3-bootstrap-registration-order-ordinal",
        "2",
    ]


def compile_runtime_runs() -> dict[str, dict[str, Any]]:
    positive_run = run_compiler(ROOT, COMPILER, POSITIVE_FIXTURE, TMP_ROOT / "positive")
    nested_generic_positive_run = run_compiler(
        ROOT,
        COMPILER,
        NESTED_GENERIC_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-nested-generic",
    )
    generic_variance_positive_run = run_compiler(
        ROOT,
        COMPILER,
        GENERIC_VARIANCE_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-generic-variance",
    )
    protocol_generic_positive_run = run_compiler(
        ROOT,
        COMPILER,
        PROTOCOL_GENERIC_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-protocol-generic",
    )
    protocol_category_positive_run = run_compiler(
        ROOT,
        COMPILER,
        PROTOCOL_CATEGORY_POSITIVE_FIXTURE,
        TMP_ROOT / "positive-protocol-category",
    )

    cross_module_nullability_drift_surface = write_drifted_nullability_contract_surface(
        ROOT,
        TMP_ROOT,
        positive_run,
    )
    cross_module_protocol_drift_surface = write_drifted_protocol_contract_surface(
        ROOT,
        TMP_ROOT,
        positive_run,
    )
    cross_module_generic_drift_surface = write_drifted_generic_contract_surface(
        ROOT,
        TMP_ROOT,
        protocol_generic_positive_run,
    )

    return {
        "positive_run": positive_run,
        "nested_generic_positive_run": nested_generic_positive_run,
        "generic_variance_positive_run": generic_variance_positive_run,
        "protocol_generic_positive_run": protocol_generic_positive_run,
        "protocol_category_positive_run": protocol_category_positive_run,
        "cross_module_nullability_consumer_run": run_compiler(
            ROOT,
            COMPILER,
            GENERIC_VARIANCE_POSITIVE_FIXTURE,
            TMP_ROOT / "positive-cross-module-nullability-consumer",
            _runtime_import_args(TMP_ROOT / "positive" / "module.runtime-import-surface.json"),
        ),
        "cross_module_nullability_drift_run": run_compiler(
            ROOT,
            COMPILER,
            GENERIC_VARIANCE_POSITIVE_FIXTURE,
            TMP_ROOT / "negative-cross-module-nullability-drift",
            _runtime_import_args(cross_module_nullability_drift_surface),
        ),
        "cross_module_protocol_drift_run": run_compiler(
            ROOT,
            COMPILER,
            PROTOCOL_CATEGORY_POSITIVE_FIXTURE,
            TMP_ROOT / "negative-cross-module-protocol-drift",
            _runtime_import_args(cross_module_protocol_drift_surface),
        ),
        "cross_module_protocol_consumer_run": run_compiler(
            ROOT,
            COMPILER,
            PROTOCOL_CATEGORY_POSITIVE_FIXTURE,
            TMP_ROOT / "positive-cross-module-protocol-consumer",
            _runtime_import_args(TMP_ROOT / "positive" / "module.runtime-import-surface.json"),
        ),
        "cross_module_generic_consumer_run": run_compiler(
            ROOT,
            COMPILER,
            GENERIC_VARIANCE_POSITIVE_FIXTURE,
            TMP_ROOT / "positive-cross-module-generic-consumer",
            _runtime_import_args(
                TMP_ROOT / "positive-protocol-generic" / "module.runtime-import-surface.json"
            ),
        ),
        "cross_module_generic_drift_run": run_compiler(
            ROOT,
            COMPILER,
            GENERIC_VARIANCE_POSITIVE_FIXTURE,
            TMP_ROOT / "negative-cross-module-generic-drift",
            _runtime_import_args(cross_module_generic_drift_surface),
        ),
        "negative_run": run_compiler(ROOT, COMPILER, NEGATIVE_FIXTURE, TMP_ROOT / "negative-duplicate-protocol"),
        "nullability_negative_run": run_compiler(ROOT, COMPILER, NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-nullability-flow"),
        "protocol_method_nullability_negative_run": run_compiler(ROOT, COMPILER, PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-method-nullability"),
        "protocol_property_nullability_negative_run": run_compiler(ROOT, COMPILER, PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-property-nullability"),
        "protocol_optional_required_conflict_negative_run": run_compiler(ROOT, COMPILER, PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-optional-required-conflict"),
        "unknown_protocol_composition_negative_run": run_compiler(ROOT, COMPILER, UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-unknown-protocol-composition"),
        "protocol_qualified_unknown_message_negative_run": run_compiler(ROOT, COMPILER, PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-qualified-unknown-message"),
        "typed_object_receiver_unknown_message_negative_run": run_compiler(ROOT, COMPILER, TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-typed-object-receiver-unknown-message"),
        "generic_constraint_violation_negative_run": run_compiler(ROOT, COMPILER, GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-constraint-violation"),
        "generic_substitution_unknown_message_negative_run": run_compiler(ROOT, COMPILER, GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-substitution-unknown-message"),
        "nested_generic_constraint_violation_negative_run": run_compiler(ROOT, COMPILER, NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE, TMP_ROOT / "negative-nested-generic-constraint-violation"),
        "generic_invariant_assignment_negative_run": run_compiler(ROOT, COMPILER, GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE, TMP_ROOT / "negative-generic-invariant-assignment"),
        "protocol_generic_unknown_protocol_negative_run": run_compiler(ROOT, COMPILER, PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE, TMP_ROOT / "negative-protocol-generic-unknown-protocol"),
    }


__all__ = ["compile_runtime_runs"]
