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
    / "check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
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
    assert payload["mode"] == "m247-d011-runtime-link-build-throughput-optimization-performance-and-quality-guardrails-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_d011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-D011/")


def test_contract_fails_closed_when_expectations_drop_d010_dependency(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_d011_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-D010`",
            "Dependencies: `M247-D399`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D011-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_d011_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6769`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D011-DOC-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_chain_drops_d010_gate(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_d011_lane_d_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m247_d010_lane_d_readiness.py",
            "scripts/run_m247_d001_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D011-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d010_checker_dependency_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing = tmp_path / "missing_d010_checker.py"
    exit_code = contract.run(["--d010-checker", str(missing), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D011-D010-DEP-ARG-01" for failure in payload["failures"])
