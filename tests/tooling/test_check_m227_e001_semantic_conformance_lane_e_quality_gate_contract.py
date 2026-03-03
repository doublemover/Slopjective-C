from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_e001_semantic_conformance_lane_e_quality_gate_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-e001-semantic-conformance-lane-e-quality-gate-contract-architecture-freeze-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m227/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("scripts/does_not_exist_m227_e001_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_doc_drops_b002_dependency(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m227_lane_e_semantic_conformance_quality_gate_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M227-B002`", "`M227-B099`"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-E001-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_doc_drops_d001_dependency(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m227_e001_semantic_conformance_lane_e_quality_gate_contract_and_architecture_freeze_packet.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M227-A001`, `M227-B002`, `M227-C001`, `M227-D001`",
            "Dependencies: `M227-A001`, `M227-B002`, `M227-C001`, `M227-D099`",
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
    assert any(failure["check_id"] == "M227-E001-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_drifts(tmp_path: Path) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        contract.DEFAULT_ARCHITECTURE_DOC.read_text(encoding="utf-8").replace(
            "M227 lane-E E001 semantic conformance quality-gate contract and architecture freeze anchors dependency references (`M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`)",
            "M227 lane-E E001 semantic conformance quality-gate contract and architecture freeze anchors dependency references (`M227-A001`, `M227-B099`, `M227-C001`, and `M227-D001`)",
            1,
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
    assert any(failure["check_id"] == "M227-E001-ARCH-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m227-e001-lane-e-quality-gate-readiness": "npm run check:objc3c:m227-a001-lane-a-readiness && npm run check:objc3c:m227-b002-lane-b-readiness && npm run check:objc3c:m227-c001-lane-c-readiness && npm run check:objc3c:m227-d001-lane-d-readiness && npm run check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract && npm run test:tooling:m227-e001-semantic-conformance-lane-e-quality-gate-contract"',
            '"check:objc3c:m227-e001-lane-e-quality-gate-readiness": "npm run check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-E001-PKG-07" for failure in payload["failures"])
