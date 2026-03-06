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
    / "check_m229_a011_class_protocol_category_metadata_generation_integration_closeout_and_gate_sign_off_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m229_a011_class_protocol_category_metadata_generation_integration_closeout_and_gate_sign_off_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m229_a011_class_protocol_category_metadata_generation_integration_closeout_and_gate_sign_off_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m229-a011-class-protocol-category-metadata-generation-integration-closeout-and-gate-sign-off-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]


def test_contract_default_summary_out_is_under_tmp_reports_m229_a011() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m229/M229-A011/")


def test_contract_fails_closed_when_expectations_dependency_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M229-A010`",
            "Dependencies: `M229-A999`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(f["check_id"] == "M229-A011-DOC-EXP-03" for f in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_a009(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m229-a011-lane-a-readiness": '
            '"npm run check:objc3c:m229-a010-lane-a-readiness '
            '&& npm run check:objc3c:m229-a011-class-protocol-category-metadata-generation-integration-closeout-and-gate-sign-off-contract '
            '&& npm run test:tooling:m229-a011-class-protocol-category-metadata-generation-integration-closeout-and-gate-sign-off-contract"',
            '"check:objc3c:m229-a011-lane-a-readiness": '
            '"npm run check:objc3c:m229-a011-class-protocol-category-metadata-generation-integration-closeout-and-gate-sign-off-contract '
            '&& npm run test:tooling:m229-a011-class-protocol-category-metadata-generation-integration-closeout-and-gate-sign-off-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(f["check_id"] == "M229-A011-PKG-03" for f in payload["failures"])


def test_contract_fails_closed_when_prerequisite_missing(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    monkeypatch.setattr(
        contract,
        "PREREQUISITE_ASSETS",
        (("M229-A011-DEP-MISSING", Path("docs/contracts/does_not_exist.md")),),
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(f["check_id"] == "M229-A011-DEP-MISSING" for f in payload["failures"])








