"""Report loading and publication-surface assertions for platform hardening."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    CHANNEL_CATALOG_PATH,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_SUMMARY_PATH,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    UPDATE_MANIFEST_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    summary_passes,
)

from .assertions import expect


EXPECTED_PUBLICATION_SURFACE = {
    "package_bridge": "objc3c",
    "inspect_support_matrix_command": "build-platform-support-matrix",
    "package_command": "package-runnable-toolchain",
    "package_channels_command": "build-package-channels",
    "packaging_validation_command": "validate-packaging-channels",
    "packaging_end_to_end_command": "validate-packaging-channels-end-to-end",
    "platform_hardening_validation_command": "validate-platform-hardening",
    "platform_hardening_end_to_end_command": "validate-platform-hardening-end-to-end",
    "release_operations_command": "validate-release-operations",
    "release_operations_end_to_end_command": "validate-release-operations-end-to-end",
}

REQUIRED_REPORT_PATHS = (
    SUPPORT_MATRIX_ARTIFACT_PATH,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_SUMMARY_PATH,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    UPDATE_MANIFEST_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    CHANNEL_CATALOG_PATH,
)


def require_reports(paths: tuple[Path, ...], failures: list[str]) -> None:
    for path in paths:
        expect(path.is_file(), f"missing expected report: {repo_rel(path)}", failures)


def load_report(path: Path) -> dict[str, Any]:
    return load_json(path) if path.is_file() else {}


def load_live_reports() -> dict[str, dict[str, Any]]:
    return {
        "support_matrix": load_report(SUPPORT_MATRIX_ARTIFACT_PATH),
        "build_package_summary": load_report(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH),
        "toolchain_range_summary": load_report(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH),
        "install_matrix_summary": load_report(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH),
        "package_channels_summary": load_report(PACKAGE_CHANNELS_SUMMARY_PATH),
        "update_manifest": load_report(UPDATE_MANIFEST_PATH),
        "upgrade_support_report": load_report(UPGRADE_SUPPORT_REPORT_PATH),
        "channel_catalog": load_report(CHANNEL_CATALOG_PATH),
    }


def assert_publication_surface(support_matrix: dict[str, Any], failures: list[str]) -> None:
    publication_surface = support_matrix.get("publication_surface", {})
    for key, expected in EXPECTED_PUBLICATION_SURFACE.items():
        expect(publication_surface.get(key) == expected, f"support matrix publication surface drifted for {key}", failures)


def assert_live_report_statuses(reports: dict[str, dict[str, Any]], failures: list[str]) -> None:
    expect(summary_passes(reports["build_package_summary"]), "build/package validation summary did not report PASS", failures)
    expect(summary_passes(reports["toolchain_range_summary"]), "toolchain-range replay summary did not report PASS", failures)
    expect(summary_passes(reports["install_matrix_summary"]), "install-matrix integration summary did not report PASS", failures)


def assert_release_metadata_links(reports: dict[str, dict[str, Any]], failures: list[str]) -> None:
    support_matrix = reports["support_matrix"]
    expected_matrix_path = repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH)
    expected_platform_ids = support_matrix.get("claim_boundary", {}).get("supported_platform_ids")
    expected_default_platform = support_matrix.get("default_platform_id")
    expected_tiers = support_matrix.get("tiers")

    package_channels_summary = reports["package_channels_summary"]
    update_manifest = reports["update_manifest"]
    upgrade_support_report = reports["upgrade_support_report"]
    channel_catalog = reports["channel_catalog"]

    expect(package_channels_summary.get("platform_support_matrix") == expected_matrix_path, "package-channels summary missing platform support matrix link", failures)
    expect(update_manifest.get("platform_support_matrix") == expected_matrix_path, "update manifest missing platform support matrix link", failures)
    expect(update_manifest.get("default_platform_id") == expected_default_platform, "update manifest default_platform_id drifted", failures)
    expect(update_manifest.get("supported_platform_ids") == expected_platform_ids, "update manifest supported_platform_ids drifted", failures)
    expect(update_manifest.get("support_tiers") == expected_tiers, "update manifest support tiers drifted", failures)
    expect(upgrade_support_report.get("platform_support_matrix") == expected_matrix_path, "upgrade support report missing platform support matrix link", failures)
    expect(upgrade_support_report.get("default_platform_id") == expected_default_platform, "upgrade support report default platform drifted", failures)
    expect(upgrade_support_report.get("supported_platform_ids") == expected_platform_ids, "upgrade support report supported platform ids drifted", failures)
    expect(upgrade_support_report.get("support_tiers") == expected_tiers, "upgrade support report support tiers drifted", failures)
    expect(channel_catalog.get("platform_support_matrix") == expected_matrix_path, "channel catalog missing platform support matrix link", failures)
    expect(channel_catalog.get("default_platform_id") == expected_default_platform, "channel catalog default platform drifted", failures)
    expect(channel_catalog.get("supported_platform_ids") == expected_platform_ids, "channel catalog supported platform ids drifted", failures)
    expect(channel_catalog.get("support_tiers") == expected_tiers, "channel catalog support tiers drifted", failures)
