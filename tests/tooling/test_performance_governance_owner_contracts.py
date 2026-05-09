from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions.performance_owner_contracts import (
    OWNER_CONTRACT_ID,
    forbidden_performance_claim_classes,
    load_performance_owner_contracts,
    owner_contract_index,
    required_public_claim_inputs,
    validate_performance_owner_contract_shape,
)

ROOT = Path(__file__).resolve().parents[2]


def load_json(relative_path: str) -> dict[str, object]:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def test_performance_owner_contracts_are_shape_valid() -> None:
    contract = load_performance_owner_contracts()

    assert contract["contract_id"] == OWNER_CONTRACT_ID
    assert validate_performance_owner_contract_shape(contract) == []
    assert set(forbidden_performance_claim_classes(contract)) == {
        "local-only-performance-evidence",
        "report-only-performance-evidence",
        "unsupported-benchmark-claim",
        "compatibility-fallback-performance-claim",
        "wrapper-only-action-surface",
    }
    assert set(required_public_claim_inputs(contract)) == {
        "checked_in_benchmark_source",
        "approved_lab_profile",
        "raw_sample_packets",
        "budget_id",
        "breach_id_or_release_ready_status",
        "owner_id",
    }


def test_every_performance_budget_has_an_explicit_owner_contract() -> None:
    contract = load_performance_owner_contracts()
    budget_model = load_json("tests/tooling/fixtures/performance_governance/budget_model.json")
    budget_owners = owner_contract_index(contract, "budget_owners", "budget_id")
    source_owners = owner_contract_index(contract, "benchmark_source_owners", "owner_id")

    expected_budget_paths = {
        str(entry["budget_id"]): str(entry["summary_path"])
        for entry in budget_model["budget_families"]
        if isinstance(entry, dict)
    }

    assert set(budget_owners) == set(expected_budget_paths)
    for budget_id, expected_summary_path in expected_budget_paths.items():
        owner = budget_owners[budget_id]
        assert owner["source_summary_path"] == expected_summary_path
        source_owner = source_owners[str(owner["benchmark_source_owner_id"])]
        assert budget_id in source_owner["budget_ids"]
        assert source_owner["report_paths"]
        assert source_owner["source_surface_path"].startswith("tests/tooling/fixtures/")


def test_every_breach_taxonomy_entry_has_a_triage_owner() -> None:
    contract = load_performance_owner_contracts()
    budget_model = load_json("tests/tooling/fixtures/performance_governance/budget_model.json")
    triage_policy = load_json("tests/tooling/fixtures/performance_governance/breach_triage_policy.json")
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
        assert owner["policy_path"] == (
            "tests/tooling/fixtures/performance_governance/breach_triage_policy.json"
        )
        assert owner["classification"] == triage_classifications[breach_id]


def test_lab_policy_and_owner_contract_use_the_same_profiles() -> None:
    contract = load_performance_owner_contracts()
    lab_policy = load_json("tests/tooling/fixtures/performance_governance/lab_policy.json")
    lab_owners = owner_contract_index(contract, "lab_owners", "lab_profile_id")

    approved_profiles = {
        str(entry["lab_profile_id"]): entry
        for entry in lab_policy["approved_lab_profiles"]
        if isinstance(entry, dict)
    }

    assert set(lab_owners) == set(approved_profiles)
    for profile_id, owner in lab_owners.items():
        profile = approved_profiles[profile_id]
        assert owner["owner_id"] == profile["owner_id"]
        assert owner["required_machine_profile_fields"] == profile[
            "required_machine_profile_fields"
        ]
        assert owner["required_report_roots"] == profile["required_report_roots"]
        assert owner["environment_breach_id"] == "environment-drift"


def test_claim_and_measurement_policies_forbid_non_authoritative_evidence() -> None:
    contract = load_performance_owner_contracts()
    claim_policy = load_json("tests/tooling/fixtures/performance_governance/claim_policy.json")
    measurement_policy = load_json("tests/tooling/fixtures/performance/measurement_policy.json")

    forbidden = set(forbidden_performance_claim_classes(contract))
    claimability = measurement_policy["claimability_policy"]

    assert set(claim_policy["forbidden_evidence_classes"]) == forbidden
    assert forbidden.issubset(set(claimability["disallowed_claim_classes"]))
    assert not forbidden.intersection(set(claimability["allowed_claim_classes"]))
    assert set(claim_policy["required_public_claim_inputs"]) == set(
        required_public_claim_inputs(contract)
    )
    assert set(claim_policy["required_public_claim_inputs"]).issubset(
        set(claimability["required_claim_inputs"])
    )
