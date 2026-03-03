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
    / "check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-type-system-objc3-forms-cross-lane-integration-sync-b012-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_type_system_objc3_forms_cross_lane_integration_sync_b012_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-type-system-objc3-forms-cross-lane-integration-sync/m227-b012-v1`",
            "Contract ID: `objc3c-type-system-objc3-forms-cross-lane-integration-sync/m227-b012-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B012-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_forbidden_ready_shortcut_appears(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_passes = tmp_path / "objc3_semantic_passes.cpp"
    drift_passes.write_text(
        contract.ARTIFACTS["semantic_passes"].read_text(encoding="utf-8")
        + "\nsummary.canonical_type_form_performance_quality_guardrails_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["semantic_passes"] = drift_passes
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B012-FORB-02" for failure in payload["failures"])
