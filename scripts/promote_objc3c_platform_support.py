#!/usr/bin/env python3
"""Promote reviewed Linux/macOS host evidence into platform source truth.

This helper is intentionally source-owned. It refuses generated-only hosted
reports and only runs from a reviewed host-promotion input fixture where the
target platform row is already approved for support source truth.
"""

from __future__ import annotations

import argparse
import json
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]

REVIEWED_SOURCE_INPUTS_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "host_promotion_reviewed_source_inputs.json"
)
PLATFORM_SUPPORT_SOURCE_TRUTH_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_support"
    / "source_truth_matrix.json"
)
PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "platform_toolchain_support_evidence.json"
)
BOUNDARY_INVENTORY_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "platform_hardening" / "boundary_inventory.json"
)
SUPPORTED_PLATFORMS_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "supported_platforms.json"
)
SUPPORT_TIER_POLICY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "platform_support_tier_policy.json"
)
DEFAULT_SUMMARY_PATH = (
    ROOT / "tmp" / "reports" / "platform-hardening" / "platform-support-promotion-summary.json"
)

REQUIRED_EVIDENCE_CLASSES = ("build", "package", "install", "execution")
REQUIRED_TOOLCHAIN_COMPONENTS = ("llvm", "clang", "cmake", "ninja", "python", "node", "pwsh")
REQUIRED_RECORD_TYPES = (
    "host_identity",
    "toolchain_probe",
    "package_root",
    "install_receipt",
    "native_execution",
    "object_identity",
    "debug_identity",
    "package_install_identity",
    "runtime_load_link_proof",
)
RECORD_SECTION_BY_TYPE = {
    "host_identity": "host_identity_records",
    "toolchain_probe": "toolchain_probe_records",
    "package_root": "package_root_evidence_records",
    "install_receipt": "install_receipt_records",
    "native_execution": "native_execution_evidence_records",
    "object_identity": "object_identity_records",
    "debug_identity": "debug_identity_records",
    "package_install_identity": "package_install_identity_records",
    "runtime_load_link_proof": "runtime_load_link_proof_records",
}
RECORD_ID_FIELD_BY_TYPE = {
    "host_identity": "host_identity_record_id",
    "toolchain_probe": "toolchain_probe_record_id",
    "package_root": "package_root_record_id",
    "install_receipt": "install_receipt_record_id",
    "native_execution": "native_execution_record_id",
    "object_identity": "object_identity_record_id",
    "debug_identity": "debug_identity_record_id",
    "package_install_identity": "package_install_identity_record_id",
    "runtime_load_link_proof": "runtime_load_link_proof_record_id",
}

PLATFORM_PROFILES = {
    "linux-x64": {
        "issue_ref": 8228,
        "host_os": "linux",
        "host_arch": "x64",
        "host_triples": ["x86_64-unknown-linux-gnu"],
        "supported_row_id": "objc3c.platform.linux-x64.tier2",
        "public_capability_id": "platform.linux-x64.tier2",
        "unsupported_row_id": "objc3c.platform.linux-x64.unsupported",
        "unsupported_capability_id": "platform.linux-x64.unsupported",
        "package_row_id": "objc3c.package.runtime.linux-x64.release",
        "fail_closed_package_row_id": "objc3c.package.runtime.linux-x64.release.fail-closed",
        "package_id": "org.objc3c.runtime:objc3c-runtime-linux-x64-release",
        "package_channel_id": "linux-x64-release",
        "runtime_library_names": ["libobjc3-runtime.so"],
        "object_format": "ELF",
        "debug_format": "DWARF",
        "loader_path_policy": (
            "ELF rpath, RUNPATH, or package-root loader resolution is proven by "
            "reviewed Linux package install and runtime load/link evidence"
        ),
        "diagnostic": (
            "Linux x64 is Tier 2 supported through reviewed Linux host build, "
            "package, install, runtime load/link, and native execution evidence."
        ),
    },
    "darwin-arm64": {
        "issue_ref": 8229,
        "host_os": "darwin",
        "host_arch": "arm64",
        "host_triples": ["aarch64-apple-darwin"],
        "supported_row_id": "objc3c.platform.darwin-arm64.tier2",
        "public_capability_id": "platform.darwin-arm64.tier2",
        "unsupported_row_id": "objc3c.platform.darwin-arm64.unsupported",
        "unsupported_capability_id": "platform.darwin-arm64.unsupported",
        "package_row_id": "objc3c.package.runtime.darwin-arm64.release",
        "fail_closed_package_row_id": "objc3c.package.runtime.darwin-arm64.release.fail-closed",
        "package_id": "org.objc3c.runtime:objc3c-runtime-darwin-arm64-release",
        "package_channel_id": "darwin-arm64-release",
        "runtime_library_names": ["libobjc3-runtime.dylib"],
        "object_format": "Mach-O",
        "debug_format": "DWARF/dSYM",
        "loader_path_policy": (
            "@rpath, install_name, codesign, and package-root loader behavior are "
            "proven by reviewed macOS package install and runtime load/link evidence"
        ),
        "diagnostic": (
            "macOS arm64 is Tier 2 supported through reviewed macOS host build, "
            "package, install, runtime load/link, Mach-O/debug identity, and native execution evidence."
        ),
    },
}


