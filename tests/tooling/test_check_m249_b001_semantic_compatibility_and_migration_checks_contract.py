from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m249_b001_semantic_compatibility_and_migration_checks_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m249_b001_semantic_compatibility_and_migration_checks_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m249-b001-semantic-compatibility-migration-checks-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 58
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-B001/")


def test_contract_fails_closed_when_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-freeze/m249-b001-v1`",
            "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-freeze/m249-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B001-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_sema_scaffold_drops_legacy_guard(tmp_path: Path) -> None:
    drift_scaffold = tmp_path / "objc3_sema_pass_flow_scaffold.cpp"
    drift_scaffold.write_text(
        contract.DEFAULT_SEMA_SCAFFOLD.read_text(encoding="utf-8").replace(
            "summary.compatibility_mode == Objc3SemaCompatibilityMode::Legacy);",
            "summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical);",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--sema-scaffold", str(drift_scaffold), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B001-SCF-05" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m249-b001-lane-b-readiness": '
            '"npm run check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract '
            '&& npm run test:tooling:m249-b001-semantic-compatibility-migration-checks-contract"',
            '"check:objc3c:m249-b001-lane-b-readiness": '
            '"npm run check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_parse_readiness_forces_compatibility_true(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        contract.DEFAULT_PARSE_READINESS.read_text(encoding="utf-8")
        + "\n"
        + "surface.compatibility_handoff_consistent = true;\n",
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--parse-readiness", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B001-PRS-FORB-01" for failure in payload["failures"])
