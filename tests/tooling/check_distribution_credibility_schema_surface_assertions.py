from __future__ import annotations

from types import ModuleType
from typing import Any

from check_distribution_credibility_schema_surface_sources import relative_schema_path


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    dashboard_schema = relative_schema_path(
        "objc3c-distribution-credibility-dashboard-v1"
    )
    trust_report_schema = relative_schema_path(
        "objc3c-distribution-trust-report-v1"
    )

    assert (
        summary["contract_id"]
        == "objc3c.distribution.credibility.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/distribution_credibility/schema_surface.json"
    )
    assert summary["dashboard_schema"] == dashboard_schema
    assert summary["trust_report_schema"] == trust_report_schema
    assert summary["schemas"] == [dashboard_schema, trust_report_schema]
    assert summary["schema_ids"] == [
        "https://objc3c.dev/schemas/objc3c-distribution-credibility-dashboard-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-distribution-trust-report-v1.schema.json",
    ]


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
