from __future__ import annotations

import sys
from copy import deepcopy
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from release_operations_publication_contracts import (  # noqa: E402
    build_release_operations_publication_payloads,
)


def _publication_payloads():
    package_channel_freshness = {
        "timestamp_sources": [
            "package_channels_summary.generated_at_utc",
            "package_channels_manifest.generated_at_utc",
            "platform_support_matrix.generated_at_utc",
        ],
        "generated_at_utc": {
            "package_channels_summary.generated_at_utc": "2026-05-21T10:00:00Z",
            "package_channels_manifest.generated_at_utc": "2026-05-21T10:10:00Z",
            "platform_support_matrix.generated_at_utc": "2026-05-21T10:30:00Z",
        },
        "artifact_skew_hours": 0.5,
        "channel_max_artifact_skew_hours": {"stable": 6, "nightly": 6},
        "stale_behavior": "fail-closed",
        "refresh_command": "npm run objc3c -- build-package-channels",
        "blocks_publication_on_stale": True,
    }
    update_manifest = {
        "current_version": "3.0.0",
        "default_channel": "stable",
        "default_platform_id": "windows-x64",
        "platform_support_matrix": "tmp/artifacts/platform/matrix.json",
        "supported_platform_ids": ["windows-x64"],
        "support_tiers": [{"tier": "supported"}],
        "local_provenance": {"git_commit": "abc123", "git_tree_dirty": False},
        "channels": [
            {
                "channel_id": "stable",
                "version": "3.0.0",
                "warning_classes": [],
                "upgrade_targets": ["nightly"],
            },
            {
                "channel_id": "nightly",
                "version": "3.0.0-nightly",
                "warning_classes": ["nightly-not-stable"],
                "upgrade_targets": ["stable"],
            },
        ],
    }
    release_channel_manifest = {
        "source_model": "tests/tooling/fixtures/release_operations/channel_operations_model.json",
        "release_evidence": {
            "release_note_sources": [
                "tests/tooling/fixtures/release_operations/channel_operations_model.json",
                "tests/tooling/fixtures/release_operations/update_channel_policy.json",
            ],
            "public_changelog_sources": [
                "docs/runbooks/objc3c_release_operations.md",
                "tests/tooling/fixtures/release_operations/upgrade_path_surface.json",
            ],
            "evidence_artifacts": ["tmp/artifacts/release-operations/channel.json"],
        },
        "channel_manifests": [
            {
                "channel_id": "stable",
                "operation_class": "stable",
                "version": "3.0.0",
                "support_status": "supported",
                "publication_scope": "public-stable",
                "update_manifest_channel": "stable",
                "release_gate_actions": [
                    "validate-release-candidate-conformance",
                    "validate-release-operations-end-to-end",
                ],
                "artifact_refs": {"portable_archive": "tmp/pkg/stable.zip"},
                "package_channel_freshness": package_channel_freshness,
                "rollback_safety": {
                    "rollback_channel": "local-installer",
                    "operator_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
                    "blocks_publication_on_failure": True,
                },
                "release_notes_policy": {
                    "source_mode": "source-derived",
                    "forbidden_sources": ["manual changelog row"],
                },
            },
            {
                "channel_id": "nightly",
                "operation_class": "nightly",
                "version": "3.0.0-nightly",
                "support_status": "nightly",
                "publication_scope": "nightly-evidence-only",
                "update_manifest_channel": "nightly",
                "release_gate_actions": ["test-nightly", "validate-release-operations"],
                "artifact_refs": {"portable_archive": "tmp/pkg/nightly.zip"},
                "package_channel_freshness": package_channel_freshness,
                "rollback_safety": {
                    "rollback_channel": "offline-bundle",
                    "operator_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
                    "blocks_publication_on_failure": True,
                },
                "release_notes_policy": {
                    "source_mode": "source-derived",
                    "forbidden_sources": ["stable support claim"],
                },
            },
        ],
    }
    return build_release_operations_publication_payloads(
        update_manifest=update_manifest,
        versioning_model={"support_windows": {"stable": {"months": 18}}},
        upgrade_surface={"upgrade_path_classes": []},
        claim_policy={"forbidden_claims": [], "upgrade_claim_classes": []},
        update_channel_policy={
            "default_channel": "stable",
            "warning_classes": [{"warning_id": "nightly-not-stable", "severity": "warn"}],
            "channels": [
                {"channel_id": "stable", "revert_channel": "local-installer"},
                {"channel_id": "nightly", "revert_channel": "offline-bundle"},
            ],
        },
        fail_closed_policy={"diagnostic_classes": []},
        metadata_surface={
            "required_upgrade_support_report_fields": [
                "contract_id",
                "generated_at_utc",
                "current_version",
                "default_platform_id",
                "platform_support_matrix",
                "supported_platform_ids",
                "support_tiers",
                "support_windows",
                "release_channel_manifest",
                "channel_operations",
                "upgrade_paths",
                "warnings",
                "revert_guidance",
                "rollback_diagnostics",
                "fail_closed_diagnostics",
                "forbidden_claims",
            ]
        },
        update_manifest_path="tmp/artifacts/release-operations/update-manifest.json",
        release_channel_manifest=release_channel_manifest,
        release_channel_manifest_path="tmp/artifacts/release-operations/channel-manifest.json",
        upgrade_support_report_path="tmp/artifacts/release-operations/upgrade-report.json",
        channel_catalog_path="tmp/artifacts/release-operations/channel-catalog.json",
        release_notes_path="tmp/artifacts/release-operations/release-notes.json",
        public_changelog_path="tmp/artifacts/release-operations/public-changelog.json",
    )


