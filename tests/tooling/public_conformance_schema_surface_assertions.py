from __future__ import annotations

from types import ModuleType
from typing import Any

from public_conformance_schema_surface_sources import relative_schema_path


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    dashboard_status_schema = relative_schema_path(
        "objc3-conformance-dashboard-status-v1"
    )
    public_scorecard_schema = relative_schema_path(
        "objc3c-public-conformance-scorecard-v1"
    )
    public_suite_schema = relative_schema_path(
        "objc3c-public-conformance-suite-v1"
    )
    public_summary_schema = relative_schema_path(
        "objc3c-public-conformance-summary-v1"
    )

    assert (
        summary["contract_id"]
        == "objc3c.public_conformance_reporting.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/public_conformance_reporting/schema_surface.json"
    )
    assert summary["dashboard_status_schema"] == dashboard_status_schema
    assert summary["public_suite_schema"] == public_suite_schema
    assert summary["public_scorecard_schema"] == public_scorecard_schema
    assert summary["public_summary_schema"] == public_summary_schema
    assert summary["schemas"] == [
        dashboard_status_schema,
        public_suite_schema,
        public_scorecard_schema,
        public_summary_schema,
    ]
    assert summary["schema_ids"] == [
        "https://objc3c.dev/schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-public-conformance-suite-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-public-conformance-scorecard-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-public-conformance-summary-v1.schema.json",
    ]


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
