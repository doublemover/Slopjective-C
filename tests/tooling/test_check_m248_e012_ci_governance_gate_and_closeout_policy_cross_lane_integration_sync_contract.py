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
    / "check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m248-e012-lane-e-ci-governance-gate-closeout-policy-cross-lane-integration-sync-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_e012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-E012/")


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


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("docs/contracts/does_not_exist_m248_e012.md"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_lane_e_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_e012_expectations.md"
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    anchor = "Issue `#6872` defines canonical lane-E cross-lane integration sync scope."
    assert anchor in original
    drift_doc.write_text(original.replace(anchor, "Issue `#6999` defines canonical lane-E cross-lane integration sync scope."), encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E012-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_readiness_command_name(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_lane_e_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_e012_expectations.md"
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    anchor = "`check:objc3c:m248-c012-lane-c-readiness`"
    assert anchor in original
    drift_text = original.replace(anchor, "`check:objc3c:m248-c999-lane-c-readiness`")
    assert anchor not in drift_text
    drift_doc.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E012-DOC-EXP-11" for failure in payload["failures"])


def test_contract_drift_findings_are_deterministic_across_runs(tmp_path: Path) -> None:
    drift_doc = tmp_path / "drift_expectations.md"
    drift_packet = tmp_path / "drift_packet.md"

    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
        .replace("Dependencies: `M248-E011`, `M248-A012`, `M248-B012`, `M248-C012`, `M248-D012`", "Dependencies: `M248-E011`, `M248-A012`, `M248-B012`, `M248-C999`, `M248-D012`", 1)
        .replace("under repeated validation runs with stable failure ordering.", "under repeated validation runs without strict ordering.", 1),
        encoding="utf-8",
    )
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace("Issue: `#6872`", "Issue: `#6999`", 1),
        encoding="utf-8",
    )

    summary_one = tmp_path / "summary_one.json"
    summary_two = tmp_path / "summary_two.json"

    exit_one = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_one),
        ]
    )
    exit_two = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_two),
        ]
    )

    assert exit_one == 1
    assert exit_two == 1

    payload_one = json.loads(summary_one.read_text(encoding="utf-8"))
    payload_two = json.loads(summary_two.read_text(encoding="utf-8"))
    assert payload_one["ok"] is False
    assert payload_two["ok"] is False
    assert payload_one["checks_total"] == payload_two["checks_total"]
    assert payload_one["checks_passed"] == payload_two["checks_passed"]
    assert payload_one["failures"] == payload_two["failures"]
