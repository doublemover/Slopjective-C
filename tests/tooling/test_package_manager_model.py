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
)
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402
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
    assert collect_lock_model_failures(payload, root=ROOT) == []


def test_package_manager_dependency_abi_drift_fails_closed() -> None:
    payload = lock_payload()
    dependencies = deepcopy(payload["dependencies"])
    dependencies[0]["abi_requirement"] = "objc3-abi-drift"
    payload["dependencies"] = dependencies

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert failures == [
        f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI requirement mismatch for {dependencies[0]['from']}->{dependencies[0]['to']}"
    ]


def test_package_manager_revocation_fails_resolution() -> None:
    payload = lock_payload()
    packages = deepcopy(payload["packages"])
    packages[0]["trust"]["revocation_state"] = "revoked"
    payload["packages"] = packages

    failures = collect_lock_model_failures(payload, root=ROOT)

    assert failures == [
        f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package cannot resolve {packages[0]['package_id']}"
    ]


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
