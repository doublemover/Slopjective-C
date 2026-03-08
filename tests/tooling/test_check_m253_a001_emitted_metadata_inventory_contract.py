from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_a001_emitted_metadata_inventory_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_a001_emitted_metadata_inventory_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_a001_emitted_metadata_inventory_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-a001-emitted-metadata-inventory-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M253-A002"
    assert payload["canonical_sections"][0] == "objc3.runtime.image_info"
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_a001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-A001/")


def test_contract_fails_closed_when_expectations_drop_retention_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m253_emitted_metadata_inventory_contract_and_architecture_freeze_a001_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`llvm.used`",
            "`llvm.metadata.used`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-A001-DOC-EXP-12" for failure in payload["failures"])


def test_contract_fails_closed_when_ast_section_name_drifts(tmp_path: Path) -> None:
    drift_ast = tmp_path / "objc3_ast.h"
    drift_ast.write_text(
        contract.DEFAULT_AST_HEADER.read_text(encoding="utf-8").replace(
            '"objc3.runtime.class_descriptors"',
            '"objc3.runtime.class_payloads"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--ast-header", str(drift_ast), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-A001-AST-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_drops_build(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m253-a001-lane-a-readiness": "npm run build:objc3c-native && npm run check:objc3c:m253-a001-emitted-metadata-inventory-contract && npm run test:tooling:m253-a001-emitted-metadata-inventory-contract"',
            '"check:objc3c:m253-a001-lane-a-readiness": "npm run check:objc3c:m253-a001-emitted-metadata-inventory-contract && npm run test:tooling:m253-a001-emitted-metadata-inventory-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-A001-PKG-03" for failure in payload["failures"])
