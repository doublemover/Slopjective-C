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
    / "check_m248_e033_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_e033_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_e033_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m248-e033-lane-e-ci-governance-gate-closeout-policy-advanced-core-workpack-shard4-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m248_e033() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-E033/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_e033_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M248-E032`, `M248-A012`, `M248-B015`, `M248-C018`, `M248-D024`",
            "Dependencies: `M248-E032`, `M248-A012`, `M248-B999`, `M248-C018`, `M248-D024`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E033-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m248_e033_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6893`",
            "Issue: `#6999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E033-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_no_longer_chains_e032(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m248_e033_lane_e_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "check:objc3c:m248-e032-lane-e-readiness",
            "check:objc3c:m248-e029-lane-e-readiness",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] in {"M248-E033-RUN-02", "M248-E033-RUN-FORB-01"} for failure in payload["failures"])


def test_contract_failure_ordering_is_stable(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_packet = tmp_path / "missing_packet.md"
    missing_runner = tmp_path / "missing_runner.py"

    exit_code = contract.run(
        [
            "--packet-doc",
            str(missing_packet),
            "--readiness-runner",
            str(missing_runner),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    failures = payload["failures"]
    assert len(failures) >= 2
    assert failures == sorted(
        failures,
        key=lambda failure: (failure["artifact"], failure["check_id"], failure["detail"]),
    )
    assert any(failure["check_id"] == "M248-E033-DOC-PKT-EXISTS" for failure in failures)
    assert any(failure["check_id"] == "M248-E033-RUN-EXISTS" for failure in failures)
