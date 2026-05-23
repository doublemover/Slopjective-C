#!/usr/bin/env python3
"""Build the machine-owned objc3c update manifest from live release artifacts."""

from __future__ import annotations

import hashlib
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Mapping

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "source_surface.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
CHANNEL_OPERATIONS_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "channel_operations_model.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "release-operations" / "update-manifest-summary.json"
MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
CHANNEL_MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-operations" / "channel-manifest" / "objc3c-release-channel-manifest.json"
PACKAGE_CHANNELS_SUMMARY = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
RELEASE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
PLATFORM_SUPPORT_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "platform-matrix-summary.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-report.json"
PACKAGE_FRESHNESS_TIMESTAMP_SOURCES = [
    "package_channels_summary.generated_at_utc",
    "package_channels_manifest.generated_at_utc",
    "platform_support_matrix.generated_at_utc",
]
PACKAGE_FRESHNESS_REFRESH_COMMAND = "npm run objc3c -- build-package-channels"
GENERATED_OUTPUT_PREFIXES = ("tmp/", "artifacts/")


def require_file(path: Path, owner_action: str) -> None:
    if not path.is_file():
        raise RuntimeError(
            f"required upstream artifact missing: {repo_rel(path)}; run owner action {owner_action}"
        )


def require_string(payload: Mapping[str, Any], field_name: str, owner_action: str) -> str:
    value = payload.get(field_name)
    if not isinstance(value, str) or not value:
        raise RuntimeError(
            f"required upstream field missing: {field_name}; run owner action {owner_action}"
        )
    return value


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def workflow_command(action: str) -> str:
    return f"npm run objc3c -- {action}"


def normalized_repo_path(raw_path: Any, field_name: str) -> str:
    if not isinstance(raw_path, str) or not raw_path:
        raise RuntimeError(f"{field_name} must be a non-empty repo-relative path")
    normalized = raw_path.replace("\\", "/")
    if normalized != raw_path:
        raise RuntimeError(f"{field_name} must use slash-separated repo paths: {raw_path}")
    path = Path(normalized)
    if path.is_absolute() or ".." in path.parts:
        raise RuntimeError(f"{field_name} must be a repo-relative path: {raw_path}")
    return normalized


def is_generated_output_path(raw_path: str) -> bool:
    return raw_path.startswith(GENERATED_OUTPUT_PREFIXES)


def require_checked_source_path(raw_path: Any, field_name: str) -> str:
    normalized = normalized_repo_path(raw_path, field_name)
    if is_generated_output_path(normalized):
        raise RuntimeError(
            f"{field_name} used generated output as release source truth: {normalized}"
        )
    if not (ROOT / normalized).is_file():
        raise RuntimeError(f"{field_name} references missing checked source: {normalized}")
    return normalized


def require_generated_evidence_path(raw_path: Any, field_name: str) -> str:
    normalized = normalized_repo_path(raw_path, field_name)
    if not is_generated_output_path(normalized):
        raise RuntimeError(
            f"{field_name} must be generated release evidence, not source truth: {normalized}"
        )
    return normalized


def require_checked_source_list(
    payload: Mapping[str, Any],
    field_name: str,
    owner: str,
) -> list[str]:
    values = payload.get(field_name)
    if not isinstance(values, list) or not values:
        raise RuntimeError(f"{owner}.{field_name} must be a non-empty source list")
    return [
        require_checked_source_path(value, f"{owner}.{field_name}[{index}]")
        for index, value in enumerate(values)
    ]


def require_generated_evidence_list(
    values: Any,
    field_name: str,
    owner: str,
) -> list[str]:
    if not isinstance(values, list) or not values:
        raise RuntimeError(f"{owner}.{field_name} must be a non-empty generated evidence list")
    return [
        require_generated_evidence_path(value, f"{owner}.{field_name}[{index}]")
        for index, value in enumerate(values)
    ]


