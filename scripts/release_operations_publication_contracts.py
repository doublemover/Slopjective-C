"""Release-operations publication payload contracts."""

from __future__ import annotations

from collections.abc import Mapping
from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any


JsonObject = dict[str, Any]
GENERATED_SOURCE_PREFIXES = ("tmp/", "artifacts/")
GENERATED_EVIDENCE_PREFIXES = ("tmp/artifacts/", "tmp/reports/", "artifacts/")
ROLLBACK_COMMAND = "npm run objc3c -- validate-packaging-channels-end-to-end"


@dataclass(frozen=True)
class ReleaseOperationsPublicationPayloads:
    upgrade_support_report: JsonObject
    channel_catalog: JsonObject
    release_notes: JsonObject
    public_changelog: JsonObject
    summary: JsonObject


def _generated_at_utc() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _validate_source_derived_paths(release_evidence: Mapping[str, Any]) -> None:
    for field_name in ("release_note_sources", "public_changelog_sources"):
        values = release_evidence.get(field_name)
        if not isinstance(values, list) or not values:
            raise RuntimeError(f"release evidence {field_name} must be a non-empty source list")
        for value in values:
            if not isinstance(value, str) or not value:
                raise RuntimeError(f"release evidence {field_name} contains a non-string source")
            normalized = value.replace("\\", "/")
            if normalized.startswith(GENERATED_SOURCE_PREFIXES):
                raise RuntimeError(
                    f"release evidence {field_name} used generated output as source truth: "
                    f"{normalized}"
                )


def _require_generated_evidence_paths(
    release_evidence: Mapping[str, Any],
    *,
    label: str = "release evidence",
) -> None:
    values = release_evidence.get("evidence_artifacts", [])
    if not isinstance(values, list) or not values:
        raise RuntimeError(f"{label} evidence_artifacts must be a non-empty generated evidence list")
    for index, value in enumerate(values):
        if not isinstance(value, str) or not value:
            raise RuntimeError(f"{label} evidence_artifacts[{index}] is not a string")
        normalized = value.replace("\\", "/")
        if not normalized.startswith(GENERATED_EVIDENCE_PREFIXES):
            raise RuntimeError(
                f"{label} evidence_artifacts must stay generated artifact "
                f"references, not source truth: {normalized}"
            )


