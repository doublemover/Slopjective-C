from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_a012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-A012/")


def test_contract_emits_json_when_requested(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_prerequisite_a011_asset_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_a011_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_A011_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-performance-and-quality-guardrails/m248-a011-v1`",
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-performance-and-quality-guardrails/m248-a011-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--a011-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A012-DEP-01" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_a011_dependency(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_a012_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M248-A011`",
            "Dependencies: `M248-A099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A012-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_sync_key_ready(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_a012_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "- `cross_lane_integration_sync_key_ready`\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A012-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_a011_readiness(
    tmp_path: Path,
) -> None:
    drift_pkg = tmp_path / "package.json"
    package_payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = package_payload["scripts"]
    scripts["check:objc3c:m248-a012-lane-a-readiness"] = scripts[
        "check:objc3c:m248-a012-lane-a-readiness"
    ].replace("npm run check:objc3c:m248-a011-lane-a-readiness && ", "", 1)
    drift_pkg.write_text(json.dumps(package_payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_pkg), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A012-PKG-08" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_milestone_optimization_input(
    tmp_path: Path,
) -> None:
    drift_pkg = tmp_path / "package.json"
    package_payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = package_payload["scripts"]
    scripts.pop("test:objc3c:parser-ast-extraction", None)
    drift_pkg.write_text(json.dumps(package_payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_pkg), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A012-PKG-05" for failure in payload["failures"])