class PromotionError(RuntimeError):
    """Raised when platform promotion would weaken source-truth guarantees."""


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise PromotionError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=False) + "\n", encoding="utf-8")


def repo_rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def require_platform(platform_id: str) -> dict[str, Any]:
    try:
        return PLATFORM_PROFILES[platform_id]
    except KeyError as exc:
        supported = ", ".join(sorted(PLATFORM_PROFILES))
        raise PromotionError(f"unsupported platform {platform_id}; expected one of {supported}") from exc


def rows_by_field(rows: list[Any], field_name: str) -> dict[str, dict[str, Any]]:
    by_id: dict[str, dict[str, Any]] = {}
    for row in rows:
        if isinstance(row, dict) and row.get(field_name):
            by_id[str(row[field_name])] = row
    return by_id


def remove_row(rows: list[Any], field_name: str, value: str) -> dict[str, Any] | None:
    for index, row in enumerate(rows):
        if isinstance(row, dict) and row.get(field_name) == value:
            return rows.pop(index)
    return None


def upsert_row(rows: list[Any], field_name: str, row: dict[str, Any]) -> None:
    key = str(row[field_name])
    for index, existing in enumerate(rows):
        if isinstance(existing, dict) and existing.get(field_name) == key:
            rows[index] = row
            return
    rows.append(row)


def append_unique(items: list[Any], value: str) -> None:
    if value not in items:
        items.append(value)


def remove_value(items: list[Any], value: str) -> None:
    while value in items:
        items.remove(value)


def reviewed_platform_row(payload: dict[str, Any], platform_id: str) -> dict[str, Any]:
    for row in payload.get("platforms", []):
        if isinstance(row, dict) and row.get("platform_id") == platform_id:
            if row.get("promotion_allowed") is not True or row.get("support_truth") is not True:
                raise PromotionError(f"{platform_id} reviewed source row is not promotion-ready")
            if row.get("remaining_blockers"):
                raise PromotionError(f"{platform_id} reviewed source row still has blockers")
            if row.get("not_promotion_ready_record_types"):
                raise PromotionError(f"{platform_id} reviewed source row still has unready record types")
            return row
    raise PromotionError(f"reviewed source inputs missing platform {platform_id}")


