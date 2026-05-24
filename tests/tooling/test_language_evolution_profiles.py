from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
PROFILE_MATRIX = (
    ROOT / "tests" / "tooling" / "fixtures" / "language_profiles" / "strict_profile_feature_matrix.json"
)
TYPED_THROWS_OPTIONALS = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "language_evolution_typed_throws_value_optionals_contract.json"
)
GENERIC_REIFICATION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "generic_callable_reification_contract.json"
)
MATCH_GUARDED = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "match_guarded_pattern_language_evolution_contract.json"
)
UMBRELLA_CONTRACT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "language_evolution_umbrella_contract.json"
)
CANONICAL_MANIFEST = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
SUPPORT_CAPABILITY_MATRIX = ROOT / "docs" / "support" / "capability_matrix.json"
SUPPORT_EVIDENCE_MAP = ROOT / "docs" / "support" / "evidence_map.json"
SUPPORT_UMBRELLA_READINESS = ROOT / "docs" / "support" / "umbrella_readiness.json"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_file(relative_path: str) -> None:
    assert not relative_path.startswith(("tmp/", "temp/", "generated/", "build/", "dist/"))
    assert (ROOT / relative_path).is_file(), relative_path


def test_typed_throws_source_owned_and_value_optionals_contract_boundaries() -> None:
    contract = _read_json(TYPED_THROWS_OPTIONALS)

    assert contract["contract_id"] == (
        "objc3c.language_evolution.typed_throws_value_optionals.source_owned.v1"
    )
    assert contract["umbrella_issue_ref"] == 8207
    assert contract["issues"] == {"typed_throws": 8233, "value_optionals": 8234}
    assert contract["support_state"] == {
        "typed_throws": "source_owned_interface_preserved_error_out_abi_lowered_catch_bridge_policy_recorded",
        "value_optionals": "semantic_type_signatures_bounded_scalar_packed_runtime_abi_fail_closed_boundaries",
    }
    assert contract["typed_throws"]["accepted_payload_arity"] == 1
    assert contract["typed_throws"]["diagnostic_symbol"] == (
        "kObjc3ParserDiagnosticReservedTypedThrowsCode"
    )
    assert contract["typed_throws"]["silent_erasure_allowed"] is False
    assert contract["typed_throws"]["interface_roundtrip_status"] == (
        "typed-payload-preserved"
    )
    assert contract["typed_throws"]["abi_status"] == "typed-error-out-abi"
    assert contract["typed_throws"]["catch_compatibility_status"] == (
        "typed-catch-exact-untyped-id-error-bridge-incompatible-rejects"
    )
    assert contract["typed_throws"]["bridge_to_id_error_policy"] == (
        "explicit-bridge-to-id<Error>-only"
    )
    assert contract["typed_throws"]["unsupported_foreign_carrier_fail_closed"] is True
    assert contract["typed_throws"]["effect_record"]["semantic_identity_status"] == (
        "exact-effect-signature-preserved"
    )
    assert contract["interface_import_contract"][
        "typed_throws_callable_compatibility_policy"
    ] == "typed-throws-exact-payload-match-error-out-abi"
    assert contract["typed_throws"]["runtime_execution_claimed"] is True
    assert contract["value_optionals"]["lowercase_alias_accepted"] is False
    assert contract["value_optionals"]["canonical_diagnostic_symbol"] == (
        "kObjc3ParserDiagnosticReservedValueOptionalCode"
    )
    assert contract["value_optionals"]["lowercase_alias_diagnostic_symbol"] == (
        "kObjc3ParserDiagnosticRemovedOptionalAliasCode"
    )
    assert contract["value_optionals"]["nil_to_scalar_coercion_allowed"] is False
    assert contract["value_optionals"]["semantic_value_model_supported"] is True
    assert contract["value_optionals"]["binding_narrowing_supported"] is True
    assert contract["value_optionals"]["implicit_nil_absence_allowed"] is False
    assert contract["value_optionals"]["nullable_pointer_conversion_allowed"] is False
    assert contract["value_optionals"]["runtime_execution_supported"] is True
    assert contract["value_optionals"]["lowering_supported"] is True
    assert contract["value_optionals"]["runtime_abi_payload_scope"] == (
        "supported-scalar-payload-forms-only"
    )
    assert contract["value_optionals"]["broad_public_runtime_support_claim_allowed"] is False
    assert contract["value_optionals"]["nested_value_optional_runtime_supported"] is False
    assert contract["value_optionals"]["generic_payload_runtime_supported"] is False
    assert contract["value_optionals"]["property_storage_supported"] is False
    assert contract["value_optionals"]["ivar_storage_supported"] is False
    assert contract["value_optionals"]["unchecked_unwrap_allowed"] is False
    assert contract["value_optionals"]["interface_roundtrip_status"] == (
        "semantic-carrier-roundtrips-bounded-scalar-runtime-abi"
    )
    assert contract["public_claim_boundary"]["support_claims"] == []
    assert contract["public_claim_boundary"]["bounded_runtime_claims"] == [
        "bounded-runtime:value-optional-scalar-packed-abi"
    ]
    assert contract["public_claim_boundary"]["runtime_claims"] == []
    assert contract["public_claim_boundary"]["reserved_public_runtime_rows_required"] == [
        "language.errors.typed-throws",
        "language.types.value-optionals",
    ]
    assert contract["public_claim_boundary"]["no_compatibility_aliases"] is True
    assert contract["umbrella_alignment"] == {
        "umbrella_contract": "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json",
        "profile_widening_allowed": False,
        "objc2_compatibility_path_allowed": False,
        "runtime_lowering_claim_allowed": True,
    }
    assert contract["diagnostic_code_symbols"] == {
        "typed_throws_invalid_payload_shape": {
            "symbol": "kObjc3ParserDiagnosticReservedTypedThrowsCode",
            "code": "O3P182",
            "owner": "parser",
        },
        "value_optional_reserved": {
            "symbol": "kObjc3ParserDiagnosticReservedValueOptionalCode",
            "code": "O3P159",
            "owner": "parser",
        },
        "lowercase_optional_alias_removed": {
            "symbol": "kObjc3ParserDiagnosticRemovedOptionalAliasCode",
            "code": "O3C004",
            "owner": "parser",
        },
    }

    expected_codes = {
        "negative_typed_throws_reserved.objc3": "O3P182",
        "negative_typed_throws_empty_payload_reserved.objc3": "O3P182",
        "negative_typed_throws_multi_payload_reserved.objc3": "O3P182",
        "negative_typed_throws_erasure_mismatch_reserved.objc3": "O3P182",
        "negative_typed_throws_protocol_mismatch.objc3": "O3S218",
        "negative_typed_throws_incompatible_catch.objc3": "O3S206",
        "negative_typed_throws_foreign_carrier_catch.objc3": "O3S206",
        "negative_value_optional_canonical_reserved.objc3": "O3P159",
        "negative_value_optional_nullable_pointer_conversion_reserved.objc3": "O3P159",
        "negative_value_optional_nil_scalar_coercion_reserved.objc3": "O3S211",
        "negative_value_optional_nested_lowercase_alias_reserved.objc3": "O3C004",
        "negative_value_optional_property_layout_unsupported.objc3": "O3P159",
        "negative_value_optional_nullable_suffix_mismatch.objc3": "O3P159",
        "value_optionals_executable_semantics_negative.contract.json": "O3P159",
    }
    observed = {
        Path(row["fixture"]).name: row["expected_diagnostic"]
        for row in contract["negative_fixtures"]
        if "expected_diagnostic" in row
    }
    assert observed == expected_codes
    assert {
        Path(row["fixture"]).name
        for row in contract["negative_fixtures"]
        if (
            "expected_contract" in row
            and Path(row["fixture"]).name.startswith("typed_throws_")
        )
    } == {
        "typed_throws_abi_lifting_negative.contract.json",
        "typed_throws_interface_mismatch_negative.contract.json",
    }
    for anchor in contract["source_anchors"]:
        _assert_repo_file(str(anchor))
    type_source = (
        ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "frontend_type_source_closure.h"
    ).read_text(encoding="utf-8")
    assert "value_optional_issue_ref = 8234" in type_source
    assert 'value_optional_canonical_spelling = "Optional<T>"' in type_source
    assert "kObjc3ParserDiagnosticReservedValueOptionalCode" in type_source
    assert "kObjc3ParserDiagnosticRemovedOptionalAliasCode" in type_source
    assert "lowercase_optional_alias_rejected = true" in type_source
    assert "value_optional_semantic_type_admission_supported = false" in type_source
    assert "value_optional_stable_layout_contract_supported = false" in type_source
    assert "value_optional_runtime_execution_fail_closed = false" in type_source
    error_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "sema"
        / "model"
        / "semantic_symbol_core_source_closures.h"
    ).read_text(encoding="utf-8")
    assert "typed_throws_issue_ref = 8233" in error_source
    assert 'typed_throws_canonical_syntax = "throws(E)"' in error_source
    assert "kObjc3ParserDiagnosticReservedTypedThrowsCode" in error_source
    assert "typed_throws_silent_erasure_allowed = false" in error_source
    assert "typed_throws_catch_compatibility_status" in error_source
    assert "typed_throws_bridge_to_id_error_policy" in error_source
    assert "typed_throws_foreign_carrier_fail_closed = true" in error_source
    capability_source = (
        ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "capability_status.h"
    ).read_text(encoding="utf-8")
    assert "summary.value_optional_issue_ref == 8234u" in capability_source
    assert "summary.value_optional_reserved_diagnostic_code" in capability_source
    assert "summary.lowercase_optional_alias_diagnostic_code" in capability_source
    assert "summary.typed_throws_issue_ref == 8233u" in capability_source
    assert "summary.typed_throws_reserved_diagnostic_code" in capability_source
    artifact_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "artifacts"
        / "objc3_frontend_source_closure_type_control_error_json.inc"
    ).read_text(encoding="utf-8")
    assert '\\"value_optional_issue_ref\\"' in artifact_source
    assert '\\"value_optional_reserved_diagnostic_code\\"' in artifact_source
    assert '\\"lowercase_optional_alias_diagnostic_code\\"' in artifact_source
    assert '\\"typed_throws_issue_ref\\"' in artifact_source
    assert '\\"typed_throws_reserved_diagnostic_code\\"' in artifact_source
    assert '\\"typed_throws_catch_compatibility_status\\"' in artifact_source
    assert '\\"typed_throws_bridge_to_id_error_policy\\"' in artifact_source
    for row in contract["negative_fixtures"]:
        fixture_path = str(row["fixture"])
        _assert_repo_file(fixture_path)
        if "expected_diagnostic" not in row:
            continue
        fixture_text = (ROOT / fixture_path).read_text(encoding="utf-8")
        assert str(row["expected_diagnostic"]) in fixture_text


