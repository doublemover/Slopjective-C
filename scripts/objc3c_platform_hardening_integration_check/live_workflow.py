"""Live platform-hardening integration workflow."""

from __future__ import annotations

import sys

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    CHANNEL_CATALOG_PATH,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_SUMMARY_PATH,
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH,
    PLATFORM_HARDENING_SUMMARY_PATHS,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    UPDATE_MANIFEST_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    platform_hardening_owner_payload,
)

from .reports import (
    REQUIRED_REPORT_PATHS,
    assert_live_report_statuses,
    assert_publication_surface,
    assert_release_metadata_links,
    load_live_reports,
    require_reports,
)
from .steps import classify_step_results, run_live_steps


def run_live_integration() -> int:
    steps = run_live_steps()
    failures: list[str] = []
    classify_step_results(steps, failures)
    require_reports((*REQUIRED_REPORT_PATHS, *PLATFORM_HARDENING_SUMMARY_PATHS), failures)

    reports = load_live_reports()
    assert_publication_surface(reports["support_matrix"], failures)
    assert_live_report_statuses(reports, failures)
    assert_release_metadata_links(reports, failures)

    payload = build_integration_summary_payload(steps, failures)
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH)}")
    if failures:
        print("platform-hardening-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("platform-hardening-integration: PASS")
    return 0


def build_integration_summary_payload(steps: list[dict[str, object]], failures: list[str]) -> dict[str, object]:
    return {
        "contract_id": "objc3c.platform.hardening.integration.summary.v1",
        "ok": not failures,
        "owner_policy": platform_hardening_owner_payload(),
        "blocker_metadata": {
            "blocker_owner": "platform-hardening-blockers",
            "blocking_conditions": [
                "platform hardening summary builder failed",
                "build/package validation did not pass",
                "toolchain range replay did not pass",
                "install matrix integration did not pass",
                "release publication metadata drifted from platform support matrix",
            ],
        },
        "failures": failures,
        "steps": steps,
        "reports": {
            "support_matrix": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
            "summary_builders": [repo_rel(path) for path in PLATFORM_HARDENING_SUMMARY_PATHS],
            "build_package_validation": repo_rel(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH),
            "toolchain_range_replay": repo_rel(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH),
            "install_matrix_integration": repo_rel(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH),
            "package_channels": repo_rel(PACKAGE_CHANNELS_SUMMARY_PATH),
            "update_manifest": repo_rel(UPDATE_MANIFEST_PATH),
            "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT_PATH),
            "channel_catalog": repo_rel(CHANNEL_CATALOG_PATH),
        },
    }