def validate_release_source_boundaries(
    channel_operations_model: Mapping[str, Any],
) -> dict[str, list[str]]:
    release_note_sources = require_checked_source_list(
        channel_operations_model,
        "release_note_sources",
        "channel_operations_model",
    )
    public_changelog_sources = require_checked_source_list(
        channel_operations_model,
        "public_changelog_sources",
        "channel_operations_model",
    )
    for channel in channel_operations_model.get("channels", []):
        if not isinstance(channel, Mapping):
            raise RuntimeError("channel operations model contained a non-object channel")
        channel_id = str(channel.get("channel_id", ""))
        policy = channel.get("release_notes_policy")
        if not isinstance(policy, Mapping):
            raise RuntimeError(f"{channel_id} channel release notes policy must be source-derived")
        if policy.get("source_mode") != "source-derived":
            raise RuntimeError(f"{channel_id} channel release notes policy must be source-derived")
        required = policy.get("required_sources")
        if not isinstance(required, list) or not required:
            raise RuntimeError(f"{channel_id} channel must define required release-note sources")
        forbidden = policy.get("forbidden_sources")
        if not isinstance(forbidden, list) or not forbidden:
            raise RuntimeError(f"{channel_id} channel must define forbidden release-note sources")
        if any(isinstance(source, str) and source.startswith("tmp/") for source in forbidden):
            raise RuntimeError(f"{channel_id} channel must not normalize tmp-only claims as forbidden prose")
    return {
        "release_note_sources": release_note_sources,
        "public_changelog_sources": public_changelog_sources,
    }


def parse_generated_at(value: Any, source_name: str) -> datetime:
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"{source_name} missing generated_at_utc")
    timestamp = value
    if timestamp.endswith("Z"):
        timestamp = timestamp[:-1] + "+00:00"
    try:
        parsed = datetime.fromisoformat(timestamp)
    except ValueError as exc:
        raise RuntimeError(f"{source_name} generated_at_utc is not ISO-8601: {value}") from exc
    if parsed.tzinfo is None:
        raise RuntimeError(f"{source_name} generated_at_utc must include a timezone")
    return parsed.astimezone(timezone.utc)


def validate_package_channel_freshness_policy(channel: Mapping[str, Any]) -> Mapping[str, Any]:
    channel_id = str(channel.get("channel_id", ""))
    policy = channel.get("package_channel_freshness")
    if not isinstance(policy, dict):
        raise RuntimeError(f"{channel_id} channel missing package channel freshness policy")
    if policy.get("timestamp_sources") != PACKAGE_FRESHNESS_TIMESTAMP_SOURCES:
        raise RuntimeError(f"{channel_id} package channel freshness timestamp sources drifted")
    max_skew = policy.get("max_artifact_skew_hours")
    if not isinstance(max_skew, int) or max_skew <= 0:
        raise RuntimeError(f"{channel_id} package channel freshness skew must be a positive hour count")
    if policy.get("stale_behavior") != "fail-closed":
        raise RuntimeError(f"{channel_id} package channel freshness must fail closed")
    if policy.get("refresh_command") != PACKAGE_FRESHNESS_REFRESH_COMMAND:
        raise RuntimeError(f"{channel_id} package channel freshness refresh command drifted")
    if policy.get("blocks_publication_on_stale") is not True:
        raise RuntimeError(f"{channel_id} package channel freshness must block publication when stale")
    return policy