def test_strict_and_strict_concurrency_profiles_are_claimable_without_aliases() -> None:
    matrix = _read_json(PROFILE_MATRIX)

    assert matrix["contract_id"] == "objc3c.language_profiles.strict_admission_matrix.v1"
    assert set(matrix["issue_refs"]) == {8207, 8237}
    assert matrix["umbrella_contract"] == (
        "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json"
    )
    assert matrix["profile_claim_policy"] == {
        "claimed_public_profiles": ["core", "strict", "strict-concurrency"],
        "native_frontend_profiles": ["canonical", "strict", "strict-concurrency"],
        "targeted_release_evidence_profiles": [
            "strict-system",
        ],
        "strict_profile_aliases_allowed": False,
        "strict_concurrency_aliases_allowed": False,
        "strict_system_native_frontend_profile_allowed": False,
        "objc2_compatibility_profile_allowed": False,
        "local_temp_evidence_allowed": False,
        "selection_source_anchor": (
            "native/objc3c/src/config/objc3_language_profile_validation.cpp"
        ),
    }
    accepted = {row["profile_id"]: row for row in matrix["accepted_profiles"]}
    rejected = {row["profile_id"]: row for row in matrix["rejected_profiles"]}
    assert accepted["canonical"]["selection_state"] == "accepted"
    assert accepted["strict"]["selection_state"] == "accepted"
    assert accepted["strict-concurrency"]["selection_state"] == "accepted"
    assert accepted["core"]["selection_state"] == "claimed-conformance-profile"
    assert rejected["strict-system"]["diagnostic_codes"] == ["O3C038"]
    assert "native-frontend-selection" in rejected["strict-system"][
        "disabled_allowances"
    ]

    cases = {
        row["case_id"]: (row["selection"], row["expected_diagnostic"])
        for row in matrix["negative_selection_fixtures"]
    }
    assert cases == {
        "strict_system_target_profile_rejected": ("strict-system", "O3C038"),
        "strict_concurrency_alias_rejected": ("strict_concurrency", "O3C001"),
    }

    profile_source = (
        ROOT / "native" / "objc3c" / "src" / "config" / "objc3_language_profile_validation.cpp"
    ).read_text(encoding="utf-8")
    assert "O3C038" in profile_source
    assert "strict-concurrency" in profile_source
    assert "strict-system" in profile_source
    assert "strict_concurrency" not in profile_source


