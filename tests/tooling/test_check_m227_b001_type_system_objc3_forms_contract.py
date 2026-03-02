from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_b001_type_system_objc3_forms_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_b001_type_system_objc3_forms_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-type-system-objc3-forms-contract-freeze-b001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 15
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_type_system_objc3_forms_contract_freeze_expectations.md"
    drift_doc.write_text(
        (
            ROOT
            / "docs"
            / "contracts"
            / "m227_type_system_objc3_forms_contract_freeze_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-type-system-objc3-forms-contract-freeze/m227-b001-v1`",
            "Contract ID: `objc3c-type-system-objc3-forms-contract-freeze/m227-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_assignment_compatibility_reverts_to_ad_hoc_literals(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_semantic_passes.cpp"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
        ).read_text(encoding="utf-8").replace(
            "if (IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||\n"
            "      IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {",
            "if (target.type == ValueType::ObjCId || value.type == ValueType::ObjCId) {",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["semantic_passes"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M227-B001-SRC-04", "M227-B001-FORB-01"}
        for failure in payload["failures"]
    )