def _validate_release_channel_publication_contracts(
    *,
    update_manifest: Mapping[str, Any],
    release_channel_manifest: Mapping[str, Any],
    update_channel_policy: Mapping[str, Any],
) -> None:
    release_evidence = release_channel_manifest["release_evidence"]
    _validate_source_derived_paths(release_evidence)
    _require_generated_evidence_paths(release_evidence)

    update_channels = {
        channel["channel_id"]: channel
        for channel in update_manifest["channels"]
        if isinstance(channel, Mapping)
    }
    policy_channels = {
        channel["channel_id"]: channel
        for channel in update_channel_policy["channels"]
        if isinstance(channel, Mapping)
    }
    if update_channel_policy.get("default_channel") != update_manifest.get("default_channel"):
        raise RuntimeError("update manifest default channel drifted from update channel policy")

    channel_manifests = release_channel_manifest["channel_manifests"]
    channel_ids = [
        str(channel.get("channel_id", ""))
        for channel in channel_manifests
        if isinstance(channel, Mapping)
    ]
    if len(channel_ids) != len(set(channel_ids)):
        raise RuntimeError("release channel manifest has duplicate channel ids")
    required_channels = release_channel_manifest.get("required_channels", [])
    missing_required = sorted(
        str(channel_id)
        for channel_id in required_channels
        if not isinstance(channel_id, str) or channel_id not in channel_ids
    )
    if missing_required:
        raise RuntimeError("release channel manifest missing required channels: " + ", ".join(missing_required))

    for channel in channel_manifests:
        if not isinstance(channel, Mapping):
            raise RuntimeError("release channel manifest channel entry must be an object")
        channel_id = str(channel.get("channel_id", ""))
        if channel_id not in update_channels:
            raise RuntimeError(f"{channel_id} channel missing from update manifest")
        if channel_id not in policy_channels:
            raise RuntimeError(f"{channel_id} channel missing from update channel policy")
        if channel.get("update_manifest_channel") != channel_id:
            raise RuntimeError(f"{channel_id} update manifest channel drifted")

        rollback_safety = channel.get("rollback_safety")
        if not isinstance(rollback_safety, Mapping):
            raise RuntimeError(f"{channel_id} channel missing rollback safety")
        policy_rollback = policy_channels[channel_id].get("revert_channel")
        if rollback_safety.get("rollback_channel") != policy_rollback:
            raise RuntimeError(f"{channel_id} rollback channel drifted from update policy")
        if rollback_safety.get("operator_command") != ROLLBACK_COMMAND:
            raise RuntimeError(f"{channel_id} rollback command drifted")
        if rollback_safety.get("blocks_publication_on_failure") is not True:
            raise RuntimeError(f"{channel_id} rollback safety must block publication")

        package_freshness = channel.get("package_channel_freshness")
        if not isinstance(package_freshness, Mapping):
            raise RuntimeError(f"{channel_id} channel missing package freshness policy")
        if package_freshness.get("stale_behavior") != "fail-closed":
            raise RuntimeError(f"{channel_id} package freshness must fail closed")
        if package_freshness.get("blocks_publication_on_stale") is not True:
            raise RuntimeError(f"{channel_id} stale package channel evidence must block publication")

        release_notes_policy = channel.get("release_notes_policy")
        if not isinstance(release_notes_policy, Mapping):
            raise RuntimeError(f"{channel_id} channel release notes policy must be source-derived")
        if release_notes_policy.get("source_mode") != "source-derived":
            raise RuntimeError(f"{channel_id} channel release notes policy must be source-derived")
        required_sources = release_notes_policy.get("required_sources")
        if not isinstance(required_sources, list) or not required_sources:
            raise RuntimeError(f"{channel_id} channel release notes policy missing required sources")
        forbidden_sources = release_notes_policy.get("forbidden_sources")
        if not isinstance(forbidden_sources, list) or not forbidden_sources:
            raise RuntimeError(f"{channel_id} channel release notes policy missing forbidden sources")

        release_gate_actions = channel.get("release_gate_actions")
        if not isinstance(release_gate_actions, list) or not release_gate_actions:
            raise RuntimeError(f"{channel_id} channel missing release gate actions")
        if channel.get("publication_scope") == "public-stable":
            if channel.get("support_status") != "supported":
                raise RuntimeError("public-stable publication requires supported channel status")
            if "validate-release-candidate-conformance" not in release_gate_actions:
                raise RuntimeError("public-stable publication requires release candidate conformance")
        if channel.get("support_status") == "blocked" and not str(channel.get("publication_scope", "")).endswith("-blocked"):
            raise RuntimeError(f"{channel_id} blocked channel requires blocked publication scope")

        if "release_evidence" in channel:
            channel_evidence = channel.get("release_evidence")
            if not isinstance(channel_evidence, Mapping):
                raise RuntimeError(f"{channel_id} channel release evidence must be an object")
            _require_generated_evidence_paths(channel_evidence, label=f"{channel_id} channel release evidence")
            commands = channel_evidence.get("replayable_public_commands", [])
            if not isinstance(commands, list) or not commands:
                raise RuntimeError(f"{channel_id} channel missing replayable public commands")
            expected_commands = {
                f"npm run objc3c -- {action}"
                for action in release_gate_actions
                if isinstance(action, str)
            }
            if not expected_commands.issubset(set(commands)):
                raise RuntimeError(f"{channel_id} channel replay commands drifted from release gates")


def _warning_payloads(
    update_manifest: Mapping[str, Any],
    update_channel_policy: Mapping[str, Any],
    fail_closed_policy: Mapping[str, Any],
) -> list[JsonObject]:
    warning_index = {
        entry["warning_id"]: entry["severity"]
        for entry in update_channel_policy["warning_classes"]
    }
    warnings: list[JsonObject] = []
    for channel in update_manifest["channels"]:
        for warning_id in channel.get("warning_classes", []):
            warnings.append(
                {
                    "channel_id": channel["channel_id"],
                    "warning_id": warning_id,
                    "severity": warning_index.get(warning_id, "warn"),
                    "message": (
                        f"{channel['channel_id']} channel emitted release-operations "
                        f"warning {warning_id}"
                    ),
                }
            )
    for diagnostic in fail_closed_policy["diagnostic_classes"]:
        warnings.append(
            {
                "channel_id": "policy",
                "warning_id": diagnostic["diagnostic_id"],
                "severity": diagnostic["severity"],
                "message": diagnostic["trigger"],
                "required_action": diagnostic["required_action"],
                "blocks_publication": diagnostic["blocks_publication"],
            }
        )
    return warnings


