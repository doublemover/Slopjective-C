from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions.release_governance_security_targets import (
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS,
    SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    SECURITY_HARDENING_PUBLIC_TARGETS,
    security_hardening_domain_owner_contracts,
    security_hardening_hard_cutover_guardrails,
    security_hardening_target,
)

ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening"


def _load_fixture(name: str) -> dict[str, object]:
    return json.loads((FIXTURE_ROOT / name).read_text(encoding="utf-8"))


def test_security_hardening_owner_contracts_match_target_catalog() -> None:
    owner_contracts = _load_fixture("owner_contracts.json")

    assert owner_contracts["hard_cutover_guardrails"] == (
        security_hardening_hard_cutover_guardrails()
    )
    assert set(owner_contracts["domain_owner_contracts"]) == set(
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
    )

    target_contracts = security_hardening_domain_owner_contracts()
    for owner_id, expected in target_contracts.items():
        fixture_contract = owner_contracts["domain_owner_contracts"][owner_id]
        for field_name, field_value in expected.items():
            assert fixture_contract[field_name] == field_value
        assert fixture_contract["required_contract_fields"]
        assert fixture_contract["forbidden_claims"]


def test_security_hardening_actions_publish_hard_cutover_owner_ids() -> None:
    assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[CHECK_SECURITY_RESPONSE_DRILL] == (
        "response_drill_owner",
    )
    assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[CHECK_SECURITY_RUNTIME_HARDENING] == (
        "runtime_hardening_owner",
    )

    for action_name, target in SECURITY_HARDENING_PUBLIC_TARGETS.items():
        assert target.owner_contract_ids == SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
        assert SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS[action_name] == (
            SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS
        )
        assert "wrapper" not in target.backend
        assert "report-only" not in target.guarantee_owner
        assert target.to_action_spec().guarantee_owner.startswith(
            ", ".join(SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS)
        )


def test_security_hardening_source_and_workflow_forbid_report_only_security_claims() -> None:
    expected_guardrails = security_hardening_hard_cutover_guardrails()

    for fixture_name in ("source_surface.json", "workflow_surface.json"):
        payload = _load_fixture(fixture_name)
        guardrails = payload["hard_cutover_security_guardrails"]
        for field_name, field_value in expected_guardrails.items():
            assert guardrails[field_name] == field_value

    boundary = _load_fixture("boundary_inventory.json")
    for owner_contract in boundary["source_owner_contracts"].values():
        assert owner_contract["report_only_allowed"] is False
        assert owner_contract["fallback_claims_allowed"] is False
        assert owner_contract["generated_report_claims_allowed"] is False
        assert owner_contract["wrapper_only_security_actions_allowed"] is False
        assert owner_contract["local_tabletop_capability_truth_allowed"] is False
        assert owner_contract["trust_bypass_claims_allowed"] is False


def test_security_hardening_domain_contracts_pin_specific_claim_owners() -> None:
    macro_policy = _load_fixture("macro_package_provenance_trust_policy.json")
    response_drill = _load_fixture("response_drill_contract.json")
    runtime_contract = _load_fixture("runtime_hardening_contract.json")
    release_key_policy = _load_fixture(
        "installer_update_release_key_hardening_policy.json"
    )

    assert macro_policy["provenance_owner_contract"] == {
        "owner_contracts": "tests/tooling/fixtures/security_hardening/owner_contracts.json",
        "macro_metadata_owner": "security-hardening-macro-provenance",
        "macro_package_identity_owner": "security-hardening-macro-provenance",
        "macro_provenance_owner": "security-hardening-macro-provenance",
        "runtime_acceptance_owner": "security-hardening-runtime",
        "report_only_security_proof_allowed": False,
        "fallback_macro_trust_allowed": False,
        "trust_bypass_for_missing_provenance_allowed": False,
        "wrapper_only_macro_security_action_allowed": False,
    }
    assert response_drill["drill_truth_contract"][
        "local_tabletop_capability_truth_allowed"
    ] is False
    assert response_drill["drill_truth_contract"][
        "fallback_release_publication_after_drill_failure_allowed"
    ] is False
    assert runtime_contract["runtime_owner_contract"][
        "runtime_fallback_acceptance_allowed"
    ] is False
    assert runtime_contract["runtime_owner_contract"][
        "trust_bypass_for_missing_runtime_case_allowed"
    ] is False
    assert release_key_policy["release_key_owner_contract"][
        "fallback_update_trust_allowed"
    ] is False
    assert release_key_policy["release_key_owner_contract"][
        "trust_bypass_for_unsigned_payload_allowed"
    ] is False

    assert security_hardening_target(CHECK_SECURITY_RESPONSE_DRILL).owner_contract_ids == (
        "response_drill_owner",
    )
    assert security_hardening_target(CHECK_SECURITY_RUNTIME_HARDENING).owner_contract_ids == (
        "runtime_hardening_owner",
    )
