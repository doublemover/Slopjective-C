from __future__ import annotations

from security_hardening_owner_contracts_behavior import (
    assert_security_hardening_actions_publish_hard_cutover_owner_ids,
    assert_security_hardening_domain_contracts_pin_specific_claim_owners,
    assert_security_hardening_owner_contracts_match_target_catalog,
    assert_security_hardening_source_and_workflow_forbid_report_only_security_claims,
)


def test_security_hardening_owner_contracts_match_target_catalog() -> None:
    assert_security_hardening_owner_contracts_match_target_catalog()


def test_security_hardening_actions_publish_hard_cutover_owner_ids() -> None:
    assert_security_hardening_actions_publish_hard_cutover_owner_ids()


def test_security_hardening_source_and_workflow_forbid_report_only_security_claims() -> None:
    assert_security_hardening_source_and_workflow_forbid_report_only_security_claims()


def test_security_hardening_domain_contracts_pin_specific_claim_owners() -> None:
    assert_security_hardening_domain_contracts_pin_specific_claim_owners()
