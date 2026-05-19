from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_release_channels import (
    RELEASE_CHANNEL_ACTION_OWNER_MAP,
    RELEASE_CHANNEL_GATE_OWNERS,
)
from scripts.objc3c_workflow.action_catalog_reporting_release import (
    STRESS_REPORTING_RELEASE_ACTION_OWNER_MAP,
    STRESS_REPORTING_RELEASE_GATE_OWNERS,
)
from scripts.objc3c_workflow.action_catalog_release_foundation import (
    RELEASE_FOUNDATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_packaging_channels import (
    PACKAGING_CHANNEL_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_release_operations import (
    RELEASE_OPERATIONS_ACTION_SPECS,
)
from scripts.objc3c_workflow.actions import release_governance_foundation_artifacts
from scripts.objc3c_workflow.actions.release_governance_owner_contracts import (
    PACKAGING_CHANNEL_ACTION_CONTRACTS,
    RELEASE_FOUNDATION_ACTION_CONTRACTS,
    RELEASE_GATE_OWNERS,
    RELEASE_OPERATIONS_ACTION_CONTRACTS,
    release_action_specs,
    release_gate_hard_cutover_guardrails,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"
PLATFORM_HARDENING_FIXTURES = (
    "tests/tooling/fixtures/platform_hardening/boundary_inventory.json",
    "tests/tooling/fixtures/platform_hardening/build_package_validation_contract.json",
    "tests/tooling/fixtures/platform_hardening/install_matrix_integration_contract.json",
    "tests/tooling/fixtures/platform_hardening/packaged_smoke_integration_contract.json",
    "tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json",
    "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json",
    "tests/tooling/fixtures/platform_hardening/toolchain_archive_claim_policy.json",
    "tests/tooling/fixtures/platform_hardening/toolchain_range_replay_contract.json",
    "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json",
)
OWNER_POLICY_EXTENSION_KEYS = {
    "behavior_owner_split_required",
    "canonical_rejection_owner",
    "provenance_owner",
    "release_drill_owner",
    "trust_owner",
    "wrapper_only_allowed",
}


def _load_fixture(relative_path: str) -> dict[str, object]:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def _assert_owner_policy_matches_model(
    surface: dict[str, object], expected_policy: dict[str, object]
) -> None:
    owner_policy = surface["owner_policy"]
    assert isinstance(owner_policy, dict)

    for key, expected_value in expected_policy.items():
        assert owner_policy[key] == expected_value

    extension_keys = set(owner_policy) - set(expected_policy)
    assert extension_keys <= OWNER_POLICY_EXTENSION_KEYS

    if extension_keys:
        if "behavior_owner_split_required" in extension_keys:
            assert owner_policy["behavior_owner_split_required"] is True
        for path_key in ("canonical_rejection_owner", "provenance_owner"):
            if path_key in owner_policy:
                assert (ROOT / str(owner_policy[path_key])).is_file()
        if "wrapper_only_allowed" in owner_policy:
            assert owner_policy["wrapper_only_allowed"] is False
        for role_key in ("trust_owner", "release_drill_owner"):
            if role_key in owner_policy:
                owner_contracts = surface["owner_contracts"]
                assert isinstance(owner_contracts, dict)
                owner_contract = owner_contracts[role_key]
                assert isinstance(owner_contract, dict)
                assert owner_policy[role_key] == owner_contract["role"]


def test_release_gate_fixture_owner_policies_match_contract_model() -> None:
    for owner in RELEASE_GATE_OWNERS.values():
        source_surface = _load_fixture(owner.source_surface)
        workflow_surface = _load_fixture(owner.workflow_surface)

        _assert_owner_policy_matches_model(source_surface, owner.owner_policy())
        _assert_owner_policy_matches_model(
            workflow_surface, owner.workflow_owner_policy()
        )
        assert set(owner.owned_actions).issubset(
            _surface_action_names(source_surface, workflow_surface)
        )
        assert workflow_surface["owner_policy"]["evidence_log_allowed"] is False


def test_release_channel_catalog_exports_gate_and_action_owners() -> None:
    release_channel_gate_ids = {
        "release-foundation",
        "packaging-channels",
        "release-operations",
        "distribution-credibility",
    }
    expected_gate_owners = {
        gate_id: owner.workflow_owner_policy()
        for gate_id, owner in RELEASE_GATE_OWNERS.items()
        if gate_id in release_channel_gate_ids
    }
    assert RELEASE_CHANNEL_GATE_OWNERS == expected_gate_owners

    for gate_id, owner in RELEASE_GATE_OWNERS.items():
        if gate_id not in release_channel_gate_ids:
            continue
        for action_name, action_owner in owner.action_owner_map().items():
            assert RELEASE_CHANNEL_ACTION_OWNER_MAP[action_name] == action_owner


def test_reporting_release_catalog_exports_all_release_gate_owners() -> None:
    expected_gate_owners = {
        gate_id: owner.workflow_owner_policy()
        for gate_id, owner in RELEASE_GATE_OWNERS.items()
    }
    assert STRESS_REPORTING_RELEASE_GATE_OWNERS == expected_gate_owners

    for owner in RELEASE_GATE_OWNERS.values():
        for action_name, action_owner in owner.action_owner_map().items():
            assert STRESS_REPORTING_RELEASE_ACTION_OWNER_MAP[action_name] == action_owner


def test_release_channel_catalogs_are_contract_facades() -> None:
    foundation_text = (
        WORKFLOW_ROOT / "action_catalog_release_foundation.py"
    ).read_text(encoding="utf-8")
    packaging_text = (
        WORKFLOW_ROOT / "action_catalog_packaging_channels.py"
    ).read_text(encoding="utf-8")
    operations_text = (
        WORKFLOW_ROOT / "action_catalog_release_operations.py"
    ).read_text(encoding="utf-8")

    assert "ActionSpec(" not in foundation_text
    assert "ActionSpec(" not in packaging_text
    assert "ActionSpec(" not in operations_text
    assert RELEASE_FOUNDATION_ACTION_SPECS == release_action_specs(
        RELEASE_FOUNDATION_ACTION_CONTRACTS
    )
    assert PACKAGING_CHANNEL_ACTION_SPECS == release_action_specs(
        PACKAGING_CHANNEL_ACTION_CONTRACTS
    )
    assert RELEASE_OPERATIONS_ACTION_SPECS == release_action_specs(
        RELEASE_OPERATIONS_ACTION_CONTRACTS
    )


def test_release_foundation_surface_refreshes_repo_superclean_before_source_check() -> None:
    calls: list[object] = []

    def fake_refresh_release_foundation_generated_upstreams() -> int:
        calls.append("refresh")
        return 0

    def fake_run(command: list[object]) -> int:
        calls.append(("run", str(command[-1])))
        return 0

    original_refresh = (
        release_governance_foundation_artifacts.refresh_release_foundation_generated_upstreams
    )
    original_run = release_governance_foundation_artifacts.run
    try:
        release_governance_foundation_artifacts.refresh_release_foundation_generated_upstreams = (
            fake_refresh_release_foundation_generated_upstreams
        )
        release_governance_foundation_artifacts.run = fake_run
        assert (
            release_governance_foundation_artifacts.action_check_release_foundation_surface(
                []
            )
            == 0
        )
    finally:
        release_governance_foundation_artifacts.refresh_release_foundation_generated_upstreams = (
            original_refresh
        )
        release_governance_foundation_artifacts.run = original_run

    assert calls[0] == "refresh"
    assert calls[1] == (
        "run",
        str(ROOT / "scripts" / "check_release_foundation_source_surface.py"),
    )


def test_release_foundation_validation_uses_public_surface_refresh_action() -> None:
    validation_text = (
        WORKFLOW_ROOT / "actions" / "release_governance_foundation_validation.py"
    ).read_text(encoding="utf-8")

    assert 'workflow_command("check-release-foundation-surface")' in validation_text
    assert "RELEASE_FOUNDATION_SOURCE_SURFACE_PY" not in validation_text


def test_packaging_and_release_operations_publish_hard_cutover_guardrails() -> None:
    packaging_guardrails = release_gate_hard_cutover_guardrails("packaging-channels")
    assert packaging_guardrails == {
        "unsupported_host_success_allowed": False,
        "supported_host_claim_owner": "platform-hardening-support-source",
        "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
        "toolchain_archive_claim_requires_owner": True,
        "package_payload_owner_action": "package-runnable-toolchain",
        "evidence_log_release_claim_allowed": False,
        "wrapper_only_action_surface_allowed": False,
    }

    operations_guardrails = release_gate_hard_cutover_guardrails("release-operations")
    assert operations_guardrails == {
        "missing_upstream_artifact_behavior": "fail-closed",
        "retired_update_path_allowed": False,
        "alternate_update_support_path_allowed": False,
        "publication_claim_owner": "release-operations-gate",
        "blocker_owner_required_before_publication": True,
        "evidence_log_release_claim_allowed": False,
        "wrapper_only_action_surface_allowed": False,
    }

    for gate_id in ("packaging-channels", "release-operations"):
        owner = RELEASE_GATE_OWNERS[gate_id]
        guardrails = release_gate_hard_cutover_guardrails(gate_id)
        for relative_path in (owner.source_surface, owner.workflow_surface):
            surface = _load_fixture(relative_path)
            assert surface["owner_policy"]["hard_cutover_guardrails"] == guardrails
            assert surface["owner_policy"]["evidence_log_allowed"] is False


def test_packaging_and_release_operations_action_specs_are_not_evidence_log_claims() -> None:
    for specs in (PACKAGING_CHANNEL_ACTION_SPECS, RELEASE_OPERATIONS_ACTION_SPECS):
        for action, spec in specs.items():
            assert "evidence-log" not in spec.guarantee_owner
            assert "wrapper" not in spec.backend
            assert RELEASE_GATE_OWNERS[
                RELEASE_CHANNEL_ACTION_OWNER_MAP[action]["gate_owner"].removesuffix("-gate")
            ]


def test_platform_hardening_fixtures_pin_archive_and_unsupported_host_owners() -> None:
    expected_owner_policy = {
        "channel_owner": "packaging-channels-source",
        "platform_support_owner": "platform-hardening-support-source",
        "installer_validation_owner": "platform-hardening-install-validation",
        "build_package_validation_owner": "platform-hardening-build-package-validation",
        "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
        "unsupported_host_failure_owner": "platform-hardening-unsupported-host-fail-closed",
        "blocker_owner": "platform-hardening-blockers",
        "source_authority": "checked-in-platform-hardening-contracts",
        "evidence_log_allowed": False,
    }

    for relative_path in PLATFORM_HARDENING_FIXTURES:
        payload = _load_fixture(relative_path)
        assert payload["owner_policy"] == expected_owner_policy
        assert payload["blocker_metadata"]["blocker_owner"] == "platform-hardening-blockers"

    unsupported_host_policy = _load_fixture(
        "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
    )
    assert all(
        failure_class["required_behavior"] == "fail-closed"
        for failure_class in unsupported_host_policy["hard_fail_classes"]
    )

    archive_claim_policy = _load_fixture(
        "tests/tooling/fixtures/platform_hardening/toolchain_archive_claim_policy.json"
    )
    assert "toolchain archive claim outside checked-in package channel set" in (
        archive_claim_policy["blocker_metadata"]["blocking_conditions"]
    )
    assert (
        "toolchain presence does not widen archive or install claims by itself"
        in archive_claim_policy["claim_boundary_rules"]
    )


def test_distribution_credibility_surface_is_split_by_owner_module() -> None:
    facade_text = (
        WORKFLOW_ROOT / "actions" / "release_governance_distribution_credibility.py"
    ).read_text(encoding="utf-8")

    assert "release_governance_distribution_credibility_artifacts" in facade_text
    assert "release_governance_distribution_credibility_validation" in facade_text
    assert "ROOT /" not in facade_text
    assert "run_composite_validation" not in facade_text


def _surface_action_names(
    source_surface: dict[str, object],
    workflow_surface: dict[str, object],
) -> set[str]:
    action_names: set[str] = set()
    for field_name in (
        "public_actions",
        "release_operations_owned_actions",
        "required_actions",
        "actions",
        "validation_child_actions",
    ):
        for payload in (source_surface, workflow_surface):
            values = payload.get(field_name)
            if isinstance(values, list):
                action_names.update(value for value in values if isinstance(value, str))
    return action_names
