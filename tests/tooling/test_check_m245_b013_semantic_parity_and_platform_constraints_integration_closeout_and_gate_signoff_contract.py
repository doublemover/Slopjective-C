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
    / "check_m245_b013_semantic_parity_and_platform_constraints_integration_closeout_and_gate_signoff_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_b013_semantic_parity_and_platform_constraints_integration_closeout_and_gate_signoff_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_b013_semantic_parity_and_platform_constraints_integration_closeout_and_gate_signoff_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new)
    assert old not in replaced
    return replaced


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m245-b013-semantic-parity-platform-constraints-integration-closeout-and-gate-signoff-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 39
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_b013() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-B013/")


def test_contract_emit_json_matches_summary_payload(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout_payload = json.loads(capsys.readouterr().out)
    summary_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == summary_payload
    assert stdout_payload["mode"] == contract.MODE
    assert stdout_payload["ok"] is True
    assert stdout_payload["checks_total"] == stdout_payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_b013_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-B012`",
            "Dependencies: `M245-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B013-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_b013_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6635`",
            "Issue: `#6999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B013-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b012_expectations_dependency_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_b012_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_B012_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-B011`",
            "Dependencies: `M245-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b012-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B013-B012-DOC-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b012_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b012_checker.py"
    exit_code = contract.run(
        [
            "--b012-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B013-DEP-B012-ARG-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b012_test_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_test = tmp_path / "missing_b012_test.py"
    exit_code = contract.run(
        [
            "--b012-test",
            str(missing_test),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B013-DEP-B012-ARG-02" for failure in payload["failures"])


def test_contract_emits_sorted_failures_in_emit_json_mode(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    drift_doc = tmp_path / "m245_b013_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-B012`",
            "Dependencies: `M245-B099`",
        ),
        encoding="utf-8",
    )
    missing_checker = tmp_path / "missing_b012_checker.py"
    summary_out = tmp_path / "summary.json"

    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--b012-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
            "--emit-json",
        ]
    )

    assert exit_code == 1
    payload = json.loads(capsys.readouterr().out)
    check_ids = [failure["check_id"] for failure in payload["failures"]]
    assert check_ids == sorted(check_ids)

