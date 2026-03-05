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
    / "check_m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m235_e002_qualifier_generic_conformance_gate_modular_split_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    updated = text.replace(old, new, 1)
    assert updated != text
    return updated


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m235-e002-qualifier-generic-conformance-gate-modular-split-scaffolding-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m235_e002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m235/M235-E002/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("docs/contracts/does_not_exist_m235_e002.md"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m235_e002_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue: `#5841`",
            "Issue: `#5999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-E002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_d001_dependency(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m235_e002_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M235-E001`, `M235-A002`, `M235-B002`, `M235-C002`, `M235-D001`",
            "Dependencies: `M235-E001`, `M235-A002`, `M235-B002`, `M235-C002`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-E002-DOC-PKT-04" for failure in payload["failures"])