def _rollback_command(revert_channel: str) -> str:
    return ROLLBACK_COMMAND


def _revert_guidance_payloads(update_channel_policy: Mapping[str, Any]) -> list[JsonObject]:
    guidance: list[JsonObject] = []
    for channel in update_channel_policy["channels"]:
        revert_channel = channel["revert_channel"]
        guidance.append(
            {
                "channel_id": channel["channel_id"],
                "instruction": (
                    f"Rollback {channel['channel_id']} through the {revert_channel} "
                    "receipt path before publishing another update channel."
                ),
                "requires_backup": revert_channel == "offline-bundle",
                "rollback_channel": revert_channel,
                "operator_command": _rollback_command(revert_channel),
            }
        )
    return guidance


def _rollback_diagnostic_payloads(
    *,
    update_channel_policy: Mapping[str, Any],
    fail_closed_policy: Mapping[str, Any],
) -> list[JsonObject]:
    default_revert_channel = {
        channel["channel_id"]: channel["revert_channel"]
        for channel in update_channel_policy["channels"]
    }[update_channel_policy["default_channel"]]
    diagnostics: list[JsonObject] = []
    for diagnostic in fail_closed_policy["diagnostic_classes"]:
        diagnostics.append(
            {
                "diagnostic_id": diagnostic["diagnostic_id"],
                "severity": diagnostic["severity"],
                "user_facing_message": (
                    f"{diagnostic['trigger']}; {diagnostic['required_action']}."
                ),
                "rollback_channel": default_revert_channel,
                "operator_command": _rollback_command(default_revert_channel),
                "blocks_publication": diagnostic["blocks_publication"],
            }
        )
    return diagnostics


def _require_upgrade_support_fields(
    upgrade_support_report: Mapping[str, Any],
    metadata_surface: Mapping[str, Any],
) -> None:
    for field_name in metadata_surface["required_upgrade_support_report_fields"]:
        if field_name not in upgrade_support_report:
            raise RuntimeError(
                f"upgrade support report missing required field {field_name}"
            )


def _channel_operation_payloads(
    release_channel_manifest: Mapping[str, Any],
) -> list[JsonObject]:
    channel_operations: list[JsonObject] = []
    for channel in release_channel_manifest["channel_manifests"]:
        rollback_safety = channel["rollback_safety"]
        package_freshness = channel["package_channel_freshness"]
        channel_operations.append(
            {
                "channel_id": channel["channel_id"],
                "operation_class": channel["operation_class"],
                "version": channel["version"],
                "publication_scope": channel["publication_scope"],
                "release_gate_actions": channel["release_gate_actions"],
                "rollback_channel": rollback_safety["rollback_channel"],
                "rollback_command": rollback_safety["operator_command"],
                "blocks_publication_on_rollback_failure": rollback_safety["blocks_publication_on_failure"],
                "package_channel_freshness": package_freshness,
                "blocks_publication_on_stale_package_channel": package_freshness["blocks_publication_on_stale"],
            }
        )
    return channel_operations


def _forbidden_release_note_sources(
    release_channel_manifest: Mapping[str, Any],
) -> list[str]:
    forbidden: set[str] = set()
    for channel in release_channel_manifest["channel_manifests"]:
        policy = channel.get("release_notes_policy", {})
        if not isinstance(policy, Mapping):
            continue
        for source in policy.get("forbidden_sources", []):
            if isinstance(source, str) and source:
                forbidden.add(source)
    return sorted(forbidden)


