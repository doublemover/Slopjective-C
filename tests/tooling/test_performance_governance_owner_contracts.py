from __future__ import annotations

from performance_governance_owner_contracts_behavior import (
    assert_claim_and_measurement_policies_forbid_non_authoritative_evidence,
    assert_every_breach_taxonomy_entry_has_a_triage_owner,
    assert_every_performance_budget_has_an_explicit_owner_contract,
    assert_lab_policy_and_owner_contract_use_the_same_profiles,
    assert_performance_owner_contracts_are_shape_valid,
)


def test_performance_owner_contracts_are_shape_valid() -> None:
    assert_performance_owner_contracts_are_shape_valid()


def test_every_performance_budget_has_an_explicit_owner_contract() -> None:
    assert_every_performance_budget_has_an_explicit_owner_contract()


def test_every_breach_taxonomy_entry_has_a_triage_owner() -> None:
    assert_every_breach_taxonomy_entry_has_a_triage_owner()


def test_lab_policy_and_owner_contract_use_the_same_profiles() -> None:
    assert_lab_policy_and_owner_contract_use_the_same_profiles()


def test_claim_and_measurement_policies_forbid_non_authoritative_evidence() -> None:
    assert_claim_and_measurement_policies_forbid_non_authoritative_evidence()
