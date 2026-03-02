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
    / "check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m228_e002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m228/M228-E002/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("scripts/does_not_exist_m228_e002_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_doc_drops_c004_dependency(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m228_lane_e_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_e002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M228-C004`", "`M228-C099`"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E002-DOC-EXP-07" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_row_drifts(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path / "m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_packet.md"
    )
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M228-E001`, `M228-A002`, `M228-B002`, `M228-C004`, `M228-D002`",
            "Dependencies: `M228-E001`, `M228-A002`, `M228-B002`, `M228-C099`, `M228-D002`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E002-DOC-PKT-05" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m228-e002-lane-e-readiness": '
            '"npm run check:objc3c:m228-e001-lane-e-readiness '
            '&& npm run check:objc3c:m228-a002-lane-a-readiness '
            '&& npm run check:objc3c:m228-b002-lane-b-readiness '
            '&& npm run check:objc3c:m228-d002-lane-d-readiness '
            '&& npm run check:objc3c:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract"',
            '"check:objc3c:m228-e002-lane-e-readiness": '
            '"npm run check:objc3c:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E002-PKG-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_optimization_input(tmp_path: Path) -> None:
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
    exit_code = contract.run(
        [
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E002-PKG-06" for failure in payload["failures"])
