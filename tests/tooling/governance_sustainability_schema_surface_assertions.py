from __future__ import annotations

from types import ModuleType
from typing import Any

from governance_sustainability_schema_surface_sources import relative_schema_path


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    budget_summary_schema = relative_schema_path(
        "objc3c-governance-budget-summary-v1"
    )
    anti_regression_summary_schema = relative_schema_path(
        "objc3c-governance-anti-regression-summary-v1"
    )
    governance_evidence_schema = relative_schema_path(
        "objc3c-governance-sustainability-evidence-v1"
    )

    assert (
        summary["contract_id"]
        == "objc3c.governance.sustainability.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/governance_sustainability/schema_surface.json"
    )
    assert summary["budget_summary_schema"] == budget_summary_schema
    assert summary["anti_regression_summary_schema"] == (
        anti_regression_summary_schema
    )
    assert summary["governance_evidence_schema"] == governance_evidence_schema
    assert summary["schemas"] == [
        budget_summary_schema,
        anti_regression_summary_schema,
        governance_evidence_schema,
    ]
    assert summary["schema_ids"] == [
        "schemas/objc3c-governance-budget-summary-v1.schema.json",
        "schemas/objc3c-governance-anti-regression-summary-v1.schema.json",
        "schemas/objc3c-governance-sustainability-evidence-v1.schema.json",
    ]


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
