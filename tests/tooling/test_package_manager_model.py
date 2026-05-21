from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_package_manager.model import (  # noqa: E402
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    LOCAL_PACKAGE_TRUST_KEY_ID,
    PACKAGE_MANAGER_TAMPER_CODE,
    build_lock_components,
    collect_lock_model_failures,
    package_manifest_rel_path,
)
from objc3c_package_manager.registry import (  # noqa: E402
    LOCAL_REGISTRY_CONTRACT_ID,
    LOCAL_REGISTRY_SCHEMA_KEY,
    collect_registry_index_failures,
    local_registry_payload,
)
from objc3c_shared.schema_registry import validate_registered_schema  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402
from scripts.objc3c_workflow.action_catalog_package_lock import (  # noqa: E402
    PACKAGE_LOCK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_lock_contracts import (  # noqa: E402
    PACKAGE_LOCK_PUBLIC_ACTIONS,
    PACKAGE_MANIFEST_ARTIFACT_ROOT,
    PACKAGE_MANIFEST_SCHEMA,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (  # noqa: E402
    ecosystem_publication_owner_contract,
)


def lock_payload() -> dict[str, object]:
    module_inventory = load_json(ROOT / "stdlib" / "module_inventory.json")
    showcase_portfolio = load_json(ROOT / "showcase" / "portfolio.json")
    components = build_lock_components(
        root=ROOT,
        module_inventory=module_inventory,
        showcase_portfolio=showcase_portfolio,
    )
    for manifest in components["package_manifests"]:
        write_json_file(
            ROOT / package_manifest_rel_path(str(manifest["package_id"])),
            manifest,
        )
    return {
        "contract_id": "objc3c.package_ecosystem.lockfile.v1",
        "lockfile_version": 1,
        "workspace": {
            "workspace_id": "objc3c-local-package-workspace",
            "source": "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json",
        },
        "package_manager": {
            "model": "checked-in-local-registry-offline-mirror-v1",
            "language_version": LOCAL_PACKAGE_LANGUAGE_VERSION,
            "abi_identity": LOCAL_PACKAGE_ABI_IDENTITY,
            "package_manifest_root": PACKAGE_MANIFEST_ARTIFACT_ROOT,
            "package_manifest_paths": [
                str(package["package_manifest"]["path"])
                for package in components["packages"]
            ],
            "network_resolution": "unsupported-fail-closed",
            "hosted_registry": "unsupported-fail-closed-if-claimed",
        },
        "packages": components["packages"],
        "dependencies": components["dependencies"],
        "provenance": components["provenance"],
        "resolution_plan": components["resolution_plan"],
        "digest_inputs": components["digest_inputs"],
        "replay": {
            "commands": [
                "npm run objc3c -- build-package-lock",
                "npm run objc3c -- validate-package-manager-model",
                "npm run objc3c -- validate-package-authoring",
            ]
        },
    }


def expected_package_ids() -> list[str]:
    module_inventory = load_json(ROOT / "stdlib" / "module_inventory.json")
    showcase_portfolio = load_json(ROOT / "showcase" / "portfolio.json")
    stdlib_ids = [
        f"stdlib:{module['module']}"
        for module in module_inventory.get("canonical_modules", [])
        if isinstance(module, dict)
    ]
    showcase_ids = [
        f"showcase:{example['id']}"
        for example in showcase_portfolio.get("examples", [])
        if isinstance(example, dict)
    ]
    return sorted([*stdlib_ids, *showcase_ids])


def expected_dependency_count() -> int:
    showcase_portfolio = load_json(ROOT / "showcase" / "portfolio.json")
    return sum(
        len(example.get("stdlib_followup_modules", []))
        for example in showcase_portfolio.get("examples", [])
        if isinstance(example, dict)
    )


def test_package_manager_model_generates_manifest_backed_lock() -> None:
    payload = lock_payload()
    packages = payload["packages"]
    dependencies = payload["dependencies"]

    assert isinstance(packages, list)
    assert isinstance(dependencies, list)
    assert [package["package_id"] for package in packages] == expected_package_ids()
    assert len(dependencies) == expected_dependency_count()
    assert payload["package_manager"]["language_version"] == LOCAL_PACKAGE_LANGUAGE_VERSION
    assert payload["package_manager"]["abi_identity"] == LOCAL_PACKAGE_ABI_IDENTITY
    assert payload["package_manager"]["network_resolution"] == "unsupported-fail-closed"
    assert all(package["package_manifest"]["contract_id"] == "objc3c.package_ecosystem.package_manifest.v1" for package in packages)
    assert all(package["trust"]["signing_key_id"] == LOCAL_PACKAGE_TRUST_KEY_ID for package in packages)
    assert payload["resolution_plan"]["resolver"] == "deterministic-local-registry"
    assert payload["resolution_plan"]["selection_policy"] == "exact-locked-version-only"
    assert set(payload["resolution_plan"]["install_order"]) == set(expected_package_ids())
    assert all(
        dependency["required_version"] == dependency["resolved_version"]
        for dependency in dependencies
    )
    assert all(
        dependency["target_manifest_digest"].startswith("sha256:")
        for dependency in dependencies
    )
    assert collect_lock_model_failures(payload, root=ROOT) == []


def test_package_manager_dependency_abi_drift_fails_closed() -> None:
    payload = lock_payload()
    dependencies = deepcopy(payload["dependencies"])
    dependencies[0]["abi_requirement"] = "objc3-abi-drift"
    payload["dependencies"] = dependencies

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI requirement mismatch for "
        f"{dependencies[0]['from']}->{dependencies[0]['to']}"
    ) in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest dependency closure drift "
        f"for {dependencies[0]['from']}"
    ) in failures


