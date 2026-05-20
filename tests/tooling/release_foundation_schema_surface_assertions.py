from __future__ import annotations

from types import ModuleType
from typing import Any

from release_foundation_schema_surface_sources import relative_schema_path


def assert_summary_matches_registered_schemas(summary: dict[str, Any]) -> None:
    release_manifest_schema = relative_schema_path("objc3c-release-manifest-v1")
    release_sbom_schema = relative_schema_path("objc3c-release-sbom-v1")
    release_attestation_schema = relative_schema_path("objc3c-release-attestation-v1")

    assert summary["contract_id"] == (
        "objc3c.release.foundation.schema.surface.summary.v1"
    )
    assert summary["status"] == "PASS"
    assert (
        summary["schema_surface"]
        == "tests/tooling/fixtures/release_foundation/schema_surface.json"
    )
    assert summary["release_manifest_schema"] == release_manifest_schema
    assert summary["release_sbom_schema"] == release_sbom_schema
    assert summary["release_attestation_schema"] == release_attestation_schema
    assert summary["schemas"] == [
        release_manifest_schema,
        release_sbom_schema,
        release_attestation_schema,
    ]
    assert summary["schema_ids"] == [
        "https://objc3c.dev/schemas/objc3c-release-manifest-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-release-sbom-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-release-attestation-v1.schema.json",
    ]
    assert summary["abi_manifest_schemas"] == {
        "objc3-abi-2025Q4": "schemas/objc3-abi-2025Q4.schema.json",
        "objc3-runtime-2025Q4-manifest": (
            "schemas/objc3-runtime-2025Q4.manifest.schema.json"
        ),
    }
    assert summary["package_attestation_schemas"] == {
        "objc3c-package-channels-manifest-v1": (
            "schemas/objc3c-package-channels-manifest-v1.schema.json"
        ),
        "objc3c-package-install-receipt-v1": (
            "schemas/objc3c-package-install-receipt-v1.schema.json"
        ),
        "objc3c-update-manifest-v1": (
            "schemas/objc3c-update-manifest-v1.schema.json"
        ),
    }


def assert_fail_closed_without_summary(checker: ModuleType) -> None:
    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
