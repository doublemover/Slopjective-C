from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-e007-lane-e-integration-gate-diagnostics-hardening-evidence-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m226() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m226/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m226_e007_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_freeze_doc_drops_e007_packet_row(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m226_lane_e_contract_freeze_20260302.md"
    drift_doc.write_text(
        contract.DEFAULT_FREEZE_DOC.read_text(encoding="utf-8").replace(
            "| `M226-E007` | `objc3c-lane-e-integration-gate-diagnostics-hardening-evidence-contract/m226-e007-v1` |",
            "| `M226-E007` | `objc3c-lane-e-integration-gate-diagnostics-hardening-evidence-contract/m226-e007-drift` |",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--freeze-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-E007-DOC-FRZ-03" for failure in payload["failures"])


def test_contract_fails_closed_when_evidence_doc_drops_diagnostics_hardening_schema(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_scaffold.md"
    drift_doc.write_text(
        contract.DEFAULT_EVIDENCE_DOC.read_text(encoding="utf-8").replace(
            "Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `edge_compatibility`, `edge_robustness`, `diagnostics_hardening`.",
            "Required top-level keys: `packet`, `artifacts`.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--evidence-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-E007-DOC-EVI-06" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_doc_drops_b007_artifact_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_packet.md"
    drift_doc.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "`tmp/reports/m226/M226-B007/parser_sema_diagnostics_hardening_summary.json`",
            "`tmp/reports/m226/M226-B007/parser_sema_diagnostics_hardening_summary_drift.json`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--packet-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-E007-DOC-PKT-12" for failure in payload["failures"])


def test_contract_fails_closed_when_freeze_doc_drops_upstream_anchor_block(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m226_lane_e_contract_freeze_20260302.md"
    drift_doc.write_text(
        contract.DEFAULT_FREEZE_DOC.read_text(encoding="utf-8").replace(
            "#### Upstream Diagnostics Hardening Anchors",
            "#### Upstream Anchors Drifted",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--freeze-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-E007-DOC-FRZ-10" for failure in payload["failures"])

