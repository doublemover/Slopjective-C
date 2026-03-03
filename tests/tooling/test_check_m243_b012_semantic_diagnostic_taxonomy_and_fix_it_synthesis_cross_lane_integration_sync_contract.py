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
    / "check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py"
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
        == "m243-b012-semantic-diagnostic-taxonomy-and-fixit-synthesis-cross-lane-integration-sync-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 44
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m243_b012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-B012/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_b012_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["expectations_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M243-B011`",
            "Dependencies: `M243-B099`",
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
    assert any(failure["check_id"] == "M243-B012-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_dependency_anchor_drops_b011(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            "npm run check:objc3c:m243-b011-lane-b-readiness",
            "npm run check:objc3c:m243-b099-lane-b-readiness",
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
    assert any(failure["check_id"] == "M243-B012-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        contract.ARTIFACTS["architecture_doc"].read_text(encoding="utf-8").replace(
            "M243 lane-B B012 semantic diagnostic taxonomy/fix-it synthesis cross-lane integration sync",
            "M243 lane-B B0XX semantic diagnostic taxonomy/fix-it synthesis cross-lane integration sync",
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
    assert any(failure["check_id"] == "M243-B012-ARCH-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b011_checker_dependency_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    artifacts = dict(contract.ARTIFACTS)
    artifacts["b011_checker"] = tmp_path / "missing_dependency.py"
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M243-B012-MISSING-B011_CHECKER" for failure in payload["failures"]
    )
