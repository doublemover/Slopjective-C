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
    / "check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m243-d021-cli-reporting-output-contract-integration-advanced-core-workpack-shard-2-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 34
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m243_d021() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-D021/")


def test_contract_emit_json_prints_canonical_summary(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--emit-json", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    stdout_payload = json.loads(capsys.readouterr().out)
    assert stdout_payload == payload
    assert payload["ok"] is True


def test_contract_fails_closed_when_expectations_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_d021_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["expectations_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M243-D020`",
            "Dependencies: `M243-D099`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["expectations_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D021-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_d020(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            "npm run check:objc3c:m243-d020-lane-d-readiness",
            "npm run check:objc3c:m243-d099-lane-d-readiness",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D021-PKG-04" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        contract.ARTIFACTS["architecture_doc"].read_text(encoding="utf-8").replace(
            "M243 lane-D D021 advanced core workpack (shard 2) anchors CLI/reporting output contract integration",
            "M243 lane-D D021 docs/runbook placeholder anchor",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["architecture_doc"] = drift_architecture
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-D021-ARC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_d020_checker_dependency_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    artifacts = dict(contract.ARTIFACTS)
    artifacts["d020_checker"] = tmp_path / "missing_dependency.py"
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M243-D021-MISSING-D020_CHECKER" for failure in payload["failures"]
    )








