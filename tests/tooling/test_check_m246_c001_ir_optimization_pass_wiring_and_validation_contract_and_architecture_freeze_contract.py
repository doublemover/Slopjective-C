from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m246-c001-ir-optimization-pass-wiring-validation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_c001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-C001/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#5077` defines canonical lane-C contract freeze scope.",
            "Issue `#5000` defines canonical lane-C contract freeze scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C001-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m246-c001-lane-c-readiness": '
            '"npm run check:objc3c:m246-c001-ir-optimization-pass-wiring-validation-contract '
            '&& npm run test:tooling:m246-c001-ir-optimization-pass-wiring-validation-contract"',
            '"check:objc3c:m246-c001-lane-c-readiness": '
            '"npm run check:objc3c:m246-c001-ir-optimization-pass-wiring-validation-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"test:objc3c:perf-budget": ',
            '"test:objc3c:perf-budget-disabled": ',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C001-PKG-07" for failure in payload["failures"])


def test_contract_reports_failures_in_deterministic_sorted_order(tmp_path: Path) -> None:
    drift_expectations = (
        tmp_path / "m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md"
    )
    drift_expectations.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#5077` defines canonical lane-C contract freeze scope.",
            "Issue `#5000` defines canonical lane-C contract freeze scope.",
            1,
        ),
        encoding="utf-8",
    )

    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        contract.DEFAULT_ARCHITECTURE_DOC.read_text(encoding="utf-8").replace(
            "M246 lane-C C001 IR optimization pass wiring and validation contract and architecture freeze anchors",
            "M246 lane-C C001 IR optimization pass wiring and validation freeze anchors",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_expectations),
            "--architecture-doc",
            str(drift_architecture),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    sort_key = lambda failure: (failure["check_id"], failure["artifact"], failure["detail"])
    assert payload["failures"] == sorted(payload["failures"], key=sort_key)