def reviewed_records(payload: dict[str, Any], platform_row: dict[str, Any]) -> dict[str, dict[str, Any]]:
    required_ids = platform_row.get("required_record_ids")
    if not isinstance(required_ids, dict):
        raise PromotionError("reviewed platform row missing required_record_ids")
    records: dict[str, dict[str, Any]] = {}
    for record_type in REQUIRED_RECORD_TYPES:
        section = RECORD_SECTION_BY_TYPE[record_type]
        id_field = RECORD_ID_FIELD_BY_TYPE[record_type]
        record_id = str(required_ids.get(id_field, ""))
        if not record_id:
            raise PromotionError(f"reviewed platform row missing {id_field}")
        section_rows = rows_by_field(payload.get(section, []), "record_id")
        record = section_rows.get(record_id)
        if record is None:
            raise PromotionError(f"reviewed source inputs missing {record_type} record {record_id}")
        if record.get("promotion_allowed") is not True:
            raise PromotionError(f"{record_id} is not promotion_allowed")
        if record.get("claim_state") != "reviewed-source":
            raise PromotionError(f"{record_id} is not a reviewed-source record")
        if record.get("generated_report_support_truth") is True:
            raise PromotionError(f"{record_id} treated generated reports as support truth")
        records[record_type] = record
    return records


def reviewed_artifact_paths(records: dict[str, dict[str, Any]]) -> list[str]:
    paths: list[str] = []
    for record in records.values():
        for field_name in ("hosted_runner_artifact_paths", "package_artifact_paths", "toolchain_artifact_paths"):
            values = record.get(field_name, [])
            if isinstance(values, list):
                for value in values:
                    text = str(value).replace("\\", "/")
                    if text and text not in paths:
                        paths.append(text)
    return paths


def evidence_id(platform_id: str, evidence_class: str) -> str:
    return f"objc3c.evidence.reviewed-source.{platform_id}.{evidence_class}"


def toolchain_evidence_id(platform_id: str, group: str) -> str:
    return f"objc3c.evidence.reviewed-source.{platform_id}.toolchain.{group}"


def evidence_record(
    *,
    platform_id: str,
    issue_ref: int,
    evidence_class: str,
    source_paths: list[str],
    replay_commands: list[str],
    generated_report_paths: list[str],
) -> dict[str, Any]:
    return {
        "evidence_id": evidence_id(platform_id, evidence_class),
        "issue_refs": [8206, issue_ref],
        "evidence_class": evidence_class,
        "claim_weight": "supporting",
        "source_paths": source_paths,
        "replay_commands": replay_commands,
        "generated_report_paths": generated_report_paths,
        "supports_platform_ids": [platform_id],
        "requires_network": False,
        "unsupported_host_behavior": "fail-closed",
    }


def toolchain_evidence_record(
    *,
    platform_id: str,
    issue_ref: int,
    group: str,
    source_paths: list[str],
    replay_commands: list[str],
    generated_report_paths: list[str],
) -> dict[str, Any]:
    record = evidence_record(
        platform_id=platform_id,
        issue_ref=issue_ref,
        evidence_class="toolchain",
        source_paths=source_paths,
        replay_commands=replay_commands,
        generated_report_paths=generated_report_paths,
    )
    record["evidence_id"] = toolchain_evidence_id(platform_id, group)
    record["issue_refs"] = [8206, issue_ref, 8232]
    return record


def promotion_source_paths() -> list[str]:
    return [
        "scripts/promote_objc3c_platform_support.py",
        "tests/tooling/fixtures/platform_hardening/host_promotion_reviewed_source_inputs.json",
        "tests/tooling/fixtures/platform_hardening/platform_host_promotion_evidence_contract.json",
        "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
        "tests/tooling/fixtures/platform_support/source_truth_matrix.json",
    ]


