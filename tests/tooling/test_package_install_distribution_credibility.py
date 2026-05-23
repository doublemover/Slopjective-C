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

from objc3c_package_manager.install_distribution import (  # noqa: E402
    INSTALL_DISTRIBUTION_ACTION,
    INSTALL_DISTRIBUTION_CONTRACT_ID,
    INSTALL_HOME_REL,
    INSTALL_LOCAL_ARTIFACT_ROOT_REL,
    INSTALL_PROOF_MANIFEST_REL,
    INSTALL_RECEIPT_REL,
    INSTALL_ROOT_REL,
    INSTALL_VALIDATION_ROOT_REL,
    INSTALL_VERIFICATION_REL,
    PACKAGE_OPERATION_RECEIPT_CONTRACT_ID,
    PACKAGE_UNINSTALL_RECEIPT_REL,
    PACKAGE_UPDATE_RECEIPT_REL,
    collect_install_distribution_failures,
    collect_install_proof_failures,
    collect_package_operation_receipt_failures,
)
from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402
from scripts.objc3c_workflow.action_catalog_package_integration import (  # noqa: E402
    PACKAGE_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (  # noqa: E402
    ecosystem_publication_owner_contract,
)
from scripts.objc3c_workflow.actions import ecosystem_publication_package  # noqa: E402
from scripts.objc3c_workflow.actions.ecosystem_publication_package_contracts import (  # noqa: E402
    PACKAGE_INSTALL_DISTRIBUTION_PY,
    PACKAGE_PUBLICATION_ACTION_CONTRACTS,
)


CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "install_distribution_credibility_contract.json"
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
REGISTRY_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"
PUBLICATION_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "publication-metadata.json"
RESTORE_RECEIPT_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "offline-install" / "objc3c-offline-mirror-restore-receipt.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "install-distribution-credibility-summary.json"
VERIFICATION_PATH = ROOT / INSTALL_VERIFICATION_REL


@pytest.fixture(scope="module")
def install_summary() -> dict[str, Any]:
    result = subprocess.run(
        [
            sys.executable,
            "scripts/check_objc3c_package_install_distribution_credibility.py",
            "--from-nothing",
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    return load_json(SUMMARY_PATH)


def generated_payloads() -> tuple[dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any]]:
    return (
        load_json(CONTRACT_PATH),
        load_json(LOCK_PATH),
        load_json(MIRROR_PATH),
        load_json(REGISTRY_PATH),
        load_json(PUBLICATION_PATH),
        load_json(RESTORE_RECEIPT_PATH),
        load_json(VERIFICATION_PATH),
    )


def test_install_distribution_check_generates_clean_root_summary(install_summary: dict[str, Any]) -> None:
    contract = load_json(CONTRACT_PATH)

    assert install_summary["status"] == "PASS"
    assert install_summary["contract_id"] == "objc3c.package_ecosystem.install_distribution_credibility.summary.v1"
    assert install_summary["package_count"] >= contract["minimum_package_count"]
    assert install_summary["installed_package_count"] == install_summary["package_count"]
    assert install_summary["dependency_count"] >= contract["minimum_dependency_count"]
    assert install_summary["network_policy"] == "no-network-during-validation"
    assert install_summary["hosted_registry_support"] == "unsupported-fail-closed-if-claimed"
    assert install_summary["install_receipt"] == INSTALL_RECEIPT_REL
    assert install_summary["update_receipt"] == PACKAGE_UPDATE_RECEIPT_REL
    assert install_summary["uninstall_receipt"] == PACKAGE_UNINSTALL_RECEIPT_REL
    from_nothing = install_summary["from_nothing_probe"]
    assert from_nothing["requested"] is True
    assert from_nothing["generated_from_clean_owned_outputs"] is True
    owned_outputs_after_clean = from_nothing["owned_outputs_exist_after_clean"]
    assert owned_outputs_after_clean["tmp/artifacts/package-ecosystem/install-validation"] is False
    assert owned_outputs_after_clean["tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"] is False
    assert not any(owned_outputs_after_clean.values())
    assert (ROOT / INSTALL_ROOT_REL).is_dir()
    assert (ROOT / INSTALL_HOME_REL / "Bootstrap-objc3cEnvironment.ps1").is_file()
    assert (ROOT / INSTALL_RECEIPT_REL).is_file()
    assert (ROOT / PACKAGE_UPDATE_RECEIPT_REL).is_file()
    assert (ROOT / PACKAGE_UNINSTALL_RECEIPT_REL).is_file()
    assert (ROOT / INSTALL_VERIFICATION_REL).is_file()
    assert (ROOT / INSTALL_PROOF_MANIFEST_REL).is_file()
    assert (ROOT / INSTALL_LOCAL_ARTIFACT_ROOT_REL).is_dir()
    assert all(
        str(path).startswith(INSTALL_VALIDATION_ROOT_REL + "/")
        for path in install_summary["generated_paths"]
    )
    verification = load_json(VERIFICATION_PATH)
    lock = load_json(LOCK_PATH)
    assert verification["install_order"] == lock["resolution_plan"]["install_order"]
    assert [
        package["package_id"]
        for package in verification["installed_packages"]
    ] == lock["resolution_plan"]["install_order"]
    assert verification["update_plan_order"] == lock["resolution_plan"]["install_order"]
    assert verification["uninstall_plan_order"] == list(
        reversed(lock["resolution_plan"]["install_order"])
    )


def test_install_distribution_operation_receipts_bind_update_and_uninstall_plans(
    install_summary: dict[str, Any],
) -> None:
    assert install_summary["status"] == "PASS"
    lock = load_json(LOCK_PATH)
    verification = load_json(VERIFICATION_PATH)
    update_receipt = load_json(ROOT / PACKAGE_UPDATE_RECEIPT_REL)
    uninstall_receipt = load_json(ROOT / PACKAGE_UNINSTALL_RECEIPT_REL)
    install_order = lock["resolution_plan"]["install_order"]

    assert update_receipt["contract_id"] == PACKAGE_OPERATION_RECEIPT_CONTRACT_ID
    assert uninstall_receipt["contract_id"] == PACKAGE_OPERATION_RECEIPT_CONTRACT_ID
    assert update_receipt["operation"] == "update"
    assert uninstall_receipt["operation"] == "uninstall"
    assert update_receipt["package_order"] == install_order
    assert uninstall_receipt["package_order"] == list(reversed(install_order))
    assert update_receipt["network_policy"] == "no-network-during-validation"
    assert uninstall_receipt["hosted_registry_support"] == "unsupported-fail-closed-if-claimed"
    assert update_receipt["package_count"] == verification["manifest_count"]
    assert uninstall_receipt["package_count"] == verification["manifest_count"]
    assert [entry["package_id"] for entry in update_receipt["package_digests"]] == install_order
    assert [entry["package_id"] for entry in uninstall_receipt["package_digests"]] == list(
        reversed(install_order)
    )


def test_install_distribution_contract_fails_on_manifest_digest_drift(install_summary: dict[str, Any]) -> None:
    assert install_summary["status"] == "PASS"
    contract, lock, mirror, registry, publication, restore_receipt, verification = generated_payloads()
    drifted = deepcopy(verification)
    drifted["installed_packages"][0]["manifest_digest"] = "sha256:" + ("0" * 64)

    failures = collect_install_distribution_failures(
        root=ROOT,
        contract=contract,
        lock=lock,
        mirror=mirror,
        registry=registry,
        publication=publication,
        restore_receipt=restore_receipt,
        verification=drifted,
    )

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: installed manifest digest drifted for "
        f"{drifted['installed_packages'][0]['package_id']}"
    ) in failures


def test_install_distribution_contract_fails_on_hosted_registry_widening(install_summary: dict[str, Any]) -> None:
    assert install_summary["status"] == "PASS"
    contract, lock, mirror, registry, publication, restore_receipt, verification = generated_payloads()
    drifted_publication = deepcopy(publication)
    drifted_publication["hosted_registry_support"] = "supported"
    drifted_verification = deepcopy(verification)
    drifted_verification["hosted_registry_support"] = "supported"

    failures = collect_install_distribution_failures(
        root=ROOT,
        contract=contract,
        lock=lock,
        mirror=mirror,
        registry=registry,
        publication=drifted_publication,
        restore_receipt=restore_receipt,
        verification=drifted_verification,
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry support widened" in failures
    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: publication hosted registry support widened" in failures


def test_install_distribution_proof_manifest_excludes_generated_reports(
    install_summary: dict[str, Any],
) -> None:
    assert install_summary["status"] == "PASS"
    proof = load_json(ROOT / INSTALL_PROOF_MANIFEST_REL)
    verification = load_json(VERIFICATION_PATH)

    release_validation = proof["release_manifest_validation"]
    assert release_validation["generated_report_inputs_allowed"] is False
    assert release_validation["generated_report_inputs"] == []
    assert release_validation["forbidden_input_prefixes"] == ["tmp/reports/"]
    assert all(
        not path.startswith("tmp/reports/")
        for path in release_validation["release_manifest_input_paths"]
    )
    assert proof["local_package_artifacts"]
    assert len(proof["local_package_artifacts"]) == verification["manifest_count"]
    assert PACKAGE_UPDATE_RECEIPT_REL in release_validation["release_manifest_input_paths"]
    assert PACKAGE_UNINSTALL_RECEIPT_REL in release_validation["release_manifest_input_paths"]


def test_install_distribution_proof_rejects_report_release_input(
    install_summary: dict[str, Any],
) -> None:
    assert install_summary["status"] == "PASS"
    proof = load_json(ROOT / INSTALL_PROOF_MANIFEST_REL)
    verification = load_json(VERIFICATION_PATH)
    drifted = deepcopy(proof)
    drifted["release_manifest_validation"]["generated_report_inputs"] = [
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    ]
    drifted["release_manifest_validation"]["release_manifest_input_paths"].append(
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    )

    failures = collect_install_proof_failures(
        root=ROOT,
        proof=drifted,
        verification=verification,
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: generated report input list is not empty" in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: generated report used as release input: "
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    ) in failures


def test_install_distribution_proof_rejects_local_artifact_digest_drift(
    install_summary: dict[str, Any],
) -> None:
    assert install_summary["status"] == "PASS"
    proof = load_json(ROOT / INSTALL_PROOF_MANIFEST_REL)
    verification = load_json(VERIFICATION_PATH)
    drifted = deepcopy(proof)
    drifted["local_package_artifacts"][0]["artifact_digest"] = "sha256:" + ("1" * 64)
    package_id = drifted["local_package_artifacts"][0]["package_id"]

    failures = collect_install_proof_failures(
        root=ROOT,
        proof=drifted,
        verification=verification,
    )

    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact digest record drifted for {package_id}"
    ) in failures
    assert (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact digest drifted for {package_id}"
    ) in failures


def test_install_distribution_rejects_operation_receipt_order_drift(
    install_summary: dict[str, Any],
) -> None:
    assert install_summary["status"] == "PASS"
    lock = load_json(LOCK_PATH)
    verification = load_json(VERIFICATION_PATH)
    expected_order = lock["resolution_plan"]["install_order"]
    drifted_verification = deepcopy(verification)
    drifted_verification["update_plan_order"] = list(reversed(expected_order))

    failures = collect_package_operation_receipt_failures(
        root=ROOT,
        lock=lock,
        verification=drifted_verification,
        receipt_path=ROOT / PACKAGE_UPDATE_RECEIPT_REL,
        operation="update",
        expected_order=expected_order,
    )

    assert f"{PACKAGE_MANAGER_TAMPER_CODE}: update plan order drifted" in failures


def test_install_distribution_public_action_and_owner_contract_are_registered() -> None:
    assert INSTALL_DISTRIBUTION_ACTION in PACKAGE_INTEGRATION_ACTION_SPECS
    assert PACKAGE_INTEGRATION_ACTION_SPECS[INSTALL_DISTRIBUTION_ACTION].pass_through_args
    assert PACKAGE_PUBLICATION_ACTION_CONTRACTS[INSTALL_DISTRIBUTION_ACTION].script == PACKAGE_INSTALL_DISTRIBUTION_PY
    assert PACKAGE_INSTALL_DISTRIBUTION_PY == ROOT / "scripts" / "check_objc3c_package_install_distribution_credibility.py"

    contract = ecosystem_publication_owner_contract(INSTALL_DISTRIBUTION_ACTION)
    assert contract.owner_role == "package-ecosystem-install-owner"
    assert "install_distribution_credibility_contract.json" in " ".join(contract.source_contracts)
    assert not contract.evidence_log_allowed
    assert not contract.wrapper_only_allowed


def test_install_distribution_public_action_passes_from_nothing_flag(monkeypatch: pytest.MonkeyPatch) -> None:
    captured: dict[str, object] = {}

    def fake_runner(action_name: str, rest: list[str] | None = None) -> int:
        captured["action_name"] = action_name
        captured["rest"] = list(rest or [])
        return 0

    monkeypatch.setattr(
        ecosystem_publication_package,
        "run_package_publication_action",
        fake_runner,
    )

    assert ecosystem_publication_package.action_validate_package_install_distribution(["--from-nothing"]) == 0
    assert captured == {
        "action_name": "validate-package-install-distribution",
        "rest": ["--from-nothing"],
    }


def test_install_distribution_public_action_defaults_to_from_nothing(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    captured: dict[str, object] = {}

    def fake_runner(action_name: str, rest: list[str] | None = None) -> int:
        captured["action_name"] = action_name
        captured["rest"] = list(rest or [])
        return 0

    monkeypatch.setattr(
        ecosystem_publication_package,
        "run_package_publication_action",
        fake_runner,
    )

    assert ecosystem_publication_package.action_validate_package_install_distribution([]) == 0
    assert captured == {
        "action_name": "validate-package-install-distribution",
        "rest": ["--from-nothing"],
    }


def test_install_distribution_verification_keeps_package_contract_boundary(install_summary: dict[str, Any]) -> None:
    assert install_summary["status"] == "PASS"
    verification = load_json(VERIFICATION_PATH)

    assert verification["contract_id"] == INSTALL_DISTRIBUTION_CONTRACT_ID
    assert verification["clean_start"]["stale_artifacts_allowed"] is False
    assert verification["source_lock"] == "tmp/artifacts/package-ecosystem/locks/objc3c-package-lock.json"
    assert verification["package_bridge"] == "objc3c"
    assert INSTALL_DISTRIBUTION_ACTION in verification["public_actions"]
