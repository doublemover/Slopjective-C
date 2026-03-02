from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_contract.py"
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
        == "m248-d002-runner-reliability-platform-operations-modular-split-scaffolding-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_d002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-D002/")


def test_contract_fails_closed_when_expectations_drop_mandatory_scope_wording(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m248_runner_reliability_and_platform_operations_modular_split_scaffolding_d002_expectations.md"
    )
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    anchor = "including code/spec anchors and milestone optimization improvements"
    assert anchor in original
    drift_text = original.replace(anchor, "including code/spec anchors and milestone improvements")
    assert anchor not in drift_text
    drift_doc.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-D002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_drops_m248_d001_token(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path / "m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_packet.md"
    )
    original = contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8")
    anchor = "M248-D001"
    assert anchor in original
    drift_text = original.replace(anchor, "M248-D099")
    assert anchor not in drift_text
    drift_packet.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-D002-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_d001_checker_mode_drifts(tmp_path: Path) -> None:
    drift_checker = tmp_path / "check_m248_d001_runner_reliability_and_platform_operations_contract.py"
    original = contract.DEFAULT_D001_CHECKER_SCRIPT.read_text(encoding="utf-8")
    anchor = 'MODE = "m248-d001-runner-reliability-platform-operations-contract-v1"'
    assert anchor in original
    drift_text = original.replace(anchor, 'MODE = "m248-d001-runner-reliability-platform-operations-contract-drift"')
    assert anchor not in drift_text
    drift_checker.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--d001-checker-script", str(drift_checker), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-D002-D001-CHK-01" for failure in payload["failures"])


def test_contract_drift_findings_are_deterministic_across_runs(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m248_runner_reliability_and_platform_operations_modular_split_scaffolding_d002_expectations.md"
    )
    drift_packet = (
        tmp_path / "m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_packet.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-runner-reliability-platform-operations-modular-split-scaffolding/m248-d002-v1`",
            "Contract ID: `objc3c-runner-reliability-platform-operations-modular-split-scaffolding/m248-d002-drift`",
        ),
        encoding="utf-8",
    )
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace("M248-D001", "M248-D099"),
        encoding="utf-8",
    )

    summary_one = tmp_path / "summary_one.json"
    summary_two = tmp_path / "summary_two.json"

    exit_one = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_one),
        ]
    )
    exit_two = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_two),
        ]
    )

    assert exit_one == 1
    assert exit_two == 1

    payload_one = json.loads(summary_one.read_text(encoding="utf-8"))
    payload_two = json.loads(summary_two.read_text(encoding="utf-8"))
    assert payload_one["ok"] is False
    assert payload_two["ok"] is False
    assert payload_one["checks_total"] == payload_two["checks_total"]
    assert payload_one["checks_passed"] == payload_two["checks_passed"]
    assert payload_one["failures"] == payload_two["failures"]
