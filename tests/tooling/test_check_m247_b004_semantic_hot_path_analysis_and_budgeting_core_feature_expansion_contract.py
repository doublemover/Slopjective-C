from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_b004_semantic_hot_path_analysis_and_budgeting_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_b004_semantic_hot_path_analysis_and_budgeting_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_b004_semantic_hot_path_analysis_and_budgeting_core_feature_expansion_contract.py"
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
    assert payload["mode"] == "m247-b004-semantic-hot-path-analysis-and-budgeting-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_b004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-B004/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b004_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-B003`",
            "Dependencies: `M247-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B004-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_b004_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6727`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B004-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_b003_chain(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_b004_lane_b_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m247_b003_lane_b_readiness.py",
            "scripts/run_m247_b000_lane_b_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B004-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b003_readiness_dependency_token_drifts(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_b002_lane_b_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_B003_READINESS_RUNNER.read_text(encoding="utf-8"),
            'DEPENDENCY_TOKEN = "M247-B002"',
            'DEPENDENCY_TOKEN = "M247-B000"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b003-readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B004-B003-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b003_checker_dependency_path_is_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--b003-checker",
            str(tmp_path / "missing_b003_checker.py"),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B004-DEP-B003-ARG-01" for failure in payload["failures"])



