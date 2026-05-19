from __future__ import annotations

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_CROSS_MODULE_GENERIC_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_CROSS_MODULE_PROTOCOL_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_GENERIC_VARIANCE_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_NESTED_GENERIC_CONSTRAINT_VIOLATION_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_NESTED_GENERIC_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_NULLABILITY_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_PROTOCOL_GENERIC_POSITIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_PROTOCOL_GENERIC_UNKNOWN_PROTOCOL_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_PROTOCOL_METHOD_NULLABILITY_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_PROTOCOL_PROPERTY_NULLABILITY_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_PROTOCOL_QUALIFIED_UNKNOWN_MESSAGE_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE
from objc3c_type_semantic_model_closure.paths import CONFORMANCE_UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE
from objc3c_type_semantic_model_closure.paths import GENERIC_CONSTRAINT_VIOLATION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_INVARIANT_ASSIGNMENT_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_SUBSTITUTION_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import GENERIC_VARIANCE_POSITIVE_FIXTURE
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
from objc3c_type_semantic_model_closure.paths import RUNTIME_IMPORT_SURFACE
from objc3c_type_semantic_model_closure.paths import SEMANTIC_MANIFEST
from objc3c_type_semantic_model_closure.paths import SEMANTIC_README
from objc3c_type_semantic_model_closure.paths import SOURCE_TRUTH_PATHS
from objc3c_type_semantic_model_closure.paths import STRESS_MANIFEST
from objc3c_type_semantic_model_closure.paths import TYPED_OBJECT_RECEIVER_UNKNOWN_MESSAGE_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import UNKNOWN_PROTOCOL_COMPOSITION_NEGATIVE_FIXTURE
from objc3c_type_semantic_model_closure.paths import read
from objc3c_type_semantic_model_closure.paths import rel


def compile_conformance_checks() -> dict[str, bool]:
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
    return {
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
        "no_tmp_source_truth": all(not rel(path).startswith("tmp/") for path in SOURCE_TRUTH_PATHS),
    }
