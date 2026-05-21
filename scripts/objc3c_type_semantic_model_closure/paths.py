from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "tmp" / "reports" / "claimability" / "type-semantic-model-closure"
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
GENERIC_METHOD_SUBSTITUTION_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_generic_method_substitution_positive.objc3"
GENERIC_FUNCTION_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "type_semantic_generic_function_positive.objc3"
PROTOCOL_CATEGORY_POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "category_attachment_protocol_runtime_library.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_duplicate_protocol_composition.objc3"
NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_nullable_to_nonnull_flow.objc3"
PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_method_nullability_conflict.objc3"
PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_property_nullability_conflict.objc3"
PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_optional_required_conflict.objc3"
UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_unknown_protocol_composition.objc3"
PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_qualified_unknown_message.objc3"
TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_typed_object_receiver_unknown_message.objc3"
GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_constraint_violation.objc3"
GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_substitution_unknown_message.objc3"
GENERIC_FUNCTION_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_function_constraint_violation.objc3"
GENERIC_FUNCTION_UNRESOLVED_RETURN_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_function_unresolved_return.objc3"
NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_nested_generic_constraint_violation.objc3"
GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_generic_invariant_assignment.objc3"
PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_type_semantic_protocol_generic_unknown_protocol.objc3"
LANGUAGE_SEMANTICS_AWAIT_OUTSIDE_ASYNC_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_language_semantics_await_outside_async.objc3"
LANGUAGE_SEMANTICS_EXECUTOR_ON_SYNC_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_language_semantics_executor_on_sync_function.objc3"
LANGUAGE_SEMANTICS_CONFLICTING_CAPTURE_OWNERSHIP_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_language_semantics_conflicting_capture_ownership.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-01.json"
CONFORMANCE_NESTED_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-12.json"
CONFORMANCE_GENERIC_VARIANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-13.json"
CONFORMANCE_PROTOCOL_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-15.json"
CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-17.json"
CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-18.json"
CONFORMANCE_PROTOCOL_CATEGORY_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-20.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-02.json"
CONFORMANCE_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-03.json"
CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-04.json"
CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-05.json"
CONFORMANCE_PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "TYP-8013-19.json"
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
RUNTIME_IMPORT_SURFACE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "runtime_import_type_system_preservation.cpp"
RUNTIME_IMPORT_SURFACE_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
RUNTIME_IMPORT_TYPE_SYSTEM_PRESERVATION_SOURCES = [
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "runtime_import_type_system_preservation_generic.cpp",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "runtime_import_type_system_preservation_nullability.cpp",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "runtime_import_type_system_preservation_protocol.cpp",
]
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"

SOURCE_TRUTH_PATHS = [
    POSITIVE_FIXTURE,
    NESTED_GENERIC_POSITIVE_FIXTURE,
    GENERIC_VARIANCE_POSITIVE_FIXTURE,
    PROTOCOL_GENERIC_POSITIVE_FIXTURE,
    GENERIC_METHOD_SUBSTITUTION_POSITIVE_FIXTURE,
    GENERIC_FUNCTION_POSITIVE_FIXTURE,
    PROTOCOL_CATEGORY_POSITIVE_FIXTURE,
    NEGATIVE_FIXTURE,
    NULLABILITY_NEGATIVE_FIXTURE,
    PROTOCOL_METHOD_NULLABILITY_NEGATIVE_FIXTURE,
    PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE_FIXTURE,
    PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE_FIXTURE,
    UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE,
    PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
    TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
    GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE,
    GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE,
    GENERIC_FUNCTION_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE,
    GENERIC_FUNCTION_UNRESOLVED_RETURN_NEGATIVE_FIXTURE,
    NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE,
    GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE,
    PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE_FIXTURE,
    LANGUAGE_SEMANTICS_AWAIT_OUTSIDE_ASYNC_NEGATIVE_FIXTURE,
    LANGUAGE_SEMANTICS_EXECUTOR_ON_SYNC_NEGATIVE_FIXTURE,
    LANGUAGE_SEMANTICS_CONFLICTING_CAPTURE_OWNERSHIP_NEGATIVE_FIXTURE,
    CONFORMANCE_POSITIVE,
    CONFORMANCE_NESTED_GENERIC_POSITIVE,
    CONFORMANCE_GENERIC_VARIANCE_POSITIVE,
    CONFORMANCE_PROTOCOL_GENERIC_POSITIVE,
    CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE,
    CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE,
    CONFORMANCE_PROTOCOL_CATEGORY_POSITIVE,
    CONFORMANCE_NEGATIVE,
    CONFORMANCE_NULLABILITY_NEGATIVE,
    CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE,
    CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE,
    CONFORMANCE_PROTOCOL_OPTIONAL_REQUIRED_CONFLICT_NEGATIVE,
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
    *RUNTIME_IMPORT_TYPE_SYSTEM_PRESERVATION_SOURCES,
    LOWERING_CONTRACT,
    IR_EMITTER,
]


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")
