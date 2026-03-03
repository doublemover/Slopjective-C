from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-d004-runtime-facing-type-metadata-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227_d004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m227/M227-D004/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("scripts/does_not_exist_m227_d004_dependency.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m227_d004_expectations_drift.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "| `M227-B004` | `M227-B004` |",
            "| `M227-B004` | `M227-B099` |",
            1,
        ),
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
    assert any(failure["check_id"] == "M227-D004-EXP-SEM-TOK-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_reference_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m227_d004_packet_drift.md"
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py",
            "scripts/check_m227_c099_typed_sema_to_lowering_core_feature_expansion_contract.py",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M227-D004-PKT-SEM-REF-04", "M227-D004-SEM-MATCH-04"}
        for failure in payload["failures"]
    )
