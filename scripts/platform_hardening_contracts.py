#!/usr/bin/env python3
"""Typed platform-hardening contracts shared by the public workflow scripts."""

from __future__ import annotations

import json
import platform
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable, Sequence

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel, resolve_repo_path

ROOT = Path(__file__).resolve().parents[1]
PLATFORM_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "platform_hardening"
PACKAGING_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels"
RELEASE_OPERATIONS_ROOT = ROOT / "tmp" / "artifacts" / "release-operations"
PLATFORM_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "platform-hardening"
PLATFORM_REPORT_ROOT = ROOT / "tmp" / "reports" / "platform-hardening"

BOUNDARY_INVENTORY_PATH = PLATFORM_FIXTURE_ROOT / "boundary_inventory.json"
SUPPORT_TIER_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "platform_support_tier_policy.json"
UNSUPPORTED_HOST_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "unsupported_host_fail_closed_policy.json"
TOOLCHAIN_ARCHIVE_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "toolchain_archive_claim_policy.json"
PLATFORM_MATRIX_ARTIFACT_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "platform_matrix_artifact_contract.json"
BUILD_PACKAGE_VALIDATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "build_package_validation_contract.json"
TOOLCHAIN_RANGE_REPLAY_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "toolchain_range_replay_contract.json"
INSTALL_MATRIX_INTEGRATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "install_matrix_integration_contract.json"
PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "packaged_smoke_integration_contract.json"
SUPPORTED_PLATFORMS_PATH = PACKAGING_FIXTURE_ROOT / "supported_platforms.json"

PLATFORM_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_platform_hardening.md"
PACKAGING_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_packaging_channels.md"
RELEASE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_release_operations.md"
PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH = ROOT / "schemas" / "objc3c-platform-support-matrix-v1.schema.json"

SUPPORT_MATRIX_ARTIFACT_PATH = PLATFORM_ARTIFACT_ROOT / "objc3c-platform-support-matrix.json"
SUPPORT_MATRIX_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "platform-support-matrix-summary.json"
BOUNDARY_INVENTORY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "boundary-inventory" / "boundary_inventory_summary.json"
SUPPORT_TIER_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "support-tier-policy" / "support_tier_policy_summary.json"
UNSUPPORTED_HOST_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "unsupported-host-policy" / "unsupported_host_policy_summary.json"
TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "toolchain-archive-policy" / "toolchain_archive_policy_summary.json"
ARTIFACT_CONTRACT_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "artifact-contract" / "artifact_contract_summary.json"
BUILD_PACKAGE_VALIDATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "build-package-validation-summary.json"
TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "toolchain-range-replay-summary.json"
INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "install-matrix-integration-summary.json"
PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "integration-summary.json"
RUNNABLE_END_TO_END_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "runnable-end-to-end-summary.json"

PACKAGE_CHANNELS_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "end-to-end-summary.json"
RELEASE_PUBLICATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"
UPDATE_MANIFEST_PATH = RELEASE_OPERATIONS_ROOT / "update-manifest" / "objc3c-update-manifest.json"
UPGRADE_SUPPORT_REPORT_PATH = RELEASE_OPERATIONS_ROOT / "publication" / "objc3c-upgrade-support-report.json"
CHANNEL_CATALOG_PATH = RELEASE_OPERATIONS_ROOT / "publication" / "objc3c-release-channel-catalog.json"
PACKAGE_MANIFEST_PATH = ROOT / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"

BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
BUILD_PACKAGE_VALIDATION_SCRIPT = ROOT / "scripts" / "check_platform_hardening_build_package_validation.py"
TOOLCHAIN_RANGE_REPLAY_SCRIPT = ROOT / "scripts" / "check_platform_hardening_toolchain_range_replay.py"
INSTALL_MATRIX_INTEGRATION_SCRIPT = ROOT / "scripts" / "check_platform_hardening_install_matrix_integration.py"