def test_generic_callable_reification_is_source_backed_and_runtime_bounded() -> None:
    contract = _read_json(GENERIC_REIFICATION)

    assert contract["contract_id"] == "objc3c.native.generic_callable_reification_contract.v1"
    assert set(contract["issue_refs"]) == {8207, 8235}
    assert contract["policy"]["generated_report_source_truth_allowed"] is False
    assert contract["umbrella_alignment"] == {
        "umbrella_contract": "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json",
        "profile_widening_allowed": False,
        "objc2_compatibility_path_allowed": False,
        "runtime_reification_claim_allowed": False,
        "module_or_profile_implicit_reification_allowed": False,
    }
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
    reserved = {row["surface"]: row for row in contract["reserved_surfaces"]}
    assert set(reserved) == {
        "c_style_generic_free_function",
        "selector_local_generic_method_clause",
        "unsupported_reification_scope",
    }
    for row in contract["accepted_surfaces"]:
        _assert_repo_file(str(row["positive_fixture"]))
    for row in contract["reserved_surfaces"]:
        _assert_repo_file(str(row["negative_fixture"]))
    for row in contract["fail_closed_semantic_contracts"]:
        _assert_repo_file(str(row["negative_fixture"]))
    assert "runtime-specialized generic metadata or body cloning" in contract[
        "unsupported_boundaries"
    ]


