"""Shared hosted platform evidence path and review-candidate contracts."""

from __future__ import annotations

from typing import Any

HOST_EVIDENCE_REPORT_ROOT = "tmp/reports/platform-host-evidence"
HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID = (
    "objc3c.platform.hosted-evidence.review-candidate-source-truth.v1"
)
HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX = (
    "review-candidate-source-truth.json"
)
HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE = (
    f"{HOST_EVIDENCE_REPORT_ROOT}/<platform>/"
    f"{HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX}"
)

HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS: tuple[str, ...] = (
    "host-evidence-report.json",
    "promotion-readiness-requirements.json",
    HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX,
    "ingestion-summary.json",
    "llvm-capabilities.json",
    "build/native_build_summary.json",
    "build/object-identity.json",
    "build/debug-identity.json",
    "package/objc3c-runnable-toolchain-package.json",
    "package/runtime-library-manifest.json",
    "install/install-receipt.json",
    "install/end-to-end-summary.json",
    "install/install-distribution-credibility-summary.json",
    "install/install-distribution-verification.json",
    "install/clean-install-distribution-receipt.json",
    "execution/runtime-load-probe.json",
    "execution/hosted-execution-smoke-summary.json",
    "execution/native-execution-smoke-summary.json",
)

HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES: tuple[str, ...] = tuple(
    suffix
    for suffix in HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS
    if suffix != "ingestion-summary.json"
)

HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES: tuple[str, ...] = (
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

HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE: dict[str, str] = {
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

HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE: dict[str, str] = {
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

HOST_EVIDENCE_REVIEW_CANDIDATE_ARTIFACTS_BY_RECORD_TYPE: dict[str, tuple[str, ...]] = {
    "host_identity": ("host-evidence-report.json",),
    "toolchain_probe": ("llvm-capabilities.json",),
    "package_root": (
        "package/objc3c-runnable-toolchain-package.json",
        "package/runtime-library-manifest.json",
    ),
    "install_receipt": ("install/install-receipt.json",),
    "native_execution": (
        "execution/hosted-execution-smoke-summary.json",
        "execution/native-execution-smoke-summary.json",
    ),
    "object_identity": ("build/object-identity.json",),
    "debug_identity": ("build/debug-identity.json",),
    "package_install_identity": (
        "package/objc3c-runnable-toolchain-package.json",
        "package/runtime-library-manifest.json",
        "install/install-receipt.json",
    ),
    "runtime_load_link_proof": (
        "package/runtime-library-manifest.json",
        "execution/runtime-load-probe.json",
    ),
}


def host_evidence_generated_report_paths_for_platform(platform_id: str) -> list[str]:
    return [
        f"{HOST_EVIDENCE_REPORT_ROOT}/{platform_id}/{relative_path}"
        for relative_path in HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS
    ]


def host_evidence_required_review_input_paths_for_platform(platform_id: str) -> list[str]:
    return [
        f"{HOST_EVIDENCE_REPORT_ROOT}/{platform_id}/{relative_path}"
        for relative_path in HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES
    ]


def host_evidence_review_candidate_path_for_platform(platform_id: str) -> str:
    return (
        f"{HOST_EVIDENCE_REPORT_ROOT}/{platform_id}/"
        f"{HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX}"
    )


def host_evidence_review_record_ids(platform_id: str) -> dict[str, str]:
    return {
        "host_identity_record_id": f"objc3c.host.{platform_id}.identity.fail-closed",
        "toolchain_probe_record_id": (
            f"objc3c.host.{platform_id}.toolchain-probes.fail-closed"
        ),
        "package_root_record_id": (
            f"objc3c.package-root.{platform_id}.release.fail-closed"
        ),
        "install_receipt_record_id": (
            f"objc3c.install-receipt.{platform_id}.release.missing"
        ),
        "native_execution_record_id": (
            f"objc3c.native-execution.{platform_id}.release.missing"
        ),
        "object_identity_record_id": (
            f"objc3c.object-identity.{platform_id}.release.missing"
        ),
        "debug_identity_record_id": (
            f"objc3c.debug-identity.{platform_id}.release.missing"
        ),
        "package_install_identity_record_id": (
            f"objc3c.package-install-identity.{platform_id}.release.missing"
        ),
        "runtime_load_link_proof_record_id": (
            f"objc3c.runtime-load-link.{platform_id}.release.missing"
        ),
    }


def host_evidence_review_candidate_targets(platform_id: str) -> list[dict[str, Any]]:
    record_ids = host_evidence_review_record_ids(platform_id)
    targets: list[dict[str, Any]] = []
    for record_type in HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES:
        record_id_field = HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE[
            record_type
        ]
        targets.append(
            {
                "record_type": record_type,
                "target_fixture_section": (
                    HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE[record_type]
                ),
                "target_record_id_field": record_id_field,
                "target_record_id": record_ids[record_id_field],
                "artifact_suffixes": list(
                    HOST_EVIDENCE_REVIEW_CANDIDATE_ARTIFACTS_BY_RECORD_TYPE[
                        record_type
                    ]
                ),
            }
        )
    return targets


__all__ = [
    "HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS",
    "HOST_EVIDENCE_REPORT_ROOT",
    "HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES",
    "HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES",
    "HOST_EVIDENCE_REVIEW_CANDIDATE_ARTIFACTS_BY_RECORD_TYPE",
    "HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID",
    "HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_SUFFIX",
    "HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE",
    "HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE",
    "HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE",
    "host_evidence_generated_report_paths_for_platform",
    "host_evidence_required_review_input_paths_for_platform",
    "host_evidence_review_candidate_path_for_platform",
    "host_evidence_review_candidate_targets",
    "host_evidence_review_record_ids",
]
