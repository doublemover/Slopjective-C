from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE  # noqa: E402
from objc3c_package_manager.network_publication import (  # noqa: E402
    NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID,
    PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID,
    collect_package_network_publication_failures,
)
from objc3c_shared.schema_registry import schema_ids  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402
from scripts.objc3c_workflow.action_catalog_package_registry import (  # noqa: E402
    PACKAGE_REGISTRY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_registry_publication import (  # noqa: E402
    PACKAGE_NETWORK_RESOLUTION_SCHEMA,
    PACKAGE_REGISTRY_PUBLIC_ACTIONS,
    PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (  # noqa: E402
    ecosystem_publication_owner_contract,
)

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem"
NETWORK_RESOLUTION_FIXTURE = (
    FIXTURE_ROOT / "network_resolution" / "network-dependency-resolution.json"
)
RELEASE_CHANNEL_PUBLICATION_FIXTURE = (
    FIXTURE_ROOT / "network_resolution" / "package-release-channel-publication.json"
)
NEGATIVE_CASES_FIXTURE = (
    FIXTURE_ROOT / "network_resolution" / "negative-network-publication-cases.json"
)
HOSTED_REGISTRY_FIXTURE = FIXTURE_ROOT / "hosted_registry" / "hosted-registry-index.json"
OFFLINE_MIRROR_FIXTURE = FIXTURE_ROOT / "hosted_registry" / "offline-mirror-index.json"


def fixture_payloads() -> tuple[dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any]]:
    return (
        load_json(NETWORK_RESOLUTION_FIXTURE),
        load_json(RELEASE_CHANNEL_PUBLICATION_FIXTURE),
        load_json(HOSTED_REGISTRY_FIXTURE),
        load_json(OFFLINE_MIRROR_FIXTURE),
    )


def _select_path(container: Any, part: str) -> Any:
    if isinstance(container, list):
        return container[int(part)]
    if isinstance(container, dict):
        return container[part]
    raise AssertionError(f"cannot descend into {type(container).__name__}")


def _apply_mutation(payload: dict[str, Any], mutation: dict[str, Any]) -> None:
    path = str(mutation["set"]).split(".")
    current: Any = payload
    for part in path[:-1]:
        current = _select_path(current, part)
    if isinstance(current, list):
        current[int(path[-1])] = mutation["value"]
    else:
        current[path[-1]] = mutation["value"]


def test_package_network_resolution_and_release_channel_publication_are_source_owned() -> None:
    network_resolution, publication, hosted_index, mirror = fixture_payloads()

    failures = collect_package_network_publication_failures(
        network_resolution,
        publication,
        hosted_index=hosted_index,
        mirror=mirror,
    )

    assert failures == []
    assert network_resolution["contract_id"] == NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID
    assert publication["contract_id"] == PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID
    assert network_resolution["network_policy"]["fallback_registry_success"] is False
    assert network_resolution["network_policy"]["live_network_fetch"] == "forbidden-fail-closed"
    assert publication["publication_policy"]["fallback_publication_success"] is False
    assert publication["publication_policy"]["release_channel_id"] == "stable"
    assert publication["channel_freshness"]["status"] == "current"
    assert 8221 in network_resolution["issue_refs"]
    assert 8222 in publication["issue_refs"]
    assert 8204 in network_resolution["umbrella_issue_refs"]
    assert 8204 in publication["umbrella_issue_refs"]


def test_package_network_publication_negative_cases_fail_closed() -> None:
    negative_cases = load_json(NEGATIVE_CASES_FIXTURE)
    case_ids = {str(case["case_id"]) for case in negative_cases["cases"]}

    assert negative_cases["diagnostic_code"] == PACKAGE_MANAGER_TAMPER_CODE
    assert {
        "dependency-identity-drift",
        "hosted-registry-endpoint-mismatch",
        "lock-mismatch",
        "stale-channel",
        "unsigned-tampered-cache",
    } <= case_ids

    for case in negative_cases["cases"]:
        network_resolution, publication, hosted_index, mirror = fixture_payloads()
        target = str(case["target"])
        mutation = case["mutation"]
        assert isinstance(mutation, dict)
        if target == "network-resolution":
            _apply_mutation(network_resolution, mutation)
        elif target == "release-channel-publication":
            _apply_mutation(publication, mutation)
        else:
            raise AssertionError(f"unsupported negative target {target}")

        failures = collect_package_network_publication_failures(
            network_resolution,
            publication,
            hosted_index=hosted_index,
            mirror=mirror,
        )
        assert any(str(case["expected_failure"]) in failure for failure in failures), (
            case["case_id"],
            failures,
        )


def test_package_network_publication_schemas_and_action_are_registered() -> None:
    assert "objc3c-package-network-resolution-v1" in schema_ids()
    assert "objc3c-package-release-channel-publication-v1" in schema_ids()
    assert "validate-package-network-publication" in PACKAGE_REGISTRY_ACTION_SPECS
    assert (
        PACKAGE_REGISTRY_ACTION_SPECS["validate-package-network-publication"].backend
        == "python:scripts/check_objc3c_package_network_publication.py"
    )
    assert any(
        action.action == "validate-package-network-publication"
        and PACKAGE_NETWORK_RESOLUTION_SCHEMA in action.schema_contracts
        and PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA in action.schema_contracts
        for action in PACKAGE_REGISTRY_PUBLIC_ACTIONS
    )
    assert (
        ecosystem_publication_owner_contract(
            "validate-package-network-publication"
        ).owner_role
        == "package-ecosystem-network-publication-owner"
    )
