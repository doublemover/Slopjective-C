from __future__ import annotations

from scripts.objc3c_workflow.actions.developer_tooling_llvm_contracts import (
    CHECK_HOSTED_LLVM_CAPABILITIES_ACTION,
    HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    LOCAL_LLVM_DIAGNOSTIC_SOURCE,
    LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS,
    hosted_llvm_capability_truth_from_summary,
)

from developer_tooling_llvm_owner_assertions import (
    assert_hosted_success_payload,
    assert_local_diagnostic_truth_payload,
    assert_object_emission_required_payload,
    fixture_contract_projection,
    owner_contract_projection,
)
from developer_tooling_llvm_owner_sources import (
    capable_llvm_summary,
    developer_tooling_llvm_hosted,
    developer_tooling_llvm_parity,
    hosted_probe_with_capability_truth_drift,
    hosted_llvm_summary,
    hosted_probe_without_object_emission,
    hosted_summary_without_clang,
    hosted_summary_without_llc,
)


def assert_llvm_tool_capability_owner_contracts_match_fixture(fixture: dict) -> None:
    actual = {
        contract.action: owner_contract_projection(contract)
        for contract in LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS
    }
    expected = {
        contract["action"]: fixture_contract_projection(contract)
        for contract in fixture["owner_contracts"]
    }

    assert actual == expected


def assert_hosted_llvm_contract_forbids_clang_only_support_claim() -> None:
    contracts_by_action = {
        contract.action: contract for contract in LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS
    }
    hosted_contract = contracts_by_action[CHECK_HOSTED_LLVM_CAPABILITIES_ACTION]

    assert hosted_contract.requires_hosted_probe_summary is True
    assert hosted_contract.fails_closed_without_live_capability is True
    assert "clang-only hosted execution" in hosted_contract.unsupported_claims


def assert_local_llvm_probe_payload_remains_diagnostic_not_capability_truth() -> None:
    truth = hosted_llvm_capability_truth_from_summary(
        capable_llvm_summary(),
        source_kind=LOCAL_LLVM_DIAGNOSTIC_SOURCE,
    )
    assert_local_diagnostic_truth_payload(truth.as_payload())


def assert_hosted_llvm_truth_payload_requires_object_emission() -> None:
    summary = capable_llvm_summary()
    summary["llc_features"] = {"supports_filetype_obj": False}

    truth = hosted_llvm_capability_truth_from_summary(
        summary,
        source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    )
    assert_object_emission_required_payload(truth.as_payload())


def assert_hosted_llvm_truth_payload_publishes_success_fields() -> None:
    payload = hosted_llvm_capability_truth_from_summary(
        capable_llvm_summary(),
        source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    ).as_payload()

    assert_hosted_success_payload(
        payload,
        expected_source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    )


def assert_hosted_llvm_action_publishes_unavailable_without_object_emission(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        developer_tooling_llvm_hosted,
        "run_hosted_llvm_probe",
        hosted_probe_without_object_emission,
    )

    assert developer_tooling_llvm_hosted.action_check_hosted_llvm_capabilities([]) == 0


def assert_hosted_llvm_action_fails_closed_on_capability_truth_drift(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        developer_tooling_llvm_hosted,
        "run_hosted_llvm_probe",
        hosted_probe_with_capability_truth_drift,
    )

    assert developer_tooling_llvm_hosted.action_check_hosted_llvm_capabilities([]) == 1


def assert_capability_routed_parity_skips_when_hosted_route_is_missing(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        developer_tooling_llvm_parity,
        "hosted_llc_object_emission_available",
        lambda: False,
    )
    monkeypatch.setattr(
        developer_tooling_llvm_parity,
        "hosted_native_object_emission_status",
        lambda: "native_object_emission_missing_llc",
    )

    assert (
        developer_tooling_llvm_parity.action_test_capability_routed_source_parity([])
        == 0
    )


def assert_hosted_summary_truth_requires_mode_ok_clang_and_object_emission(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        hosted_llvm_summary,
        "hosted_llvm_summary",
        hosted_summary_without_clang,
    )

    assert hosted_llvm_summary.hosted_llc_object_emission_available() is False
    assert hosted_llvm_summary.hosted_native_object_emission_status() == "native_object_emission_unavailable"

    monkeypatch.setattr(
        hosted_llvm_summary,
        "hosted_llvm_summary",
        hosted_summary_without_llc,
    )

    assert hosted_llvm_summary.hosted_llc_object_emission_available() is False
    assert hosted_llvm_summary.hosted_native_object_emission_status() == "native_object_emission_missing_llc"