def test_guarded_match_and_bounded_match_expressions_are_source_backed() -> None:
    contract = _read_json(MATCH_GUARDED)

    assert contract["contract_id"] == (
        "objc3c.language_evolution.match_guarded_pattern.contract.v1"
    )
    assert contract["umbrella_issue_ref"] == 8207
    assert contract["issue_refs"] == [8236]
    assert contract["support_state"] == {
        "statement_guarded_match_patterns": "supported_bounded",
        "match_expressions": "supported_bounded",
        "fat_arrow_expression_arms": "supported_bounded",
        "fat_arrow_statement_arms": "reserved_fail_closed",
        "type_test_patterns": "reserved_fail_closed",
    }
    assert contract["admitted_surface"]["guard_condition_type"] == "bool"
    assert contract["admitted_surface"]["guard_condition_checked_after_binding"] is True
    assert contract["admitted_surface"]["guarded_catch_all_exhaustiveness"] == (
        "does-not-make-match-exhaustive-by-itself"
    )
    assert contract["admitted_surface"]["where_keyword_scope"] == (
        "contextual-match-case-only"
    )
    assert contract["public_claim_boundary"]["support_claims"] == [
        "objc3c.behavior.language.control-flow.statement-guarded-match",
        "objc3c.behavior.language.control-flow.match-expression",
    ]
    assert contract["public_claim_boundary"][
        "expression_lowering_claim_bounded_to_scalar_non_result_case"
    ] is True
    assert contract["public_claim_boundary"]["no_result_payload_abi_lowering_claim"] is True
    assert contract["reserved_source_fields"] == ["type_test_pattern_fail_closed"]

    for fixture in contract["admitted_surface"]["positive_fixtures"]:
        _assert_repo_file(str(fixture))
    for anchor in contract["source_anchors"]:
        _assert_repo_file(str(anchor))
    control_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "sema"
        / "model"
        / "semantic_symbol_core_source_closures.h"
    ).read_text(encoding="utf-8")
    assert "guarded_match_issue_ref = 8236" in control_source
    assert 'guarded_match_admitted_syntax =\n      "case pattern where bool_condition:"' in control_source
    assert "guarded_match_condition_bool_required = true" in control_source
    helper_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "pipeline"
        / "frontend_control_flow_source_closure_helpers.cpp"
    ).read_text(encoding="utf-8")
    assert "summary.match_expression_source_supported = true" in helper_source
    assert "summary.match_expression_result_typing_supported = true" in helper_source
    assert "summary.match_fat_arrow_arms_supported = true" in helper_source
    assert "summary.type_test_pattern_fail_closed = true" in helper_source
    replay_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "pipeline"
        / "source_closure"
        / "frontend_source_closure_replay_keys_core.cpp"
    ).read_text(encoding="utf-8")
    assert "guarded_match_admitted_syntax" in replay_source
    capability_source = (
        ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "capability_status.h"
    ).read_text(encoding="utf-8")
    assert "summary.guarded_match_issue_ref == 8236u" in capability_source
    assert "!summary.match_expression_fail_closed" in capability_source
    assert "summary.match_expression_result_typing_supported" in capability_source
    assert "summary.match_fat_arrow_arms_supported" in capability_source
    artifact_source = (
        ROOT
        / "native"
        / "objc3c"
        / "src"
        / "artifacts"
        / "objc3_frontend_source_closure_type_control_error_json.inc"
    ).read_text(encoding="utf-8")
    assert '\\"guarded_match_issue_ref\\"' in artifact_source
    assert '\\"match_expression_result_typing_supported\\"' in artifact_source
    for row in contract["reserved_surfaces"]:
        _assert_repo_file(str(row["negative_fixture"]))
        fixture_text = (ROOT / str(row["negative_fixture"])).read_text(encoding="utf-8")
        assert str(row["expected_diagnostic"]) in fixture_text
    for row in contract["semantic_negative_surfaces"]:
        _assert_repo_file(str(row["negative_fixture"]))
        fixture_text = (ROOT / str(row["negative_fixture"])).read_text(encoding="utf-8")
        assert str(row["expected_diagnostic"]) in fixture_text


