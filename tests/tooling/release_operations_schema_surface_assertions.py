from __future__ import annotations

from types import ModuleType
from typing import Any


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    assert (
        summary["contract_id"]
        == "objc3c.release.operations.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["metadata_surface"]
        == "tests/tooling/fixtures/release_operations/metadata_surface.json"
    )
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/release_operations/schema_surface.json"
    )
    assert summary["update_manifest"] == (
        "schemas/objc3c-update-manifest-v1.schema.json"
    )
    assert summary["release_channel_manifest"] == (
        "schemas/objc3c-release-channel-operations-v1.schema.json"
    )
    assert summary["upgrade_support_report"] == (
        "schemas/objc3c-upgrade-support-report-v1.schema.json"
    )
    assert summary["schemas"] == [
        "schemas/objc3c-update-manifest-v1.schema.json",
        "schemas/objc3c-release-channel-operations-v1.schema.json",
        "schemas/objc3c-upgrade-support-report-v1.schema.json",
    ]
    assert summary["schema_ids"] == [
        "https://objc3c.dev/schemas/objc3c-update-manifest-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-release-channel-operations-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-upgrade-support-report-v1.schema.json",
    ]


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
