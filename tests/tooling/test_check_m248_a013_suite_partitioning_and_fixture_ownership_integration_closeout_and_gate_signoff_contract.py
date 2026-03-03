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
    / "check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py"
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
        == "m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_a013() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-A013/")


def test_contract_emits_json_when_requested(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_prerequisite_a012_asset_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m248_a012_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_A012_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-cross-lane-integration-sync/m248-a012-v1`",
            "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-cross-lane-integration-sync/m248-a012-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--a012-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A013-DEP-01" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_a012_dependency(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_a013_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M248-A012`",
            "Dependencies: `M248-A099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A013-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_issue_6800_anchor(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_a013_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#6800` defines canonical lane-A integration closeout and gate signoff scope.",
            "Issue `#6899` defines canonical lane-A integration closeout and gate signoff scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A013-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_deterministic_invariant_key(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_a013_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "- `integration_closeout_and_gate_signoff_key_ready`\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A013-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_readiness_command_name(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_a013_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "- `check:objc3c:m248-a012-lane-a-readiness`\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-A013-DOC-EXP-13" for failure in payload["failures"])
