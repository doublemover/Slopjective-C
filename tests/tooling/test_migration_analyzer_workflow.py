from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_migration_analyzer import (
    analyze_migration_input,
    apply_rewrite_plan,
    build_rewrite_workflow_report,
    load_contract,
)
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility"
POSITIVE_INPUT = FIXTURE_ROOT / "migration_inputs" / "objc2_swift_cpp_positive.json"
NEGATIVE_INPUT = FIXTURE_ROOT / "migration_inputs" / "objc2_swift_cpp_negative.json"


def test_migration_analyzer_contract_covers_required_surfaces_and_actions() -> None:
    contract = load_contract()

    assert contract["issue_ids"] == [8077, 8078, 8157]
    assert contract["required_surfaces"] == [
        "header-import-export",
        "abi-alignment",
        "foreign-type-diagnostics",
        "mixed-image-loading",
        "packaged-execution",
    ]
    assert contract["public_actions"] == [
        "analyze-migration-source",
        "rewrite-migration-source",
        "validate-migration-workflow",
    ]
    assert contract["required_diagnostic_fields"] == [
        "code",
        "severity",
        "message",
        "category",
        "explanation",
        "next_step",
        "surface",
        "range",
    ]
    metadata_codes = {item["code"] for item in contract["diagnostic_metadata"]}
    assert set(contract["diagnostic_codes"]).issubset(metadata_codes)
    assert "standalone report output as a capability basis" in contract["support_boundary"]


def test_migration_analyzer_passes_positive_and_fails_closed_negative() -> None:
    positive = analyze_migration_input(POSITIVE_INPUT)
    negative = analyze_migration_input(NEGATIVE_INPUT)

    assert positive.ok is True
    assert positive.payload["status"] == "PASS"
    assert positive.payload["observed_surfaces"] == {
        "header-import-export": True,
        "abi-alignment": True,
        "foreign-type-diagnostics": True,
        "mixed-image-loading": True,
        "packaged-execution": True,
    }
    assert positive.payload["rewrite_plan"]["automatic_edit_count"] >= 6
    assert positive.payload["rewrite_plan"]["manual_step_count"] >= 3
    assert positive.payload["fail_closed"] is False

    assert negative.ok is False
    assert negative.payload["status"] == "FAIL"
    codes = {item["code"] for item in negative.payload["diagnostics"]}
    assert {"O3M010", "O3M016", "O3M017", "O3M102", "O3M210"}.issubset(codes)
    for item in negative.payload["diagnostics"]:
        assert {"category", "explanation", "next_step"}.issubset(item)
        assert item["category"]
        assert item["explanation"]
        assert item["next_step"]
    diagnostics_by_code = {item["code"]: item for item in negative.payload["diagnostics"]}
    assert diagnostics_by_code["O3M102"]["category"] == "surface-evidence"
    assert diagnostics_by_code["O3M102"]["next_step"] == "Add source evidence for migrated ABI-sensitive types."
    assert diagnostics_by_code["O3M210"]["category"] == "unsafe-loading"
    assert diagnostics_by_code["O3M210"]["next_step"] == "Replace the marker with explicit imported-module ownership."
    assert negative.payload["rewrite_plan"]["safe_to_apply"] is False


def test_migration_rewrite_applies_only_safe_plan_and_reports_manual_work() -> None:
    analysis = analyze_migration_input(POSITIVE_INPUT)
    rewritten = apply_rewrite_plan(analysis.source_text, analysis.payload["rewrite_plan"])
    report = build_rewrite_workflow_report(
        analysis,
        input_path=POSITIVE_INPUT,
        rewritten_output_path=ROOT / "tmp" / "artifacts" / "migration-analyzer" / "unit-rewritten.objc3",
        rewritten_text=rewritten,
        analysis_report_path=ROOT / "tmp" / "reports" / "migration-analyzer" / "unit-analysis.json",
    )

    assert "import Foundation;" in rewritten
    assert 'import foreign "LegacyWidget-Swift.h";' in rewritten
    assert 'import cxx "CppWidget.hpp";' in rewritten
    assert "Bool enabled = true;" in rewritten
    assert "id fallback = nil;" in rewritten
    assert "@interface LegacyWidget" in rewritten
    assert report["status"] == "PASS"
    assert report["applied_edit_count"] == analysis.payload["rewrite_plan"]["automatic_edit_count"]
    assert report["manual_step_count"] == analysis.payload["rewrite_plan"]["manual_step_count"]


def test_migration_workflow_actions_are_public_and_runnable() -> None:
    for action in (
        "analyze-migration-source",
        "rewrite-migration-source",
        "validate-migration-workflow",
    ):
        assert action in ACTION_SPECS
        assert action in ACTION_HANDLERS
        assert ACTION_SPECS[action].pass_through_args is (action != "validate-migration-workflow")

    result = subprocess.run(
        [sys.executable, "scripts/analyze_objc3c_migration.py", str(POSITIVE_INPUT)],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0, result.stderr
    assert "analysis_report_path:" in result.stdout
    payload_start = result.stdout.find("{")
    payload = json.loads(result.stdout[payload_start:])
    assert payload["contract_id"] == "objc3c.migration_analyzer.report.v1"
    assert payload["status"] == "PASS"