def validate_package_channel_freshness(
    *,
    package_channels_summary: Mapping[str, Any],
    package_channels_manifest: Mapping[str, Any],
    platform_support_matrix: Mapping[str, Any],
    channel_operations_by_id: Mapping[str, Mapping[str, Any]],
) -> dict[str, Any]:
    timestamps = {
        "package_channels_summary.generated_at_utc": parse_generated_at(
            package_channels_summary.get("generated_at_utc"),
            "package channels summary",
        ),
        "package_channels_manifest.generated_at_utc": parse_generated_at(
            package_channels_manifest.get("generated_at_utc"),
            "package channels manifest",
        ),
        "platform_support_matrix.generated_at_utc": parse_generated_at(
            platform_support_matrix.get("generated_at_utc"),
            "platform support matrix",
        ),
    }
    oldest = min(timestamps.values())
    newest = max(timestamps.values())
    skew_hours = (newest - oldest).total_seconds() / 3600
    channel_limits: dict[str, int] = {}
    for channel_id, channel in channel_operations_by_id.items():
        policy = validate_package_channel_freshness_policy(channel)
        max_skew = int(policy["max_artifact_skew_hours"])
        channel_limits[channel_id] = max_skew
        if skew_hours > max_skew:
            raise RuntimeError(
                f"{channel_id} package channel evidence is stale: "
                f"{skew_hours:.2f}h artifact skew exceeds {max_skew}h; "
                f"run {PACKAGE_FRESHNESS_REFRESH_COMMAND}"
            )
    return {
        "timestamp_sources": PACKAGE_FRESHNESS_TIMESTAMP_SOURCES,
        "generated_at_utc": {
            key: value.isoformat().replace("+00:00", "Z")
            for key, value in timestamps.items()
        },
        "artifact_skew_hours": round(skew_hours, 6),
        "channel_max_artifact_skew_hours": channel_limits,
        "stale_behavior": "fail-closed",
        "refresh_command": PACKAGE_FRESHNESS_REFRESH_COMMAND,
        "blocks_publication_on_stale": True,
    }


def validate_channel_operations_model(
    *,
    channel_operations_model: Mapping[str, Any],
    update_channel_policy: Mapping[str, Any],
) -> dict[str, Mapping[str, Any]]:
    if channel_operations_model.get("contract_id") != "objc3c.release.operations.channel.operations.model.v1":
        raise RuntimeError("channel operations model contract_id drifted")
    validate_release_source_boundaries(channel_operations_model)
    channels = channel_operations_model.get("channels")
    if not isinstance(channels, list) or not channels:
        raise RuntimeError("channel operations model must declare channels")
    channel_by_id: dict[str, Mapping[str, Any]] = {}
    for channel in channels:
        if not isinstance(channel, dict):
            raise RuntimeError("channel operations model contained a non-object channel")
        channel_id = channel.get("channel_id")
        if not isinstance(channel_id, str) or not channel_id:
            raise RuntimeError("channel operations model channel missing channel_id")
        channel_by_id[channel_id] = channel

    policy_channel_ids = {
        str(channel.get("channel_id"))
        for channel in update_channel_policy.get("channels", [])
        if isinstance(channel, dict)
    }
    missing_policy_channels = sorted(set(channel_by_id) - policy_channel_ids)
    if missing_policy_channels:
        raise RuntimeError(
            "channel operations model referenced channels outside update policy: "
            + ", ".join(missing_policy_channels)
        )

    required_channels = channel_operations_model.get("required_channels")
    if not isinstance(required_channels, list) or not required_channels:
        raise RuntimeError("channel operations model must declare required_channels")
    missing_required = [
        channel_id
        for channel_id in required_channels
        if not isinstance(channel_id, str) or channel_id not in channel_by_id
    ]
    if missing_required:
        raise RuntimeError(
            "channel operations model is missing required channels: "
            + ", ".join(str(channel_id) for channel_id in missing_required)
        )

    stable = channel_by_id.get("stable")
    nightly = channel_by_id.get("nightly")
    if stable is None or nightly is None:
        raise RuntimeError("stable and nightly channel operations are required")
    stable_gates = stable.get("release_gate_actions")
    nightly_gates = nightly.get("release_gate_actions")
    if not isinstance(stable_gates, list) or "validate-release-candidate-conformance" not in stable_gates:
        raise RuntimeError("stable channel gate must require release candidate conformance")
    if not isinstance(nightly_gates, list) or "test-nightly" not in nightly_gates:
        raise RuntimeError("nightly channel gate must require the nightly workflow")
    if set(stable_gates) == set(nightly_gates):
        raise RuntimeError("stable and nightly release gates must differ")
    for channel in channel_by_id.values():
        validate_package_channel_freshness_policy(channel)

    return channel_by_id


