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
    / "check_m227_b027_type_system_objc3_forms_advanced_core_workpack_shard3_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m227_b027_type_system_objc3_forms_advanced_core_workpack_shard3_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m227_b027_type_system_objc3_forms_advanced_core_workpack_shard3_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-type-system-objc3-forms-advanced-core-workpack-shard3-b027-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m227_type_system_objc3_forms_advanced_core_workpack_shard3_b027_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M227-B026`",
            "Dependencies: `M227-B099`",
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
    assert any(failure["check_id"] == "M227-B027-DOC-03" for failure in payload["failures"])


def test_contract_fails_closed_when_lane_readiness_drops_b019_dependency(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m227-b027-lane-b-readiness": "npm run check:objc3c:m227-b026-lane-b-readiness',
            '"check:objc3c:m227-b027-lane-b-readiness": "npm run check:objc3c:m227-b099-lane-b-readiness',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B027-PKG-05" for failure in payload["failures"])


def test_contract_fails_closed_when_runbook_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_runbook = tmp_path / "m227_wave_execution_runbook.md"
    drift_runbook.write_text(
        contract.ARTIFACTS["runbook_doc"].read_text(encoding="utf-8").replace(
            "objc3c-type-system-objc3-forms-advanced-core-workpack-shard3/m227-b027-v1",
            "objc3c-type-system-objc3-forms-advanced-core-workpack-shard3/m227-b027-drift",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["runbook_doc"] = drift_runbook
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B027-RUN-02" for failure in payload["failures"])




