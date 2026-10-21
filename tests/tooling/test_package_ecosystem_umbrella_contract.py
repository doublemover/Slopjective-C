from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_workflow.action_catalog_package_integration import (
    PACKAGE_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_integration_claims import (
    PACKAGE_INTEGRATION_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.action_catalog_package_lock import PACKAGE_LOCK_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_package_lock_contracts import (
    PACKAGE_LOCK_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.action_catalog_package_registry import (
    PACKAGE_REGISTRY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_registry_publication import (
    PACKAGE_REGISTRY_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (
    ecosystem_publication_owner_contract,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_metadata import (
    PACKAGE_FEED_METADATA,
)


ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem"
UMBRELLA = FIXTURE_ROOT / "package_ecosystem_umbrella_contract.json"
INTEGRATION_SCRIPT = ROOT / "scripts" / "check_objc3c_package_ecosystem_integration.py"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _case_ids(payload: dict[str, Any]) -> set[str]:
    cases = payload.get("cases", payload.get("negative_cases", []))
    assert isinstance(cases, list)
    return {str(case["case_id"]) for case in cases if isinstance(case, dict)}


def _all_package_public_actions() -> dict[str, Any]:
    actions: dict[str, Any] = {}
    for action in (
        *PACKAGE_LOCK_PUBLIC_ACTIONS,
        *PACKAGE_REGISTRY_PUBLIC_ACTIONS,
        *PACKAGE_INTEGRATION_PUBLIC_ACTIONS,
    ):
        actions[action.action] = action
    return actions


def test_package_ecosystem_umbrella_contract_is_checked_in_source_truth() -> None:
    umbrella = _read_json(UMBRELLA)

    assert umbrella["contract_id"] == "objc3c.package_ecosystem.umbrella_closure.v1"
    assert umbrella["issue_refs"] == [8204]
    assert {8219, 8220, 8221, 8222, 8223} <= set(umbrella["integrated_issue_refs"])
    assert umbrella["hard_cutover_policy"]["live_network_fetch_allowed"] is False
    assert umbrella["hard_cutover_policy"]["fallback_registry_success_allowed"] is False
    assert umbrella["hard_cutover_policy"]["temp_source_truth_allowed"] is False
    assert umbrella["hard_cutover_policy"]["wrapper_only_publication_allowed"] is False

    for rel_path in umbrella["source_contracts"]:
        assert not str(rel_path).startswith(("tmp/", "temp/"))
        assert (ROOT / rel_path).is_file(), rel_path

    for rel_path in umbrella["schema_contracts"]:
        assert (ROOT / rel_path).is_file(), rel_path

    assert all(
        str(root).startswith("tmp/artifacts/package-ecosystem")
        for root in umbrella["generated_artifact_roots"]
    )


def test_package_ecosystem_umbrella_actions_are_public_owned_and_source_backed() -> None:
    umbrella = _read_json(UMBRELLA)
    required_actions = set(umbrella["required_public_actions"])
    public_actions = _all_package_public_actions()
    action_specs = {
        **PACKAGE_LOCK_ACTION_SPECS,
        **PACKAGE_REGISTRY_ACTION_SPECS,
        **PACKAGE_INTEGRATION_ACTION_SPECS,
    }

    assert required_actions <= set(public_actions)
    assert required_actions <= set(action_specs)
    assert required_actions <= set(PACKAGE_FEED_METADATA["package-ecosystem"].action_names)

    for action_name in required_actions:
        public_action = public_actions[action_name]
        assert "package_ecosystem_umbrella_contract.json" in " ".join(public_action.source_paths)

        owner = ecosystem_publication_owner_contract(action_name)
        assert "package_ecosystem_umbrella_contract.json" in " ".join(owner.source_contracts)
        assert owner.evidence_contracts
        assert not owner.wrapper_only_allowed
        assert not owner.evidence_log_allowed
        assert not owner.retired_source_acceptance_claim_allowed


def test_package_ecosystem_umbrella_contracts_share_required_action_lists() -> None:
    umbrella = _read_json(UMBRELLA)
    required_actions = set(umbrella["required_public_actions"])
    artifact = _read_json(FIXTURE_ROOT / "artifact_contract.json")
    boundary = _read_json(FIXTURE_ROOT / "boundary_inventory.json")
    model = _read_json(FIXTURE_ROOT / "package_manager_model_contract.json")
    install = _read_json(FIXTURE_ROOT / "install_distribution_credibility_contract.json")

    assert required_actions <= set(artifact["required_actions"])
    assert required_actions <= set(boundary["required_actions"])
    assert required_actions == set(model["umbrella_required_public_actions"])
    assert set(umbrella["required_action_groups"]["package_operations_and_install_distribution"]) <= set(
        install["required_public_actions"]
    )
    assert set(umbrella["required_action_groups"]["signing_and_verification"]) <= set(
        install["required_public_actions"]
    )
    assert set(umbrella["required_action_groups"]["direct_import"]) <= set(
        install["required_public_actions"]
    )


def test_package_ecosystem_cross_fixture_identity_and_fail_closed_policy_agree() -> None:
    direct = _read_json(FIXTURE_ROOT / "direct_import_module_syntax_contract.json")
    model = _read_json(FIXTURE_ROOT / "package_manager_model_contract.json")
    hosted = _read_json(FIXTURE_ROOT / "hosted_registry" / "hosted-registry-index.json")
    network = _read_json(FIXTURE_ROOT / "network_resolution" / "network-dependency-resolution.json")
    publication = _read_json(
        FIXTURE_ROOT / "network_resolution" / "package-release-channel-publication.json"
    )

    direct_failure_modes = {
        str(case["failure_mode"]) for case in direct["negative_fixtures"]
    }
    assert {"missing-package-provenance", "ambiguous-module-identity"} <= direct_failure_modes
    assert "direct @import syntax is admitted only when" in " ".join(model["model_rules"])

    assert hosted["endpoint_identity"] == network["hosted_registry_endpoint"]
    assert hosted["endpoint_identity"] == publication["registry_endpoint"]
    assert hosted["endpoint_identity"]["network_fetch"] == "forbidden-fail-closed"
    assert hosted["endpoint_identity"]["fallback_registry_success"] is False
    assert network["network_policy"]["fallback_registry_success"] is False
    assert publication["publication_policy"]["fallback_publication_success"] is False
    assert publication["publication_policy"]["live_network_publication"] == "forbidden-fail-closed"

    assert hosted["lock_trust_material"] == network["lock_trust_material"]
    assert publication["packages"][0]["lock_digest"] == network["lock_trust_material"]["source_lock_digest"]
    assert publication["source_resolution"] == (
        "tests/tooling/fixtures/package_ecosystem/network_resolution/network-dependency-resolution.json"
    )


def test_package_ecosystem_negative_cases_cover_umbrella_failure_modes() -> None:
    umbrella = _read_json(UMBRELLA)
    hosted_negative = _read_json(FIXTURE_ROOT / "hosted_registry" / "negative-registry-cases.json")
    network_negative = _read_json(
        FIXTURE_ROOT / "network_resolution" / "negative-network-publication-cases.json"
    )
    security = _read_json(FIXTURE_ROOT / "package_security_hardening_contract.json")

    assert {
        "unpinned-hosted-dependency",
        "registry-trust-mismatch",
        "missing-package-provenance",
    } <= _case_ids(hosted_negative)
    assert {
        "stale-channel",
        "unsigned-tampered-cache",
        "lock-mismatch",
        "dependency-identity-drift",
        "hosted-registry-endpoint-mismatch",
    } <= _case_ids(network_negative)
    assert {
        "absolute-extraction-path",
        "parent-traversal-extraction-path",
        "symlink-extraction-entry",
        "overwrite-existing-install-path",
        "duplicate-extraction-path",
        "case-conflicting-extraction-path",
        "installer-key-active-overclaim",
        "release-trust-root-active",
        "registry-trust-root-active",
    } <= _case_ids(security)

    failure_bindings = {
        case
        for binding in umbrella["cross_contract_bindings"]
        for case in binding["fail_closed_cases"]
    }
    assert failure_bindings <= (
        _case_ids(hosted_negative) | _case_ids(network_negative) | _case_ids(security)
        | {"ambiguous-module-identity"}
    )


def test_package_ecosystem_integration_runner_names_all_umbrella_surfaces() -> None:
    source = INTEGRATION_SCRIPT.read_text(encoding="utf-8")

    for command in (
        "check_objc3c_direct_import_module_syntax.py",
        "check_objc3c_package_security_hardening.py",
        "check_objc3c_package_manager_model.py",
        "check_objc3c_package_registry_model.py",
        "check_objc3c_package_network_publication.py",
        "check_objc3c_package_install_distribution_credibility.py",
        "check_objc3c_package_operations.py",
    ):
        assert command in source

    for report in (
        "direct-import-module-syntax-summary.json",
        "package-security-hardening-summary.json",
        "hosted-registry-resolution-summary.json",
        "network-publication-summary.json",
        "install-distribution-credibility-summary.json",
        "package-operations-summary.json",
    ):
        assert report in source
