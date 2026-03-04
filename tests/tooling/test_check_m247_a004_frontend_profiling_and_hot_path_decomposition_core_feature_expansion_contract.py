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
    / "check_m247_a004_frontend_profiling_and_hot_path_decomposition_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_a004_frontend_profiling_and_hot_path_decomposition_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_a004_frontend_profiling_and_hot_path_decomposition_core_feature_expansion_contract.py"
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
    assert payload["mode"] == "m247-a004-frontend-profiling-hot-path-decomposition-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_a004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-A004/")


def test_contract_emit_json_parity_with_summary_file(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout_payload = json.loads(capsys.readouterr().out)
    file_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == file_payload
    assert stdout_payload["mode"] == contract.MODE
    assert stdout_payload["ok"] is True


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_a004_expectations.md"
    drift_doc.write_text(
        replace_first(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-A003`",
            "Dependencies: `M247-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A004-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_a004_packet.md"
    drift_packet.write_text(
        replace_first(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6711`",
            "Issue: `#5099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A004-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_a003_prerequisite(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_a004_lane_a_readiness.py"
    drift_runner.write_text(
        replace_first(
            contract.DEFAULT_RUN_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m247_a003_lane_a_readiness.py",
            "scripts/run_m247_a099_lane_a_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--run-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A004-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_a003_packet_dependency_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_a003_packet.md"
    drift_packet.write_text(
        replace_first(
            contract.DEFAULT_A003_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-A002`",
            "Dependencies: `M247-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--a003-packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A004-A003-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_a003_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_a003_checker.py"
    exit_code = contract.run(
        [
            "--a003-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A004-DEP-A003-ARG-01" for failure in payload["failures"])


def test_contract_failures_are_sorted_deterministically(tmp_path: Path) -> None:
    drift_expectations = tmp_path / "expectations.md"
    drift_expectations.write_text("# drift\n", encoding="utf-8")
    drift_packet = tmp_path / "packet.md"
    drift_packet.write_text("# drift\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_expectations),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    failures = payload["failures"]
    sorted_failures = sorted(failures, key=lambda item: (item["artifact"], item["check_id"], item["detail"]))
    assert failures == sorted_failures