def test_language_evolution_umbrella_keeps_claims_source_owned_and_fail_closed() -> None:
    contract = _read_json(UMBRELLA_CONTRACT)

    assert contract["contract_id"] == (
        "objc3c.language_evolution.umbrella.fail_closed_contract.v1"
    )
    assert contract["umbrella_issue_ref"] == 8207
    assert set(contract["issue_refs"]) == {8233, 8234, 8235, 8236, 8237}
    assert contract["generated_or_temp_evidence_allowed"] is False
    assert contract["objc2_compatibility_paths_allowed"] is False
    assert contract["compatibility_aliases_allowed"] is False

    contracts = {row["feature_id"]: row for row in contract["feature_contracts"]}
    assert set(contracts) == {
        "typed_throws",
        "value_optionals",
        "generic_callable_reification",
        "match_guarded_patterns",
        "strict_profiles",
    }
    for row in contracts.values():
        _assert_repo_file(str(row["contract"]))
        assert row["support_state"]
        assert row["negative_case_ids"]
    assert contracts["typed_throws"]["support_state"] == (
        "source_owned_interface_preserved_error_out_abi_lowered_catch_bridge_policy_recorded"
    )
    assert "language.errors.typed-throws-runtime-lowering" not in contract[
        "admitted_public_claims"
    ]
    assert "language.errors.typed-throws" in contract["reserved_public_claims"]
    assert (
        "language.errors.typed-throws-runtime-lowering"
        in contract["reserved_public_claims"]
    )
    assert "language.profiles.strict-system" in contract["reserved_public_claims"]
    assert "language.profiles.strict" not in contract["reserved_public_claims"]
    assert "language.profiles.strict-concurrency" not in contract["reserved_public_claims"]
    assert "language.generics.generic-callable-reification" not in contract[
        "reserved_public_claims"
    ]
    assert "objc3c.behavior.language.control-flow.statement-guarded-match" in contract[
        "admitted_public_claims"
    ]
    assert "objc3c.behavior.language.control-flow.match-expression" in contract[
        "admitted_public_claims"
    ]
    assert "objc3c.behavior.language.generics.generic-callable-reification" in contract[
        "admitted_public_claims"
    ]
    assert "objc3c.behavior.language.profiles.strict-admission" in contract[
        "admitted_public_claims"
    ]
    support_rows = {row["row_id"]: row for row in contract["lead_support_row_recommendations"]}
    assert set(support_rows) == {
        "language.evolution.umbrella-alignment",
        "language.errors.typed-throws",
        "language.types.value-optionals",
        "language.generics.generic-callable-reification",
        "language.control-flow.statement-guarded-match",
        "language.profiles.strict-admission",
    }
    assert support_rows["language.generics.generic-callable-reification"]["status"] == (
        "source-owned-generic-callable-policy"
    )
    assert support_rows["language.errors.typed-throws"]["status"] == (
        "source-owned-interface-preserved-error-out-abi-lowered-catch-bridge-policy-recorded-public-row-reserved"
    )
    assert support_rows["language.control-flow.statement-guarded-match"]["status"] == (
        "supported-bounded-statement-and-expression"
    )
    assert support_rows["language.profiles.strict-admission"]["status"] == (
        "implemented-strict-and-strict-concurrency-claimable"
    )


