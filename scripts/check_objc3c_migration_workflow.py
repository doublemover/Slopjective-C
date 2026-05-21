#!/usr/bin/env python3
"""Validate the migration analyzer and rewrite workflow contracts."""

from __future__ import annotations

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_migration_analyzer import (
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
EXPECTED_REWRITE_OUTPUT = FIXTURE_ROOT / "migration_outputs" / "objc2_swift_cpp_positive_rewritten.objc3"
EXPECTED_NEGATIVE_DIAGNOSTICS = FIXTURE_ROOT / "migration_outputs" / "objc2_swift_cpp_negative_diagnostics.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "migration-analyzer" / "workflow-validation-summary.json"
REQUIRED_ACTIONS = (
    "analyze-migration-source",
    "rewrite-migration-source",
    "validate-migration-workflow",
)


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    failures: list[str] = []
    contract = load_contract()
    positive = analyze_migration_input(POSITIVE_INPUT)
    negative = analyze_migration_input(NEGATIVE_INPUT)
    rewritten_text = apply_rewrite_plan(positive.source_text, positive.payload["rewrite_plan"])
    expected_rewritten_text = EXPECTED_REWRITE_OUTPUT.read_text(encoding="utf-8")
    expected_negative_diagnostics = load_json_object(EXPECTED_NEGATIVE_DIAGNOSTICS)
    rewrite_report = build_rewrite_workflow_report(
        positive,
        input_path=POSITIVE_INPUT,
        rewritten_output_path=ROOT / "tmp" / "artifacts" / "migration-analyzer" / "validation-rewritten.objc3",
        rewritten_text=rewritten_text,
        analysis_report_path=ROOT / "tmp" / "reports" / "migration-analyzer" / "validation-analysis.json",
    )

    expect(positive.ok, "positive migration input did not pass", failures)
    expect(not negative.ok, "negative migration input did not fail closed", failures)
    expect(
        positive.payload["observed_surfaces"] == {surface: True for surface in contract["required_surfaces"]},
        "positive observed surfaces drifted",
        failures,
    )
    expect(
        positive.payload["rewrite_plan"]["automatic_edit_count"] >= 6,
        "rewrite plan did not include safe automatic edits",
        failures,
    )
    expect(
        positive.payload["rewrite_plan"]["manual_step_count"] >= 3,
        "rewrite plan did not surface manual migration steps",
        failures,
    )
    expect("import Foundation;" in rewritten_text, "Foundation import was not rewritten", failures)
    expect("Bool enabled = true;" in rewritten_text, "BOOL/YES rewrite was not applied", failures)
    expect(rewritten_text == expected_rewritten_text, "checked-in migration rewrite output drifted", failures)
    expect(rewrite_report["status"] == "PASS", "rewrite workflow report did not pass", failures)
    expect(
        any(item["code"] == "O3M210" for item in negative.payload["diagnostics"]),
        "negative unsafe mixed-image diagnostic missing",
        failures,
    )
    negative_diagnostics = [item for item in negative.payload["diagnostics"] if isinstance(item, dict)]
    for required in expected_negative_diagnostics["required_diagnostics"]:
        matches = [
            item
            for item in negative_diagnostics
            if item.get("code") == required["code"]
            and item.get("category") == required["category"]
            and item.get("surface") == required["surface"]
        ]
        expect(
            len(matches) >= int(required.get("minimum_count", 1)),
            f"checked-in negative migration diagnostic missing {required['code']}",
            failures,
        )
        for field in required["metadata_fields"]:
            expect(
                all(field in item and item[field] not in ("", None) for item in matches),
                f"negative migration diagnostic {required['code']} missing metadata field {field}",
                failures,
            )
    for action in REQUIRED_ACTIONS:
        expect(action in ACTION_SPECS, f"missing workflow action spec {action}", failures)
        expect(action in ACTION_HANDLERS, f"missing workflow action handler {action}", failures)

    summary = {
        "contract_id": "objc3c.migration_analyzer.workflow_validation.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract_path": repo_rel(FIXTURE_ROOT / "migration_analyzer_contract.json"),
        "positive_input": repo_rel(POSITIVE_INPUT),
        "negative_input": repo_rel(NEGATIVE_INPUT),
        "expected_rewrite_output": repo_rel(EXPECTED_REWRITE_OUTPUT),
        "expected_negative_diagnostics": repo_rel(EXPECTED_NEGATIVE_DIAGNOSTICS),
        "public_actions": list(REQUIRED_ACTIONS),
        "positive_deterministic_digest": positive.payload["deterministic_digest"],
        "negative_deterministic_digest": negative.payload["deterministic_digest"],
        "automatic_edit_count": positive.payload["rewrite_plan"]["automatic_edit_count"],
        "manual_step_count": positive.payload["rewrite_plan"]["manual_step_count"],
        "failures": failures,
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-migration-workflow: PASS" if not failures else "objc3c-migration-workflow: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
