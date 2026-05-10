from __future__ import annotations

from security_hardening_owner_contracts_assertions import (
    assert_false_fields,
    assert_mapping_fields_match,
    assert_non_empty_fields,
)
from security_hardening_owner_contracts_sources import (
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS,
    SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    SECURITY_HARDENING_PUBLIC_TARGETS,
    boundary_inventory_fixture,
    domain_claim_owner_fixtures,
    owner_contracts_fixture,
    security_hardening_domain_owner_contracts,
    security_hardening_hard_cutover_guardrails,
    security_hardening_target,
    source_and_workflow_surface_fixtures,
)


def assert_security_hardening_owner_contracts_match_target_catalog() -> None:
    owner_contracts = owner_contracts_fixture()

    assert owner_contracts["hard_cutover_guardrails"] == (
        security_hardening_hard_cutover_guardrails()
    )
    assert set(owner_contracts["domain_owner_contracts"]) == set(
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
    )

    target_contracts = security_hardening_domain_owner_contracts()
    for owner_id, expected in target_contracts.items():
        fixture_contract = owner_contracts["domain_owner_contracts"][owner_id]
        assert_mapping_fields_match(fixture_contract, expected)
        assert_non_empty_fields(
            fixture_contract,
            ("required_contract_fields", "forbidden_claims"),
        )


def assert_security_hardening_actions_publish_hard_cutover_owner_ids() -> None:
    assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[
        CHECK_SECURITY_RESPONSE_DRILL
    ] == ("response_drill_owner",)
    assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[
        CHECK_SECURITY_RUNTIME_HARDENING
    ] == ("runtime_hardening_owner",)

    for action_name, target in SECURITY_HARDENING_PUBLIC_TARGETS.items():
        assert target.owner_contract_ids == SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
        assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[action_name] == (
            SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
        )
        assert "wrapper" not in target.backend
        assert "evidence-log" not in target.guarantee_owner
        assert target.to_action_spec().guarantee_owner.startswith(
            ", ".join(SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS)
        )


def assert_security_hardening_source_and_workflow_forbid_evidence_log_security_claims() -> None:
    expected_guardrails = security_hardening_hard_cutover_guardrails()

    for payload in source_and_workflow_surface_fixtures():
        guardrails = payload["hard_cutover_security_guardrails"]
        assert_mapping_fields_match(guardrails, expected_guardrails)

    boundary = boundary_inventory_fixture()
    for owner_contract in boundary["source_owner_contracts"].values():
        assert_false_fields(
            owner_contract,
            (
                "evidence_log_allowed",
                "retired_route_claims_allowed",
                "generated_report_claims_allowed",
                "wrapper_only_security_actions_allowed",
                "local_tabletop_capability_truth_allowed",
                "trust_bypass_claims_allowed",
            ),
        )


def assert_security_hardening_domain_contracts_pin_specific_claim_owners() -> None:
    (
        macro_policy,
        response_drill,
        runtime_contract,
        release_key_policy,
    ) = domain_claim_owner_fixtures()

    assert macro_policy["provenance_owner_contract"] == {
        "owner_contracts": "tests/tooling/fixtures/security_hardening/owner_contracts.json",
        "macro_metadata_owner": "security-hardening-macro-provenance",
        "macro_package_identity_owner": "security-hardening-macro-provenance",
        "macro_provenance_owner": "security-hardening-macro-provenance",
        "runtime_acceptance_owner": "security-hardening-runtime",
        "evidence_log_security_proof_allowed": False,
        "retired_route_macro_trust_allowed": False,
        "trust_bypass_for_missing_provenance_allowed": False,
        "wrapper_only_macro_security_action_allowed": False,
    }
    assert response_drill["drill_truth_contract"][
        "local_tabletop_capability_truth_allowed"
    ] is False
    assert response_drill["drill_truth_contract"][
        "retired_route_release_publication_after_drill_failure_allowed"
    ] is False
    assert runtime_contract["runtime_owner_contract"][
        "runtime_retired_route_acceptance_allowed"
    ] is False
    assert runtime_contract["runtime_owner_contract"][
        "trust_bypass_for_missing_runtime_case_allowed"
    ] is False
    assert release_key_policy["release_key_owner_contract"][
        "retired_route_update_trust_allowed"
    ] is False
    assert release_key_policy["release_key_owner_contract"][
        "trust_bypass_for_unsigned_payload_allowed"
    ] is False

    assert security_hardening_target(
        CHECK_SECURITY_RESPONSE_DRILL
    ).owner_contract_ids == ("response_drill_owner",)
    assert security_hardening_target(
        CHECK_SECURITY_RUNTIME_HARDENING
    ).owner_contract_ids == ("runtime_hardening_owner",)
