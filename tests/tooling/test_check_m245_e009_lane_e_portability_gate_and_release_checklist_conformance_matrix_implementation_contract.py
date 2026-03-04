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
    / "check_m245_e009_lane_e_portability_gate_and_release_checklist_conformance_matrix_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_e009_lane_e_portability_gate_and_release_checklist_conformance_matrix_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load "
        "scripts/check_m245_e009_lane_e_portability_gate_and_release_checklist_conformance_matrix_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_parse_args_accepts_overrides(tmp_path: Path) -> None:
    custom_expectations = tmp_path / "expectations.md"
    custom_packet = tmp_path / "packet.md"
    custom_package = tmp_path / "package.json"
    custom_summary = tmp_path / "summary.json"

    args = contract.parse_args(
        [
            "--expectations-doc",
            str(custom_expectations),
            "--packet-doc",
            str(custom_packet),
            "--package-json",
            str(custom_package),
            "--summary-out",
            str(custom_summary),
        ]
    )

    assert args.expectations_doc == custom_expectations
    assert args.packet_doc == custom_packet
    assert args.package_json == custom_package
    assert args.summary_out == custom_summary


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m245-e009-lane-e-portability-gate-release-checklist-conformance-matrix-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []
    assert list(payload.keys()) == ["mode", "ok", "checks_total", "checks_passed", "failures"]


def test_contract_default_summary_out_is_under_tmp_reports_m245_e009() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-E009/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m245_e009_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_b004_dependency_token(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_lane_e_portability_gate_and_release_checklist_conformance_matrix_implementation_e009_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-E008`, `M245-A003`, `M245-B004`, `M245-C005`, `M245-D007`",
            "Dependencies: `M245-E008`, `M245-A003`, `M245-B099`, `M245-C005`, `M245-D007`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-E009-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_e009_lane_e_portability_gate_and_release_checklist_conformance_matrix_implementation_packet.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6681`",
            "Issue: `#9999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-E009-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_missing_diagnostics_replay_token(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:diagnostics-replay-proof": ',
            '"test:objc3c:diagnostics-replay-proof-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-E009-PKG-03" for failure in payload["failures"])


def test_contract_drift_findings_are_deterministic_across_runs(tmp_path: Path) -> None:
    drift_expectations = tmp_path / "m245_e009_expectations.md"
    drift_packet = tmp_path / "m245_e009_packet.md"

    drift_expectations.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-lane-e-portability-gate-release-checklist-conformance-matrix-implementation/m245-e009-v1`",
            "Contract ID: `objc3c-lane-e-portability-gate-release-checklist-conformance-matrix-implementation/m245-e009-drift`",
        ),
        encoding="utf-8",
    )
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-E008`, `M245-A003`, `M245-B004`, `M245-C005`, `M245-D007`",
            "Dependencies: `M245-E008`, `M245-A003`, `M245-B004`, `M245-C005`, `M245-D099`",
        ),
        encoding="utf-8",
    )

    summary_one = tmp_path / "summary_one.json"
    summary_two = tmp_path / "summary_two.json"

    exit_one = contract.run(
        [
            "--expectations-doc",
            str(drift_expectations),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_one),
        ]
    )
    exit_two = contract.run(
        [
            "--expectations-doc",
            str(drift_expectations),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_two),
        ]
    )

    assert exit_one == 1
    assert exit_two == 1

    text_one = summary_one.read_text(encoding="utf-8")
    text_two = summary_two.read_text(encoding="utf-8")
    assert text_one == text_two

    payload_one = json.loads(text_one)
    payload_two = json.loads(text_two)
    assert payload_one["ok"] is False
    assert payload_two["ok"] is False
    assert payload_one["checks_total"] == payload_two["checks_total"]
    assert payload_one["checks_passed"] == payload_two["checks_passed"]
    assert payload_one["failures"] == payload_two["failures"]
