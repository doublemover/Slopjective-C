#!/usr/bin/env python3
"""Collect generated hosted-runner platform evidence without promoting support."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import shutil
import sys
from pathlib import Path
from typing import Any

from platform_hardening_contracts.host_evidence_contract import (
    HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES,
    HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES,
    HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
    HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX,
    HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE,
    host_evidence_generated_report_paths_for_platform,
    host_evidence_required_review_input_paths_for_platform,
    host_evidence_review_candidate_path_for_platform,
    host_evidence_review_candidate_targets,
    host_evidence_review_record_ids,
)

ROOT = Path(__file__).resolve().parents[1]
REPORT_ROOT = ROOT / "tmp" / "reports" / "platform-host-evidence"
WORKFLOW_PATH = ".github/workflows/platform-host-evidence.yml"
DISPATCH_GATEWAY_WORKFLOW_PATH = ".github/workflows/conformance-minima.yml"
ACCEPTED_WORKFLOW_PATHS: tuple[str, ...] = (
    WORKFLOW_PATH,
    DISPATCH_GATEWAY_WORKFLOW_PATH,
)
DISPATCH_GATEWAY_WORKFLOW_PATHS: tuple[str, ...] = (
    DISPATCH_GATEWAY_WORKFLOW_PATH,
)

PLATFORM_CONFIG: dict[str, dict[str, Any]] = {
    "linux-x64": {
        "issue_ref": "8228",
        "host_os": "linux",
        "host_arch": "x64",
        "host_triple": "x86_64-unknown-linux-gnu",
        "runner_label": "ubuntu-24.04",
        "fail_closed_evidence_id": "objc3c.evidence.unsupported.linux-x64.fail-closed",
        "package_variant_row_id": "objc3c.package.runtime.linux-x64.release.fail-closed",
        "package_root_record_id": "objc3c.package-root.linux-x64.release.fail-closed",
        "native_execution_record_id": "objc3c.native-execution.linux-x64.release.missing",
        "object_format": "ELF",
        "debug_format": "DWARF",
        "runtime_library_names": ["libobjc3-runtime.so"],
        "loader_path_policy": "ELF rpath, RUNPATH, or package-root loader resolution must be proven before support",
        "package_root_layout": [
            "artifacts/package/objc3c-runnable-toolchain-package.json",
            "artifacts/bin/objc3c-native",
            "artifacts/lib/libobjc3-runtime.so",
            "stdlib/workspace.json",
            "stdlib/modules/objc3.core/module.json",
            "docs/runbooks/objc3c_packaging_channels.md",
        ],
    },
    "darwin-arm64": {
        "issue_ref": "8229",
        "host_os": "darwin",
        "host_arch": "arm64",
        "host_triple": "aarch64-apple-darwin",
        "runner_label": "macos-15",
        "fail_closed_evidence_id": "objc3c.evidence.unsupported.darwin-arm64.fail-closed",
        "package_variant_row_id": "objc3c.package.runtime.darwin-arm64.release.fail-closed",
        "package_root_record_id": "objc3c.package-root.darwin-arm64.release.fail-closed",
        "native_execution_record_id": "objc3c.native-execution.darwin-arm64.release.missing",
        "object_format": "Mach-O",
        "debug_format": "DWARF/dSYM",
        "runtime_library_names": ["libobjc3-runtime.dylib"],
        "loader_path_policy": "@rpath, install_name, codesign, and package-root loader behavior must be proven before support",
        "package_root_layout": [
            "artifacts/package/objc3c-runnable-toolchain-package.json",
            "artifacts/bin/objc3c-native",
            "artifacts/lib/libobjc3-runtime.dylib",
            "stdlib/workspace.json",
            "stdlib/modules/objc3.core/module.json",
            "docs/runbooks/objc3c_packaging_channels.md",
        ],
    },
}

PROMOTION_REVIEW_REQUIRED_FIELDS: tuple[str, ...] = (
    "host_identity",
    "toolchain_probe",
    "build",
    "package",
    "install",
    "object_identity",
    "debug_identity",
    "package_install_identity",
    "runtime_load_link_proof",
    "object_format",
    "debug_format",
    "runtime_link_load",
    "native_execution",
)

PROMOTION_REVIEWED_SOURCE_FIELDS: tuple[str, ...] = (
    "object_identity",
    "debug_identity",
    "package_install_identity",
    "runtime_load_link_proof",
)

PROMOTION_REVIEWED_SOURCE_FIELD_CONTRACTS: tuple[dict[str, str], ...] = (
    {
        "field_id": "object_identity",
        "required_record_id_field": "object_identity_record_id",
        "generated_report_path_suffix": "build/object-identity.json",
        "failure_class": "wrong-object-debug-format",
        "required_behavior": "fail-closed-before-package-publication",
    },
    {
        "field_id": "debug_identity",
        "required_record_id_field": "debug_identity_record_id",
        "generated_report_path_suffix": "build/debug-identity.json",
        "failure_class": "wrong-object-debug-format",
        "required_behavior": "fail-closed-before-package-publication",
    },
    {
        "field_id": "package_install_identity",
        "required_record_id_field": "package_install_identity_record_id",
        "generated_report_path_suffix": "install/install-receipt.json",
        "failure_class": "missing-install-receipt",
        "required_behavior": "fail-closed-before-native-execution-claim",
    },
    {
        "field_id": "runtime_load_link_proof",
        "required_record_id_field": "runtime_load_link_proof_record_id",
        "generated_report_path_suffix": "execution/runtime-load-probe.json",
        "failure_class": "runtime-load-failure",
        "required_behavior": "fail-closed-before-native-execution-claim",
    },
)

PROMOTION_BLOCKING_EVIDENCE_CLASSES: tuple[str, ...] = (
    "build",
    "package",
    "install",
    "execution",
)

REQUIRED_DURABLE_PROMOTION_ARTIFACT_SUFFIXES: tuple[str, ...] = (
    HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES
)

NATIVE_BUILD_SUMMARY_PATH = "tmp/build-objc3c-native/native_build_summary.json"
RUNNABLE_PACKAGE_MANIFEST_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"
PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH = (
    "tmp/reports/package-channels/end-to-end-summary.json"
)
PACKAGE_INSTALL_DISTRIBUTION_SUMMARY_PATH = (
    "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
)
PACKAGE_INSTALL_DISTRIBUTION_VERIFICATION_PATH = (
    "tmp/artifacts/package-ecosystem/install-validation/objc3c-install-distribution-verification.json"
)
HOSTED_EXECUTION_SMOKE_SUMMARY_PATH = "tmp/reports/hosted-execution-smoke/summary.json"
NATIVE_EXECUTION_SMOKE_SUMMARY_PATH = (
    "tmp/reports/objc3c-native-execution-smoke/summary.json"
)
FAIL_CLOSED_PLACEHOLDER_CONTRACT_ID = (
    "objc3c.platform.hosted-evidence.fail-closed-placeholder.v1"
)
INCOMPLETE_GENERATED_ARTIFACT_STATUSES: frozenset[str] = frozenset(
    {
        "fail-closed",
        "missing-source-generated-fail-closed",
        "package-target-mismatch-generated-fail-closed",
        "install-receipt-target-mismatch-generated-fail-closed",
        "identity-mismatch-generated-fail-closed",
        "runtime-load-unavailable-generated-fail-closed",
        "runtime-load-failed-generated-fail-closed",
        "producer-failed-before-success-artifact",
    }
)

STEP_CONTRACTS: tuple[tuple[str, str, tuple[tuple[str, str], ...]], ...] = (
    (
        "toolchain_probe",
        "toolchain",
        (
            (
                "tmp/reports/platform-host-evidence/{platform_id}/llvm-capabilities.json",
                "tmp/reports/platform-host-evidence/{platform_id}/llvm-capabilities.json",
            ),
        ),
    ),
    (
        "build",
        "build",
        (
            (
                "tmp/build-objc3c-native/native_build_summary.json",
                "tmp/reports/platform-host-evidence/{platform_id}/build/native_build_summary.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/build/object-identity.json",
                "tmp/reports/platform-host-evidence/{platform_id}/build/object-identity.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/build/debug-identity.json",
                "tmp/reports/platform-host-evidence/{platform_id}/build/debug-identity.json",
            ),
        ),
    ),
    (
        "package",
        "package",
        (
            (
                "artifacts/package/objc3c-runnable-toolchain-package.json",
                "tmp/reports/platform-host-evidence/{platform_id}/package/objc3c-runnable-toolchain-package.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/package/runtime-library-manifest.json",
                "tmp/reports/platform-host-evidence/{platform_id}/package/runtime-library-manifest.json",
            ),
        ),
    ),
    (
        "install",
        "install",
        (
            (
                "tmp/reports/package-channels/end-to-end-summary.json",
                "tmp/reports/platform-host-evidence/{platform_id}/install/end-to-end-summary.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json",
                "tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json",
            ),
        ),
    ),
    (
        "clean_install_distribution",
        "install",
        (
            (
                PACKAGE_INSTALL_DISTRIBUTION_SUMMARY_PATH,
                "tmp/reports/platform-host-evidence/{platform_id}/install/install-distribution-credibility-summary.json",
            ),
            (
                PACKAGE_INSTALL_DISTRIBUTION_VERIFICATION_PATH,
                "tmp/reports/platform-host-evidence/{platform_id}/install/install-distribution-verification.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/install/clean-install-distribution-receipt.json",
                "tmp/reports/platform-host-evidence/{platform_id}/install/clean-install-distribution-receipt.json",
            ),
        ),
    ),
    (
        "execution",
        "execution",
        (
            (
                "tmp/reports/hosted-execution-smoke/summary.json",
                "tmp/reports/platform-host-evidence/{platform_id}/execution/hosted-execution-smoke-summary.json",
            ),
            (
                "tmp/reports/objc3c-native-execution-smoke/summary.json",
                "tmp/reports/platform-host-evidence/{platform_id}/execution/native-execution-smoke-summary.json",
            ),
            (
                "tmp/reports/platform-host-evidence/{platform_id}/execution/runtime-load-probe.json",
                "tmp/reports/platform-host-evidence/{platform_id}/execution/runtime-load-probe.json",
            ),
        ),
    ),
)

OUTCOME_ENV = {
    "dependency_install": "OBJC3C_PLATFORM_EVIDENCE_DEPENDENCY_OUTCOME",
    "toolchain_setup": "OBJC3C_PLATFORM_EVIDENCE_TOOLCHAIN_SETUP_OUTCOME",
    "toolchain_probe": "OBJC3C_PLATFORM_EVIDENCE_TOOLCHAIN_PROBE_OUTCOME",
    "build": "OBJC3C_PLATFORM_EVIDENCE_BUILD_OUTCOME",
    "package": "OBJC3C_PLATFORM_EVIDENCE_PACKAGE_OUTCOME",
    "install": "OBJC3C_PLATFORM_EVIDENCE_INSTALL_OUTCOME",
    "clean_install_distribution": "OBJC3C_PLATFORM_EVIDENCE_CLEAN_INSTALL_OUTCOME",
    "execution": "OBJC3C_PLATFORM_EVIDENCE_EXECUTION_OUTCOME",
}


def repo_rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def generated_artifact(path_text: str) -> dict[str, Any]:
    path = ROOT / path_text
    if not path.is_file():
        return {
            "path": path_text,
            "exists": False,
        }
    artifact = {
        "path": path_text,
        "exists": True,
        "size_bytes": path.stat().st_size,
        "sha256": sha256_file(path),
    }
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError, json.JSONDecodeError):
        return artifact
    if isinstance(payload, dict):
        status = str(payload.get("status", ""))
        source_paths = source_artifact_paths_from_payload(payload)
        if payload.get("contract_id") == FAIL_CLOSED_PLACEHOLDER_CONTRACT_ID:
            artifact["fail_closed_placeholder"] = True
        elif status in INCOMPLETE_GENERATED_ARTIFACT_STATUSES:
            artifact["incomplete_generated_artifact"] = True
        if artifact.get("fail_closed_placeholder") or artifact.get("incomplete_generated_artifact"):
            artifact["promotion_usable"] = False
            artifact["producer_step_id"] = str(payload.get("producer_step_id", ""))
            artifact["status"] = status
            missing_source_path = str(payload.get("missing_source_path", ""))
            required_sources = source_paths
            if missing_source_path and missing_source_path not in required_sources:
                required_sources = [*required_sources, missing_source_path]
            if required_sources:
                artifact["required_source_artifacts"] = required_sources
            diagnostics = payload.get("diagnostics")
            if isinstance(diagnostics, dict):
                artifact["diagnostics"] = diagnostics
            elif required_sources:
                artifact["diagnostics"] = incomplete_artifact_diagnostics(
                    status=status,
                    source_paths=tuple(required_sources),
                )
    return artifact


def source_artifact_paths_from_payload(payload: dict[str, Any]) -> list[str]:
    paths: list[str] = []
    artifacts = payload.get("source_artifacts")
    if isinstance(artifacts, list):
        for entry in artifacts:
            if not isinstance(entry, dict):
                continue
            path_text = str(entry.get("path", "")).replace("\\", "/")
            if path_text and path_text not in paths:
                paths.append(path_text)
    return paths


def write_fail_closed_placeholder_artifact(
    *,
    platform_id: str,
    step_id: str,
    evidence_class: str,
    outcome: str,
    source_path_text: str,
    scoped_path_text: str,
) -> None:
    scoped_path = ROOT / scoped_path_text
    if scoped_path.is_file():
        return
    status = "producer-failed-before-success-artifact"
    payload = {
        "contract_id": FAIL_CLOSED_PLACEHOLDER_CONTRACT_ID,
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "generated_report_path": scoped_path_text,
        "missing_source_path": source_path_text,
        "producer_step_id": step_id,
        "evidence_class": evidence_class,
        "producer_outcome": outcome,
        "status": status,
        "support_truth": False,
        "promotion_allowed_from_generated_evidence": False,
        "generated_report_support_truth": False,
        "reviewed_source_required": True,
        "review_result": "fail-closed-not-promotion-ready",
        "failure_class": f"{evidence_class}-producer-failed-before-success-artifact",
        "required_behavior": "fail-closed-before-support-promotion",
        "source_artifacts": source_artifacts(source_path_text),
        "diagnostics": incomplete_artifact_diagnostics(
            status=status,
            source_paths=(source_path_text,),
        ),
    }
    write_json(scoped_path, payload)


def materialize_generated_artifact(
    source_path_text: str,
    scoped_path_text: str,
    *,
    platform_id: str,
    step_id: str,
    evidence_class: str,
    outcome: str,
) -> dict[str, Any]:
    source_path = ROOT / source_path_text
    scoped_path = ROOT / scoped_path_text
    if source_path.is_file() and source_path.resolve() != scoped_path.resolve():
        scoped_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source_path, scoped_path)
    elif (
        not source_path.is_file()
        and not scoped_path.is_file()
        and outcome not in {"success", "not-recorded"}
        and scoped_path_text in {
            platform_scoped_path(platform_id, suffix)
            for suffix in REQUIRED_DURABLE_PROMOTION_ARTIFACT_SUFFIXES
        }
    ):
        write_fail_closed_placeholder_artifact(
            platform_id=platform_id,
            step_id=step_id,
            evidence_class=evidence_class,
            outcome=outcome,
            source_path_text=source_path_text,
            scoped_path_text=scoped_path_text,
        )
    artifact = generated_artifact(scoped_path_text)
    artifact["source_path"] = source_path_text
    artifact["scoped_copy"] = source_path_text != scoped_path_text
    return artifact


def read_json_object(path_text: str) -> dict[str, Any]:
    path = ROOT / path_text
    if not path.is_file():
        return {}
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError, json.JSONDecodeError) as exc:
        return {"_json_read_error": str(exc)}
    if not isinstance(payload, dict):
        return {"_json_type_error": type(payload).__name__}
    return payload


def dict_field(payload: dict[str, Any], field_name: str) -> dict[str, Any]:
    value = payload.get(field_name)
    return value if isinstance(value, dict) else {}


def list_field(payload: dict[str, Any], field_name: str) -> list[Any]:
    value = payload.get(field_name)
    return value if isinstance(value, list) else []


def repo_or_absolute_path(path_text: str) -> Path:
    path = Path(path_text)
    if path.is_absolute():
        return path
    return ROOT / path_text


def prefixed_repo_path(prefix: str, relative_path: str) -> str:
    if not prefix:
        return relative_path
    return f"{prefix.rstrip('/')}/{relative_path.lstrip('/')}"


def platform_artifact_path(platform_id: str, path_suffix: str) -> Path:
    return ROOT / platform_scoped_path(platform_id, path_suffix)


def platform_artifact_exists(platform_id: str, path_suffix: str) -> bool:
    return platform_artifact_path(platform_id, path_suffix).is_file()


def source_artifacts(*path_texts: str) -> list[dict[str, Any]]:
    artifacts: list[dict[str, Any]] = []
    seen: set[str] = set()
    for path_text in path_texts:
        if not path_text:
            continue
        normalized = path_text.replace("\\", "/")
        if normalized in seen:
            continue
        seen.add(normalized)
        artifacts.append(generated_artifact(normalized))
    return artifacts


def existing_artifact_paths(*path_texts: str) -> list[str]:
    paths: list[str] = []
    for path_text in path_texts:
        if not path_text:
            continue
        normalized = path_text.replace("\\", "/")
        if normalized in paths:
            continue
        if (ROOT / normalized).is_file():
            paths.append(normalized)
    return paths


def incomplete_artifact_diagnostics(
    *,
    status: str,
    source_paths: tuple[str, ...],
) -> dict[str, Any]:
    return {
        "status": status,
        "classification": "incomplete-review-candidate",
        "review_result": "fail-closed-not-promotion-ready",
        "required_source_artifacts": [
            path_text for path_text in source_paths if path_text
        ],
        "message": (
            "producer did not provide complete generated evidence; keep as "
            "review candidate only until required source artifacts exist"
        ),
    }


def attach_incomplete_artifact_diagnostics(
    payload: dict[str, Any],
    *,
    source_paths: tuple[str, ...],
) -> None:
    status = str(payload.get("status", ""))
    if status not in INCOMPLETE_GENERATED_ARTIFACT_STATUSES:
        return
    payload["generated_report_support_truth"] = False
    payload["reviewed_source_required"] = True
    payload["review_result"] = "fail-closed-not-promotion-ready"
    payload["diagnostics"] = incomplete_artifact_diagnostics(
        status=status,
        source_paths=source_paths,
    )


def expected_platform_identity(platform_id: str) -> dict[str, Any]:
    config = PLATFORM_CONFIG[platform_id]
    return {
        "target_platform_id": platform_id,
        "target_triple": config["host_triple"],
        "arch": config["host_arch"],
        "object_format": config["object_format"],
        "debug_format": config["debug_format"],
        "runtime_library_names": config["runtime_library_names"],
        "loader_path_policy": config["loader_path_policy"],
        "package_root_layout": config["package_root_layout"],
    }


def native_build_target_identity(build_summary: dict[str, Any]) -> dict[str, Any]:
    target = dict_field(build_summary, "target")
    return {
        "target_platform_id": str(target.get("platform_id", "")),
        "target_triple": str(target.get("target_triple", "")),
        "object_format": str(target.get("object_format", "")),
        "debug_format": str(target.get("debug_format", "")),
        "runtime_library_kind": str(target.get("runtime_library_kind", "")),
        "runtime_library_file_name": str(target.get("runtime_library_file_name", "")),
    }


def generated_identity_status(
    *,
    source_exists: bool,
    actual: dict[str, Any],
    expected: dict[str, Any],
    fields: tuple[str, ...],
) -> str:
    if not source_exists:
        return "missing-source-generated-fail-closed"
    for field_name in fields:
        if actual.get(field_name) != expected.get(field_name):
            return "identity-mismatch-generated-fail-closed"
    return "generated-host-artifact-present"


def runtime_manifest_status(
    *,
    source_exists: bool,
    runtime_artifacts: list[dict[str, Any]],
    package_target_platform_id: str,
    platform_id: str,
) -> str:
    if not source_exists or not any(item.get("exists") for item in runtime_artifacts):
        return "missing-source-generated-fail-closed"
    if package_target_platform_id and package_target_platform_id != platform_id:
        return "package-target-mismatch-generated-fail-closed"
    return "generated-host-artifact-present"


def install_receipt_status(
    *,
    source_receipt_path: str,
    source_receipt: dict[str, Any],
    platform_id: str,
) -> str:
    if not source_receipt_path:
        return "missing-source-generated-fail-closed"
    receipt_target = str(source_receipt.get("target_platform_id", ""))
    if not receipt_target:
        receipt_target = str(
            dict_field(source_receipt, "package_runtime_model").get("target_platform_id", "")
        )
    if receipt_target and receipt_target != platform_id:
        return "install-receipt-target-mismatch-generated-fail-closed"
    return "generated-host-artifact-present"


def runtime_load_probe_status(
    *,
    source_exists: bool,
    exit_code: Any,
    native_status: str,
    skip_reason: str,
) -> str:
    if not source_exists:
        return "missing-source-generated-fail-closed"
    normalized_status = native_status.upper()
    if skip_reason or normalized_status in {"UNAVAILABLE", "SKIP", "SKIPPED"}:
        return "runtime-load-unavailable-generated-fail-closed"
    if isinstance(exit_code, int) and exit_code == 0 and normalized_status == "PASS":
        return "generated-host-artifact-present"
    return "runtime-load-failed-generated-fail-closed"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_platform_generated_json(platform_id: str, path_suffix: str) -> dict[str, Any]:
    path_text = platform_scoped_path(platform_id, path_suffix)
    path = ROOT / path_text
    if not path.is_file():
        raise RuntimeError(f"host evidence generated artifact missing: {path_text}")
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError, json.JSONDecodeError) as exc:
        raise RuntimeError(f"host evidence generated artifact is not valid JSON: {path_text}: {exc}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"host evidence generated artifact is not a JSON object: {path_text}")
    return payload


def load_optional_platform_generated_json(
    platform_id: str,
    path_suffix: str,
) -> dict[str, Any] | None:
    path_text = platform_scoped_path(platform_id, path_suffix)
    if not (ROOT / path_text).is_file():
        return None
    return load_platform_generated_json(platform_id, path_suffix)


def require_common_generated_artifact(
    payload: dict[str, Any],
    *,
    platform_id: str,
    path_suffix: str,
    contract_id: str,
    reviewed_record_id: str | None = None,
    reviewed_source_required: bool = False,
) -> None:
    expected = PLATFORM_CONFIG[platform_id]
    expected_path = platform_scoped_path(platform_id, path_suffix)
    require(payload.get("contract_id") == contract_id, f"{expected_path} contract_id drifted")
    require(payload.get("schema_version") == 1, f"{expected_path} schema_version drifted")
    require(payload.get("platform_id") == platform_id, f"{expected_path} platform_id drifted")
    require(payload.get("issue_ref") == int(expected["issue_ref"]), f"{expected_path} issue_ref drifted")
    require(payload.get("generated_report_path") == expected_path, f"{expected_path} report path drifted")
    require(payload.get("support_truth") is False, f"{expected_path} attempted to become support truth")
    require(
        payload.get("promotion_allowed_from_generated_evidence") is False,
        f"{expected_path} allowed generated-only promotion",
    )
    if reviewed_record_id is not None:
        require(payload.get("record_id") == reviewed_record_id, f"{expected_path} reviewed record_id drifted")
    if reviewed_source_required:
        require(payload.get("reviewed_source_required") is True, f"{expected_path} did not require source review")


def require_artifact_entry_shape(entry: Any, owner: str) -> dict[str, Any]:
    require(isinstance(entry, dict), f"{owner} artifact entry must be an object")
    path_text = entry.get("path")
    require(isinstance(path_text, str) and path_text, f"{owner} artifact entry missing path")
    exists = entry.get("exists")
    require(isinstance(exists, bool), f"{owner} artifact entry missing boolean exists")
    if exists:
        require(isinstance(entry.get("size_bytes"), int) and entry["size_bytes"] > 0, f"{owner} existing artifact missing size")
        digest = entry.get("sha256")
        require(isinstance(digest, str) and len(digest) == 64, f"{owner} existing artifact missing sha256")
    return entry


def artifact_exists_in_payload(artifacts: list[Any], path_text: str) -> bool:
    normalized = path_text.replace("\\", "/")
    for raw_entry in artifacts:
        entry = require_artifact_entry_shape(raw_entry, normalized)
        if str(entry.get("path", "")).replace("\\", "/") == normalized:
            return entry.get("exists") is True
    return False


def require_source_artifacts(payload: dict[str, Any], owner: str) -> list[Any]:
    artifacts = payload.get("source_artifacts")
    require(isinstance(artifacts, list), f"{owner} source_artifacts must be a list")
    for entry in artifacts:
        require_artifact_entry_shape(entry, owner)
    return artifacts


def require_status(payload: dict[str, Any], expected_status: str, owner: str) -> None:
    actual_status = payload.get("status")
    require(isinstance(actual_status, str) and actual_status, f"{owner} missing status")
    require(actual_status == expected_status, f"{owner} status is not internally consistent")
    if actual_status == "generated-host-artifact-present":
        require(payload.get("support_truth") is False, f"{owner} present generated artifact became support truth")
        require(
            payload.get("promotion_allowed_from_generated_evidence") is False,
            f"{owner} present generated artifact allowed source-truth promotion",
        )


def is_fail_closed_placeholder(payload: dict[str, Any]) -> bool:
    return payload.get("contract_id") == FAIL_CLOSED_PLACEHOLDER_CONTRACT_ID


def is_incomplete_generated_artifact(payload: dict[str, Any]) -> bool:
    return str(payload.get("status", "")) in INCOMPLETE_GENERATED_ARTIFACT_STATUSES


def require_fail_closed_placeholder(payload: dict[str, Any], *, platform_id: str, owner: str) -> None:
    require(is_fail_closed_placeholder(payload), f"{owner} is not a fail-closed placeholder")
    require(payload.get("schema_version") == 1, f"{owner} placeholder schema_version drifted")
    require(payload.get("platform_id") == platform_id, f"{owner} placeholder platform_id drifted")
    require(payload.get("issue_ref") == int(PLATFORM_CONFIG[platform_id]["issue_ref"]), f"{owner} placeholder issue_ref drifted")
    require(payload.get("generated_report_path") == owner, f"{owner} placeholder report path drifted")
    require(payload.get("status") == "producer-failed-before-success-artifact", f"{owner} placeholder status drifted")
    require(payload.get("support_truth") is False, f"{owner} placeholder attempted support truth")
    require(
        payload.get("promotion_allowed_from_generated_evidence") is False,
        f"{owner} placeholder allowed generated promotion",
    )
    require(
        payload.get("generated_report_support_truth") is False,
        f"{owner} placeholder became support truth",
    )
    require(
        payload.get("review_result") == "fail-closed-not-promotion-ready",
        f"{owner} placeholder review result drifted",
    )
    require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)


def require_incomplete_generated_artifact(
    payload: dict[str, Any],
    *,
    platform_id: str,
    owner: str,
) -> None:
    status = str(payload.get("status", ""))
    require(
        status in INCOMPLETE_GENERATED_ARTIFACT_STATUSES,
        f"{owner} incomplete artifact status drifted",
    )
    require(payload.get("platform_id") == platform_id, f"{owner} incomplete artifact platform_id drifted")
    require(payload.get("support_truth") is False, f"{owner} incomplete artifact attempted support truth")
    require(
        payload.get("promotion_allowed_from_generated_evidence") is False,
        f"{owner} incomplete artifact allowed generated promotion",
    )
    require(
        payload.get("generated_report_support_truth") is False,
        f"{owner} incomplete artifact became support truth",
    )
    require(
        payload.get("reviewed_source_required") is True,
        f"{owner} incomplete artifact did not require source review",
    )
    require(
        payload.get("review_result") == "fail-closed-not-promotion-ready",
        f"{owner} incomplete artifact review result drifted",
    )
    source_artifact_entries: list[Any] = []
    source_artifacts_value = payload.get("source_artifacts")
    if source_artifacts_value is not None:
        source_artifact_entries = require_source_artifacts(payload, owner)
    missing_source_path = str(payload.get("missing_source_path", ""))
    require(
        source_artifact_entries or missing_source_path,
        f"{owner} incomplete artifact missing source_artifacts",
    )
    diagnostics = payload.get("diagnostics")
    if diagnostics is None and (missing_source_path or source_artifact_entries):
        source_paths = source_artifact_paths_from_payload(payload)
        if missing_source_path and missing_source_path not in source_paths:
            source_paths.append(missing_source_path)
        diagnostics = incomplete_artifact_diagnostics(
            status=status,
            source_paths=tuple(source_paths),
        )
    require(isinstance(diagnostics, dict), f"{owner} incomplete artifact missing diagnostics")
    require(
        diagnostics.get("classification") == "incomplete-review-candidate",
        f"{owner} incomplete artifact diagnostics classification drifted",
    )
    require(
        diagnostics.get("review_result") == "fail-closed-not-promotion-ready",
        f"{owner} incomplete artifact diagnostics review result drifted",
    )
    required_sources = diagnostics.get("required_source_artifacts")
    require(
        isinstance(required_sources, list)
        and all(isinstance(path_text, str) and path_text for path_text in required_sources),
        f"{owner} incomplete artifact diagnostics missing required source artifacts",
    )
    if source_artifact_entries:
        declared_sources = {
            str(entry.get("path", "")).replace("\\", "/")
            for entry in source_artifact_entries
        }
        missing_sources = sorted(
            str(path_text).replace("\\", "/")
            for path_text in required_sources
            if str(path_text).replace("\\", "/") not in declared_sources
        )
        require(
            not missing_sources,
            f"{owner} incomplete artifact source_artifacts missing required sources: {', '.join(missing_sources)}",
        )


def require_platform_identity_fields(
    payload: dict[str, Any],
    *,
    owner: str,
    identity_field: str,
    expected_fields: tuple[str, ...],
) -> dict[str, Any]:
    expected_identity = expected_platform_identity(str(payload["platform_id"]))
    identity = payload.get(identity_field)
    require(isinstance(identity, dict), f"{owner} missing {identity_field}")
    for field_name in expected_fields:
        require(
            identity.get(field_name) == expected_identity.get(field_name),
            f"{owner} {identity_field}.{field_name} drifted",
        )
    return identity


def require_platform_root_fields(payload: dict[str, Any], platform_id: str, owner: str) -> None:
    expected = expected_platform_identity(platform_id)
    require(payload.get("target_platform_id") == platform_id, f"{owner} target_platform_id drifted")
    require(payload.get("target_triple") == expected["target_triple"], f"{owner} target_triple drifted")
    require(
        payload.get("package_root_layout") == expected["package_root_layout"],
        f"{owner} package_root_layout drifted",
    )


def validate_object_identity_artifact(platform_id: str) -> None:
    path_suffix = "build/object-identity.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    record_ids = reviewed_source_record_ids(platform_id)
    payload = load_platform_generated_json(platform_id, path_suffix)
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require_common_generated_artifact(
        payload,
        platform_id=platform_id,
        path_suffix=path_suffix,
        contract_id="objc3c.platform.hosted-object-identity.generated.v1",
        reviewed_record_id=record_ids["object_identity_record_id"],
        reviewed_source_required=True,
    )
    expected_identity = require_platform_identity_fields(
        payload,
        owner=owner,
        identity_field="expected_identity",
        expected_fields=("target_platform_id", "target_triple", "arch", "object_format"),
    )
    if is_incomplete_generated_artifact(payload):
        require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)
        return
    actual_identity = payload.get("actual_identity")
    require(isinstance(actual_identity, dict), f"{owner} missing actual_identity")
    source_artifacts = require_source_artifacts(payload, owner)
    require_status(
        payload,
        generated_identity_status(
            source_exists=artifact_exists_in_payload(source_artifacts, NATIVE_BUILD_SUMMARY_PATH),
            actual=actual_identity,
            expected=expected_identity,
            fields=("target_platform_id", "target_triple", "object_format"),
        ),
        owner,
    )


def validate_debug_identity_artifact(platform_id: str) -> None:
    path_suffix = "build/debug-identity.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    record_ids = reviewed_source_record_ids(platform_id)
    payload = load_platform_generated_json(platform_id, path_suffix)
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require_common_generated_artifact(
        payload,
        platform_id=platform_id,
        path_suffix=path_suffix,
        contract_id="objc3c.platform.hosted-debug-identity.generated.v1",
        reviewed_record_id=record_ids["debug_identity_record_id"],
        reviewed_source_required=True,
    )
    expected_identity = require_platform_identity_fields(
        payload,
        owner=owner,
        identity_field="expected_identity",
        expected_fields=("target_platform_id", "target_triple", "arch", "debug_format"),
    )
    if is_incomplete_generated_artifact(payload):
        require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)
        return
    actual_identity = payload.get("actual_identity")
    require(isinstance(actual_identity, dict), f"{owner} missing actual_identity")
    source_artifacts = require_source_artifacts(payload, owner)
    require_status(
        payload,
        generated_identity_status(
            source_exists=artifact_exists_in_payload(source_artifacts, NATIVE_BUILD_SUMMARY_PATH),
            actual=actual_identity,
            expected=expected_identity,
            fields=("target_platform_id", "target_triple", "debug_format"),
        ),
        owner,
    )


def validate_runtime_library_manifest_artifact(platform_id: str) -> None:
    path_suffix = "package/runtime-library-manifest.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    payload = load_platform_generated_json(platform_id, path_suffix)
    expected = expected_platform_identity(platform_id)
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require_common_generated_artifact(
        payload,
        platform_id=platform_id,
        path_suffix=path_suffix,
        contract_id="objc3c.platform.hosted-runtime-library-manifest.generated.v1",
    )
    require(payload.get("native_execution_claimed") is False, f"{owner} claimed native execution")
    if is_incomplete_generated_artifact(payload):
        require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)
        return
    require_platform_root_fields(payload, platform_id, owner)
    require(payload.get("runtime_library_names") == expected["runtime_library_names"], f"{owner} runtime libraries drifted")
    require(payload.get("loader_path_policy") == expected["loader_path_policy"], f"{owner} loader policy drifted")
    package_artifact = require_artifact_entry_shape(payload.get("package_manifest_artifact"), owner)
    runtime_artifacts = payload.get("runtime_library_artifacts")
    require(isinstance(runtime_artifacts, list), f"{owner} runtime_library_artifacts must be a list")
    for entry in runtime_artifacts:
        artifact = require_artifact_entry_shape(entry, owner)
        artifact_path = str(artifact.get("path", "")).replace("\\", "/")
        require(
            any(artifact_path.endswith(f"/{name}") or artifact_path == f"artifacts/lib/{name}" for name in expected["runtime_library_names"]),
            f"{owner} runtime artifact path did not match expected runtime library names",
        )
    require_source_artifacts(payload, owner)
    require_status(
        payload,
        runtime_manifest_status(
            source_exists=package_artifact.get("exists") is True,
            runtime_artifacts=runtime_artifacts,
            package_target_platform_id=str(payload.get("source_package_target_platform_id", "")),
            platform_id=platform_id,
        ),
        owner,
    )


def validate_install_receipt_artifact(platform_id: str) -> None:
    path_suffix = "install/install-receipt.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    record_ids = reviewed_source_record_ids(platform_id)
    payload = load_platform_generated_json(platform_id, path_suffix)
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require_common_generated_artifact(
        payload,
        platform_id=platform_id,
        path_suffix=path_suffix,
        contract_id="objc3c.platform.hosted-install-receipt.generated.v1",
        reviewed_record_id=record_ids["package_install_identity_record_id"],
        reviewed_source_required=True,
    )
    require(payload.get("native_execution_claimed") is False, f"{owner} claimed native execution")
    if is_incomplete_generated_artifact(payload):
        require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)
        return
    require_platform_root_fields(payload, platform_id, owner)
    require(payload.get("package_manifest") == RUNNABLE_PACKAGE_MANIFEST_PATH, f"{owner} package manifest path drifted")
    require_artifact_entry_shape(payload.get("package_manifest_artifact"), owner)
    require_artifact_entry_shape(payload.get("package_channels_summary_artifact"), owner)
    receipt_artifact = payload.get("source_install_receipt_artifact")
    require(isinstance(receipt_artifact, dict), f"{owner} source_install_receipt_artifact must be an object")
    if receipt_artifact.get("exists") is True:
        require_artifact_entry_shape(receipt_artifact, owner)
    else:
        require(receipt_artifact.get("exists") is False, f"{owner} receipt artifact exists flag drifted")
    source_receipt = payload.get("source_install_receipt")
    require(isinstance(source_receipt, dict), f"{owner} source_install_receipt must be an object")
    if payload.get("status") == "missing-source-generated-fail-closed":
        source_artifacts = require_source_artifacts(payload, owner)
        require(
            artifact_exists_in_payload(source_artifacts, PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH)
            is False,
            f"{owner} missing-source receipt unexpectedly had package-channel summary",
        )
        return
    require_installed_root_execution_record(
        payload,
        field_name="installed_root_execution",
        expected_channel_id="local-installer",
        platform_id=platform_id,
        owner=owner,
    )
    require_installed_root_execution_record(
        payload,
        field_name="offline_installed_root_execution",
        expected_channel_id="offline-bundle",
        platform_id=platform_id,
        owner=owner,
    )
    source_artifacts = require_source_artifacts(payload, owner)
    require(
        artifact_exists_in_payload(source_artifacts, PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH)
        or payload.get("status") == "missing-source-generated-fail-closed",
        f"{owner} source_artifacts missing package-channel summary",
    )
    require(
        artifact_exists_in_payload(source_artifacts, RUNNABLE_PACKAGE_MANIFEST_PATH)
        or payload.get("status") == "missing-source-generated-fail-closed",
        f"{owner} source_artifacts missing runnable package manifest",
    )
    require_status(
        payload,
        install_receipt_status(
            source_receipt_path=str(payload.get("source_install_receipt_path", "")),
            source_receipt=source_receipt,
            platform_id=platform_id,
        ),
        owner,
    )


def require_installed_root_execution_record(
    payload: dict[str, Any],
    *,
    field_name: str,
    expected_channel_id: str,
    platform_id: str,
    owner: str,
) -> None:
    proof = payload.get(field_name)
    require(isinstance(proof, dict), f"{owner} {field_name} must be an object")
    require(
        proof.get("contract_id") == "objc3c.packaging.channels.installed-root-native-execution.v1",
        f"{owner} {field_name} contract_id drifted",
    )
    require(proof.get("status") == "PASS", f"{owner} {field_name} did not pass")
    require(
        proof.get("channel_id") == expected_channel_id,
        f"{owner} {field_name} channel id drifted",
    )
    require(
        proof.get("execution_source") == "installed-root",
        f"{owner} {field_name} execution source drifted",
    )
    require(
        proof.get("repo_temp_dependency") is False,
        f"{owner} {field_name} depended on repo temp output",
    )
    require(
        proof.get("preexisting_artifacts_dependency") is False,
        f"{owner} {field_name} depended on preexisting artifacts",
    )
    require(proof.get("returncode") == 2, f"{owner} {field_name} return code drifted")
    require(
        proof.get("usage_banner_seen") is True,
        f"{owner} {field_name} did not reach objc3c-native usage path",
    )
    if payload.get("status") == "generated-host-artifact-present":
        require(
            proof.get("target_platform_id") == platform_id,
            f"{owner} {field_name} target platform drifted",
        )


def validate_clean_install_distribution_summary(platform_id: str) -> None:
    path_suffix = "install/install-distribution-credibility-summary.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    payload = load_optional_platform_generated_json(platform_id, path_suffix)
    if payload is None:
        return
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require(
        payload.get("contract_id") == "objc3c.package_ecosystem.install_distribution_credibility.summary.v1",
        f"{owner} contract_id drifted",
    )
    require(payload.get("status") == "PASS", f"{owner} did not record PASS status")
    require(payload.get("failures") == [], f"{owner} recorded failures")
    require(payload.get("network_policy") == "no-network-during-validation", f"{owner} network policy drifted")
    require(
        payload.get("hosted_registry_support") == "unsupported-fail-closed-if-claimed",
        f"{owner} hosted registry policy drifted",
    )
    probe = payload.get("from_nothing_probe")
    require(isinstance(probe, dict), f"{owner} missing from_nothing_probe")
    require(probe.get("requested") is True, f"{owner} did not request from-nothing validation")
    require(
        probe.get("generated_from_clean_owned_outputs") is True,
        f"{owner} did not record clean owned output generation",
    )
    owned_after_clean = probe.get("owned_outputs_exist_after_clean")
    require(isinstance(owned_after_clean, dict), f"{owner} missing owned_outputs_exist_after_clean")
    require(
        all(value is False for value in owned_after_clean.values()),
        f"{owner} found preexisting owned outputs after clean",
    )
    require(
        payload.get("install_receipt") == "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c-install-receipt.json",
        f"{owner} install receipt path drifted",
    )
    require(
        payload.get("install_verification") == PACKAGE_INSTALL_DISTRIBUTION_VERIFICATION_PATH,
        f"{owner} install verification path drifted",
    )
    require(isinstance(payload.get("generated_paths"), list), f"{owner} generated_paths must be a list")


def validate_clean_install_distribution_verification(platform_id: str) -> None:
    path_suffix = "install/install-distribution-verification.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    payload = load_optional_platform_generated_json(platform_id, path_suffix)
    if payload is None:
        return
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require(
        payload.get("contract_id") == "objc3c.package_ecosystem.install_distribution_credibility.v1",
        f"{owner} contract_id drifted",
    )
    require(payload.get("network_resolution_support") == "unsupported", f"{owner} network support drifted")
    require(
        payload.get("hosted_registry_support") == "unsupported-fail-closed-if-claimed",
        f"{owner} hosted registry policy drifted",
    )
    clean_start = payload.get("clean_start")
    require(isinstance(clean_start, dict), f"{owner} missing clean_start")
    require(clean_start.get("stale_artifacts_allowed") is False, f"{owner} allowed stale artifacts")
    require(
        payload.get("install_receipt") == "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c-install-receipt.json",
        f"{owner} install receipt path drifted",
    )
    require(payload.get("package_bridge") == "objc3c", f"{owner} package bridge drifted")
    package_count = payload.get("package_count")
    manifest_count = payload.get("manifest_count")
    installed_packages = payload.get("installed_packages")
    require(isinstance(package_count, int) and package_count > 0, f"{owner} package_count invalid")
    require(isinstance(manifest_count, int) and manifest_count == package_count, f"{owner} manifest_count invalid")
    require(isinstance(installed_packages, list), f"{owner} installed_packages must be a list")
    require(len(installed_packages) == package_count, f"{owner} installed package count drifted")
    for package in installed_packages:
        require(isinstance(package, dict), f"{owner} installed package entry must be an object")
        for field_name in ("package_id", "source_manifest", "installed_manifest", "manifest_digest", "lock_manifest_digest"):
            require(isinstance(package.get(field_name), str) and package[field_name], f"{owner} installed package missing {field_name}")
        require(
            package.get("manifest_digest") == package.get("lock_manifest_digest"),
            f"{owner} installed package digest drifted",
        )
    platform_host_evidence = payload.get("platform_host_evidence")
    if isinstance(platform_host_evidence, dict):
        install_receipt = platform_host_evidence.get("install_receipt")
        if isinstance(install_receipt, dict):
            require(install_receipt.get("platform_id") == platform_id, f"{owner} platform install receipt platform drifted")
            require(
                install_receipt.get("platform_scoped_clean_install_receipt")
                == platform_scoped_path(platform_id, "install/clean-install-distribution-receipt.json"),
                f"{owner} platform clean install receipt path drifted",
            )
            require(
                install_receipt.get("host_promotion_receipt_path_reserved")
                == platform_scoped_path(platform_id, "install/install-receipt.json"),
                f"{owner} host promotion receipt reservation drifted",
            )
            digest = install_receipt.get("source_receipt_sha256")
            require(isinstance(digest, str) and len(digest) == 64, f"{owner} clean install receipt digest missing")


def validate_clean_install_distribution_receipt(platform_id: str) -> None:
    path_suffix = "install/clean-install-distribution-receipt.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    payload = load_optional_platform_generated_json(platform_id, path_suffix)
    if payload is None:
        return
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require(
        payload.get("contract_id") == "objc3c.package_ecosystem.from_nothing_install_receipt.v1",
        f"{owner} contract_id drifted",
    )
    require(payload.get("package_bridge") == "objc3c", f"{owner} package bridge drifted")
    require(
        payload.get("install_command") == "npm run objc3c -- validate-package-install-distribution --from-nothing",
        f"{owner} install command drifted",
    )
    require(payload.get("machine_owned") is True, f"{owner} machine_owned drifted")
    require(
        payload.get("install_root") == "tmp/artifacts/package-ecosystem/install-validation/clean-root",
        f"{owner} install_root drifted",
    )
    require(
        payload.get("install_home") == "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c",
        f"{owner} install_home drifted",
    )


def validate_runtime_load_probe_artifact(platform_id: str) -> None:
    path_suffix = "execution/runtime-load-probe.json"
    owner = platform_scoped_path(platform_id, path_suffix)
    record_ids = reviewed_source_record_ids(platform_id)
    payload = load_platform_generated_json(platform_id, path_suffix)
    expected = expected_platform_identity(platform_id)
    if is_fail_closed_placeholder(payload):
        require_fail_closed_placeholder(payload, platform_id=platform_id, owner=owner)
        return
    require_common_generated_artifact(
        payload,
        platform_id=platform_id,
        path_suffix=path_suffix,
        contract_id="objc3c.platform.hosted-runtime-load-probe.generated.v1",
        reviewed_record_id=record_ids["runtime_load_link_proof_record_id"],
        reviewed_source_required=True,
    )
    require(payload.get("native_execution_claimed") is False, f"{owner} claimed native execution")
    if is_incomplete_generated_artifact(payload):
        require_incomplete_generated_artifact(payload, platform_id=platform_id, owner=owner)
        return
    require(payload.get("target_platform_id") == platform_id, f"{owner} target_platform_id drifted")
    require(payload.get("target_triple") == expected["target_triple"], f"{owner} target_triple drifted")
    require(payload.get("runtime_library_names") == expected["runtime_library_names"], f"{owner} runtime libraries drifted")
    require(payload.get("loader_path_policy") == expected["loader_path_policy"], f"{owner} loader policy drifted")
    require(isinstance(payload.get("resolved_runtime_paths"), list), f"{owner} resolved_runtime_paths must be a list")
    require(isinstance(payload.get("driver_linker_flags"), list), f"{owner} driver_linker_flags must be a list")
    source_artifacts = require_source_artifacts(payload, owner)
    require_status(
        payload,
        runtime_load_probe_status(
            source_exists=artifact_exists_in_payload(source_artifacts, NATIVE_EXECUTION_SMOKE_SUMMARY_PATH),
            exit_code=payload.get("load_probe_exit_code", -1),
            native_status=str(payload.get("native_execution_status", "")),
            skip_reason=str(payload.get("skip_reason", "")),
        ),
        owner,
    )
    if payload.get("status") == "generated-host-artifact-present":
        runtime_library = str(payload.get("runtime_library", ""))
        require(
            any(runtime_library.endswith(name) for name in expected["runtime_library_names"]),
            f"{owner} runtime library did not match expected platform runtime",
        )
        require(payload.get("resolved_runtime_paths"), f"{owner} present runtime proof had no resolved runtime paths")


def validate_generated_platform_artifact_content(platform_id: str) -> None:
    validate_object_identity_artifact(platform_id)
    validate_debug_identity_artifact(platform_id)
    validate_runtime_library_manifest_artifact(platform_id)
    validate_install_receipt_artifact(platform_id)
    validate_clean_install_distribution_summary(platform_id)
    validate_clean_install_distribution_verification(platform_id)
    validate_clean_install_distribution_receipt(platform_id)
    validate_runtime_load_probe_artifact(platform_id)


def write_object_identity_artifact(platform_id: str) -> None:
    path_suffix = "build/object-identity.json"
    if platform_artifact_exists(platform_id, path_suffix):
        return
    build_summary = read_json_object(NATIVE_BUILD_SUMMARY_PATH)
    llvm_capabilities_path = platform_scoped_path(platform_id, "llvm-capabilities.json")
    llvm_capabilities = read_json_object(llvm_capabilities_path)
    native_summary = read_json_object(NATIVE_EXECUTION_SMOKE_SUMMARY_PATH)
    expected = expected_platform_identity(platform_id)
    actual = native_build_target_identity(build_summary)
    build_artifact = generated_artifact(NATIVE_BUILD_SUMMARY_PATH)
    record_ids = reviewed_source_record_ids(platform_id)
    build_artifacts = dict_field(build_summary, "artifacts")
    status = generated_identity_status(
        source_exists=bool(build_artifact.get("exists")),
        actual=actual,
        expected=expected,
        fields=("target_platform_id", "target_triple", "object_format"),
    )

    payload = {
        "contract_id": "objc3c.platform.hosted-object-identity.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "record_id": record_ids["object_identity_record_id"],
        "generated_report_path": platform_scoped_path(platform_id, path_suffix),
        "source_summary_path": NATIVE_BUILD_SUMMARY_PATH,
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": status,
        "expected_identity": {
            "target_platform_id": expected["target_platform_id"],
            "target_triple": expected["target_triple"],
            "arch": expected["arch"],
            "object_format": expected["object_format"],
        },
        "actual_identity": actual,
        "native_object_emission": {
            "native_object_emission_status": llvm_capabilities.get(
                "native_object_emission_status",
                llvm_capabilities.get("hosted_native_object_emission_status", ""),
            ),
            "llc_filetype_obj_available": llvm_capabilities.get(
                "llc_filetype_obj_available",
                False,
            ),
            "coherent_toolchain_root": llvm_capabilities.get(
                "coherent_toolchain_root",
                False,
            ),
            "native_execution_object_artifact": native_summary.get("object_artifact", ""),
            "native_execution_object_format": native_summary.get("object_format", ""),
        },
        "build_artifacts": {
            "native_executable": dict_field(build_artifacts, "native_executable"),
            "compile_commands": dict_field(build_artifacts, "compile_commands"),
        },
        "source_artifacts": source_artifacts(
            NATIVE_BUILD_SUMMARY_PATH,
            llvm_capabilities_path,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        ),
    }
    attach_incomplete_artifact_diagnostics(
        payload,
        source_paths=(
            NATIVE_BUILD_SUMMARY_PATH,
            llvm_capabilities_path,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        ),
    )
    write_json(ROOT / payload["generated_report_path"], payload)


def write_debug_identity_artifact(platform_id: str) -> None:
    path_suffix = "build/debug-identity.json"
    if platform_artifact_exists(platform_id, path_suffix):
        return
    build_summary = read_json_object(NATIVE_BUILD_SUMMARY_PATH)
    native_summary = read_json_object(NATIVE_EXECUTION_SMOKE_SUMMARY_PATH)
    expected = expected_platform_identity(platform_id)
    actual = native_build_target_identity(build_summary)
    build_artifact = generated_artifact(NATIVE_BUILD_SUMMARY_PATH)
    record_ids = reviewed_source_record_ids(platform_id)
    build_artifacts = dict_field(build_summary, "artifacts")
    status = generated_identity_status(
        source_exists=bool(build_artifact.get("exists")),
        actual=actual,
        expected=expected,
        fields=("target_platform_id", "target_triple", "debug_format"),
    )

    payload = {
        "contract_id": "objc3c.platform.hosted-debug-identity.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "record_id": record_ids["debug_identity_record_id"],
        "generated_report_path": platform_scoped_path(platform_id, path_suffix),
        "source_summary_path": NATIVE_BUILD_SUMMARY_PATH,
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": status,
        "expected_identity": {
            "target_platform_id": expected["target_platform_id"],
            "target_triple": expected["target_triple"],
            "arch": expected["arch"],
            "debug_format": expected["debug_format"],
        },
        "actual_identity": actual,
        "debug_artifacts": {
            "native_executable": dict_field(build_artifacts, "native_executable"),
            "compile_commands": dict_field(build_artifacts, "compile_commands"),
            "native_execution_debug_format": native_summary.get("debug_format", ""),
        },
        "source_artifacts": source_artifacts(
            NATIVE_BUILD_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        ),
    }
    attach_incomplete_artifact_diagnostics(
        payload,
        source_paths=(
            NATIVE_BUILD_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        ),
    )
    write_json(ROOT / payload["generated_report_path"], payload)


def runtime_library_source_paths(package_manifest: dict[str, Any], platform_id: str) -> list[str]:
    expected = expected_platform_identity(platform_id)
    runtime_library = str(
        package_manifest.get(
            "runtime_library",
            f"artifacts/lib/{expected['runtime_library_names'][0]}",
        )
    )
    package_root = str(package_manifest.get("package_root", ""))
    paths = [runtime_library]
    if package_root:
        paths.insert(0, prefixed_repo_path(package_root, runtime_library))
    return paths


def write_runtime_library_manifest_artifact(platform_id: str) -> None:
    path_suffix = "package/runtime-library-manifest.json"
    if platform_artifact_exists(platform_id, path_suffix):
        return
    package_manifest = read_json_object(RUNNABLE_PACKAGE_MANIFEST_PATH)
    build_summary = read_json_object(NATIVE_BUILD_SUMMARY_PATH)
    expected = expected_platform_identity(platform_id)
    runtime_paths = runtime_library_source_paths(package_manifest, platform_id)
    runtime_artifacts = source_artifacts(*runtime_paths)
    package_artifact = generated_artifact(RUNNABLE_PACKAGE_MANIFEST_PATH)
    package_target_platform_id = str(package_manifest.get("target_platform_id", ""))
    status = runtime_manifest_status(
        source_exists=bool(package_artifact.get("exists")),
        runtime_artifacts=runtime_artifacts,
        package_target_platform_id=package_target_platform_id,
        platform_id=platform_id,
    )

    payload = {
        "contract_id": "objc3c.platform.hosted-runtime-library-manifest.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "generated_report_path": platform_scoped_path(platform_id, path_suffix),
        "source_package_manifest_path": RUNNABLE_PACKAGE_MANIFEST_PATH,
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": status,
        "target_platform_id": platform_id,
        "source_package_target_platform_id": package_target_platform_id,
        "target_triple": expected["target_triple"],
        "runtime_library_kind": package_manifest.get(
            "runtime_library_kind",
            dict_field(build_summary, "target").get("runtime_library_kind", ""),
        ),
        "runtime_library_names": expected["runtime_library_names"],
        "runtime_library_artifacts": runtime_artifacts,
        "loader_path_policy": expected["loader_path_policy"],
        "package_root": package_manifest.get("package_root", ""),
        "package_root_layout": package_manifest.get("package_root_layout", []),
        "package_manifest_artifact": package_artifact,
        "source_artifacts": source_artifacts(
            RUNNABLE_PACKAGE_MANIFEST_PATH,
            NATIVE_BUILD_SUMMARY_PATH,
        ),
    }
    attach_incomplete_artifact_diagnostics(
        payload,
        source_paths=(
            *existing_artifact_paths(
                RUNNABLE_PACKAGE_MANIFEST_PATH,
                NATIVE_BUILD_SUMMARY_PATH,
                *runtime_paths,
            ),
        ),
    )
    write_json(ROOT / payload["generated_report_path"], payload)


def find_install_receipt_source(end_to_end_summary: dict[str, Any]) -> str:
    for root_field in ("offline_install_root", "install_root"):
        root_text = str(end_to_end_summary.get(root_field, ""))
        if not root_text:
            continue
        receipt_path = repo_or_absolute_path(root_text) / "objc3c-install-receipt.json"
        if receipt_path.is_file():
            return repo_rel(receipt_path)
    return ""


def write_install_receipt_artifact(platform_id: str) -> None:
    path_suffix = "install/install-receipt.json"
    if platform_artifact_exists(platform_id, path_suffix):
        return
    end_to_end_summary = read_json_object(PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH)
    package_manifest = read_json_object(RUNNABLE_PACKAGE_MANIFEST_PATH)
    source_receipt_path = find_install_receipt_source(end_to_end_summary)
    source_receipt = read_json_object(source_receipt_path) if source_receipt_path else {}
    installed_root_execution = dict_field(end_to_end_summary, "installed_root_execution")
    offline_installed_root_execution = dict_field(
        end_to_end_summary,
        "offline_installed_root_execution",
    )
    record_ids = reviewed_source_record_ids(platform_id)
    expected = expected_platform_identity(platform_id)
    status = install_receipt_status(
        source_receipt_path=source_receipt_path,
        source_receipt=source_receipt,
        platform_id=platform_id,
    )

    payload = {
        "contract_id": "objc3c.platform.hosted-install-receipt.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "record_id": record_ids["package_install_identity_record_id"],
        "generated_report_path": platform_scoped_path(platform_id, path_suffix),
        "source_summary_path": PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
        "source_install_receipt_path": source_receipt_path,
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": status,
        "target_platform_id": platform_id,
        "target_triple": expected["target_triple"],
        "package_root": package_manifest.get("package_root", ""),
        "package_root_layout": package_manifest.get("package_root_layout", []),
        "package_manifest": RUNNABLE_PACKAGE_MANIFEST_PATH,
        "package_manifest_artifact": generated_artifact(RUNNABLE_PACKAGE_MANIFEST_PATH),
        "package_channels_summary_artifact": generated_artifact(
            PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH
        ),
        "source_install_receipt_artifact": (
            generated_artifact(source_receipt_path) if source_receipt_path else {"exists": False}
        ),
        "source_install_receipt": source_receipt,
        "installed_root_execution": installed_root_execution,
        "offline_installed_root_execution": offline_installed_root_execution,
        "installed_root_execution_status": installed_root_execution.get("status", ""),
        "offline_installed_root_execution_status": offline_installed_root_execution.get("status", ""),
        "source_artifacts": source_artifacts(
            PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
            RUNNABLE_PACKAGE_MANIFEST_PATH,
            source_receipt_path,
        ),
    }
    attach_incomplete_artifact_diagnostics(
        payload,
        source_paths=(
            *existing_artifact_paths(
                PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
                RUNNABLE_PACKAGE_MANIFEST_PATH,
                source_receipt_path,
            ),
        ),
    )
    write_json(ROOT / payload["generated_report_path"], payload)


def write_runtime_load_probe_artifact(platform_id: str) -> None:
    path_suffix = "execution/runtime-load-probe.json"
    if platform_artifact_exists(platform_id, path_suffix):
        return
    native_summary = read_json_object(NATIVE_EXECUTION_SMOKE_SUMMARY_PATH)
    hosted_summary = read_json_object(HOSTED_EXECUTION_SMOKE_SUMMARY_PATH)
    package_manifest = read_json_object(RUNNABLE_PACKAGE_MANIFEST_PATH)
    expected = expected_platform_identity(platform_id)
    record_ids = reviewed_source_record_ids(platform_id)
    exit_code = native_summary.get("exit_code", -1)
    native_summary_exists = bool(generated_artifact(NATIVE_EXECUTION_SMOKE_SUMMARY_PATH).get("exists"))
    native_status = str(native_summary.get("status", ""))
    skip_reason = str(native_summary.get("skip_reason", hosted_summary.get("skip_reason", "")))
    load_probe_exit_code = (
        exit_code
        if isinstance(exit_code, int)
        and native_status.upper() not in {"UNAVAILABLE", "SKIP", "SKIPPED"}
        else -1
    )
    status = runtime_load_probe_status(
        source_exists=native_summary_exists,
        exit_code=exit_code,
        native_status=native_status,
        skip_reason=skip_reason,
    )

    payload = {
        "contract_id": "objc3c.platform.hosted-runtime-load-probe.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "record_id": record_ids["runtime_load_link_proof_record_id"],
        "generated_report_path": platform_scoped_path(platform_id, path_suffix),
        "source_summary_path": NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": status,
        "target_platform_id": platform_id,
        "target_triple": expected["target_triple"],
        "runtime_library_names": expected["runtime_library_names"],
        "runtime_library": native_summary.get(
            "runtime_library",
            package_manifest.get(
                "runtime_library",
                f"artifacts/lib/{expected['runtime_library_names'][0]}",
            ),
        ),
        "runtime_library_kind": native_summary.get(
            "runtime_library_kind",
            package_manifest.get("runtime_library_kind", ""),
        ),
        "runtime_load_environment_variable": native_summary.get(
            "runtime_load_environment_variable",
            "",
        ),
        "loader_path_policy": native_summary.get(
            "loader_path_policy",
            expected["loader_path_policy"],
        ),
        "load_probe_exit_code": load_probe_exit_code,
        "resolved_runtime_paths": list_field(native_summary, "load_path"),
        "driver_linker_flags": list_field(native_summary, "driver_linker_flags"),
        "hosted_execution_status": hosted_summary.get("status", ""),
        "native_execution_status": native_status,
        "skip_reason": skip_reason,
        "source_artifacts": source_artifacts(
            HOSTED_EXECUTION_SMOKE_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
            RUNNABLE_PACKAGE_MANIFEST_PATH,
        ),
    }
    attach_incomplete_artifact_diagnostics(
        payload,
        source_paths=(
            HOSTED_EXECUTION_SMOKE_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
            RUNNABLE_PACKAGE_MANIFEST_PATH,
        ),
    )
    write_json(ROOT / payload["generated_report_path"], payload)


def ensure_platform_promotion_artifacts(platform_id: str) -> None:
    write_object_identity_artifact(platform_id)
    write_debug_identity_artifact(platform_id)
    write_runtime_library_manifest_artifact(platform_id)
    write_install_receipt_artifact(platform_id)
    write_runtime_load_probe_artifact(platform_id)


def env_outcome(step_id: str) -> str:
    value = os.environ.get(OUTCOME_ENV.get(step_id, ""), "")
    return value if value else "not-recorded"


def platform_report_path(platform_id: str, filename: str) -> Path:
    return REPORT_ROOT / platform_id / filename


def build_host_identity(platform_id: str, runner_label: str) -> dict[str, Any]:
    config = PLATFORM_CONFIG[platform_id]
    return {
        "platform_id": platform_id,
        "expected_host_os": config["host_os"],
        "expected_host_arch": config["host_arch"],
        "expected_host_triple": config["host_triple"],
        "runner_label": runner_label,
        "runner_os": os.environ.get("RUNNER_OS", ""),
        "runner_arch": os.environ.get("RUNNER_ARCH", ""),
        "image_os": os.environ.get("ImageOS", ""),
        "image_version": os.environ.get("ImageVersion", ""),
        "platform_system": platform.system(),
        "platform_machine": platform.machine(),
        "python_version": platform.python_version(),
    }


def platform_scoped_path(platform_id: str, path_suffix: str) -> str:
    return f"tmp/reports/platform-host-evidence/{platform_id}/{path_suffix}"


def build_artifact_identity_reference(platform_id: str) -> dict[str, Any]:
    config = PLATFORM_CONFIG[platform_id]
    return {
        "package_variant_row_id": config["package_variant_row_id"],
        "package_root_record_id": config["package_root_record_id"],
        "native_execution_record_id": config["native_execution_record_id"],
        "reviewed_source_record_ids": reviewed_source_record_ids(platform_id),
        "object_format": config["object_format"],
        "debug_format": config["debug_format"],
        "runtime_library_names": config["runtime_library_names"],
        "loader_path_policy": config["loader_path_policy"],
        "package_root_layout": config["package_root_layout"],
        "source_truth_reference": (
            "tests/tooling/fixtures/platform_hardening/"
            f"platform_toolchain_support_evidence.json#{config['package_variant_row_id']}"
        ),
        "support_truth": False,
    }


def reviewed_source_record_ids(platform_id: str) -> dict[str, str]:
    return host_evidence_review_record_ids(platform_id)


def build_reviewed_source_field_requirements(platform_id: str) -> list[dict[str, Any]]:
    record_ids = reviewed_source_record_ids(platform_id)
    requirements: list[dict[str, Any]] = []
    for field_contract in PROMOTION_REVIEWED_SOURCE_FIELD_CONTRACTS:
        required_record_id_field = field_contract["required_record_id_field"]
        requirements.append(
            {
                **field_contract,
                "required_record_id": record_ids[required_record_id_field],
                "generated_report_path": platform_scoped_path(
                    platform_id,
                    field_contract["generated_report_path_suffix"],
                ),
                "reviewed_source_required": True,
                "generated_report_support_truth": False,
                "promotion_allowed_from_generated_evidence": False,
            }
        )
    return requirements


def build_promotion_readiness_requirements(platform_id: str) -> dict[str, Any]:
    artifact_identity = build_artifact_identity_reference(platform_id)
    reviewed_source_field_requirements = build_reviewed_source_field_requirements(platform_id)
    return {
        "contract_id": "objc3c.platform.hosted-evidence.promotion-readiness.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(PLATFORM_CONFIG[platform_id]["issue_ref"]),
        "canonical_workflow_path": WORKFLOW_PATH,
        "accepted_workflow_paths": list(ACCEPTED_WORKFLOW_PATHS),
        "dispatch_gateway_workflow_paths": list(DISPATCH_GATEWAY_WORKFLOW_PATHS),
        "support_claim_published": False,
        "source_truth_update_allowed": False,
        "review_candidate_source_truth_path": (
            host_evidence_review_candidate_path_for_platform(platform_id)
        ),
        "review_candidate_source_truth_path_template": (
            HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE
        ),
        "review_candidate_contract_id": HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
        "generated_only_result": "refuse-source-truth-promotion",
        "review_promotion_policy": "checked-in-source-truth-required",
        "stale_evidence_allowed": False,
        "prose_only_evidence_allowed": False,
        "local_temp_claims_promote_support": False,
        "required_review_fields": list(PROMOTION_REVIEW_REQUIRED_FIELDS),
        "required_source_record_types": list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES),
        "required_reviewed_source_fields": list(PROMOTION_REVIEWED_SOURCE_FIELDS),
        "reviewed_source_field_requirements": reviewed_source_field_requirements,
        "required_promotion_evidence_classes": list(PROMOTION_BLOCKING_EVIDENCE_CLASSES),
        "required_durable_promotion_artifact_suffixes": list(
            REQUIRED_DURABLE_PROMOTION_ARTIFACT_SUFFIXES
        ),
        "artifact_identity_reference": artifact_identity,
        "support_rows_remain_fail_closed_until_reviewed": True,
        "hosted_artifact_references": [
            {
                "reference_id": "toolchain-probe",
                "evidence_class": "toolchain",
                "command": "python scripts/probe_objc3c_llvm_capabilities.py",
                "path": platform_scoped_path(platform_id, "llvm-capabilities.json"),
                "required_fields": [
                    "native_object_emission_status",
                    "llc_filetype_obj_available",
                    "coherent_toolchain_root",
                ],
            },
            {
                "reference_id": "native-build",
                "evidence_class": "build",
                "command": "npm run objc3c -- build-native-binaries",
                "path": platform_scoped_path(platform_id, "build/native_build_summary.json"),
                "required_fields": [
                    "artifacts.native_executable",
                    "artifacts.runtime_library",
                    "artifacts.compile_commands",
                ],
            },
            {
                "reference_id": "runnable-package",
                "evidence_class": "package",
                "command": "npm run objc3c -- package-runnable-toolchain",
                "path": platform_scoped_path(platform_id, "package/objc3c-runnable-toolchain-package.json"),
                "required_fields": [
                    "native_executable",
                    "runtime_library",
                    "package_root",
                    "manifest_artifact",
                ],
            },
            {
                "reference_id": "package-install",
                "evidence_class": "install",
                "command": "npm run objc3c -- validate-packaging-channels-end-to-end",
                "path": platform_scoped_path(platform_id, "install/end-to-end-summary.json"),
                "required_fields": [
                    "install",
                    "package",
                    "runtime_library",
                    "loader_path",
                    "installed_root_execution.status",
                    "offline_installed_root_execution.status",
                    "installed_root_execution.execution_source",
                    "offline_installed_root_execution.execution_source",
                    "reviewed_source_field_requirements.package_install_identity",
                ],
            },
            {
                "reference_id": "clean-package-install-distribution",
                "evidence_class": "install",
                "command": "npm run objc3c -- validate-package-install-distribution --from-nothing",
                "path": platform_scoped_path(
                    platform_id,
                    "install/install-distribution-credibility-summary.json",
                ),
                "required_fields": [
                    "install_receipt",
                    "update_receipt",
                    "uninstall_receipt",
                    "install_verification",
                    "from_nothing_probe.preexisting_outputs_absent",
                    "platform_host_evidence.install_receipt",
                ],
            },
            {
                "reference_id": "object-format-debug",
                "evidence_class": "object-format-debug",
                "path": platform_scoped_path(platform_id, "promotion-readiness-requirements.json"),
                "required_fields": [
                    "artifact_identity_reference.object_format",
                    "artifact_identity_reference.debug_format",
                    "reviewed_source_field_requirements.object_identity",
                    "reviewed_source_field_requirements.debug_identity",
                ],
                "expected_values": {
                    "object_format": artifact_identity["object_format"],
                    "debug_format": artifact_identity["debug_format"],
                },
            },
            {
                "reference_id": "runtime-link-load",
                "evidence_class": "runtime-link-load",
                "path": platform_scoped_path(platform_id, "promotion-readiness-requirements.json"),
                "required_fields": [
                    "artifact_identity_reference.runtime_library_names",
                    "artifact_identity_reference.loader_path_policy",
                    "reviewed_source_field_requirements.runtime_load_link_proof",
                    "execution.native_execution_summary",
                ],
                "expected_values": {
                    "runtime_library_names": artifact_identity["runtime_library_names"],
                    "loader_path_policy": artifact_identity["loader_path_policy"],
                },
            },
            {
                "reference_id": "hosted-execution-smoke",
                "evidence_class": "execution",
                "command": "npm run objc3c -- test-hosted-execution-smoke",
                "path": platform_scoped_path(platform_id, "execution/hosted-execution-smoke-summary.json"),
                "required_fields": [
                    "status",
                    "native_object_emission",
                    "skip_reason",
                ],
            },
            {
                "reference_id": "native-execution-smoke",
                "evidence_class": "execution",
                "command": "npm run objc3c -- test-hosted-execution-smoke",
                "path": platform_scoped_path(platform_id, "execution/native-execution-smoke-summary.json"),
                "required_fields": [
                    "status",
                    "results",
                    "runtime_library",
                    "native_object_emission",
                    "skip_reason",
                    "link_command",
                    "load_path",
                ],
            },
        ],
        "promotion_blockers_until_reviewed": [
            "generated hosted reports are not source truth",
            "Linux and macOS support rows remain fail-closed until checked-in source rows are promoted",
            "object-format/debug and runtime link/load expectations require matching real host artifacts",
            "native execution must consume the package-root runtime library on the target host",
        ],
    }


def build_review_candidate_source_truth(
    platform_id: str,
    *,
    workflow_path: str,
    runner_label: str,
) -> dict[str, Any]:
    config = PLATFORM_CONFIG[platform_id]
    required_checked_source_paths = [
        "tests/tooling/fixtures/platform_hardening/host_promotion_reviewed_source_inputs.json",
        "tests/tooling/fixtures/platform_hardening/platform_host_promotion_evidence_contract.json",
        "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
        "tests/tooling/fixtures/platform_support/source_truth_matrix.json",
        "docs/runbooks/objc3c_platform_toolchain_support_matrix.md",
    ]
    candidate_rows: list[dict[str, Any]] = []
    for target in host_evidence_review_candidate_targets(platform_id):
        artifact_paths = [
            platform_scoped_path(platform_id, suffix)
            for suffix in target["artifact_suffixes"]
        ]
        artifacts = [generated_artifact(path) for path in artifact_paths]
        incomplete_diagnostics = []
        required_source_artifacts: list[str] = []
        for artifact in artifacts:
            if (
                artifact.get("exists") is True
                and artifact.get("fail_closed_placeholder") is not True
                and artifact.get("incomplete_generated_artifact") is not True
            ):
                continue
            artifact_diagnostics = artifact.get("diagnostics")
            required_sources = artifact.get("required_source_artifacts")
            if not isinstance(required_sources, list):
                required_sources = []
            for source_path in required_sources:
                normalized_source = str(source_path).replace("\\", "/")
                if normalized_source and normalized_source not in required_source_artifacts:
                    required_source_artifacts.append(normalized_source)
            incomplete_diagnostics.append(
                {
                    "path": str(artifact.get("path", "")).replace("\\", "/"),
                    "exists": artifact.get("exists") is True,
                    "status": str(artifact.get("status", "missing-generated-artifact")),
                    "fail_closed_placeholder": artifact.get("fail_closed_placeholder") is True,
                    "diagnostics": artifact_diagnostics
                    if isinstance(artifact_diagnostics, dict)
                    else None,
                    "required_source_artifacts": required_sources,
                }
            )
        candidate_rows.append(
            {
                **target,
                "platform_id": platform_id,
                "issue_ref": int(config["issue_ref"]),
                "generated_artifact_paths": artifact_paths,
                "generated_artifacts": artifacts,
                "generated_artifacts_complete": all(
                    artifact.get("exists") is True
                    and artifact.get("fail_closed_placeholder") is not True
                    and artifact.get("incomplete_generated_artifact") is not True
                    for artifact in artifacts
                ),
                "incomplete_diagnostics": incomplete_diagnostics,
                "required_source_artifacts": required_source_artifacts,
                "review_status": "pending-reviewed-source-truth",
                "promotion_allowed": False,
                "support_truth": False,
                "generated_report_support_truth": False,
                "source_truth_update_allowed": False,
                "required_checked_source_paths": required_checked_source_paths,
            }
        )
    return {
        "contract_id": HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(config["issue_ref"]),
        "workflow_path": workflow_path,
        "canonical_workflow_path": WORKFLOW_PATH,
        "accepted_workflow_paths": list(ACCEPTED_WORKFLOW_PATHS),
        "dispatch_gateway_workflow_paths": list(DISPATCH_GATEWAY_WORKFLOW_PATHS),
        "runner_label": runner_label,
        "candidate_path": host_evidence_review_candidate_path_for_platform(platform_id),
        "candidate_source_truth_path_template": (
            HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE
        ),
        "generated_report_only": True,
        "generated_only_result": "refuse-source-truth-promotion",
        "review_promotion_policy": "checked-in-source-truth-required",
        "reviewed_source_truth_required": True,
        "support_rows_remain_fail_closed_until_reviewed": True,
        "support_claim_published": False,
        "source_truth_update_allowed": False,
        "promotion_allowed": False,
        "support_truth": False,
        "local_temp_claims_promote_support": False,
        "prose_only_evidence_allowed": False,
        "stale_evidence_allowed": False,
        "required_source_record_types": list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES),
        "required_hosted_review_input_suffixes": list(
            HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES
        ),
        "required_hosted_review_input_paths": (
            host_evidence_required_review_input_paths_for_platform(platform_id)
        ),
        "generated_report_paths": (
            host_evidence_generated_report_paths_for_platform(platform_id)
        ),
        "review_candidate_rows": candidate_rows,
        "required_checked_source_paths": required_checked_source_paths,
    }


def build_report(args: argparse.Namespace) -> dict[str, Any]:
    platform_id = args.platform_id
    config = PLATFORM_CONFIG[platform_id]
    ensure_platform_promotion_artifacts(platform_id)
    steps: list[dict[str, Any]] = [
        {
            "step_id": "dependency_install",
            "evidence_class": "host-setup",
            "outcome": env_outcome("dependency_install"),
            "generated_report_paths": [],
            "generated_artifacts": [],
        },
        {
            "step_id": "toolchain_setup",
            "evidence_class": "host-setup",
            "outcome": env_outcome("toolchain_setup"),
            "generated_report_paths": [],
            "generated_artifacts": [],
        },
    ]
    for step_id, evidence_class, path_pairs in STEP_CONTRACTS:
        outcome = env_outcome(step_id)
        generated_paths = [
            scoped_path.format(platform_id=platform_id)
            for _, scoped_path in path_pairs
        ]
        steps.append(
            {
                "step_id": step_id,
                "evidence_class": evidence_class,
                "outcome": outcome,
                "generated_report_paths": generated_paths,
                "generated_artifacts": [
                    materialize_generated_artifact(
                        source_path.format(platform_id=platform_id),
                        scoped_path.format(platform_id=platform_id),
                        platform_id=platform_id,
                        step_id=step_id,
                        evidence_class=evidence_class,
                        outcome=outcome,
                    )
                    for source_path, scoped_path in path_pairs
                ],
            }
        )

    promotion_readiness = build_promotion_readiness_requirements(platform_id)
    return {
        "contract_id": "objc3c.platform.hosted-runner.evidence-report.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": int(config["issue_ref"]),
        "workflow_path": args.workflow_path,
        "canonical_workflow_path": WORKFLOW_PATH,
        "accepted_workflow_paths": list(ACCEPTED_WORKFLOW_PATHS),
        "dispatch_gateway_workflow_paths": list(DISPATCH_GATEWAY_WORKFLOW_PATHS),
        "runner_label": args.runner_label,
        "github": {
            "run_id": os.environ.get("GITHUB_RUN_ID", ""),
            "run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT", ""),
            "workflow": os.environ.get("GITHUB_WORKFLOW", ""),
            "job": os.environ.get("GITHUB_JOB", ""),
            "sha": os.environ.get("GITHUB_SHA", ""),
            "ref": os.environ.get("GITHUB_REF", ""),
        },
        "host_identity": build_host_identity(platform_id, args.runner_label),
        "required_evidence_classes": [
            "build",
            "package",
            "install",
            "execution",
        ],
        "required_review_fields": list(PROMOTION_REVIEW_REQUIRED_FIELDS),
        "required_source_record_types": list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES),
        "required_reviewed_source_fields": list(PROMOTION_REVIEWED_SOURCE_FIELDS),
        "reviewed_source_field_requirements": promotion_readiness[
            "reviewed_source_field_requirements"
        ],
        "artifact_identity_reference": promotion_readiness["artifact_identity_reference"],
        "promotion_readiness_requirements": promotion_readiness,
        "generated_report_paths": host_evidence_generated_report_paths_for_platform(
            platform_id
        ),
        "review_candidate_source_truth_path": (
            host_evidence_review_candidate_path_for_platform(platform_id)
        ),
        "review_candidate_source_truth_path_template": (
            HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE
        ),
        "review_candidate_contract_id": HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
        "artifact_upload": {
            "artifact_name": f"objc3c-platform-host-evidence-{platform_id}",
            "upload_root": f"tmp/reports/platform-host-evidence/{platform_id}",
            "path_glob": f"tmp/reports/platform-host-evidence/{platform_id}/**",
            "if_no_files_found": "error",
            "readback_scope": "single-platform-host-evidence-root",
        },
        "steps": steps,
        "source_truth_ingestion": {
            "generated_report_only": True,
            "generated_only_result": "refuse-source-truth-promotion",
            "review_required": True,
            "review_promotion_policy": "checked-in-source-truth-required",
            "stale_evidence_allowed": False,
            "prose_only_evidence_allowed": False,
            "local_temp_claims_promote_support": False,
            "required_reviewed_source_fields": list(PROMOTION_REVIEWED_SOURCE_FIELDS),
            "required_source_record_types": list(
                HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES
            ),
            "review_candidate_source_truth_path": (
                host_evidence_review_candidate_path_for_platform(platform_id)
            ),
            "review_candidate_contract_id": HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
            "source_truth_update_allowed": False,
            "support_claim_published": False,
            "fail_closed_evidence_id": config["fail_closed_evidence_id"],
            "required_checked_source_paths": [
                "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
                "tests/tooling/fixtures/platform_support/source_truth_matrix.json",
                "docs/runbooks/objc3c_platform_toolchain_support_matrix.md",
            ],
        },
    }


def load_report(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"host evidence report is not a JSON object: {repo_rel(path)}")
    return payload


def validate_report(report: dict[str, Any], platform_id: str) -> list[str]:
    if report.get("contract_id") != "objc3c.platform.hosted-runner.evidence-report.v1":
        raise RuntimeError("host evidence report contract_id drifted")
    if report.get("platform_id") != platform_id:
        raise RuntimeError("host evidence report platform_id drifted")
    workflow_path = str(report.get("workflow_path", ""))
    if workflow_path not in ACCEPTED_WORKFLOW_PATHS:
        raise RuntimeError("host evidence report workflow_path drifted")
    if report.get("canonical_workflow_path") != WORKFLOW_PATH:
        raise RuntimeError("host evidence report canonical workflow path drifted")
    if report.get("accepted_workflow_paths") != list(ACCEPTED_WORKFLOW_PATHS):
        raise RuntimeError("host evidence report accepted workflow paths drifted")
    if report.get("dispatch_gateway_workflow_paths") != list(DISPATCH_GATEWAY_WORKFLOW_PATHS):
        raise RuntimeError("host evidence report dispatch gateway workflow paths drifted")
    expected_runner_label = PLATFORM_CONFIG[platform_id]["runner_label"]
    if report.get("runner_label") != expected_runner_label:
        raise RuntimeError("host evidence report runner_label drifted")

    upload = report.get("artifact_upload")
    expected_upload_root = f"tmp/reports/platform-host-evidence/{platform_id}"
    if not isinstance(upload, dict):
        raise RuntimeError("host evidence report missing artifact_upload")
    if upload.get("artifact_name") != f"objc3c-platform-host-evidence-{platform_id}":
        raise RuntimeError("host evidence report artifact name drifted")
    if upload.get("upload_root") != expected_upload_root:
        raise RuntimeError("host evidence report upload root drifted")
    if upload.get("path_glob") != f"{expected_upload_root}/**":
        raise RuntimeError("host evidence report upload path glob drifted")
    if upload.get("if_no_files_found") != "error":
        raise RuntimeError("host evidence report upload must fail closed when no files are found")
    if upload.get("readback_scope") != "single-platform-host-evidence-root":
        raise RuntimeError("host evidence report readback scope drifted")

    ingestion = report.get("source_truth_ingestion")
    if not isinstance(ingestion, dict):
        raise RuntimeError("host evidence report missing source_truth_ingestion")
    if ingestion.get("generated_report_only") is not True:
        raise RuntimeError("host evidence report did not mark generated_report_only")
    if ingestion.get("generated_only_result") != "refuse-source-truth-promotion":
        raise RuntimeError("host evidence report did not refuse generated-only promotion")
    if ingestion.get("review_required") is not True:
        raise RuntimeError("host evidence report did not require review")
    if ingestion.get("review_promotion_policy") != "checked-in-source-truth-required":
        raise RuntimeError("host evidence report review policy drifted")
    if ingestion.get("stale_evidence_allowed") is not False:
        raise RuntimeError("host evidence report allowed stale evidence")
    if ingestion.get("prose_only_evidence_allowed") is not False:
        raise RuntimeError("host evidence report allowed prose-only evidence")
    if ingestion.get("local_temp_claims_promote_support") is not False:
        raise RuntimeError("host evidence report allowed local temp promotion claims")
    if ingestion.get("source_truth_update_allowed") is not False:
        raise RuntimeError("generated host evidence attempted to update source truth")
    if ingestion.get("support_claim_published") is not False:
        raise RuntimeError("generated host evidence attempted to publish support")

    required_classes = {"build", "package", "install", "execution"}
    if report.get("required_review_fields") != list(PROMOTION_REVIEW_REQUIRED_FIELDS):
        raise RuntimeError("host evidence report required review fields drifted")
    if report.get("required_source_record_types") != list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES):
        raise RuntimeError("host evidence report required source record types drifted")
    if report.get("required_reviewed_source_fields") != list(PROMOTION_REVIEWED_SOURCE_FIELDS):
        raise RuntimeError("host evidence report reviewed source fields drifted")
    if report.get("reviewed_source_field_requirements") != build_reviewed_source_field_requirements(platform_id):
        raise RuntimeError("host evidence report reviewed source requirements drifted")
    artifact_identity = report.get("artifact_identity_reference")
    expected_identity = build_artifact_identity_reference(platform_id)
    if artifact_identity != expected_identity:
        raise RuntimeError("host evidence report artifact identity reference drifted")
    promotion_readiness = report.get("promotion_readiness_requirements")
    if not isinstance(promotion_readiness, dict):
        raise RuntimeError("host evidence report missing promotion readiness requirements")
    if promotion_readiness.get("contract_id") != "objc3c.platform.hosted-evidence.promotion-readiness.v1":
        raise RuntimeError("host evidence promotion readiness contract drifted")
    if promotion_readiness.get("platform_id") != platform_id:
        raise RuntimeError("host evidence promotion readiness platform_id drifted")
    if promotion_readiness.get("canonical_workflow_path") != WORKFLOW_PATH:
        raise RuntimeError("host evidence promotion readiness canonical workflow path drifted")
    if promotion_readiness.get("accepted_workflow_paths") != list(ACCEPTED_WORKFLOW_PATHS):
        raise RuntimeError("host evidence promotion readiness accepted workflow paths drifted")
    if promotion_readiness.get("dispatch_gateway_workflow_paths") != list(DISPATCH_GATEWAY_WORKFLOW_PATHS):
        raise RuntimeError("host evidence promotion readiness dispatch gateway workflow paths drifted")
    if promotion_readiness.get("support_claim_published") is not False:
        raise RuntimeError("host evidence promotion readiness attempted to publish support")
    if promotion_readiness.get("source_truth_update_allowed") is not False:
        raise RuntimeError("host evidence promotion readiness attempted to update source truth")
    if promotion_readiness.get("stale_evidence_allowed") is not False:
        raise RuntimeError("host evidence promotion readiness allowed stale evidence")
    if promotion_readiness.get("prose_only_evidence_allowed") is not False:
        raise RuntimeError("host evidence promotion readiness allowed prose-only evidence")
    if promotion_readiness.get("local_temp_claims_promote_support") is not False:
        raise RuntimeError("host evidence promotion readiness allowed local temp promotion claims")
    if promotion_readiness.get("artifact_identity_reference") != expected_identity:
        raise RuntimeError("host evidence promotion readiness artifact identity drifted")
    if promotion_readiness.get("required_review_fields") != list(PROMOTION_REVIEW_REQUIRED_FIELDS):
        raise RuntimeError("host evidence promotion readiness required fields drifted")
    if promotion_readiness.get("required_reviewed_source_fields") != list(PROMOTION_REVIEWED_SOURCE_FIELDS):
        raise RuntimeError("host evidence promotion readiness reviewed source fields drifted")
    if promotion_readiness.get("reviewed_source_field_requirements") != build_reviewed_source_field_requirements(platform_id):
        raise RuntimeError("host evidence promotion readiness reviewed source requirements drifted")
    if promotion_readiness.get("support_rows_remain_fail_closed_until_reviewed") is not True:
        raise RuntimeError("host evidence promotion readiness did not keep support rows fail-closed")
    expected_candidate_path = host_evidence_review_candidate_path_for_platform(platform_id)
    if report.get("review_candidate_source_truth_path") != expected_candidate_path:
        raise RuntimeError("host evidence report review candidate path drifted")
    if report.get("review_candidate_contract_id") != HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID:
        raise RuntimeError("host evidence report review candidate contract drifted")
    if promotion_readiness.get("review_candidate_source_truth_path") != expected_candidate_path:
        raise RuntimeError("host evidence promotion readiness review candidate path drifted")
    if promotion_readiness.get("review_candidate_contract_id") != HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID:
        raise RuntimeError("host evidence promotion readiness review candidate contract drifted")
    if promotion_readiness.get("required_source_record_types") != list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES):
        raise RuntimeError("host evidence promotion readiness source record types drifted")
    if promotion_readiness.get("required_promotion_evidence_classes") != list(PROMOTION_BLOCKING_EVIDENCE_CLASSES):
        raise RuntimeError("host evidence promotion readiness required evidence classes drifted")
    if promotion_readiness.get("required_durable_promotion_artifact_suffixes") != list(REQUIRED_DURABLE_PROMOTION_ARTIFACT_SUFFIXES):
        raise RuntimeError("host evidence promotion readiness durable artifact suffixes drifted")
    if report.get("generated_report_paths") != host_evidence_generated_report_paths_for_platform(platform_id):
        raise RuntimeError("host evidence report generated path inventory drifted")
    expected_path_prefix = f"tmp/reports/platform-host-evidence/{platform_id}/"
    for reference in promotion_readiness.get("hosted_artifact_references", []):
        if not isinstance(reference, dict):
            raise RuntimeError("host evidence promotion readiness references must be objects")
        path_text = str(reference.get("path", "")).replace("\\", "/")
        if not path_text.startswith(expected_path_prefix):
            raise RuntimeError(f"host evidence promotion readiness used non-platform-scoped path: {path_text}")
    for requirement in promotion_readiness.get("reviewed_source_field_requirements", []):
        if not isinstance(requirement, dict):
            raise RuntimeError("host evidence reviewed source requirements must be objects")
        if requirement.get("reviewed_source_required") is not True:
            raise RuntimeError("host evidence reviewed source requirement did not require review")
        if requirement.get("generated_report_support_truth") is not False:
            raise RuntimeError("host evidence generated report became reviewed source truth")
        if requirement.get("promotion_allowed_from_generated_evidence") is not False:
            raise RuntimeError("host evidence generated report allowed promotion")
        path_text = str(requirement.get("generated_report_path", "")).replace("\\", "/")
        if not path_text.startswith(expected_path_prefix):
            raise RuntimeError(f"host evidence reviewed source path left platform scope: {path_text}")

    seen_classes = {
        str(step.get("evidence_class", ""))
        for step in report.get("steps", [])
        if isinstance(step, dict)
    }
    missing = sorted(required_classes - seen_classes)
    if missing:
        raise RuntimeError(f"host evidence report missing evidence classes: {', '.join(missing)}")

    generated_paths: list[str] = []
    for step in report.get("steps", []):
        if not isinstance(step, dict):
            continue
        for raw_path in step.get("generated_report_paths", []):
            path_text = str(raw_path).replace("\\", "/")
            if path_text and not path_text.startswith(("tmp/", "artifacts/")):
                raise RuntimeError(f"host evidence report used non-generated path: {path_text}")
            if path_text and not path_text.startswith(expected_path_prefix):
                raise RuntimeError(f"host evidence report used non-platform-scoped path: {path_text}")
            if path_text:
                generated_paths.append(path_text)
        for artifact in step.get("generated_artifacts", []):
            if not isinstance(artifact, dict):
                raise RuntimeError("host evidence report generated_artifacts entries must be objects")
            path_text = str(artifact.get("path", "")).replace("\\", "/")
            if path_text and not path_text.startswith(expected_path_prefix):
                raise RuntimeError(f"host evidence artifact used non-platform-scoped path: {path_text}")
    required_artifact_paths = {
        platform_scoped_path(platform_id, suffix)
        for suffix in REQUIRED_DURABLE_PROMOTION_ARTIFACT_SUFFIXES
    }
    declared_generated_paths = set(generated_paths)
    declared_generated_paths.update(
        str(path).replace("\\", "/") for path in report.get("generated_report_paths", [])
    )
    missing_report_paths = sorted(required_artifact_paths - declared_generated_paths)
    if missing_report_paths:
        raise RuntimeError(
            "host evidence report missing durable promotion artifact paths: "
            + ", ".join(missing_report_paths)
        )
    missing_files = sorted(
        path_text
        for path_text in required_artifact_paths
        if not (ROOT / path_text).is_file()
    )
    if missing_files:
        raise RuntimeError(
            "host evidence report missing durable promotion artifact files: "
            + ", ".join(missing_files)
        )
    validate_generated_platform_artifact_content(platform_id)
    return generated_paths


def validate_review_candidate_source_truth(
    candidate: dict[str, Any],
    platform_id: str,
) -> None:
    if candidate.get("contract_id") != HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID:
        raise RuntimeError("host evidence review candidate contract_id drifted")
    if candidate.get("platform_id") != platform_id:
        raise RuntimeError("host evidence review candidate platform_id drifted")
    expected_candidate_path = host_evidence_review_candidate_path_for_platform(platform_id)
    if candidate.get("candidate_path") != expected_candidate_path:
        raise RuntimeError("host evidence review candidate path drifted")
    for field_name in (
        "generated_report_only",
        "reviewed_source_truth_required",
        "support_rows_remain_fail_closed_until_reviewed",
    ):
        if candidate.get(field_name) is not True:
            raise RuntimeError(f"host evidence review candidate {field_name} drifted")
    for field_name in (
        "support_claim_published",
        "source_truth_update_allowed",
        "promotion_allowed",
        "support_truth",
        "local_temp_claims_promote_support",
        "prose_only_evidence_allowed",
        "stale_evidence_allowed",
    ):
        if candidate.get(field_name) is not False:
            raise RuntimeError(f"host evidence review candidate {field_name} drifted")
    if candidate.get("generated_only_result") != "refuse-source-truth-promotion":
        raise RuntimeError("host evidence review candidate generated-only result drifted")
    if candidate.get("review_promotion_policy") != "checked-in-source-truth-required":
        raise RuntimeError("host evidence review candidate promotion policy drifted")
    if candidate.get("required_source_record_types") != list(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES):
        raise RuntimeError("host evidence review candidate source record types drifted")
    if candidate.get("required_hosted_review_input_suffixes") != list(HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES):
        raise RuntimeError("host evidence review candidate required suffixes drifted")
    if candidate.get("required_hosted_review_input_paths") != host_evidence_required_review_input_paths_for_platform(platform_id):
        raise RuntimeError("host evidence review candidate required paths drifted")
    rows = candidate.get("review_candidate_rows")
    if not isinstance(rows, list):
        raise RuntimeError("host evidence review candidate rows must be a list")
    by_type = {
        str(row.get("record_type", "")): row
        for row in rows
        if isinstance(row, dict)
    }
    if set(by_type) != set(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES):
        raise RuntimeError("host evidence review candidate record types drifted")
    expected_prefix = f"tmp/reports/platform-host-evidence/{platform_id}/"
    for record_type, row in by_type.items():
        if row.get("platform_id") != platform_id:
            raise RuntimeError(f"{record_type} review candidate platform drifted")
        for field_name in (
            "promotion_allowed",
            "support_truth",
            "generated_report_support_truth",
            "source_truth_update_allowed",
        ):
            if row.get(field_name) is not False:
                raise RuntimeError(f"{record_type} review candidate promoted support")
        for path_text in row.get("generated_artifact_paths", []):
            normalized = str(path_text).replace("\\", "/")
            if not normalized.startswith(expected_prefix):
                raise RuntimeError(
                    f"{record_type} review candidate used non-platform path: {normalized}"
                )
        for artifact in row.get("generated_artifacts", []):
            if not isinstance(artifact, dict):
                raise RuntimeError(f"{record_type} review candidate artifact must be object")
            normalized = str(artifact.get("path", "")).replace("\\", "/")
            if not normalized.startswith(expected_prefix):
                raise RuntimeError(
                    f"{record_type} review candidate artifact path drifted: {normalized}"
                )
        if row.get("generated_artifacts_complete") is not True:
            diagnostics = row.get("incomplete_diagnostics")
            if not isinstance(diagnostics, list) or not diagnostics:
                raise RuntimeError(
                    f"{record_type} review candidate missing incomplete diagnostics"
                )
            for diagnostic in diagnostics:
                if not isinstance(diagnostic, dict):
                    raise RuntimeError(
                        f"{record_type} review candidate incomplete diagnostic must be object"
                    )
                diagnostic_path = str(diagnostic.get("path", "")).replace("\\", "/")
                if not diagnostic_path.startswith(expected_prefix):
                    raise RuntimeError(
                        f"{record_type} review candidate incomplete diagnostic path drifted: {diagnostic_path}"
                    )
            required_sources = row.get("required_source_artifacts")
            if not isinstance(required_sources, list):
                raise RuntimeError(
                    f"{record_type} review candidate required source artifacts drifted"
                )


def build_summary(
    report: dict[str, Any],
    generated_paths: list[str],
    *,
    report_path: Path,
    requirements_path: Path,
    review_candidate_path: Path,
    summary_path: Path,
) -> dict[str, Any]:
    ingestion = report["source_truth_ingestion"]
    all_generated_paths = {
        *generated_paths,
        repo_rel(report_path),
        repo_rel(requirements_path),
        repo_rel(review_candidate_path),
        repo_rel(summary_path),
    }
    return {
        "contract_id": "objc3c.platform.host-evidence.ingestion.summary.v1",
        "status": "GENERATED_ONLY_REFUSED_FOR_SOURCE_TRUTH",
        "platform_id": report["platform_id"],
        "issue_ref": report["issue_ref"],
        "workflow_path": report["workflow_path"],
        "canonical_workflow_path": report["canonical_workflow_path"],
        "accepted_workflow_paths": report["accepted_workflow_paths"],
        "dispatch_gateway_workflow_paths": report["dispatch_gateway_workflow_paths"],
        "runner_label": report["runner_label"],
        "generated_report_contract_id": report["contract_id"],
        "generated_report_only": ingestion["generated_report_only"],
        "generated_only_result": ingestion["generated_only_result"],
        "review_required": ingestion["review_required"],
        "review_promotion_policy": ingestion["review_promotion_policy"],
        "stale_evidence_allowed": ingestion["stale_evidence_allowed"],
        "prose_only_evidence_allowed": ingestion["prose_only_evidence_allowed"],
        "local_temp_claims_promote_support": ingestion["local_temp_claims_promote_support"],
        "source_truth_update_allowed": ingestion["source_truth_update_allowed"],
        "support_claim_published": ingestion["support_claim_published"],
        "support_rows_remain_fail_closed": True,
        "required_checked_source_paths": ingestion["required_checked_source_paths"],
        "required_reviewed_source_fields": report["required_reviewed_source_fields"],
        "required_source_record_types": report["required_source_record_types"],
        "reviewed_source_field_requirements": report["reviewed_source_field_requirements"],
        "review_candidate_source_truth_path": repo_rel(review_candidate_path),
        "review_candidate_contract_id": HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
        "artifact_upload": report["artifact_upload"],
        "generated_report_paths": sorted(all_generated_paths),
    }


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    if argv and argv[:1] == ["--"]:
        argv = argv[1:]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--platform-id", choices=sorted(PLATFORM_CONFIG), required=True)
    parser.add_argument("--runner-label", default="")
    parser.add_argument("--workflow-path", default=WORKFLOW_PATH)
    parser.add_argument("--report-in", type=Path)
    parser.add_argument("--report-out", type=Path)
    parser.add_argument("--requirements-out", type=Path)
    parser.add_argument("--review-candidate-out", type=Path)
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    if argv is None:
        argv = sys.argv[1:]
    args = parse_args(argv)
    if not args.runner_label:
        args.runner_label = os.environ.get("RUNNER_LABEL", "unknown")
    report_out = args.report_out or platform_report_path(args.platform_id, "host-evidence-report.json")
    requirements_out = args.requirements_out or platform_report_path(args.platform_id, "promotion-readiness-requirements.json")
    review_candidate_out = args.review_candidate_out or platform_report_path(
        args.platform_id,
        HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX,
    )
    summary_out = args.summary_out or platform_report_path(args.platform_id, "ingestion-summary.json")

    if args.report_in:
        report = load_report(args.report_in)
        report_path = args.report_in
    else:
        report = build_report(args)
        write_json(report_out, report)
        report_path = report_out

    write_json(requirements_out, report["promotion_readiness_requirements"])
    review_candidate = build_review_candidate_source_truth(
        args.platform_id,
        workflow_path=report["workflow_path"],
        runner_label=report["runner_label"],
    )
    write_json(review_candidate_out, review_candidate)
    generated_paths = validate_report(report, args.platform_id)
    validate_review_candidate_source_truth(review_candidate, args.platform_id)
    summary = build_summary(
        report,
        generated_paths,
        report_path=report_path,
        requirements_path=requirements_out,
        review_candidate_path=review_candidate_out,
        summary_path=summary_out,
    )
    write_json(summary_out, summary)
    print(f"host_evidence_report: {repo_rel(report_out)}")
    print(f"host_evidence_promotion_requirements: {repo_rel(requirements_out)}")
    print(f"host_evidence_review_candidate: {repo_rel(review_candidate_out)}")
    print(f"host_evidence_ingestion_summary: {repo_rel(summary_out)}")
    print("objc3c-platform-host-evidence: GENERATED_ONLY_REFUSED_FOR_SOURCE_TRUTH")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