PUBLICATION_SURFACE: dict[str, str] = {
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

PLATFORM_HARDENING_OWNER_POLICY: dict[str, object] = {
    "channel_owner": "packaging-channels-source",
    "platform_support_owner": "platform-hardening-support-source",
    "installer_validation_owner": "platform-hardening-install-validation",
    "build_package_validation_owner": "platform-hardening-build-package-validation",
    "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
    "unsupported_host_failure_owner": "platform-hardening-unsupported-host-fail-closed",
    "blocker_owner": "platform-hardening-blockers",
    "source_authority": "checked-in-platform-hardening-contracts",
    "evidence_log_allowed": False,
}

PLATFORM_HARDENING_OWNER_FIELDS: tuple[str, ...] = tuple(PLATFORM_HARDENING_OWNER_POLICY)

PLATFORM_HARDENING_SUMMARY_BUILDERS: tuple[Path, ...] = (
    ROOT / "scripts" / "build_platform_hardening_boundary_inventory_summary.py",
    ROOT / "scripts" / "build_platform_hardening_support_tier_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_unsupported_host_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_toolchain_archive_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_artifact_contract_summary.py",
)

PLATFORM_HARDENING_SUMMARY_PATHS: tuple[Path, ...] = (
    BOUNDARY_INVENTORY_SUMMARY_PATH,
    SUPPORT_TIER_POLICY_SUMMARY_PATH,
    UNSUPPORTED_HOST_POLICY_SUMMARY_PATH,
    TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH,
    ARTIFACT_CONTRACT_SUMMARY_PATH,
)

SUPPORTED_HOST_ARCH_ALIASES: dict[str, set[str]] = {
    "windows-x64": {"amd64", "x86_64"},
}


@dataclass(frozen=True)
class ToolProbe:
    command: tuple[str, ...]
    available: bool
    exit_code: int
    headline: str

    def as_json(self) -> dict[str, Any]:
        return {
            "command": list(self.command),
            "available": self.available,
            "exit_code": self.exit_code,
            "headline": self.headline,
        }


@dataclass(frozen=True)
class HostSnapshot:
    os: str
    arch: str
    system: str
    machine: str

    def as_json(self) -> dict[str, str]:
        return {"os": self.os, "arch": self.arch}


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"{repo_rel(path)} did not contain a JSON object")
    return payload


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(path, payload)


def write_markdown_summary(path: Path, title: str, rows: Sequence[tuple[str, Any]]) -> None:
    body = f"# {title}\n\n" + "".join(f"- {label}: `{value}`\n" for label, value in rows)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(body, encoding="utf-8")


def current_host() -> HostSnapshot:
    return HostSnapshot(
        os=platform.platform(),
        arch=platform.machine() or "",
        system=platform.system().lower(),
        machine=(platform.machine() or "").lower(),
    )


def run_probe(command: Sequence[str]) -> ToolProbe:
    result = subprocess.run(list(command), cwd=ROOT, text=True, capture_output=True, check=False)
    headline = ""
    for stream in (result.stdout, result.stderr):
        for line in stream.splitlines():
            stripped = line.strip()
            if stripped:
                headline = stripped
                break
        if headline:
            break
    return ToolProbe(tuple(command), result.returncode == 0, result.returncode, headline)


def missing_tool_probe(tool_name: str, command_name: str | None = None) -> ToolProbe:
    display_name = command_name or tool_name
    return ToolProbe((display_name, "--version"), False, 127, f"{display_name} not found")


def required_tool_probes() -> dict[str, dict[str, Any]]:
    pwsh_path = shutil.which("pwsh")
    clang_path = shutil.which("clang++") or shutil.which("clang")
    probes = {
        "python": ToolProbe((sys.executable, "--version"), True, 0, sys.version.splitlines()[0]),
        "pwsh": run_probe((pwsh_path, "--version")) if pwsh_path else missing_tool_probe("pwsh"),
        "clang": run_probe((clang_path, "--version")) if clang_path else missing_tool_probe("clang", "clang++"),
    }
    return {name: probe.as_json() for name, probe in probes.items()}


