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
            "bin/objc3c-native",
            "lib/libobjc3-runtime.so",
            "include/objc3/runtime",
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
            "bin/objc3c-native",
            "lib/libobjc3-runtime.dylib",
            "include/objc3/runtime",
        ],
    },
}

PROMOTION_REVIEW_REQUIRED_FIELDS: tuple[str, ...] = (
    "host_identity",
    "toolchain_probe",
    "build",
    "package",
    "install",
    "object_format",
    "debug_format",
    "runtime_link_load",
    "native_execution",
)

PROMOTION_BLOCKING_EVIDENCE_CLASSES: tuple[str, ...] = (
    "build",
    "package",
    "install",
    "execution",
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
    return {
        "path": path_text,
        "exists": True,
        "size_bytes": path.stat().st_size,
        "sha256": sha256_file(path),
    }


def materialize_generated_artifact(source_path_text: str, scoped_path_text: str) -> dict[str, Any]:
    source_path = ROOT / source_path_text
    scoped_path = ROOT / scoped_path_text
    if source_path.is_file() and source_path.resolve() != scoped_path.resolve():
        scoped_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source_path, scoped_path)
    artifact = generated_artifact(scoped_path_text)
    artifact["source_path"] = source_path_text
    artifact["scoped_copy"] = source_path_text != scoped_path_text
    return artifact


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


def build_promotion_readiness_requirements(platform_id: str) -> dict[str, Any]:
    artifact_identity = build_artifact_identity_reference(platform_id)
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
        "generated_only_result": "refuse-source-truth-promotion",
        "review_promotion_policy": "checked-in-source-truth-required",
        "required_review_fields": list(PROMOTION_REVIEW_REQUIRED_FIELDS),
        "required_promotion_evidence_classes": list(PROMOTION_BLOCKING_EVIDENCE_CLASSES),
        "artifact_identity_reference": artifact_identity,
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
                ],
            },
            {
                "reference_id": "object-format-debug",
                "evidence_class": "object-format-debug",
                "path": platform_scoped_path(platform_id, "promotion-readiness-requirements.json"),
                "required_fields": [
                    "artifact_identity_reference.object_format",
                    "artifact_identity_reference.debug_format",
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


def build_report(args: argparse.Namespace) -> dict[str, Any]:
    platform_id = args.platform_id
    config = PLATFORM_CONFIG[platform_id]
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
        generated_paths = [
            scoped_path.format(platform_id=platform_id)
            for _, scoped_path in path_pairs
        ]
        steps.append(
            {
                "step_id": step_id,
                "evidence_class": evidence_class,
                "outcome": env_outcome(step_id),
                "generated_report_paths": generated_paths,
                "generated_artifacts": [
                    materialize_generated_artifact(
                        source_path.format(platform_id=platform_id),
                        scoped_path.format(platform_id=platform_id),
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
        "artifact_identity_reference": promotion_readiness["artifact_identity_reference"],
        "promotion_readiness_requirements": promotion_readiness,
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
    if ingestion.get("source_truth_update_allowed") is not False:
        raise RuntimeError("generated host evidence attempted to update source truth")
    if ingestion.get("support_claim_published") is not False:
        raise RuntimeError("generated host evidence attempted to publish support")

    required_classes = {"build", "package", "install", "execution"}
    if report.get("required_review_fields") != list(PROMOTION_REVIEW_REQUIRED_FIELDS):
        raise RuntimeError("host evidence report required review fields drifted")
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
    if promotion_readiness.get("artifact_identity_reference") != expected_identity:
        raise RuntimeError("host evidence promotion readiness artifact identity drifted")
    if promotion_readiness.get("required_review_fields") != list(PROMOTION_REVIEW_REQUIRED_FIELDS):
        raise RuntimeError("host evidence promotion readiness required fields drifted")
    if promotion_readiness.get("required_promotion_evidence_classes") != list(PROMOTION_BLOCKING_EVIDENCE_CLASSES):
        raise RuntimeError("host evidence promotion readiness required evidence classes drifted")
    expected_path_prefix = f"tmp/reports/platform-host-evidence/{platform_id}/"
    for reference in promotion_readiness.get("hosted_artifact_references", []):
        if not isinstance(reference, dict):
            raise RuntimeError("host evidence promotion readiness references must be objects")
        path_text = str(reference.get("path", "")).replace("\\", "/")
        if not path_text.startswith(expected_path_prefix):
            raise RuntimeError(f"host evidence promotion readiness used non-platform-scoped path: {path_text}")

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
    return generated_paths


def build_summary(
    report: dict[str, Any],
    generated_paths: list[str],
    *,
    report_path: Path,
    requirements_path: Path,
    summary_path: Path,
) -> dict[str, Any]:
    ingestion = report["source_truth_ingestion"]
    all_generated_paths = {
        *generated_paths,
        repo_rel(report_path),
        repo_rel(requirements_path),
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
        "source_truth_update_allowed": ingestion["source_truth_update_allowed"],
        "support_claim_published": ingestion["support_claim_published"],
        "support_rows_remain_fail_closed": True,
        "required_checked_source_paths": ingestion["required_checked_source_paths"],
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
    summary_out = args.summary_out or platform_report_path(args.platform_id, "ingestion-summary.json")

    if args.report_in:
        report = load_report(args.report_in)
        report_path = args.report_in
    else:
        report = build_report(args)
        write_json(report_out, report)
        report_path = report_out

    generated_paths = validate_report(report, args.platform_id)
    write_json(requirements_out, report["promotion_readiness_requirements"])
    summary = build_summary(
        report,
        generated_paths,
        report_path=report_path,
        requirements_path=requirements_out,
        summary_path=summary_out,
    )
    write_json(summary_out, summary)
    print(f"host_evidence_report: {repo_rel(report_out)}")
    print(f"host_evidence_promotion_requirements: {repo_rel(requirements_out)}")
    print(f"host_evidence_ingestion_summary: {repo_rel(summary_out)}")
    print("objc3c-platform-host-evidence: GENERATED_ONLY_REFUSED_FOR_SOURCE_TRUTH")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
