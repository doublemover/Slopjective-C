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
    / "check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py"
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
        == "m228-e007-replay-proof-performance-closeout-gate-diagnostics-hardening-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m228_e007() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m228/M228-E007/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("docs/contracts/does_not_exist_m228_e007.md"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_pending_c007_token(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m228_lane_e_replay_proof_and_performance_closeout_gate_diagnostics_hardening_e007_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M228-C007`", "`M228-C099`", 1),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E007-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_packet.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M228-E006`, `M228-A007`, `M228-B007`, `M228-D007`",
            "Dependencies: `M228-E006`, `M228-A007`, `M228-B099`, `M228-D007`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E007-DOC-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_runbook_anchor_drifts(tmp_path: Path) -> None:
    drift_runbook = tmp_path / "m228_wave_execution_runbook.md"
    drift_runbook.write_text(
        contract.DEFAULT_RUNBOOK_DOC.read_text(encoding="utf-8").replace(
            "objc3c-lane-e-replay-proof-performance-closeout-gate-diagnostics-hardening-contract/m228-e007-v1",
            "objc3c-lane-e-replay-proof-performance-closeout-gate-diagnostics-hardening-contract/m228-e999-v1",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--runbook-doc", str(drift_runbook), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E007-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_c007_readiness(tmp_path: Path) -> None:
    drift_pkg = tmp_path / "package.json"
    package_payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = package_payload["scripts"]
    scripts["check:objc3c:m228-e007-lane-e-readiness"] = scripts[
        "check:objc3c:m228-e007-lane-e-readiness"
    ].replace(
        "npm run check:objc3c:m228-c007-lane-c-readiness && ",
        "",
        1,
    )
    drift_pkg.write_text(json.dumps(package_payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_pkg), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-E007-CFG-06" for failure in payload["failures"])
