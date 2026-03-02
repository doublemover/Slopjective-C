from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py"
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
    assert payload["mode"] == "m248-a002-suite-partitioning-fixture-ownership-modular-split-scaffolding-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-A002/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_a002_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M248-A001`",
            "Dependencies: `M248-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_mandatory_scope_wording(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m248_a002_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
            "code/spec anchors and milestone improvements as mandatory scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A002-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_a001_contract_id_drifts(tmp_path: Path) -> None:
    drift_a001_doc = tmp_path / "m248_a001_expectations.md"
    drift_a001_doc.write_text(
        replace_all(
            contract.DEFAULT_A001_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-contract/m248-a001-v1`",
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-contract/m248-a001-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--a001-expectations-doc",
            str(drift_a001_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A002-A001-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_a001_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_a001_checker.py"
    exit_code = contract.run(
        [
            "--a001-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A002-DEP-A001-ARG-01" for failure in payload["failures"])
