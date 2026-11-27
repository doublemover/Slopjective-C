from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
GENERIC_CALLABLE_REIFICATION_CONTRACT_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "generic_callable_reification_contract.json"
)
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "

EXPECTED_GENERIC_ROWS = {
    "objc3c.behavior.language.generics.protocol-qualified-arguments": {
        "capability_id": "language.generics.protocol-qualified-arguments",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-protocol-generic-positive",
        "required_positive": {
            "tests/conformance/semantic/TYP-8013-15.json",
            "scripts/objc3c_type_semantic_model_closure/positive.py",
            "native/objc3c/src/sema/objc3_semantic_passes_generic_protocol_message_validation.inc",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_generic_unknown_protocol.objc3",
            "tests/conformance/semantic/TYP-8013-09.json",
        },
        "required_codes": {"O3S206"},
    },
    "objc3c.behavior.language.generics.callable-type-parameters": {
        "capability_id": "language.generics.callable-type-parameters",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_generic_method_substitution_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-callable-generic-contract",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_generic_function_positive.objc3",
            "tests/conformance/semantic/TYP-8013-23.json",
            "tests/conformance/semantic/TYP-8013-24.json",
            "scripts/objc3c_type_semantic_model_closure/positive.py",
            "native/objc3c/src/sema/model/language_semantics_concrete_contracts.h",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_function_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_function_unresolved_return.objc3",
            "tests/conformance/semantic/TYP-8013-09.json",
        },
        "required_codes": {"O3S206"},
    },
    "objc3c.behavior.language.generics.generic-callable-reification": {
        "capability_id": "language.generics.generic-callable-reification",
        "owner_phase": "sema",
        "behavior_fixture": (
            "tests/tooling/fixtures/native/"
            "type_semantic_generic_reified_objc_method_positive.objc3"
        ),
        "runtime_acceptance_case": "type-semantic-model-generic-callable-reification-contract",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_generic_reified_function_positive.objc3",
            "tests/tooling/fixtures/native/type_semantic_generic_objc_method_positive.objc3",
            "tests/tooling/fixtures/native/type_semantic_generic_function_positive.objc3",
            "tests/tooling/fixtures/native/generic_callable_reification_contract.json",
            "schemas/objc3c-generic-callable-model-v1.schema.json",
            "native/objc3c/src/sema/objc3_semantic_passes_generic_callable_contracts.inc",
            "native/objc3c/src/sema/objc3_sema_contract_semantic_type_metadata_method_records.h",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_generic_method_selector_form_ambiguous.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_reify_generics_unsupported_scope.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_method_override_mismatch.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_callable_constraint_cycle.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_erased_to_reified_redeclaration_drift.objc3",
            "tests/tooling/fixtures/native/generic_callable_cross_module_mangling_policy_mismatch.json",
        },
        "required_codes": {"O3P114", "O3S206"},
    },
    "objc3c.behavior.language.generics.variance-specialization": {
        "capability_id": "language.generics.variance-specialization",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-generic-variance-positive",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3",
            "tests/conformance/semantic/TYP-8013-12.json",
            "tests/conformance/semantic/TYP-8013-13.json",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3",
        },
        "required_codes": {"O3S206", "O3S216"},
    },
    "objc3c.behavior.runtime.generics.cross-module-metadata": {
        "capability_id": "runtime.generics.cross-module-metadata",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-cross-module-generic-contract",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
            "tests/conformance/semantic/TYP-8013-17.json",
            "native/objc3c/src/pipeline/runtime_import_type_system_preservation_generic.cpp",
            "tests/tooling/fixtures/objc3c/validation_generic_metadata_abi_contract/replay_run_1/module.manifest.json",
            "tests/tooling/fixtures/objc3c/validation_lightweight_generics_constraints_contract/replay_run_1/module.manifest.json",
        },
        "required_negative": {
            "scripts/objc3c_type_semantic_model_closure/cross_module.py",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
        },
        "required_codes": {"O3S206"},
    },
}

EXPECTED_GENERIC_FIXTURES = {
    "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/type_semantic_generic_method_substitution_positive.objc3": (
        "positive",
        "",
    ),
    "tests/tooling/fixtures/native/type_semantic_generic_function_positive.objc3": (
        "positive",
        "",
    ),
    "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3": (
        "diagnostic_negative",
        "O3S216",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_function_constraint_violation.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_function_unresolved_return.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_generic_unknown_protocol.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_reify_generics_marker_reserved.objc3": (
        "parser",
        "canonical_rejection",
        "O3P114",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_generic_method_type_parameter_clause_reserved.objc3": (
        "parser",
        "canonical_rejection",
        "O3P114",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_cstyle_generic_function_reserved.objc3": (
        "parser",
        "canonical_rejection",
        "O3P114",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_function_signature_drift.objc3": (
        "sema",
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_generic_method_selector_form_ambiguous.objc3": (
        "parser",
        "canonical_rejection",
        "O3P114",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_reify_generics_unsupported_scope.objc3": (
        "parser",
        "canonical_rejection",
        "O3P114",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_method_override_mismatch.objc3": (
        "sema",
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_callable_constraint_cycle.objc3": (
        "sema",
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_erased_to_reified_redeclaration_drift.objc3": (
        "sema",
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/generic_callable_cross_module_mangling_policy_mismatch.json": (
        "artifact_manifest",
        "diagnostic_negative",
        "O3S206",
    ),
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_generic_fixture_manifest_rows_are_canonical_and_diagnostic_specific() -> None:
    manifest = _read_json(MANIFEST_PATH)
    fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    for fixture_path, expected in EXPECTED_GENERIC_FIXTURES.items():
        if len(expected) == 2:
            owner_phase = "sema"
            fixture_kind, diagnostic_code = expected
        else:
            owner_phase, fixture_kind, diagnostic_code = expected
        fixture = fixtures[fixture_path]
        assert fixture["origin"] == "hand-authored"
        assert fixture["owner_phase"] == owner_phase
        assert fixture["behavior_family"] == "generics"
        assert fixture["fixture_kind"] == fixture_kind
        assert fixture["expected_diagnostic_code"] == diagnostic_code
        _assert_repo_path_exists(fixture_path)
