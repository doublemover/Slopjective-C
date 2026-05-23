from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import subprocess
import sys
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_package_manager.hosted_registry import sign_hosted_registry_record  # noqa: E402
from objc3c_package_manager.operations import (  # noqa: E402
    PACKAGE_OPERATION_HOSTED_SUPPORT,
    PACKAGE_OPERATION_NETWORK_POLICY,
    PACKAGE_OPERATION_RECEIPT_CONTRACT_ID,
    PACKAGE_OPERATION_RECEIPT_ROOT,
    PACKAGE_OPERATIONS,
    PackageOperationError,
    PackageOperationRequest,
    collect_package_operation_failures,
    collect_package_operation_receipt_failures,
    package_operation_plan,
    package_operation_receipt,
)
from objc3c_shared.schema_registry import validate_registered_schema  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402
from scripts.objc3c_workflow.action_catalog_package_integration import (  # noqa: E402
    PACKAGE_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.actions import ecosystem_publication_package  # noqa: E402
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (  # noqa: E402
    ecosystem_publication_owner_contract,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_package_contracts import (  # noqa: E402
    PACKAGE_OPERATIONS_PY,
    PACKAGE_PUBLICATION_ACTION_CONTRACTS,
)


LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
REGISTRY_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"


@pytest.fixture(scope="module")
def package_operation_inputs() -> tuple[dict[str, Any], dict[str, Any], dict[str, Any], str]:
    result = subprocess.run(
        [sys.executable, "scripts/build_objc3c_package_mirror.py"],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    lock = load_json(LOCK_PATH)
    mirror = load_json(MIRROR_PATH)
    registry = load_json(REGISTRY_PATH)
    dependency_sources = sorted(
        str(edge["from"])
        for edge in lock["dependencies"]
        if isinstance(edge, dict)
    )
    package_id = dependency_sources[0]
    return lock, mirror, registry, package_id


def installed_state(lock: dict[str, Any]) -> dict[str, Any]:
    records: list[dict[str, str]] = []
    for package in lock["packages"]:
        package_id = str(package["package_id"])
        namespace, _, name = package_id.partition(":")
        safe_name = name.replace(".", "_")
        records.append(
            {
                "package_id": package_id,
                "installed_manifest": (
                    "tmp/artifacts/package-ecosystem/install-validation/clean-root/"
                    f"objc3c/packages/{namespace}/{safe_name}/package-manifest.json"
                ),
                "local_install_artifact": (
                    "tmp/artifacts/package-ecosystem/install-validation/"
                    f"local-package-artifacts/{namespace}/{safe_name}.json"
                ),
            }
        )
    return {"installed_packages": records}


def operation_plan(
    inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
    operation: str,
    **request_overrides: Any,
) -> dict[str, Any]:
    lock, mirror, registry, package_id = inputs
    request = PackageOperationRequest(
        operation=operation,
        package_id=str(request_overrides.pop("package_id", package_id)),
        **request_overrides,
    )
    return package_operation_plan(
        root=ROOT,
        lock=lock,
        mirror=mirror,
        registry=registry,
        request=request,
        installed_state=installed_state(lock),
    )


def test_package_operations_emit_deterministic_plans_and_receipts(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    for operation in PACKAGE_OPERATIONS:
        plan = operation_plan(package_operation_inputs, operation)
        receipt = package_operation_receipt(plan)

        validate_registered_schema(receipt, "objc3c-package-operation-receipt-v1")
        assert receipt["contract_id"] == PACKAGE_OPERATION_RECEIPT_CONTRACT_ID
        assert receipt["operation"] == operation
        assert receipt["operation_action"] == f"package-{operation}"
        assert receipt["network_policy"] == PACKAGE_OPERATION_NETWORK_POLICY
        assert receipt["hosted_registry_support"] == PACKAGE_OPERATION_HOSTED_SUPPORT
        assert receipt["live_network_publication"] == "fail-closed"
        assert receipt["manifest_digest"].startswith("sha256:")
        assert receipt["offline_mirror_pin"]["cache_digest"].startswith("sha256:")
        assert receipt["rollback_token"].startswith("sha256:")
        assert receipt["machine_owned"] is True
        assert collect_package_operation_receipt_failures(plan=plan, receipt=receipt) == []


def test_package_install_and_update_bind_dependency_edges(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    install = operation_plan(package_operation_inputs, "install")
    update = operation_plan(package_operation_inputs, "update")

    assert install["dependency_edges"]
    assert update["dependency_edges"] == install["dependency_edges"]
    assert install["package_order"][-1] == install["package_id"]
    assert all(edge["abi_requirement"] == "objc3-abi-2025Q4" for edge in install["dependency_edges"])
    assert all(edge["language_requirement"] == "3.0" for edge in install["dependency_edges"])
    assert all(edge["source_authority"] == "showcase/portfolio.json" for edge in install["dependency_edges"])
    assert all(edge["source_authority_digest"].startswith("sha256:") for edge in install["dependency_edges"])


def test_package_publish_live_network_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    with pytest.raises(PackageOperationError) as exc_info:
        operation_plan(
            package_operation_inputs,
            "publish",
            allow_network=True,
            live_registry_url="https://registry.example.invalid/publish",
        )

    assert any("live network package publish rejected" in failure for failure in exc_info.value.failures)


def test_package_install_bad_signature_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    drifted_lock = deepcopy(lock)
    drifted_lock["packages"][0]["trust"]["signature"] = "sha256:" + ("0" * 64)

    failures = collect_package_operation_failures(
        root=ROOT,
        lock=drifted_lock,
        mirror=mirror,
        registry=registry,
        request=PackageOperationRequest(operation="install", package_id=package_id),
        installed_state=installed_state(lock),
    )

    assert any("bad signature" in failure or "lock trust signature drift" in failure for failure in failures)


def test_package_update_abi_dependency_mismatch_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    drifted_lock = deepcopy(lock)
    edge = next(edge for edge in drifted_lock["dependencies"] if edge["from"] == package_id)
    edge["abi_requirement"] = "objc3-abi-drift"

    failures = collect_package_operation_failures(
        root=ROOT,
        lock=drifted_lock,
        mirror=mirror,
        registry=registry,
        request=PackageOperationRequest(operation="update", package_id=package_id),
        installed_state=installed_state(lock),
    )

    assert any("dependency ABI mismatch" in failure for failure in failures)


def test_package_install_offline_cache_pin_mismatch_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    drifted_mirror = deepcopy(mirror)
    for package in drifted_mirror["packages"]:
        if package["package_id"] == package_id:
            package["cache_digest"] = "sha256:" + ("1" * 64)
            break

    failures = collect_package_operation_failures(
        root=ROOT,
        lock=lock,
        mirror=drifted_mirror,
        registry=registry,
        request=PackageOperationRequest(operation="install", package_id=package_id),
        installed_state=installed_state(lock),
    )

    assert any("offline mirror cache digest mismatch" in failure for failure in failures)


def test_package_uninstall_rejects_paths_outside_owned_roots(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    state = installed_state(lock)
    for record in state["installed_packages"]:
        if record["package_id"] == package_id:
            record["installed_manifest"] = "docs/support/not-owned.json"
            break

    failures = collect_package_operation_failures(
        root=ROOT,
        lock=lock,
        mirror=mirror,
        registry=registry,
        request=PackageOperationRequest(operation="uninstall", package_id=package_id),
        installed_state=state,
    )

    assert any("outside package-owned roots" in failure for failure in failures)


def test_package_rollback_missing_artifact_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    drifted_mirror = deepcopy(mirror)
    for package in drifted_mirror["packages"]:
        if package["package_id"] == package_id:
            package["cache_path"] = (
                "tmp/artifacts/package-ecosystem/mirrors/cache/missing/rollback.json"
            )
            break

    failures = collect_package_operation_failures(
        root=ROOT,
        lock=lock,
        mirror=drifted_mirror,
        registry=registry,
        request=PackageOperationRequest(operation="rollback", package_id=package_id),
        installed_state=installed_state(lock),
    )

    assert any("missing offline mirror cache entry" in failure for failure in failures)


def test_package_operation_receipt_digest_drift_fails_closed(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    plan = operation_plan(package_operation_inputs, "install")
    receipt = package_operation_receipt(plan)
    receipt["receipt_digest"] = "sha256:" + ("2" * 64)

    failures = collect_package_operation_receipt_failures(plan=plan, receipt=receipt)

    assert "O3PKG8055: install receipt payload drifted" in failures
    assert "O3PKG8055: operation receipt digest drifted" in failures


def test_package_operations_verify_hosted_registry_metadata_when_provided(
    package_operation_inputs: tuple[dict[str, Any], dict[str, Any], dict[str, Any], str],
) -> None:
    lock, mirror, registry, package_id = package_operation_inputs
    lock_package = next(package for package in lock["packages"] if package["package_id"] == package_id)
    mirror_package = next(package for package in mirror["packages"] if package["package_id"] == package_id)
    record = {
        "package_id": package_id,
        "source": lock_package["source"],
        "source_digest": lock_package["source_digest"],
        "source_kind": lock_package["source_kind"],
        "package_version": lock_package["package_version"],
        "language_version": lock_package["language_version"],
        "abi_identity": lock_package["abi_identity"],
        "package_manifest": lock_package["package_manifest"],
        "trust": lock_package["trust"],
        "offline_mirror": {
            "index_path": "tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json",
            "cache_path": mirror_package["cache_path"],
            "cache_digest": mirror_package["cache_digest"],
        },
        "selection": {
            "policy": "exact-version-from-offline-fixture",
            "candidate_rank": 0,
        },
    }
    hosted_registry = {
        "contract_id": "objc3c.package_ecosystem.hosted_registry_index.v1",
        "registry_id": "objc3c-hosted-registry-local-operation-fixture-v1",
        "registry_version": 1,
        "registry_state": "not-revoked",
        "network_policy": "offline-fixture-metadata-only",
        "resolver": "deterministic-hosted-registry-offline-resolver-v1",
        "language_version": "3.0",
        "abi_identity": "objc3-abi-2025Q4",
        "packages": [sign_hosted_registry_record(record)],
        "revocations": {
            "revoked_registry_ids": [],
            "revoked_package_ids": [],
            "revoked_signature_ids": [],
        },
        "replay": {
            "commands": [
                "npm run objc3c -- validate-package-registry-model",
                "npm run objc3c -- package-registry-resolve",
            ]
        },
    }
    failures = collect_package_operation_failures(
        root=ROOT,
        lock=lock,
        mirror=mirror,
        registry=registry,
        request=PackageOperationRequest(operation="install", package_id=package_id),
        hosted_registry=hosted_registry,
        hosted_mirror=mirror,
        installed_state=installed_state(lock),
    )
    assert failures == []

    hosted_registry["packages"][0]["metadata_digest"] = "sha256:" + ("3" * 64)
    failures = collect_package_operation_failures(
        root=ROOT,
        lock=lock,
        mirror=mirror,
        registry=registry,
        request=PackageOperationRequest(operation="install", package_id=package_id),
        hosted_registry=hosted_registry,
        hosted_mirror=mirror,
        installed_state=installed_state(lock),
    )
    assert any("registry metadata digest mismatch" in failure for failure in failures)


def test_package_operation_public_actions_are_registered(monkeypatch: pytest.MonkeyPatch) -> None:
    for action in (
        "package-publish",
        "package-install",
        "package-update",
        "package-uninstall",
        "package-rollback",
        "validate-package-operations",
    ):
        assert action in PACKAGE_INTEGRATION_ACTION_SPECS
        assert PACKAGE_INTEGRATION_ACTION_SPECS[action].pass_through_args
        assert PACKAGE_PUBLICATION_ACTION_CONTRACTS[action].script == PACKAGE_OPERATIONS_PY
        assert ecosystem_publication_owner_contract(action).owner_role == "package-ecosystem-operations-owner"

    captured: list[tuple[str, list[str]]] = []

    def fake_runner(action_name: str, rest: list[str] | None = None) -> int:
        captured.append((action_name, list(rest or [])))
        return 0

    monkeypatch.setattr(
        ecosystem_publication_package,
        "run_package_publication_action",
        fake_runner,
    )

    assert ecosystem_publication_package.action_package_publish([]) == 0
    assert ecosystem_publication_package.action_package_install(["--package-id", "showcase:demo"]) == 0
    assert captured[0] == ("package-publish", ["--operation", "publish"])
    assert captured[1] == (
        "package-install",
        ["--operation", "install", "--package-id", "showcase:demo"],
    )
    assert PACKAGE_OPERATION_RECEIPT_ROOT == "tmp/artifacts/package-ecosystem/operations"
