from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"
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
        == "m247-a013-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_a013() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-A013/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_a013_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-A012`",
            "Dependencies: `M247-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_a013_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6720`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-DOC-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_a012_readiness_chain(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_a013_lane_a_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m247_a012_lane_a_readiness.py",
            "scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_a012_expectations_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_a012_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_A012_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-cross-lane-integration-sync/m247-a012-v1`",
            "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-cross-lane-integration-sync/m247-a012-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--a012-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-A012-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_runner_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m247-a013-lane-a-readiness": "python scripts/run_m247_a013_lane_a_readiness.py"',
            '"check:objc3c:m247-a013-lane-a-readiness": "python scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_a012_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_a012_checker.py"
    exit_code = contract.run(
        [
            "--a012-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A013-DEP-A012-ARG-01" for failure in payload["failures"])

