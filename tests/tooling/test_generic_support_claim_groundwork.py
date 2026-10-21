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


def test_generic_support_claim_groundwork_links_manifest_and_catalog_rows() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    assert 8160 in catalog["issue_refs"]

    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)
    }

    for claim_id, expected in EXPECTED_GENERIC_ROWS.items():
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]

        assert claim["owner_phase"] == expected["owner_phase"]
        assert claim["behavior_fixture"] == expected["behavior_fixture"]
        assert claim["executable_command"].startswith(PUBLIC_COMMAND_PREFIX)

        assert row["capability_id"] == expected["capability_id"]
        assert row["owner_phase"] == expected["owner_phase"]
        assert row["runnable_command"] == claim["executable_command"]
        assert row["runtime_acceptance_case"] == expected["runtime_acceptance_case"]

        positive = set(row["positive_evidence"])
        negative = set(row["negative_evidence"])
        assert len(positive) == len(row["positive_evidence"])
        assert len(negative) == len(row["negative_evidence"])
        assert claim["behavior_fixture"] in positive
        assert expected["required_positive"].issubset(positive)
        assert expected["required_negative"].issubset(negative)
        assert expected["required_codes"].issubset(set(row["required_diagnostic_codes"]))
        assert row["source_truth_requirements"]

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


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


def test_generic_callable_reification_contract_is_source_backed_and_fail_closed() -> None:
    contract = _read_json(GENERIC_CALLABLE_REIFICATION_CONTRACT_PATH)

    assert contract["contract_id"] == "objc3c.native.generic_callable_reification_contract.v1"
    assert 8235 in contract["issue_refs"]
    assert contract["policy"]["generated_report_source_truth_allowed"] is False
    assert contract["policy"]["validation_required_before_support_expansion"] is True

    accepted = {row["surface"]: row for row in contract["accepted_surfaces"]}
    assert set(accepted) == {
        "generic_free_function",
        "generic_free_function_explicit_reified_metadata_policy",
        "objective_c_generic_method",
        "objective_c_generic_method_explicit_reified_metadata_policy",
    }
    assert accepted["generic_free_function"]["reification_policy"] == "erased_default"
    assert accepted["generic_free_function_explicit_reified_metadata_policy"][
        "reification_policy"
    ] == "explicit_reified"
    assert accepted["objective_c_generic_method"]["selector_identity_includes_generic_clause"] is False
    assert accepted["objective_c_generic_method_explicit_reified_metadata_policy"][
        "runtime_specialization_claimed"
    ] is False
    for surface in accepted.values():
        for path in [
            surface["parser_owner"],
            surface["sema_owner"],
            surface["positive_fixture"],
        ]:
            _assert_repo_path_exists(path)
    assert {
        "generic_callable_signature_replay_key",
        "generic_callable_reification_policy",
        "generic_callable_mangling_policy_id",
    } <= set(accepted["objective_c_generic_method"]["metadata_fields"])

    reserved = contract["reserved_surfaces"]
    assert {row["diagnostic_code"] for row in reserved} == {"O3P114"}
    assert {
        "c_style_generic_free_function",
        "selector_local_generic_method_clause",
        "unsupported_reification_scope",
    } == {row["surface"] for row in reserved}
    for row in reserved:
        _assert_repo_path_exists(row["negative_fixture"])

    fail_closed = contract["fail_closed_semantic_contracts"]
    contracts = {row["contract"]: row for row in fail_closed}
    assert set(contracts) == {
        "generic_free_function_redeclaration_signature_drift",
        "generic_method_override_signature_drift",
        "generic_callable_constraint_cycle",
        "erased_to_reified_redeclaration_drift",
        "cross_module_mangling_policy_mismatch",
    }
    drift = contracts["generic_free_function_redeclaration_signature_drift"]
    assert drift["diagnostic_code"] == "O3S206"
    assert {
        "generic arity",
        "source-order parameter names",
        "reification policy",
        "generic callable signature replay key",
    } <= set(drift["required_invariants"])
    for row in fail_closed:
        _assert_repo_path_exists(row["negative_fixture"])
