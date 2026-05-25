from __future__ import annotations

from performance_governance_owner_contracts_assertions import (
    assert_no_overlap,
    assert_same_keys,
    assert_same_values,
    assert_subset,
)
from performance_governance_owner_contracts_sources import (
    BREACH_TRIAGE_POLICY_PATH,
    OWNER_CONTRACT_ID,
    budget_model_fixture,
    breach_triage_policy_fixture,
    claim_policy_fixture,
    forbidden_performance_claim_classes,
    lab_policy_fixture,
    measurement_policy_fixture,
    owner_contract_index,
    performance_owner_contract,
    required_public_claim_inputs,
    validate_performance_owner_contract_shape,
)


def assert_performance_owner_contracts_are_shape_valid() -> None:
    contract = performance_owner_contract()

    assert contract["contract_id"] == OWNER_CONTRACT_ID
    assert validate_performance_owner_contract_shape(contract) == []
    assert_same_values(
        forbidden_performance_claim_classes(contract),
        {
            "local-only-performance-evidence",
            "evidence-log-performance-evidence",
            "unsupported-benchmark-claim",
            "compatibility-retired-route-performance-claim",
            "wrapper-only-action-surface",
            "optimization-speedup-without-safety-proof",
            "generated-report-optimization-source-truth",
            "optimization-fallback-success-path",
        },
    )
    assert_same_values(
        required_public_claim_inputs(contract),
        {
            "checked_in_benchmark_source",
            "approved_lab_profile",
            "raw_sample_packets",
            "budget_id",
            "breach_id_or_release_ready_status",
            "owner_id",
        },
    )


def assert_every_performance_budget_has_an_explicit_owner_contract() -> None:
    contract = performance_owner_contract()
    budget_model = budget_model_fixture()
    budget_owners = owner_contract_index(contract, "budget_owners", "budget_id")
    source_owners = owner_contract_index(contract, "benchmark_source_owners", "owner_id")

    expected_budget_paths = {
        str(entry["budget_id"]): str(entry["summary_path"])
        for entry in budget_model["budget_families"]
        if isinstance(entry, dict)
    }

    assert_same_keys(budget_owners, expected_budget_paths)
    for budget_id, expected_summary_path in expected_budget_paths.items():
        owner = budget_owners[budget_id]
        assert owner["source_summary_path"] == expected_summary_path
        source_owner = source_owners[str(owner["benchmark_source_owner_id"])]
        assert budget_id in source_owner["budget_ids"]
        assert source_owner["report_paths"]
        assert source_owner["source_surface_path"].startswith("tests/tooling/fixtures/")


def assert_every_breach_taxonomy_entry_has_a_triage_owner() -> None:
    contract = performance_owner_contract()
    budget_model = budget_model_fixture()
    triage_policy = breach_triage_policy_fixture()
    breach_owners = owner_contract_index(contract, "breach_owners", "breach_id")

    expected_breach_ids = {
        str(entry["breach_id"])
        for entry in budget_model["breach_taxonomy"]
        if isinstance(entry, dict)
    }
    triage_classifications = {
        breach_id: str(entry["classification"])
        for entry in triage_policy["classifications"]
        if isinstance(entry, dict)
        for breach_id in entry["allowed_breach_ids"]
    }

    assert set(breach_owners) == expected_breach_ids
    for breach_id in expected_breach_ids:
        owner = breach_owners[breach_id]
        assert owner["policy_path"] == BREACH_TRIAGE_POLICY_PATH
        assert owner["classification"] == triage_classifications[breach_id]


def assert_lab_policy_and_owner_contract_use_the_same_profiles() -> None:
    contract = performance_owner_contract()
    lab_policy = lab_policy_fixture()
    lab_owners = owner_contract_index(contract, "lab_owners", "lab_profile_id")

    approved_profiles = {
        str(entry["lab_profile_id"]): entry
        for entry in lab_policy["approved_lab_profiles"]
        if isinstance(entry, dict)
    }

    assert_same_keys(lab_owners, approved_profiles)
    for profile_id, owner in lab_owners.items():
        profile = approved_profiles[profile_id]
        assert owner["owner_id"] == profile["owner_id"]
        assert owner["required_machine_profile_fields"] == profile[
            "required_machine_profile_fields"
        ]
        assert owner["required_report_roots"] == profile["required_report_roots"]
        assert owner["environment_breach_id"] == "environment-drift"


def assert_claim_and_measurement_policies_forbid_non_authoritative_evidence() -> None:
    contract = performance_owner_contract()
    claim_policy = claim_policy_fixture()
    measurement_policy = measurement_policy_fixture()

    forbidden = set(forbidden_performance_claim_classes(contract))
    claimability = measurement_policy["claimability_policy"]

    assert set(claim_policy["forbidden_evidence_classes"]) == forbidden
    assert_subset(forbidden, claimability["disallowed_claim_classes"])
    assert_no_overlap(forbidden, claimability["allowed_claim_classes"])
    assert_same_values(
        claim_policy["required_public_claim_inputs"],
        required_public_claim_inputs(contract),
    )
    assert_subset(
        claim_policy["required_public_claim_inputs"],
        claimability["required_claim_inputs"],
    )
