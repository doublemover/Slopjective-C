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
    / "check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
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
        == "m247-b014-semantic-hot-path-analysis-budgeting-release-candidate-and-replay-dry-run-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 65
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_b014() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-B014/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b014_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-B013`",
            "Dependencies: `M247-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_b014_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6737`",
            "Issue: `#6999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_invariant_wording_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b014_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "release-candidate/replay command sequencing and release-candidate-replay-key continuity remain deterministic and fail-closed across lane-B readiness wiring.",
            "release-candidate/replay command sequencing and release-candidate-replay-key continuity become best-effort across lane-B readiness wiring.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_no_longer_chains_b013(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_b014_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m247_b013_lane_b_readiness.py",
            "scripts/check_m247_b013_semantic_hot_path_analysis_and_budgeting_docs_and_operator_runbook_synchronization_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m247-b014-lane-b-readiness": "python scripts/run_m247_b014_lane_b_readiness.py"',
            '"check:objc3c:m247-b014-lane-b-readiness": "python scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_drifts(tmp_path: Path) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        replace_all(
            contract.DEFAULT_ARCHITECTURE_DOC.read_text(encoding="utf-8"),
            "M247 lane-B B014 semantic hot-path analysis/budgeting release-candidate and replay dry-run anchors",
            "M247 lane-B B014 semantic hot-path analysis/budgeting release-candidate dry-run anchors",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--architecture-doc",
            str(drift_architecture),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-ARCH-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b013_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b013_checker.py"
    exit_code = contract.run(
        [
            "--b013-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B014-DEP-B013-ARG-01" for failure in payload["failures"])


def test_contract_failure_ordering_is_stable(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b013_checker.py"
    missing_test = tmp_path / "missing_b013_test.py"

    exit_code = contract.run(
        [
            "--b013-checker",
            str(missing_checker),
            "--b013-test",
            str(missing_test),
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
    assert any(failure["check_id"] == "M247-B014-DEP-B013-ARG-01" for failure in failures)
    assert any(failure["check_id"] == "M247-B014-DEP-B013-ARG-02" for failure in failures)
