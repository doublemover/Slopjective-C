from __future__ import annotations

import json
from pathlib import Path

from scripts.platform_hardening_contracts.report_payloads import build_support_matrix_payload
from scripts.release_operations_publication_contracts import (
    build_release_operations_publication_payloads,
)


ROOT = Path(__file__).resolve().parents[2]


def load_fixture(relative_path: str) -> dict:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def test_platform_support_matrix_publishes_matrix_dimensions_and_acceptance() -> None:
    payload = build_support_matrix_payload()

    assert payload["matrix_dimensions"]["host_os"] == ["windows"]
    assert payload["matrix_dimensions"]["host_arch"] == ["x64"]
    assert "signed-installer" in payload["matrix_dimensions"]["release_validation_surfaces"]
    assert payload["packaged_runtime_acceptance"] == [
        {
            "platform_id": "windows-x64",
            "required_public_actions": [
                "test-runtime-acceptance-fast",
                "validate-platform-hardening-end-to-end",
                "validate-packaging-channels-end-to-end",
            ],
            "required_artifacts": [
                "portable_archive",
                "installer_archive",
                "offline_archive",
                "installer_signature",
            ],
            "rollback_required": True,
        }
    ]


def test_release_operations_publication_publishes_rollback_diagnostics() -> None:
    update_channel_policy = load_fixture(
        "tests/tooling/fixtures/release_operations/update_channel_policy.json"
    )
    fail_closed_policy = load_fixture(
        "tests/tooling/fixtures/release_operations/fail_closed_diagnostics_policy.json"
    )
    payloads = build_release_operations_publication_payloads(
        update_manifest={
            "current_version": "3.1.0",
            "default_channel": "stable",
            "default_platform_id": "windows-x64",
            "platform_support_matrix": "tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json",
            "supported_platform_ids": ["windows-x64"],
            "support_tiers": [{"tier_id": "tier-1", "status": "supported"}],
            "channels": [
                {
                    "channel_id": "stable",
                    "warning_classes": ["deprecated-channel"],
                }
            ],
        },
        versioning_model=load_fixture(
            "tests/tooling/fixtures/release_operations/versioning_model.json"
        ),
        upgrade_surface=load_fixture(
            "tests/tooling/fixtures/release_operations/upgrade_path_surface.json"
        ),
        claim_policy=load_fixture(
            "tests/tooling/fixtures/release_operations/upgrade_support_claim_policy.json"
        ),
        update_channel_policy=update_channel_policy,
        fail_closed_policy=fail_closed_policy,
        metadata_surface=load_fixture(
            "tests/tooling/fixtures/release_operations/metadata_surface.json"
        ),
        update_manifest_path="tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json",
        upgrade_support_report_path="tmp/artifacts/release-operations/publication/objc3c-upgrade-support-report.json",
        channel_catalog_path="tmp/artifacts/release-operations/publication/objc3c-release-channel-catalog.json",
    )

    rollback_diagnostics = payloads.upgrade_support_report["rollback_diagnostics"]
    assert len(rollback_diagnostics) == len(fail_closed_policy["diagnostic_classes"])
    assert rollback_diagnostics[0]["user_facing_message"]
    assert rollback_diagnostics[0]["rollback_channel"] == "local-installer"
    assert rollback_diagnostics[0]["operator_command"] == "npm run objc3c -- build-package-channels"
    assert payloads.summary["rollback_diagnostic_count"] == len(rollback_diagnostics)