def test_language_evolution_support_docs_point_to_source_contracts() -> None:
    matrix = _read_json(SUPPORT_CAPABILITY_MATRIX)
    capabilities = {row["id"]: row for row in matrix["capabilities"]}

    guarded = capabilities["language.control-flow.statement-guarded-match"]
    guarded_paths = {row["path"] for row in guarded["evidence"]}
    assert "tests/tooling/fixtures/native/match_guarded_pattern_language_evolution_contract.json" in guarded_paths
    assert "objc3c.behavior.language.control-flow.statement-guarded-match" in guarded[
        "support_claims"
    ]

    match_expression = capabilities["language.control-flow.match-expression"]
    match_paths = {row["path"] for row in match_expression["evidence"]}
    assert "tests/tooling/fixtures/native/recovery/positive/match_expression_literal_result.objc3" in match_paths
    assert "tests/tooling/fixtures/native/recovery/negative/negative_match_expression_non_exhaustive.objc3" in match_paths
    assert "objc3c.behavior.language.control-flow.match-expression" in match_expression[
        "support_claims"
    ]

    generic_reification = capabilities["language.generics.generic-callable-reification"]
    assert generic_reification["state"] == "implemented"
    assert "objc3c.behavior.language.generics.generic-callable-reification" in generic_reification[
        "support_claims"
    ]
    generic_paths = {row["path"] for row in generic_reification["evidence"]}
    assert "tests/tooling/fixtures/native/type_semantic_generic_reified_objc_method_positive.objc3" in generic_paths
    assert "tests/tooling/fixtures/native/recovery/negative/negative_reify_generics_unsupported_scope.objc3" in generic_paths

    strict_profiles = capabilities["language.profiles.strict-admission"]
    assert strict_profiles["state"] == "implemented"
    assert "objc3c.behavior.language.profiles.strict-admission" in strict_profiles[
        "support_claims"
    ]
    strict_paths = {row["path"] for row in strict_profiles["evidence"]}
    assert "tests/conformance/profile_strict_boundary/strict_profile_boundary_contract.json" in strict_paths
    assert "tests/conformance/profile_strict_boundary/strict_profile_value_flow.objc3" in strict_paths
    assert "tests/conformance/profile_strict_boundary/strict_concurrency_actor_executor_value_flow.objc3" in strict_paths
    assert "tests/conformance/profile_strict_boundary/strict_system_profile_mismatch_negative.objc3" in strict_paths
    assert "schemas/objc3c-strict-profile-boundary-v1.schema.json" in strict_paths

    umbrella = capabilities["language.evolution.umbrella-alignment"]
    assert umbrella["state"] == "reserved"
    assert "support_claims" not in umbrella
    umbrella_paths = {row["path"] for row in umbrella["evidence"]}
    assert "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json" in umbrella_paths
    assert "tests/tooling/fixtures/native/match_guarded_pattern_language_evolution_contract.json" in umbrella_paths
    assert "tests/tooling/fixtures/native/language_evolution_typed_throws_value_optionals_contract.json" in umbrella_paths
    assert "tests/tooling/fixtures/native/generic_callable_reification_contract.json" in umbrella_paths
    assert "tests/tooling/fixtures/language_profiles/strict_profile_feature_matrix.json" in umbrella_paths

    evidence = _read_json(SUPPORT_EVIDENCE_MAP)
    evidence_paths = {
        (row["capability_id"], row["path"]) for row in evidence["rows"]
    }
    assert (
        "language.evolution.umbrella-alignment",
        "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json",
    ) in evidence_paths
    assert (
        "language.evolution.umbrella-alignment",
        "tests/tooling/fixtures/native/match_guarded_pattern_language_evolution_contract.json",
    ) in evidence_paths
    assert (
        "language.control-flow.match-expression",
        "tests/tooling/fixtures/native/recovery/positive/match_expression_literal_result.objc3",
    ) in evidence_paths
    assert (
        "language.generics.generic-callable-reification",
        "tests/tooling/fixtures/native/type_semantic_generic_reified_objc_method_positive.objc3",
    ) in evidence_paths
    assert (
        "language.profiles.strict-admission",
        "tests/conformance/profile_strict_boundary/strict_profile_boundary_contract.json",
    ) in evidence_paths

    readiness = _read_json(SUPPORT_UMBRELLA_READINESS)
    language_entry = next(
        row
        for row in readiness["entries"]
        if row["umbrella_capability_id"] == "language.evolution.umbrella-alignment"
    )
    readiness_paths = {row.get("path") for row in language_entry["required_source_anchors"]}
    assert "tests/tooling/fixtures/native/language_evolution_umbrella_contract.json" in readiness_paths
    assert "tests/tooling/fixtures/native/match_guarded_pattern_language_evolution_contract.json" in readiness_paths


