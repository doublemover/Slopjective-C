from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
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
        == "m247-b017-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 59
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_b017() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-B017/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b017_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-B016`",
            "Dependencies: `M247-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_b017_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6740`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_b016_readiness_chain(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_b017_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m247_b016_lane_b_readiness.py",
            "scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b016_expectations_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b016_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_B016_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-advanced-edge-compatibility-workpack-shard-1/m247-b016-v1`",
            "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-advanced-edge-compatibility-workpack-shard-1/m247-b016-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b016-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-B016-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_shared_anchor_reference(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_b017_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "- `native/objc3c/src/ARCHITECTURE.md`",
            "- `native/objc3c/src/ARCHITECTURE_DRIFT.md`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-DOC-PKT-15" for failure in payload["failures"])


def test_contract_fails_closed_when_b016_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b016_checker.py"
    exit_code = contract.run(
        [
            "--b016-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B017-DEP-B016-ARG-01" for failure in payload["failures"])
