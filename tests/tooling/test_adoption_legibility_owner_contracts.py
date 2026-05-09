from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility"


def _load_fixture(name: str) -> dict[str, object]:
    return json.loads((FIXTURE_ROOT / name).read_text(encoding="utf-8"))


def test_adoption_claim_policy_forbids_compatibility_migration_and_fallback_overclaims() -> None:
    policy = _load_fixture("public_claim_policy.json")
    truthfulness = policy["hard_cutover_truthfulness"]
    assert isinstance(truthfulness, dict)
    assert truthfulness["source_compatibility_claims_allowed"] is False
    assert truthfulness["migration_or_fallback_support_claims_allowed"] is False
    assert truthfulness["manual_adoption_metrics_allowed"] is False

    forbidden = " ".join(policy["forbidden_claims"])
    assert "source-compatible migration support" in forbidden
    assert "fallback support path" in forbidden
    assert "manual adoption metric" in forbidden

    fail_closed = " ".join(policy["fail_closed_conditions"])
    assert "source-compatible migration support" in fail_closed
    assert "fallback support" in fail_closed


def test_adoption_artifact_contract_requires_non_wrapper_publication_owners() -> None:
    contract = _load_fixture("artifact_contract.json")
    hard_cutover = contract["hard_cutover_claim_policy"]
    assert isinstance(hard_cutover, dict)
    assert hard_cutover["source_compatibility_claims_allowed"] is False
    assert hard_cutover["migration_or_fallback_support_claims_allowed"] is False
    assert hard_cutover["report_only_allowed"] is False
    assert hard_cutover["wrapper_only_publication_allowed"] is False

    owner_contracts = contract["owner_contracts"]
    assert isinstance(owner_contracts, dict)
    assert {
        "public_claim_owner",
        "adoption_comparison_owner",
        "migration_playbook_owner",
        "publication_owner",
    } <= set(owner_contracts)

    claim_rules = " ".join(contract["claim_rules"])
    assert "wrapper-only" in claim_rules
    assert "source compatibility" in claim_rules
    assert "fallback support" in claim_rules


def test_adoption_migration_playbook_bounds_replay_fields_to_risk_inventory() -> None:
    playbook = _load_fixture("migration_playbook_semantics.json")
    truthfulness = playbook["hard_cutover_truthfulness"]
    assert isinstance(truthfulness, dict)
    assert truthfulness["source_compatibility_claims_allowed"] is False
    assert truthfulness["migration_or_fallback_support_claims_allowed"] is False
    assert truthfulness["wrapper_only_publication_allowed"] is False
    assert "risk inventory" in str(truthfulness["upgrade_support_report_boundary"])
    assert "not a source-compatibility guarantee" in str(truthfulness["upgrade_support_report_boundary"])
    assert "not fallback support" in str(truthfulness["rollback_target_boundary"])
