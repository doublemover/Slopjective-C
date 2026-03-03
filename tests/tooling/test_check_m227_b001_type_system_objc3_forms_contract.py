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
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m227_b001_type_system_objc3_forms_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-b001-type-system-objc3-forms-contract-and-architecture-freeze-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m227/M227-B001/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m227_type_system_objc3_forms_contract_and_architecture_freeze_b001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-type-system-objc3-forms-contract-and-architecture-freeze/m227-b001-v1`",
            "Contract ID: `objc3c-type-system-objc3-forms-contract-and-architecture-freeze/m227-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B001-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m227-b001-lane-b-readiness": '
            '"npm run check:objc3c:m227-b001-type-system-objc3-forms-contract '
            '&& npm run test:tooling:m227-b001-type-system-objc3-forms-contract"',
            '"check:objc3c:m227-b001-lane-b-readiness": '
            '"npm run check:objc3c:m227-b001-type-system-objc3-forms-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_assignment_compatibility_reverts_to_ad_hoc_literals(tmp_path: Path) -> None:
    drift_source = tmp_path / "objc3_semantic_passes.cpp"
    drift_source.write_text(
        contract.DEFAULT_SEMANTIC_PASSES.read_text(encoding="utf-8").replace(
            "if (IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||\n"
            "      IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {",
            "if (target.type == ValueType::ObjCId || value.type == ValueType::ObjCId) {",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--semantic-passes", str(drift_source), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M227-B001-PSS-04", "M227-B001-PSS-FORB-01"}
        for failure in payload["failures"]
    )