def test_package_manager_dependency_version_and_manifest_digest_drift_fail_closed() -> None:
    payload = lock_payload()
    dependencies = deepcopy(payload["dependencies"])
    dependencies[0]["required_version"] = "9.9.9"
    dependencies[0]["target_manifest_digest"] = "sha256:" + ("0" * 64)
    payload["dependencies"] = dependencies

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: package resolution plan drifted" in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: version requirement mismatch for "
        f"{dependencies[0]['from']}->{dependencies[0]['to']}"
    ) in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: target manifest digest mismatch for "
        f"{dependencies[0]['from']}->{dependencies[0]['to']}"
    ) in failures


def test_package_manager_manifest_trust_tamper_fails_closed() -> None:
    payload = lock_payload()
    packages = deepcopy(payload["packages"])
    packages[0]["trust"]["signature"] = "sha256:" + ("1" * 64)
    payload["packages"] = packages

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: lock trust signature drift for "
        f"{packages[0]['package_id']}"
    ) in failures


def test_package_manager_revocation_fails_resolution() -> None:
    payload = lock_payload()
    packages = deepcopy(payload["packages"])
    packages[0]["trust"]["revocation_state"] = "revoked"
    payload["packages"] = packages

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package cannot resolve "
        f"{packages[0]['package_id']}"
    ) in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: lock trust signature drift for "
        f"{packages[0]['package_id']}"
    ) in failures


def test_local_registry_index_records_exact_version_dependencies_and_replay_evidence() -> None:
    payload = lock_payload()
    registry = local_registry_payload(
        payload,
        source_mirror="tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json",
        source_restore_receipt=(
            "tmp/artifacts/package-ecosystem/offline-install/"
            "objc3c-offline-mirror-restore-receipt.json"
        ),
    )

    validate_registered_schema(registry, LOCAL_REGISTRY_SCHEMA_KEY)

    assert registry["contract_id"] == LOCAL_REGISTRY_CONTRACT_ID
    assert registry["registry_policy"]["dependency_resolution"] == "locked-local-registry-only"
    assert registry["registry_policy"]["version_selection"] == "exact-locked-version-only"
    assert registry["registry_policy"]["hosted_registry"] == "unsupported-fail-closed-if-claimed"
    assert registry["resolution_plan"] == payload["resolution_plan"]
    assert registry["resolution_plan"]["install_order"] == payload["resolution_plan"]["install_order"]
    assert registry["error_policy"]["diagnostic_code"] == PACKAGE_MANAGER_TAMPER_CODE
    assert collect_registry_index_failures(registry, payload, root=ROOT) == []

    dependency_edge = registry["dependency_edges"][0]
    target_package = next(
        package
        for package in payload["packages"]
        if package["package_id"] == dependency_edge["to"]
    )
    assert dependency_edge["required_version"] == target_package["package_version"]
    assert dependency_edge["resolved_version"] == target_package["package_version"]
    assert dependency_edge["target_manifest_digest"] == target_package["package_manifest"]["digest"]

    source_package = next(
        package
        for package in registry["packages"]
        if package["package_id"] == dependency_edge["from"]
    )
    assert source_package["version"]["candidate_versions"] == [
        source_package["package_version"]
    ]
    assert source_package["evidence"]["replay_commands"] == [
        "npm run objc3c -- build-package-lock",
        "npm run objc3c -- validate-package-manager-model",
        "npm run objc3c -- validate-package-mirror",
    ]


def test_local_registry_hosted_claim_fails_closed() -> None:
    payload = lock_payload()
    registry = local_registry_payload(
        payload,
        source_mirror="tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json",
        source_restore_receipt=(
            "tmp/artifacts/package-ecosystem/offline-install/"
            "objc3c-offline-mirror-restore-receipt.json"
        ),
    )
    registry["registry_policy"]["hosted_registry"] = "supported"

    failures = collect_registry_index_failures(registry, payload, root=ROOT)

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: registry policy drifted for hosted_registry" in failures


def test_local_registry_dependency_version_drift_fails_closed() -> None:
    payload = lock_payload()
    registry = local_registry_payload(
        payload,
        source_mirror="tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json",
        source_restore_receipt=(
            "tmp/artifacts/package-ecosystem/offline-install/"
            "objc3c-offline-mirror-restore-receipt.json"
        ),
    )
    edge = registry["dependency_edges"][0]
    edge["required_version"] = "9.9.9"

    failures = collect_registry_index_failures(registry, payload, root=ROOT)

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency version mismatch "
        f"for {edge['from']}->{edge['to']}"
    ) in failures


def test_package_manager_public_action_and_owner_contract_are_registered() -> None:
    assert "validate-package-manager-model" in PACKAGE_LOCK_ACTION_SPECS
    action = PACKAGE_LOCK_ACTION_SPECS["validate-package-manager-model"]
    assert action.backend == "python:scripts/check_objc3c_package_manager_model.py"
    public_action = next(
        item
        for item in PACKAGE_LOCK_PUBLIC_ACTIONS
        if item.action == "validate-package-manager-model"
    )
    assert PACKAGE_MANIFEST_SCHEMA in public_action.schema_contracts

    contract = ecosystem_publication_owner_contract("validate-package-manager-model")
    assert contract.owner_role == "package-ecosystem-manager-owner"
    assert "package_manager_model_contract.json" in " ".join(contract.source_contracts)
    assert not contract.wrapper_only_allowed