def build_evidence_records(
    platform_id: str,
    profile: dict[str, Any],
    records: dict[str, dict[str, Any]],
) -> list[dict[str, Any]]:
    issue_ref = int(profile["issue_ref"])
    source_paths = promotion_source_paths()
    artifact_paths = reviewed_artifact_paths(records)
    return [
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="build",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- build-native-binaries"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/build/native_build_summary.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/build/object-identity.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/build/debug-identity.json",
            ],
        ),
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="package",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- package-runnable-toolchain"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/package/objc3c-runnable-toolchain-package.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/package/runtime-library-manifest.json",
            ],
        ),
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="install",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- validate-packaging-channels-end-to-end"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/install/end-to-end-summary.json",
            ],
        ),
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="execution",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- test-execution-smoke"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/execution/runtime-load-probe.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/execution/native-execution-smoke-summary.json",
            ],
        ),
        toolchain_evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            group="llvm",
            source_paths=source_paths,
            replay_commands=[
                f"npm run objc3c -- review-platform-host-evidence -- --platform-id {platform_id}"
            ],
            generated_report_paths=[f"tmp/reports/platform-host-evidence/{platform_id}/llvm-capabilities.json"],
        ),
        toolchain_evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            group="native-build-resolution",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- build-native-binaries"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/build/native_build_summary.json"
            ],
        ),
        toolchain_evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            group="package-bridge",
            source_paths=source_paths,
            replay_commands=["npm run objc3c -- build-platform-support-matrix"],
            generated_report_paths=artifact_paths
            or [f"tmp/reports/platform-host-evidence/{platform_id}/host-evidence-report.json"],
        ),
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="hosted_ci",
            source_paths=source_paths + [".github/workflows/conformance-minima.yml"],
            replay_commands=["npm run objc3c -- test-hosted-execution-smoke"],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/host-evidence-report.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/execution/hosted-execution-smoke-summary.json",
            ],
        ),
        evidence_record(
            platform_id=platform_id,
            issue_ref=issue_ref,
            evidence_class="clean_room",
            source_paths=source_paths,
            replay_commands=[
                "npm run objc3c -- validate-package-install-distribution --from-nothing"
            ],
            generated_report_paths=[
                f"tmp/reports/platform-host-evidence/{platform_id}/install/install-distribution-verification.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/install/clean-install-distribution-receipt.json",
            ],
        ),
    ]