def test_release_operations_publication_rejects_generated_release_note_source() -> None:
    payloads = _publication_payloads()
    release_channel_manifest = {
        "source_model": payloads.release_notes["source_model"],
        "release_evidence": {
            **payloads.channel_catalog["release_evidence"],
            "release_note_sources": [
                "tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json"
            ],
        },
        "channel_manifests": deepcopy(payloads.channel_catalog["channel_operations"]),
    }

    # Reuse the direct builder contract with only the source-list failure under test.
    with pytest.raises(RuntimeError, match="used generated output as source truth"):
        build_release_operations_publication_payloads(
            update_manifest={
                "current_version": "3.0.0",
                "default_channel": "stable",
                "default_platform_id": "windows-x64",
                "platform_support_matrix": "tmp/artifacts/platform/matrix.json",
                "supported_platform_ids": ["windows-x64"],
                "support_tiers": [{"tier": "supported"}],
                "local_provenance": {},
                "channels": [],
            },
            versioning_model={"support_windows": {}},
            upgrade_surface={"upgrade_path_classes": []},
            claim_policy={"forbidden_claims": [], "upgrade_claim_classes": []},
            update_channel_policy={
                "default_channel": "stable",
                "warning_classes": [],
                "channels": [{"channel_id": "stable", "revert_channel": "local-installer"}],
            },
            fail_closed_policy={"diagnostic_classes": []},
            metadata_surface={"required_upgrade_support_report_fields": []},
            update_manifest_path="tmp/artifacts/release-operations/update-manifest.json",
            release_channel_manifest=release_channel_manifest,
            release_channel_manifest_path="tmp/artifacts/release-operations/channel-manifest.json",
            upgrade_support_report_path="tmp/artifacts/release-operations/upgrade-report.json",
            channel_catalog_path="tmp/artifacts/release-operations/channel-catalog.json",
            release_notes_path="tmp/artifacts/release-operations/release-notes.json",
            public_changelog_path="tmp/artifacts/release-operations/public-changelog.json",
        )


def test_release_operations_publication_emits_source_derived_release_notes() -> None:
    payloads = _publication_payloads()
    release_notes = payloads.release_notes

    assert release_notes["contract_id"] == "objc3c.release.operations.release-notes.v1"
    assert release_notes["source_mode"] == "source-derived"
    assert release_notes["source_model"] == (
        "tests/tooling/fixtures/release_operations/channel_operations_model.json"
    )
    assert release_notes["release_note_sources"] == [
        "tests/tooling/fixtures/release_operations/channel_operations_model.json",
        "tests/tooling/fixtures/release_operations/update_channel_policy.json",
    ]
    assert release_notes["forbidden_sources"] == [
        "manual changelog row",
        "stable support claim",
    ]
    assert [entry["channel_id"] for entry in release_notes["channels"]] == [
        "stable",
        "nightly",
    ]
    assert release_notes["channels"][0]["rollback_channel"] == "local-installer"
    assert release_notes["channels"][0]["package_channel_freshness"]["stale_behavior"] == (
        "fail-closed"
    )


def test_release_operations_publication_emits_public_changelog_from_release_notes() -> None:
    payloads = _publication_payloads()
    public_changelog = payloads.public_changelog

    assert (
        public_changelog["contract_id"]
        == "objc3c.release.operations.public-changelog.v1"
    )
    assert public_changelog["source_mode"] == "source-derived"
    assert (
        public_changelog["release_notes"]
        == "tmp/artifacts/release-operations/release-notes.json"
    )
    assert public_changelog["public_changelog_sources"] == [
        "docs/runbooks/objc3c_release_operations.md",
        "tests/tooling/fixtures/release_operations/upgrade_path_surface.json",
    ]
    assert [entry["channel_id"] for entry in public_changelog["entries"]] == [
        "stable",
        "nightly",
    ]
    assert payloads.summary["release_notes"] == (
        "tmp/artifacts/release-operations/release-notes.json"
    )
    assert payloads.summary["public_changelog"] == (
        "tmp/artifacts/release-operations/public-changelog.json"
    )
    assert payloads.summary["package_channel_freshness_channel_count"] == 2
