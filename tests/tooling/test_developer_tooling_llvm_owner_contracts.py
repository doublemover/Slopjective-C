from __future__ import annotations

from developer_tooling_llvm_owner_behavior import (
    assert_capability_routed_parity_fails_closed_when_hosted_route_is_missing,
    assert_hosted_llvm_action_fails_closed_without_object_emission,
    assert_hosted_llvm_contract_forbids_clang_only_support_claim,
    assert_hosted_llvm_truth_payload_publishes_success_fields,
    assert_hosted_llvm_truth_payload_requires_object_emission,
    assert_hosted_summary_truth_requires_mode_ok_clang_and_object_emission,
    assert_llvm_tool_capability_owner_contracts_match_fixture,
    assert_local_llvm_probe_payload_remains_diagnostic_not_capability_truth,
)
from developer_tooling_llvm_owner_sources import load_owner_contract_fixture


def test_llvm_tool_capability_owner_contracts_match_machine_readable_fixture() -> None:
    assert_llvm_tool_capability_owner_contracts_match_fixture(
        load_owner_contract_fixture()
    )


def test_hosted_llvm_contract_forbids_clang_only_support_claim() -> None:
    assert_hosted_llvm_contract_forbids_clang_only_support_claim()


def test_local_llvm_probe_payload_remains_diagnostic_not_capability_truth() -> None:
    assert_local_llvm_probe_payload_remains_diagnostic_not_capability_truth()


def test_hosted_llvm_truth_payload_requires_object_emission() -> None:
    assert_hosted_llvm_truth_payload_requires_object_emission()


def test_hosted_llvm_truth_payload_publishes_success_fields() -> None:
    assert_hosted_llvm_truth_payload_publishes_success_fields()


def test_hosted_llvm_action_fails_closed_without_object_emission(monkeypatch) -> None:
    assert_hosted_llvm_action_fails_closed_without_object_emission(monkeypatch)


def test_capability_routed_parity_fails_closed_when_hosted_route_is_missing(
    monkeypatch,
) -> None:
    assert_capability_routed_parity_fails_closed_when_hosted_route_is_missing(
        monkeypatch
    )


def test_hosted_summary_truth_requires_mode_ok_clang_and_object_emission(
    monkeypatch,
) -> None:
    assert_hosted_summary_truth_requires_mode_ok_clang_and_object_emission(monkeypatch)
