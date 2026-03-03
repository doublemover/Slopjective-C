from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m243_d001_cli_reporting_and_output_contract_integration_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m243_d001_cli_reporting_and_output_contract_integration_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m243-d001-cli-reporting-output-contract-integration-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m243_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-D001/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_cli_reporting_and_output_contract_integration_d001_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-freeze/m243-d001-v1`",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-freeze/m243-d001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_summary_mode_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_runner = tmp_path / "objc3c_frontend_c_api_runner.cpp"
    drift_runner.write_text(
        contract.ARTIFACTS["c_api_runner_source"].read_text(encoding="utf-8").replace(
            'out << "  \\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\",\\n";',
            'out << "  \\"mode\\": \\"objc3c-frontend-c-api-runner-v2\\",\\n";',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["c_api_runner_source"] = drift_runner
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D001-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_drops_tooling_test(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m243-d001-lane-d-readiness": '
            '"npm run check:objc3c:m243-d001-cli-reporting-output-contract-integration-contract '
            '&& npm run test:tooling:m243-d001-cli-reporting-output-contract-integration-contract"',
            '"check:objc3c:m243-d001-lane-d-readiness": '
            '"npm run check:objc3c:m243-d001-cli-reporting-output-contract-integration-contract"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_anchor_drops_contract_phrase(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_spec = tmp_path / "LOWERING_AND_RUNTIME_CONTRACTS.md"
    drift_spec.write_text(
        contract.ARTIFACTS["lowering_spec"].read_text(encoding="utf-8").replace(
            "CLI/reporting and output contract integration governance shall preserve",
            "CLI/reporting and output governance shall preserve",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["lowering_spec"] = drift_spec
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D001-SPC-01" for failure in payload["failures"])