def host_matches_supported_platform(default_platform_id: str, host: HostSnapshot | None = None) -> bool:
    snapshot = host or current_host()
    if default_platform_id != "windows-x64":
        return False
    return snapshot.system == "windows" and snapshot.machine in SUPPORTED_HOST_ARCH_ALIASES["windows-x64"]


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def build_support_matrix_payload() -> dict[str, Any]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    tier_policy = load_json_object(SUPPORT_TIER_POLICY_PATH)
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    host = current_host()
    payload = {
        "contract_id": "objc3c.platform.hardening.support.matrix.v1",
        "schema_version": 1,
        "generated_at_utc": utc_now(),
        "default_platform_id": supported_platforms["default_platform_id"],
        "platform_count": len(supported_platforms["supported_platforms"]),
        "platforms": supported_platforms["supported_platforms"],
        "channels": boundary["supported_channels"],
        "tiers": tier_policy["tiers"],
        "current_host": host.as_json(),
        "required_tool_probes": required_tool_probes(),
        "claim_boundary": {
            "supported_platform_ids": boundary["supported_platform_ids"],
            "gap_claims": boundary["gap_claims"],
            "forbidden_claims": tier_policy["forbidden_claims"],
        },
        "publication_surface": PUBLICATION_SURFACE,
    }
    return payload


def write_support_matrix(payload: dict[str, Any]) -> dict[str, Any]:
    write_json(SUPPORT_MATRIX_ARTIFACT_PATH, payload)
    summary = {
        "contract_id": "objc3c.platform.hardening.support.matrix.summary.v1",
        "status": "PASS",
        "artifact_path": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "default_platform_id": payload["default_platform_id"],
        "platform_count": payload["platform_count"],
        "channel_count": len(payload["channels"]),
        "tier_count": len(payload["tiers"]),
    }
    write_json(SUPPORT_MATRIX_SUMMARY_PATH, summary)
    return summary


def require_required_fields(payload: dict[str, Any], field_names: Iterable[str], surface_name: str) -> None:
    for field_name in field_names:
        expect(field_name in payload, f"{surface_name} missing required field {field_name}")


def platform_hardening_owner_payload() -> dict[str, object]:
    return dict(PLATFORM_HARDENING_OWNER_POLICY)


def require_platform_hardening_owner_policy(payload: dict[str, Any], *, surface_name: str) -> dict[str, Any]:
    owner_policy = payload.get("owner_policy")
    expect(isinstance(owner_policy, dict), f"{surface_name} missing owner_policy")
    missing_fields = [field for field in PLATFORM_HARDENING_OWNER_FIELDS if field not in owner_policy]
    expect(not missing_fields, f"{surface_name} owner_policy missing fields: {', '.join(missing_fields)}")
    expect(owner_policy.get("evidence_log_allowed") is False, f"{surface_name} owner_policy must forbid evidence-log publication")
    for field_name, expected_value in PLATFORM_HARDENING_OWNER_POLICY.items():
        expect(owner_policy.get(field_name) == expected_value, f"{surface_name} owner_policy drifted for {field_name}")
    return owner_policy


def require_platform_hardening_blocker_metadata(
    payload: dict[str, Any],
    *,
    surface_name: str,
    required_blockers: Iterable[str] = (),
) -> dict[str, Any]:
    blocker_metadata = payload.get("blocker_metadata")
    expect(isinstance(blocker_metadata, dict), f"{surface_name} missing blocker_metadata")
    expect(
        blocker_metadata.get("blocker_owner") == PLATFORM_HARDENING_OWNER_POLICY["blocker_owner"],
        f"{surface_name} blocker owner drifted",
    )
    blocking_conditions = blocker_metadata.get("blocking_conditions")
    expect(isinstance(blocking_conditions, list) and len(blocking_conditions) > 0, f"{surface_name} missing blocking_conditions")
    missing_blockers = [blocker for blocker in required_blockers if blocker not in blocking_conditions]
    expect(not missing_blockers, f"{surface_name} blocker_metadata missing blockers: {', '.join(missing_blockers)}")
    return blocker_metadata


def require_paths_exist(paths: Iterable[Path], *, description: str) -> None:
    for path in paths:
        expect(path.is_file(), f"missing expected {description}: {repo_rel(path)}")


def policy_path_entries() -> list[Path]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    return [resolve_repo_path(str(path)) for path in boundary["policy_contract_paths"]]