def test_language_evolution_fixtures_are_canonical_manifest_owned() -> None:
    manifest = _read_json(CANONICAL_MANIFEST)
    fixtures = {
        str(row["path"]): row for row in manifest["fixtures"] if isinstance(row, dict)
    }
    expected = {
        "tests/tooling/fixtures/native/recovery/negative/negative_typed_throws_empty_payload_reserved.objc3": (
            "parser",
            "errors",
            "canonical_rejection",
            "O3P182",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_typed_throws_protocol_mismatch.objc3": (
            "sema",
            "errors",
            "diagnostic_negative",
            "O3S218",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_value_optional_nil_scalar_coercion_reserved.objc3": (
            "sema",
            "types",
            "diagnostic_negative",
            "O3S211",
        ),
        "tests/tooling/fixtures/native/value_optionals_executable_semantics_negative.contract.json": (
            "sema",
            "types",
            "diagnostic_negative",
            "O3P159",
        ),
        "tests/tooling/fixtures/native/recovery/positive/match_guarded_pattern_statement.objc3": (
            "sema",
            "control-flow",
            "positive",
            "",
        ),
        "tests/tooling/fixtures/native/recovery/positive/match_expression_literal_result.objc3": (
            "sema",
            "control-flow",
            "positive",
            "",
        ),
        "tests/tooling/fixtures/native/recovery/positive/match_expression_guarded_bool.objc3": (
            "sema",
            "control-flow",
            "positive",
            "",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_match_expression_non_exhaustive.objc3": (
            "sema",
            "control-flow",
            "diagnostic_negative",
            "O3S206",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_match_expression_type_test_reserved.objc3": (
            "parser",
            "control-flow",
            "canonical_rejection",
            "O3P158",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_guarded_match_pattern_non_bool.objc3": (
            "sema",
            "control-flow",
            "diagnostic_negative",
            "O3S206",
        ),
        "tests/tooling/fixtures/native/recovery/negative/negative_match_statement_fat_arrow_ambiguous.objc3": (
            "parser",
            "control-flow",
            "canonical_rejection",
            "O3P156",
        ),
        "tests/tooling/fixtures/native/match_type_test_pattern_fail_closed_negative.objc3": (
            "parser",
            "control-flow",
            "canonical_rejection",
            "O3P158",
        ),
    }
    for path, (owner_phase, behavior_family, fixture_kind, diagnostic) in expected.items():
        row = fixtures[path]
        assert row["origin"] == "hand-authored"
        assert row["owner_phase"] == owner_phase
        assert row["behavior_family"] == behavior_family
        assert row["fixture_kind"] == fixture_kind
        assert row["expected_diagnostic_code"] == diagnostic
        _assert_repo_file(path)