def _release_note_channel_payloads(
    release_channel_manifest: Mapping[str, Any],
    update_manifest: Mapping[str, Any],
) -> list[JsonObject]:
    update_channels = {
        channel["channel_id"]: channel
        for channel in update_manifest["channels"]
        if isinstance(channel, Mapping)
    }
    notes: list[JsonObject] = []
    for channel in release_channel_manifest["channel_manifests"]:
        channel_id = channel["channel_id"]
        update_channel = update_channels[channel_id]
        notes.append(
            {
                "channel_id": channel_id,
                "version": channel["version"],
                "operation_class": channel["operation_class"],
                "support_status": channel["support_status"],
                "publication_scope": channel["publication_scope"],
                "update_manifest_channel": channel["update_manifest_channel"],
                "release_gate_actions": channel["release_gate_actions"],
                "rollback_channel": channel["rollback_safety"]["rollback_channel"],
                "rollback_command": channel["rollback_safety"]["operator_command"],
                "package_channel_freshness": channel["package_channel_freshness"],
                "artifact_refs": channel["artifact_refs"],
                "warning_classes": update_channel.get("warning_classes", []),
                "upgrade_targets": update_channel.get("upgrade_targets", []),
            }
        )
    return notes


def _build_release_notes_payload(
    *,
    generated_at_utc: str,
    update_manifest: Mapping[str, Any],
    release_channel_manifest: Mapping[str, Any],
    update_manifest_path: str,
    release_channel_manifest_path: str,
) -> JsonObject:
    release_evidence = release_channel_manifest["release_evidence"]
    return {
        "contract_id": "objc3c.release.operations.release-notes.v1",
        "generated_at_utc": generated_at_utc,
        "source_mode": "source-derived",
        "update_manifest": update_manifest_path,
        "release_channel_manifest": release_channel_manifest_path,
        "source_model": release_channel_manifest["source_model"],
        "release_note_sources": release_evidence["release_note_sources"],
        "forbidden_sources": _forbidden_release_note_sources(release_channel_manifest),
        "local_provenance": update_manifest["local_provenance"],
        "channels": _release_note_channel_payloads(
            release_channel_manifest=release_channel_manifest,
            update_manifest=update_manifest,
        ),
    }


def _build_public_changelog_payload(
    *,
    generated_at_utc: str,
    update_manifest: Mapping[str, Any],
    release_channel_manifest: Mapping[str, Any],
    release_notes_path: str,
) -> JsonObject:
    release_evidence = release_channel_manifest["release_evidence"]
    entries: list[JsonObject] = []
    for channel in release_channel_manifest["channel_manifests"]:
        entries.append(
            {
                "channel_id": channel["channel_id"],
                "version": channel["version"],
                "publication_scope": channel["publication_scope"],
                "summary": (
                    f"{channel['channel_id']} {channel['version']} is governed by "
                    f"{len(channel['release_gate_actions'])} release gate actions "
                    f"and rollback channel {channel['rollback_safety']['rollback_channel']}."
                ),
                "gate_actions": channel["release_gate_actions"],
                "rollback_channel": channel["rollback_safety"]["rollback_channel"],
                "package_channel_freshness": channel["package_channel_freshness"],
            }
        )
    return {
        "contract_id": "objc3c.release.operations.public-changelog.v1",
        "generated_at_utc": generated_at_utc,
        "source_mode": "source-derived",
        "current_version": update_manifest["current_version"],
        "default_channel": update_manifest["default_channel"],
        "release_notes": release_notes_path,
        "public_changelog_sources": release_evidence["public_changelog_sources"],
        "entries": entries,
    }


