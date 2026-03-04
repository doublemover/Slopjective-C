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
    / "check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_first(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new, 1)
    assert replaced != text
    return replaced


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m246-a006-frontend-optimization-hint-capture-edge-case-expansion-and-robustness-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_a006() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-A006/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_a006_expectations.md"
    drift_doc.write_text(
        replace_first(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-A005`",
            "Dependencies: `M246-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A006-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m246_a006_packet.md"
    drift_packet.write_text(
        replace_first(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5053`",
            "Issue: `#5099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A006-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_a005_prerequisite(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m246_a006_lane_a_readiness.py"
    drift_runner.write_text(
        replace_first(
            contract.DEFAULT_RUN_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_a005_lane_a_readiness.py",
            "scripts/run_m246_a099_lane_a_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--run-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A006-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("docs/contracts/does_not_exist_m246_a006.md"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_summary_payload_contract_and_determinism(tmp_path: Path) -> None:
    summary_out_one = tmp_path / "summary_one.json"
    summary_out_two = tmp_path / "summary_two.json"

    first_exit = contract.run(["--summary-out", str(summary_out_one)])
    second_exit = contract.run(["--summary-out", str(summary_out_two)])

    assert first_exit == 0
    assert second_exit == 0

    first_text = summary_out_one.read_text(encoding="utf-8")
    second_text = summary_out_two.read_text(encoding="utf-8")
    assert first_text == second_text

    payload = json.loads(first_text)
    assert sorted(payload.keys()) == ["checks_passed", "checks_total", "failures", "mode", "ok"]
    assert isinstance(payload["checks_total"], int)
    assert isinstance(payload["checks_passed"], int)
    assert isinstance(payload["failures"], list)

