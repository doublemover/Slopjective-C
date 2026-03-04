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
    / "check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"
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
        == "m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 38
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_a001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-A001/")


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


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "- Issue: `#6708`",
            "- Issue: `#6000`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A001-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_contract_command(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_a001_packet.md"
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "`check:objc3c:m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract`",
            "`check:objc3c:m247-a001-contract-freeze`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A001-DOC-PKT-05" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_completion_marker(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_a001_lane_a_readiness.py"
    drift_runner.write_text(
        contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8").replace(
            "[ok] M247-A001 lane-A readiness chain completed",
            "[ok] done",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A001-RUN-05" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m247-a001-lane-a-readiness": "python scripts/run_m247_a001_lane_a_readiness.py"',
            '"check:objc3c:m247-a001-lane-a-readiness": "python scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_input(tmp_path: Path) -> None:
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
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A001-PKG-07" for failure in payload["failures"])


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