def support_row(
    *,
    platform_id: str,
    profile: dict[str, Any],
    records: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    return {
        "row_id": profile["supported_row_id"],
        "platform_id": platform_id,
        "host_os": profile["host_os"],
        "host_arch": profile["host_arch"],
        "host_triples": profile["host_triples"],
        "support_state": "supported",
        "claim_class": "supported-but-not-default",
        "tier_id": "tier-2",
        "required_evidence_classes": list(REQUIRED_EVIDENCE_CLASSES),
        "required_missing_evidence_classes": [],
        "required_toolchain_components": list(REQUIRED_TOOLCHAIN_COMPONENTS),
        "evidence": {
            evidence_class: evidence_id(platform_id, evidence_class)
            for evidence_class in REQUIRED_EVIDENCE_CLASSES
        },
        "issue_ref": profile["issue_ref"],
        "package_variant_row_ids": [profile["package_row_id"]],
        "negative_contracts": [],
        "toolchain_evidence_ids": [
            toolchain_evidence_id(platform_id, "llvm"),
            toolchain_evidence_id(platform_id, "native-build-resolution"),
            toolchain_evidence_id(platform_id, "package-bridge"),
        ],
        "hosted_ci_evidence_ids": [evidence_id(platform_id, "hosted_ci")],
        "local_clean_room_evidence_ids": [evidence_id(platform_id, "clean_room")],
        "reviewed_source_record_ids": {
            record_type: str(record["record_id"])
            for record_type, record in records.items()
        },
        "diagnostic": profile["diagnostic"],
    }


def source_truth_support_row(
    *,
    platform_id: str,
    profile: dict[str, Any],
    support: dict[str, Any],
) -> dict[str, Any]:
    row = {
        key: deepcopy(value)
        for key, value in support.items()
        if key
        not in {
            "support_state",
            "claim_class",
            "tier_id",
            "negative_contracts",
            "reviewed_source_record_ids",
        }
    }
    row["public_replay_commands"] = [
        "npm run objc3c -- build-native-binaries",
        "npm run objc3c -- package-runnable-toolchain",
        "npm run objc3c -- validate-packaging-channels-end-to-end",
        "npm run objc3c -- test-execution-smoke",
        "npm run objc3c -- test-hosted-execution-smoke",
        "npm run objc3c -- validate-package-install-distribution --from-nothing",
        "npm run objc3c -- build-platform-support-matrix",
        f"npm run objc3c -- review-platform-host-evidence -- --platform-id {platform_id}",
        f"npm run objc3c -- review-platform-support-promotion -- --platform-id {platform_id} --apply",
    ]
    row["host_promotion_constraints"] = {
        "promotion_policy": "reviewed-source-host-execution-required",
        "real_host_execution_required": True,
        "generated_host_evidence_support_truth": False,
        "reviewed_source_truth_required": True,
        "hosted_runner_summary_behavior": "summary-only-no-support-promotion",
    }
    row["diagnostic"] = profile["diagnostic"]
    return row


def package_variant_row(platform_id: str, profile: dict[str, Any], old_row: dict[str, Any]) -> dict[str, Any]:
    row = deepcopy(old_row)
    row["row_id"] = profile["package_row_id"]
    row["claim_state"] = "evidence-bound"
    row["platform_ids"] = [platform_id]
    row["channel_scope"] = ["portable-archive", "local-installer", "offline-bundle"]
    row["required_missing_evidence_classes"] = []
    row["evidence_ids"] = [evidence_id(platform_id, item) for item in REQUIRED_EVIDENCE_CLASSES]
    row["negative_contracts"] = []
    row["diagnostic"] = (
        f"{platform_id} release runtime packaging is evidence-bound through reviewed "
        "build, package, install, runtime load/link, and native execution source truth."
    )
    metadata = row.setdefault("metadata_freshness_guard", {})
    metadata["metadata_source"] = (
        "tests/tooling/fixtures/platform_hardening/"
        f"platform_toolchain_support_evidence.json#{profile['package_row_id']}"
    )
    gate = row.setdefault("promotion_gate_contract", {})
    gate["blocked_until_evidence_classes"] = []
    gate["blocked_publication_surfaces"] = ["publication"]
    identity = row.setdefault("artifact_identity_contract", {})
    identity["loader_path_policy"] = profile["loader_path_policy"]
    return row


def update_boundary(boundary: dict[str, Any], platform_id: str) -> None:
    append_unique(boundary.setdefault("supported_platform_ids", []), platform_id)


def update_supported_platforms(payload: dict[str, Any], platform_id: str, profile: dict[str, Any]) -> None:
    unsupported = remove_row(payload.setdefault("unsupported_platforms", []), "platform_id", platform_id)
    supported_row = {
        "platform_id": platform_id,
        "host_os": profile["host_os"],
        "host_arch": profile["host_arch"],
        "supported_channels": ["portable-archive", "local-installer", "offline-bundle"],
        "required_tools": ["pwsh", "python", "node", "clang++", "cmake", "ninja", "llc"],
        "support_tier": "tier-2",
        "unsupported_claims": [
            "system package manager publication",
            "signed or notarized installer publication",
            "sanitizer runtime package parity",
        ],
    }
    upsert_row(payload.setdefault("supported_platforms", []), "platform_id", supported_row)
    for row in payload.setdefault("package_runtime_models", []):
        if isinstance(row, dict) and row.get("platform_id") == platform_id:
            row["support_state"] = "supported"
            row["claim_state"] = "evidence-bound"
            row["package_variant_row_id"] = profile["package_row_id"]
            row["required_missing_evidence_classes"] = []
            row["support_truth"] = True
            row["native_execution_claimed"] = True
            row["promotion_allowed"] = True
    if unsupported:
        unsupported["support_state"] = "supported"


def update_tier_policy(payload: dict[str, Any], platform_id: str) -> None:
    for tier in payload.get("tiers", []):
        if not isinstance(tier, dict):
            continue
        platform_ids = tier.setdefault("platform_ids", [])
        remove_value(platform_ids, platform_id)
        if tier.get("tier_id") == "tier-2":
            append_unique(platform_ids, platform_id)


def update_toolchain_ranges(payload: dict[str, Any], platform_id: str) -> None:
    evidence_by_component = {
        "llvm": toolchain_evidence_id(platform_id, "llvm"),
        "clang": toolchain_evidence_id(platform_id, "native-build-resolution"),
        "cmake": toolchain_evidence_id(platform_id, "native-build-resolution"),
        "ninja": toolchain_evidence_id(platform_id, "native-build-resolution"),
        "python": toolchain_evidence_id(platform_id, "package-bridge"),
        "node": toolchain_evidence_id(platform_id, "package-bridge"),
        "pwsh": toolchain_evidence_id(platform_id, "package-bridge"),
    }
    for row in payload.get("toolchain_ranges", []):
        if not isinstance(row, dict):
            continue
        component = str(row.get("component", ""))
        evidence = evidence_by_component.get(component)
        if evidence:
            append_unique(row.setdefault("platform_ids", []), platform_id)
            append_unique(row.setdefault("evidence_ids", []), evidence)


def update_llvm_matrix(payload: dict[str, Any], platform_id: str) -> None:
    matrix = payload.get("llvm_version_support_matrix", {})
    if not isinstance(matrix, dict):
        return
    entries = matrix.setdefault("matrix_entries", [])
    entry_id = f"objc3c.llvm.current-probed-executable.{platform_id}"
    if any(isinstance(row, dict) and row.get("entry_id") == entry_id for row in entries):
        return
    entries.append(
        {
            "entry_id": entry_id,
            "platform_id": platform_id,
            "claim_state": "evidence-bound",
            "llvm_version_claim": "current-probed-executable-only",
            "evidence_ids": [toolchain_evidence_id(platform_id, "llvm")],
            "unsupported_version_behavior": "fail-closed-no-range-claim",
        }
    )


def update_platform_evidence(
    payload: dict[str, Any],
    platform_id: str,
    profile: dict[str, Any],
    records: dict[str, dict[str, Any]],
) -> None:
    remove_row(payload.setdefault("support_rows", []), "platform_id", platform_id)
    support = support_row(platform_id=platform_id, profile=profile, records=records)
    upsert_row(payload["support_rows"], "platform_id", support)

    old_package = remove_row(
        payload.setdefault("package_variant_rows", []),
        "row_id",
        str(profile["fail_closed_package_row_id"]),
    )
    if old_package is None:
        old_package = rows_by_field(payload["package_variant_rows"], "row_id").get(str(profile["package_row_id"]))
    if old_package is None:
        raise PromotionError(f"missing package row {profile['fail_closed_package_row_id']}")
    upsert_row(
        payload["package_variant_rows"],
        "row_id",
        package_variant_row(platform_id, profile, old_package),
    )

    for record in build_evidence_records(platform_id, profile, records):
        upsert_row(payload.setdefault("evidence_records", []), "evidence_id", record)
    update_toolchain_ranges(payload, platform_id)
    update_llvm_matrix(payload, platform_id)


def update_source_truth(
    payload: dict[str, Any],
    platform_id: str,
    profile: dict[str, Any],
    platform_evidence: dict[str, Any],
) -> None:
    remove_row(payload.setdefault("unsupported_rows", []), "platform_id", platform_id)
    support = rows_by_field(platform_evidence["support_rows"], "platform_id")[platform_id]
    upsert_row(
        payload.setdefault("supported_rows", []),
        "platform_id",
        source_truth_support_row(platform_id=platform_id, profile=profile, support=support),
    )

    remove_row(
        payload.setdefault("package_variant_rows", []),
        "row_id",
        str(profile["fail_closed_package_row_id"]),
    )
    package = rows_by_field(platform_evidence["package_variant_rows"], "row_id")[str(profile["package_row_id"])]
    upsert_row(payload["package_variant_rows"], "row_id", deepcopy(package))

    umbrella = payload.setdefault("umbrella_readiness_contract", {})
    supported_ids = [str(row["platform_id"]) for row in payload.get("supported_rows", []) if isinstance(row, dict)]
    umbrella["support_claim_boundary"] = "source-owned:" + ",".join(sorted(supported_ids))
    append_unique(umbrella.setdefault("supported_platform_row_ids", []), str(profile["supported_row_id"]))
    child_contracts = umbrella.setdefault("child_issue_contracts", [])
    for row in child_contracts:
        if isinstance(row, dict) and row.get("issue_ref") == profile["issue_ref"]:
            row["row_id"] = profile["supported_row_id"]
            row["claim_state"] = "evidence-bound"
            row["diagnostic"] = profile["diagnostic"]
            break
    umbrella["lead_projection_rule"] = (
        "Supported platform projection is source-owned for "
        + ", ".join(sorted(supported_ids))
        + "; generated Linux/macOS hosted reports remain non-promoting unless reviewed source truth is checked in."
    )


def promote_platform(platform_id: str, *, apply: bool, summary_path: Path) -> dict[str, Any]:
    profile = require_platform(platform_id)
    reviewed = load_json(REVIEWED_SOURCE_INPUTS_PATH)
    platform_row = reviewed_platform_row(reviewed, platform_id)
    records = reviewed_records(reviewed, platform_row)

    boundary = load_json(BOUNDARY_INVENTORY_PATH)
    supported_platforms = load_json(SUPPORTED_PLATFORMS_PATH)
    tier_policy = load_json(SUPPORT_TIER_POLICY_PATH)
    platform_evidence = load_json(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH)
    source_truth = load_json(PLATFORM_SUPPORT_SOURCE_TRUTH_PATH)

    update_boundary(boundary, platform_id)
    update_supported_platforms(supported_platforms, platform_id, profile)
    update_tier_policy(tier_policy, platform_id)
    update_platform_evidence(platform_evidence, platform_id, profile, records)
    update_source_truth(source_truth, platform_id, profile, platform_evidence)

    changed_paths = [
        BOUNDARY_INVENTORY_PATH,
        SUPPORTED_PLATFORMS_PATH,
        SUPPORT_TIER_POLICY_PATH,
        PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH,
        PLATFORM_SUPPORT_SOURCE_TRUTH_PATH,
    ]
    summary = {
        "contract_id": "objc3c.platform.support-promotion.source-truth-application.v1",
        "status": "APPLIED" if apply else "DRY_RUN_READY",
        "platform_id": platform_id,
        "issue_ref": profile["issue_ref"],
        "tier_id": "tier-2",
        "generated_reports_are_source_truth": False,
        "reviewed_source_input": repo_rel(REVIEWED_SOURCE_INPUTS_PATH),
        "changed_source_paths": [repo_rel(path) for path in changed_paths],
        "supported_row_id": profile["supported_row_id"],
        "package_variant_row_id": profile["package_row_id"],
        "public_capability_id": profile["public_capability_id"],
    }
    if apply:
        write_json(BOUNDARY_INVENTORY_PATH, boundary)
        write_json(SUPPORTED_PLATFORMS_PATH, supported_platforms)
        write_json(SUPPORT_TIER_POLICY_PATH, tier_policy)
        write_json(PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH, platform_evidence)
        write_json(PLATFORM_SUPPORT_SOURCE_TRUTH_PATH, source_truth)
    write_json(summary_path, summary)
    return summary


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Promote reviewed Linux/macOS host evidence into checked platform source truth."
    )
    parser.add_argument("--platform-id", required=True, choices=sorted(PLATFORM_PROFILES))
    parser.add_argument(
        "--apply",
        action="store_true",
        help="write checked source fixtures; without this flag only the summary is written",
    )
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_PATH)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    summary = promote_platform(args.platform_id, apply=args.apply, summary_path=args.summary_out)
    print(f"summary_path: {repo_rel(args.summary_out)}")
    print(f"status: {summary['status']}")
    print("objc3c-platform-support-promotion: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