def release_evidence_payload(
    *,
    channel_operations_model: Mapping[str, Any],
    evidence_artifacts: list[str],
    channel_entries: list[Mapping[str, Any]],
) -> dict[str, Any]:
    source_boundaries = validate_release_source_boundaries(channel_operations_model)
    generated_evidence_artifacts = require_generated_evidence_list(
        evidence_artifacts,
        "evidence_artifacts",
        "release_evidence",
    )
    replayable_public_commands = sorted(
        {
            workflow_command(str(action))
            for channel in channel_entries
            for action in channel.get("release_gate_actions", [])
            if isinstance(action, str) and action
        }
    )
    return {
        "release_note_sources": source_boundaries["release_note_sources"],
        "public_changelog_sources": source_boundaries["public_changelog_sources"],
        "evidence_artifacts": generated_evidence_artifacts,
        "replayable_public_commands": replayable_public_commands,
    }


def main() -> int:
    load_json(SOURCE_SURFACE)
    versioning_model = load_json(VERSIONING_MODEL)
    upgrade_surface = load_json(UPGRADE_PATH_SURFACE)
    update_channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    channel_operations_model = load_json(CHANNEL_OPERATIONS_MODEL)
    metadata_surface = load_json(METADATA_SURFACE)

    require_file(PACKAGE_CHANNELS_SUMMARY, "build-package-channels")
    package_channels_summary = load_json(PACKAGE_CHANNELS_SUMMARY)
    package_channels_manifest = ROOT / require_string(
        package_channels_summary,
        "manifest_path",
        "build-package-channels",
    )
    platform_support_matrix = ROOT / require_string(
        package_channels_summary,
        "platform_support_matrix",
        "build-package-channels",
    )
    require_file(RELEASE_MANIFEST, "build-release-manifest")
    require_file(package_channels_manifest, "build-package-channels")
    require_file(platform_support_matrix, "build-platform-support-matrix")
    require_file(PLATFORM_SUPPORT_SUMMARY, "build-platform-support-matrix")

    release_manifest = load_json(RELEASE_MANIFEST)
    package_channels_manifest_payload = load_json(package_channels_manifest)
    platform_support_matrix_payload = load_json(platform_support_matrix)
    archive_digests = package_channels_summary.get("archive_digests")
    if not isinstance(archive_digests, dict) or not archive_digests:
        raise RuntimeError("package channel summary missing archive_digests; run owner action build-package-channels")

    artifacts = {
        "portable_archive": package_channels_summary["portable_archive"],
        "installer_archive": package_channels_summary["installer_archive"],
        "offline_archive": package_channels_summary["offline_archive"],
        "installer_signature": package_channels_summary["installer_signature"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "platform_support_matrix": repo_rel(platform_support_matrix),
    }
    source_policy_paths = [
        repo_rel(VERSIONING_MODEL),
        repo_rel(UPGRADE_PATH_SURFACE),
        repo_rel(UPDATE_CHANNEL_POLICY),
        repo_rel(CHANNEL_OPERATIONS_MODEL),
        repo_rel(METADATA_SURFACE),
    ]
    source_stamps = release_manifest.get("source_stamps")
    if not isinstance(source_stamps, dict):
        raise RuntimeError("release manifest missing source_stamps")
    local_provenance = {
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "release_manifest_sha256": sha256_file(RELEASE_MANIFEST),
        "package_channels_manifest": repo_rel(package_channels_manifest),
        "package_channels_manifest_sha256": sha256_file(package_channels_manifest),
        "release_payload_digest_sha256": release_manifest["release_payload_digest_sha256"],
        "package_model": release_manifest["package_model"],
        "git_commit": require_string(source_stamps, "git_commit", "build-release-manifest"),
        "git_tree_dirty": bool(source_stamps.get("git_tree_dirty")),
        "artifact_digests": archive_digests,
        "installer_signature": package_channels_summary["installer_signature"],
        "platform_support_matrix": repo_rel(platform_support_matrix),
        "platform_support_summary": repo_rel(PLATFORM_SUPPORT_SUMMARY),
        "source_policy_paths": source_policy_paths,
    }

    channel_order = update_channel_policy["channel_order"]
    channel_by_id = {
        entry["channel_id"]: entry
        for entry in update_channel_policy["channels"]
    }
    channel_operations_by_id = validate_channel_operations_model(
        channel_operations_model=channel_operations_model,
        update_channel_policy=update_channel_policy,
    )
    package_channel_freshness = validate_package_channel_freshness(
        package_channels_summary=package_channels_summary,
        package_channels_manifest=package_channels_manifest_payload,
        platform_support_matrix=platform_support_matrix_payload,
        channel_operations_by_id=channel_operations_by_id,
    )
    if sorted(channel_order) != sorted(channel_by_id):
        raise RuntimeError("update channel policy channel_order drifted from channel definitions")
    if update_channel_policy["default_channel"] not in channel_by_id:
        raise RuntimeError("default update channel is not declared in channel policy")

    channels = []
    channel_manifest_entries = []
    for channel_id in channel_order:
        channel_policy = channel_by_id[channel_id]
        channel_operations = channel_operations_by_id.get(channel_id)
        if channel_operations is None:
            raise RuntimeError(f"missing release-channel operations model for {channel_id}")
        version_field = require_string(
            channel_operations,
            "version_field",
            "check-release-operations-surface",
        )
        version = require_string(versioning_model, version_field, "check-release-operations-surface")
        release_gate_actions = channel_operations.get("release_gate_actions")
        rollback_safety = channel_operations.get("rollback_safety")
        release_notes_policy = channel_operations.get("release_notes_policy")
        if not isinstance(release_gate_actions, list) or not release_gate_actions:
            raise RuntimeError(f"{channel_id} channel missing release gate actions")
        if not isinstance(rollback_safety, dict) or not rollback_safety.get("blocks_publication_on_failure"):
            raise RuntimeError(f"{channel_id} channel missing blocking rollback safety")
        if not isinstance(release_notes_policy, dict) or release_notes_policy.get("source_mode") != "source-derived":
            raise RuntimeError(f"{channel_id} channel release notes policy must be source-derived")

        channel_entry = {
            "channel_id": channel_id,
            "version": version,
            "support_status": channel_policy["support_status"],
            "warning_classes": channel_policy["warning_classes"],
            "upgrade_targets": channel_policy["permitted_upgrade_targets"],
            "artifacts": artifacts,
        }
        channels.append(channel_entry)
        channel_manifest_entries.append({
            "channel_id": channel_id,
            "operation_class": channel_operations["operation_class"],
            "version": version,
            "support_status": channel_operations["support_status"],
            "publication_scope": channel_operations["publication_scope"],
            "update_manifest_channel": channel_operations["update_manifest_channel"],
            "release_gate_actions": release_gate_actions,
            "artifact_refs": artifacts,
            "package_channel_freshness": package_channel_freshness,
            "rollback_safety": rollback_safety,
            "release_notes_policy": release_notes_policy,
            "release_evidence": {
                "evidence_artifacts": [
                    repo_rel(RELEASE_MANIFEST),
                    package_channels_summary["manifest_path"],
                    repo_rel(platform_support_matrix),
                    repo_rel(MANIFEST_PATH),
                    repo_rel(CHANNEL_MANIFEST_PATH),
                    repo_rel(UPGRADE_SUPPORT_REPORT),
                ],
                "replayable_public_commands": [
                    workflow_command(str(action))
                    for action in release_gate_actions
                    if isinstance(action, str) and action
                ],
                "local_provenance_required": channel_operations_model["local_provenance_required"],
            },
        })

    evidence_artifacts = [
        repo_rel(RELEASE_MANIFEST),
        repo_rel(PACKAGE_CHANNELS_SUMMARY),
        package_channels_summary["manifest_path"],
        repo_rel(platform_support_matrix),
        repo_rel(PLATFORM_SUPPORT_SUMMARY),
        repo_rel(MANIFEST_PATH),
        repo_rel(CHANNEL_MANIFEST_PATH),
        repo_rel(UPGRADE_SUPPORT_REPORT),
    ]
    release_evidence = release_evidence_payload(
        channel_operations_model=channel_operations_model,
        evidence_artifacts=evidence_artifacts,
        channel_entries=channel_manifest_entries,
    )

    payload = {
        "contract_id": "objc3c.release.operations.update-manifest.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "current_version": versioning_model["current_stable_version"],
        "default_channel": update_channel_policy["default_channel"],
        "default_platform_id": platform_support_matrix_payload["default_platform_id"],
        "supported_major_line": versioning_model["supported_major_line"],
        "package_model": release_manifest["package_model"],
        "platform_support_matrix": repo_rel(platform_support_matrix),
        "platform_support_summary": repo_rel(PLATFORM_SUPPORT_SUMMARY),
        "supported_platform_ids": platform_support_matrix_payload["claim_boundary"]["supported_platform_ids"],
        "support_tiers": platform_support_matrix_payload["tiers"],
        "channels": channels,
        "release_channel_manifest": repo_rel(CHANNEL_MANIFEST_PATH),
        "local_provenance": local_provenance,
        "release_evidence": release_evidence,
        "upgrade_paths": upgrade_surface["upgrade_path_classes"],
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
    }
    for field_name in metadata_surface["required_update_manifest_fields"]:
        if field_name not in payload:
            raise RuntimeError(f"update manifest missing required field {field_name}")

    MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(MANIFEST_PATH, payload)
    channel_manifest = {
        "contract_id": "objc3c.release.operations.channel-manifest.v1",
        "generated_at_utc": payload["generated_at_utc"],
        "source_model": repo_rel(CHANNEL_OPERATIONS_MODEL),
        "default_channel": payload["default_channel"],
        "required_channels": channel_operations_model["required_channels"],
        "local_provenance": local_provenance,
        "channel_manifests": channel_manifest_entries,
        "fail_closed_rules": channel_operations_model["fail_closed_rules"],
        "release_evidence": release_evidence,
    }
    CHANNEL_MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(CHANNEL_MANIFEST_PATH, channel_manifest)

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.update-manifest.summary.v1",
        "status": "PASS",
        "update_manifest_path": repo_rel(MANIFEST_PATH),
        "release_channel_manifest": repo_rel(CHANNEL_MANIFEST_PATH),
        "channel_count": len(channels),
        "default_channel": payload["default_channel"],
        "default_platform_id": payload["default_platform_id"],
        "platform_support_matrix": payload["platform_support_matrix"],
        "platform_support_summary": payload["platform_support_summary"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "upgrade_support_report": payload["upgrade_support_report"],
        "required_channels": channel_operations_model["required_channels"],
        "release_evidence_artifacts": release_evidence["evidence_artifacts"],
        "upstream_artifacts": [
            repo_rel(PACKAGE_CHANNELS_SUMMARY),
            repo_rel(RELEASE_MANIFEST),
            repo_rel(platform_support_matrix),
            repo_rel(PLATFORM_SUPPORT_SUMMARY),
        ],
    }
    write_json_file(REPORT_PATH, summary)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-update-manifest: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
