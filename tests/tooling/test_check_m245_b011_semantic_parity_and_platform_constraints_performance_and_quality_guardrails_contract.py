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
    / "check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_b011_semantic_parity_and_platform_constraints_performance_and_quality_guardrails_contract.py"
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
    assert payload["mode"] == "m245-b011-semantic-parity-platform-constraints-performance-and-quality-guardrails-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 39
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_b011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-B011/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_b011_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-B010`",
            "Dependencies: `M245-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_b011_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6633`",
            "Issue: `#6999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_expectations_dependency_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_b010_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_B010_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-B009`",
            "Dependencies: `M245-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b010-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-B010-DOC-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_packet_summary_path_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_b010_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_B010_PACKET_DOC.read_text(encoding="utf-8"),
            "tmp/reports/m245/M245-B010/semantic_parity_and_platform_constraints_conformance_corpus_expansion_summary.json",
            "tmp/reports/m245/M245-B010/non_deterministic_summary.json",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b010-packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-B010-PKT-06" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b010_checker.py"
    exit_code = contract.run(
        [
            "--b010-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-DEP-B010-ARG-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_test_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_test = tmp_path / "missing_b010_test.py"
    exit_code = contract.run(
        [
            "--b010-test",
            str(missing_test),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B011-DEP-B010-ARG-02" for failure in payload["failures"])
