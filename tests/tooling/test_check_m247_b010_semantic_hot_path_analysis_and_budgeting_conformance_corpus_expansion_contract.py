from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py"
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
        == "m247-b010-semantic-hot-path-analysis-budgeting-conformance-corpus-expansion-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_b010() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-B010/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b010_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-B009`",
            "Dependencies: `M247-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_b010_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6733`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_b009_readiness_chain(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_b010_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m247_b009_lane_b_readiness.py",
            "scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b009_expectations_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_b009_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_B009_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-conformance-matrix-implementation/m247-b009-v1`",
            "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-conformance-matrix-implementation/m247-b009-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b009-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-B009-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_runner_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m247-b010-lane-b-readiness": "python scripts/run_m247_b010_lane_b_readiness.py"',
            '"check:objc3c:m247-b010-lane-b-readiness": "python scripts/check_m247_b010_semantic_hot_path_analysis_and_budgeting_conformance_corpus_expansion_contract.py"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b009_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b009_checker.py"
    exit_code = contract.run(
        [
            "--b009-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-B010-DEP-B009-ARG-01" for failure in payload["failures"])
