from __future__ import annotations

from typing import Any

from packaging_channels_schema_surface_sources import relative_schema_path


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    manifest_schema = relative_schema_path("objc3c-package-channels-manifest-v1")
    receipt_schema = relative_schema_path("objc3c-package-install-receipt-v1")
    runtime_manifest_schema = relative_schema_path(
        "objc3c-sanitizer-runtime-library-manifest-v1"
    )

    assert (
        summary["contract_id"]
        == "objc3c.packaging.channels.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["metadata_surface"]
        == "tests/tooling/fixtures/packaging_channels/metadata_surface.json"
    )
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/packaging_channels/schema_surface.json"
    )
    assert summary["package_channels_manifest"] == manifest_schema
    assert summary["install_receipt"] == receipt_schema
    assert summary["sanitizer_runtime_library_manifest"] == runtime_manifest_schema
    assert summary["schema_count"] == 3
    assert summary["schemas"] == [
        manifest_schema,
        receipt_schema,
        runtime_manifest_schema,
    ]
    assert summary["schema_ids"] == [
        "https://objc3c.dev/schemas/objc3c-package-channels-manifest-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-sanitizer-runtime-library-manifest-v1.schema.json",
    ]


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
