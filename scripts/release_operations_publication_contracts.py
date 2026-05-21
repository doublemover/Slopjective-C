"""Release-operations publication payload contracts."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any, Mapping


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class ReleaseOperationsPublicationPayloads:
    upgrade_support_report: JsonObject
    channel_catalog: JsonObject
    summary: JsonObject


def _generated_at_utc() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


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
    return "npm run objc3c -- validate-packaging-channels-end-to-end"


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
            }
        )
    return channel_operations


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
) -> ReleaseOperationsPublicationPayloads:
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

    summary = {
        "contract_id": "objc3c.release.operations.publication.summary.v1",
        "status": "PASS",
        "update_manifest": update_manifest_path,
        "release_channel_manifest": release_channel_manifest_path,
        "upgrade_support_report": upgrade_support_report_path,
        "channel_catalog": channel_catalog_path,
        "warning_count": len(warnings),
        "rollback_diagnostic_count": len(rollback_diagnostics),
        "channel_operation_count": len(channel_operations),
        "claim_class_count": len(claim_policy["upgrade_claim_classes"]),
        "platform_support_matrix": update_manifest["platform_support_matrix"],
    }
    return ReleaseOperationsPublicationPayloads(
        upgrade_support_report=upgrade_support_report,
        channel_catalog=channel_catalog,
        summary=summary,
    )


__all__ = [
    "ReleaseOperationsPublicationPayloads",
    "build_release_operations_publication_payloads",
]