def build_release_operations_publication_payloads(
    *,
    update_manifest: Mapping[str, Any],
    versioning_model: Mapping[str, Any],
    upgrade_surface: Mapping[str, Any],
    claim_policy: Mapping[str, Any],
    update_channel_policy: Mapping[str, Any],
    fail_closed_policy: Mapping[str, Any],
    metadata_surface: Mapping[str, Any],
    update_manifest_path: str,
    release_channel_manifest: Mapping[str, Any],
    release_channel_manifest_path: str,
    upgrade_support_report_path: str,
    channel_catalog_path: str,
    release_notes_path: str,
    public_changelog_path: str,
) -> ReleaseOperationsPublicationPayloads:
    _validate_release_channel_publication_contracts(
        update_manifest=update_manifest,
        release_channel_manifest=release_channel_manifest,
        update_channel_policy=update_channel_policy,
    )
    warnings = _warning_payloads(
        update_manifest=update_manifest,
        update_channel_policy=update_channel_policy,
        fail_closed_policy=fail_closed_policy,
    )
    generated_at_utc = _generated_at_utc()
    rollback_diagnostics = _rollback_diagnostic_payloads(
        update_channel_policy=update_channel_policy,
        fail_closed_policy=fail_closed_policy,
    )
    channel_operations = _channel_operation_payloads(release_channel_manifest)
    upgrade_support_report = {
        "contract_id": "objc3c.release.operations.upgrade-support-report.v1",
        "generated_at_utc": generated_at_utc,
        "current_version": update_manifest["current_version"],
        "default_platform_id": update_manifest["default_platform_id"],
        "platform_support_matrix": update_manifest["platform_support_matrix"],
        "supported_platform_ids": update_manifest["supported_platform_ids"],
        "support_tiers": update_manifest["support_tiers"],
        "support_windows": versioning_model["support_windows"],
        "release_channel_manifest": release_channel_manifest_path,
        "channel_operations": channel_operations,
        "upgrade_paths": upgrade_surface["upgrade_path_classes"],
        "warnings": warnings,
        "revert_guidance": _revert_guidance_payloads(update_channel_policy),
        "rollback_diagnostics": rollback_diagnostics,
        "fail_closed_diagnostics": fail_closed_policy["diagnostic_classes"],
        "forbidden_claims": claim_policy["forbidden_claims"],
    }
    _require_upgrade_support_fields(
        upgrade_support_report=upgrade_support_report,
        metadata_surface=metadata_surface,
    )

    channel_catalog = {
        "contract_id": "objc3c.release.operations.channel-catalog.v1",
        "generated_at_utc": generated_at_utc,
        "default_channel": update_manifest["default_channel"],
        "default_platform_id": update_manifest["default_platform_id"],
        "platform_support_matrix": update_manifest["platform_support_matrix"],
        "supported_platform_ids": update_manifest["supported_platform_ids"],
        "support_tiers": update_manifest["support_tiers"],
        "upgrade_support_report": upgrade_support_report_path,
        "release_channel_manifest": release_channel_manifest_path,
        "local_provenance": update_manifest["local_provenance"],
        "release_evidence": release_channel_manifest["release_evidence"],
        "channels": update_manifest["channels"],
        "channel_operations": channel_operations,
    }
    release_notes = _build_release_notes_payload(
        generated_at_utc=generated_at_utc,
        update_manifest=update_manifest,
        release_channel_manifest=release_channel_manifest,
        update_manifest_path=update_manifest_path,
        release_channel_manifest_path=release_channel_manifest_path,
    )
    public_changelog = _build_public_changelog_payload(
        generated_at_utc=generated_at_utc,
        update_manifest=update_manifest,
        release_channel_manifest=release_channel_manifest,
        release_notes_path=release_notes_path,
    )

    summary = {
        "contract_id": "objc3c.release.operations.publication.summary.v1",
        "status": "PASS",
        "update_manifest": update_manifest_path,
        "release_channel_manifest": release_channel_manifest_path,
        "upgrade_support_report": upgrade_support_report_path,
        "channel_catalog": channel_catalog_path,
        "release_notes": release_notes_path,
        "public_changelog": public_changelog_path,
        "warning_count": len(warnings),
        "rollback_diagnostic_count": len(rollback_diagnostics),
        "channel_operation_count": len(channel_operations),
        "package_channel_freshness_channel_count": sum(
            1
            for channel in channel_operations
            if channel.get("blocks_publication_on_stale_package_channel") is True
        ),
        "release_note_channel_count": len(release_notes["channels"]),
        "public_changelog_entry_count": len(public_changelog["entries"]),
        "claim_class_count": len(claim_policy["upgrade_claim_classes"]),
        "platform_support_matrix": update_manifest["platform_support_matrix"],
    }
    return ReleaseOperationsPublicationPayloads(
        upgrade_support_report=upgrade_support_report,
        channel_catalog=channel_catalog,
        release_notes=release_notes,
        public_changelog=public_changelog,
        summary=summary,
    )


__all__ = [
    "ReleaseOperationsPublicationPayloads",
    "build_release_operations_publication_payloads",
]
