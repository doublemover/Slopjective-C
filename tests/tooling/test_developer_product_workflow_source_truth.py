from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts import check_developer_tooling_product_workflow_source_truth as checker  # noqa: E402


def load_contract() -> dict[str, object]:
    return checker.load_json_object(checker.CONTRACT_PATH)


def package_stage(contract: dict[str, object]) -> dict[str, object]:
    journey = contract["normal_developer_from_nothing"]
    assert isinstance(journey, dict)
    stages = journey["stages"]
    assert isinstance(stages, list)
    for stage in stages:
        assert isinstance(stage, dict)
        if stage["stage_id"] == "package-install-receipts":
            return stage
    raise AssertionError("package-install-receipts stage missing")


def test_product_workflow_source_truth_validates_normal_developer_journey() -> None:
    summary = checker.validate_contract(load_contract())

    assert summary["ok"] is True
    journey = summary["normal_developer_from_nothing"]
    assert journey["status"] == "PASS"
    assert journey["covered_issues"] == sorted(checker.OWNED_ISSUES)
    stage_by_id = {stage["stage_id"]: stage for stage in journey["stages"]}
    package = stage_by_id["package-install-receipts"]
    assert package["public_action"] == "validate-package-install-distribution"
    assert package["clean_start_required"] is True
    assert package["required_receipt_count"] == 3


def test_product_workflow_rejects_clean_package_stage_without_from_nothing() -> None:
    contract = deepcopy(load_contract())
    stage = package_stage(contract)
    stage["public_command"] = "npm run objc3c -- validate-package-install-distribution"
    for row in contract["workflow_rows"]:
        assert isinstance(row, dict)
        if row["workflow_id"] == "package-manager-local-registry":
            row["public_commands"].append(stage["public_command"])

    summary = checker.validate_contract(contract)

    assert summary["ok"] is False
    assert any(
        "package-install-receipts: clean-start stage must use --from-nothing public replay"
        in failure
        for failure in summary["failures"]
    )


def test_product_workflow_rejects_generated_report_as_source_truth() -> None:
    contract = deepcopy(load_contract())
    stage = package_stage(contract)
    stage["source_truth_paths"].append(
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    )

    summary = checker.validate_contract(contract)

    assert summary["ok"] is False
    assert any(
        "source truth cannot use generated output" in failure
        for failure in summary["failures"]
    )


def test_product_workflow_requires_stage_source_truth_on_workflow_row() -> None:
    contract = deepcopy(load_contract())
    stage = package_stage(contract)
    stage["source_truth_paths"].append("tests/tooling/fixtures/package_ecosystem/artifact_contract.json")

    summary = checker.validate_contract(contract)

    assert summary["ok"] is False
    assert any(
        "stage source truth paths are missing from workflow row" in failure
        for failure in summary["failures"]
    )
