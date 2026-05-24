"""Reusable host-promotion evidence model for unsupported platform rows."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Iterable

from .constants import PLATFORM_IDENTITY_CONTRACTS, UNSUPPORTED_PROMOTION_PLATFORM_IDS
from .host_evidence_contract import (
    HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS,
    HOST_EVIDENCE_REPORT_ROOT,
    HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES,
    HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES,
    HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE,
    HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE,
    host_evidence_generated_report_paths_for_platform,
    host_evidence_review_candidate_path_for_platform,
    host_evidence_review_record_ids,
)

HOST_PROMOTION_EVIDENCE_CONTRACT_ID = (
    "objc3c.platform.host-promotion.evidence.contract.v1"
)
HOST_PROMOTION_EVIDENCE_SUMMARY_CONTRACT_ID = (
    "objc3c.platform.host-promotion.evidence.summary.v1"
)
HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH = (
    "tests/tooling/fixtures/platform_hardening/"
    "platform_host_promotion_evidence_contract.json"
)
HOST_PROMOTION_REVIEWED_SOURCE_INPUT_CONTRACT_ID = (
    "objc3c.platform.host-promotion.reviewed-source-inputs.v1"
)
HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH = (
    "tests/tooling/fixtures/platform_hardening/"
    "host_promotion_reviewed_source_inputs.json"
)
HOST_PROMOTION_REVIEWED_SOURCE_DURABLE_FIXTURE_PATHS: tuple[str, ...] = (
    HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH,
    HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH,
    "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
)
HOST_PROMOTION_EVIDENCE_ACTION = "check-platform-host-promotion-evidence"
HOST_PROMOTION_VALIDATE_PLATFORM_HARDENING_ACTION = "validate-platform-hardening"
HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH = (
    "tmp/reports/platform-hardening/host-promotion-evidence-summary.json"
)
HOST_PROMOTION_GENERATED_REPORT_ROOT = HOST_EVIDENCE_REPORT_ROOT

HOST_PROMOTION_UPSTREAM_SOURCE_PATHS: dict[str, str] = {
    "support_evidence": (
        "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json"
    ),
    "platform_expansion_claims": (
        "tests/tooling/fixtures/platform_hardening/platform_expansion_claim_contract.json"
    ),
    "hosted_runner_capability_summaries": (
        "tests/tooling/fixtures/platform_hardening/hosted_runner_capability_summaries.json"
    ),
    "unsupported_host_policy": (
        "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
    ),
}

HOST_PROMOTION_POLICY: dict[str, object] = {
    "support_promotion_allowed": False,
    "policy_id": "fail-closed-until-reviewed-source-truth",
    "no_support_promotion_statement": (
        "Linux x64 and macOS arm64 remain unsupported. Hosted runner output, "
        "generated reports, object emission probes, package metadata, install "
        "summaries, sanitizer runs, and runtime smoke summaries are not support "
        "truth until reviewed into checked source-truth rows."
    ),
    "source_truth_required": True,
    "generated_evidence_required": True,
    "generated_evidence_support_truth": False,
    "reviewed_source_truth_support_truth": True,
    "native_execution_required": True,
    "package_install_required": True,
    "prose_only_platform_support_claims_allowed": False,
    "sanitizer_variants_promote_platform_support": False,
}

HOST_PROMOTION_PACKAGE_CHANNEL_MANIFEST_PATH = (
    "artifacts/package/objc3c-runnable-toolchain-package.json"
)
HOST_PROMOTION_PACKAGE_CHANNEL_COMMON_PAYLOAD_PATHS: tuple[str, ...] = (
    "stdlib/workspace.json",
    "stdlib/modules/objc3.core/module.json",
    "docs/runbooks/objc3c_packaging_channels.md",
)
HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM: dict[str, tuple[str, ...]] = {
    "windows-x64": (
        HOST_PROMOTION_PACKAGE_CHANNEL_MANIFEST_PATH,
        "artifacts/bin/objc3c-native.exe",
        "artifacts/lib/objc3_runtime.lib",
        *HOST_PROMOTION_PACKAGE_CHANNEL_COMMON_PAYLOAD_PATHS,
    ),
    "linux-x64": (
        HOST_PROMOTION_PACKAGE_CHANNEL_MANIFEST_PATH,
        "artifacts/bin/objc3c-native",
        "artifacts/lib/libobjc3-runtime.so",
        *HOST_PROMOTION_PACKAGE_CHANNEL_COMMON_PAYLOAD_PATHS,
    ),
    "darwin-arm64": (
        HOST_PROMOTION_PACKAGE_CHANNEL_MANIFEST_PATH,
        "artifacts/bin/objc3c-native",
        "artifacts/lib/libobjc3-runtime.dylib",
        *HOST_PROMOTION_PACKAGE_CHANNEL_COMMON_PAYLOAD_PATHS,
    ),
}
HOST_PROMOTION_WINDOWS_PACKAGE_CHANNEL_REQUIRED_PATHS: tuple[str, ...] = (
    "artifacts/bin/objc3c-native.exe",
    "artifacts/lib/objc3_runtime.lib",
)
HOST_PROMOTION_INSTALL_PREFIX_PACKAGE_ROOT_LAYOUT_PATHS: tuple[str, ...] = (
    "bin/objc3-runtime.dll",
    "bin/objc3c-native",
    "bin/objc3c-native.exe",
    "lib/libobjc3-runtime.so",
    "lib/libobjc3-runtime.dylib",
    "lib/objc3-runtime.lib",
    "lib/objc3_runtime.lib",
    "include/objc3/runtime",
)
HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES: tuple[str, ...] = (
    HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES
)

HOST_PROMOTION_REQUIRED_GATE_CLASSES: tuple[str, ...] = (
    "build",
    "package",
    "install",
    "execution",
)

HOST_PROMOTION_FAIL_CLOSED_BLOCKER_CLASSES: tuple[str, ...] = (
    "generated-only-evidence",
    "wrong-object-debug-format",
    "wrong-architecture",
    "missing-package-root",
    "runtime-load-failure",
    "missing-install-receipt",
    "unsupported-llvm-version",
    "sanitizer-variant-leakage",
)

HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES: tuple[str, ...] = (
    HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES
)

HOST_PROMOTION_PROMOTION_PREREQUISITE_RECORD_TYPES: tuple[str, ...] = (
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

HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE: dict[str, str] = dict(
    HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE
)

HOST_PROMOTION_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE: dict[str, str] = dict(
    HOST_EVIDENCE_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE
)

HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS: tuple[str, ...] = (
    "object_identity",
    "debug_identity",
    "package_install_identity",
    "runtime_load_link_proof",
)

HOST_PROMOTION_GENERATED_REPORT_RELATIVE_PATHS: tuple[str, ...] = (
    HOST_EVIDENCE_GENERATED_REPORT_RELATIVE_PATHS
)


@dataclass(frozen=True)
class HostPromotionEvidenceClass:
    class_id: str
    claim_weight: str
    support_truth: Any
    required_fields: tuple[str, ...]
    promotion_result: str
    required: bool | None = None

    def as_json(self) -> dict[str, Any]:
        payload = {
            "class_id": self.class_id,
            "claim_weight": self.claim_weight,
            "support_truth": self.support_truth,
            "required_fields": list(self.required_fields),
            "promotion_result": self.promotion_result,
        }
        if self.required is not None:
            payload["required"] = self.required
        return payload


HOST_PROMOTION_EVIDENCE_CLASSES: tuple[HostPromotionEvidenceClass, ...] = (
    HostPromotionEvidenceClass(
        class_id="generated_hosted_evidence",
        claim_weight="required-hosted-evidence",
        support_truth=False,
        required=True,
        required_fields=(
            "runner_label",
            "workflow_path",
            "generated_report_root",
            "generated_report_contract_id",
            "generated_report_paths",
            "candidate_evidence_record_id",
            "review_candidate_source_truth_path",
            "ingestion_summary_path",
        ),
        promotion_result="refuse-source-truth-promotion",
    ),
    HostPromotionEvidenceClass(
        class_id="reviewed_source_truth_evidence",
        claim_weight="required-for-future-promotion",
        support_truth="only-after-source-review",
        required_fields=(
            "source_review_record_id",
            "reviewed_source_paths",
            "support_row_id",
            "package_variant_row_id",
            "required_record_ids",
            "required_reviewed_source_fields",
            "reviewed_source_field_status",
            "missing_record_classes",
        ),
        promotion_result="eligible-only-if-all-blockers-empty",
    ),
    HostPromotionEvidenceClass(
        class_id="package_install_native_execution_evidence",
        claim_weight="required-for-future-promotion",
        support_truth="only-after-source-review",
        required_fields=(
            "package_root_layout",
            "package_manifest_path",
            "install_receipt_path",
            "execution_summary_path",
            "native_smoke_summary_path",
            "runtime_library_names",
            "loader_policy",
            "host_identity_record_id",
        ),
        promotion_result="fail-closed-until-package-install-and-native-execution-pass",
    ),
    HostPromotionEvidenceClass(
        class_id="object_debug_identity",
        claim_weight="required-for-future-promotion",
        support_truth="only-after-source-review",
        required_fields=(
            "object_format",
            "debug_format",
            "target_triple",
            "arch",
            "object_identity_report_path",
            "debug_identity_report_path",
            "llvm_toolchain_record_id",
        ),
        promotion_result="fail-closed-on-format-or-arch-mismatch",
    ),
    HostPromotionEvidenceClass(
        class_id="runtime_link_load_identity",
        claim_weight="required-for-future-promotion",
        support_truth="only-after-source-review",
        required_fields=(
            "runtime_library_names",
            "runtime_library_manifest_path",
            "linker_flags",
            "loader_policy",
            "load_probe_path",
            "load_probe_exit_code",
            "resolved_runtime_paths",
        ),
        promotion_result="fail-closed-on-link-or-load-failure",
    ),
    HostPromotionEvidenceClass(
        class_id="promotion_blockers",
        claim_weight="policy",
        support_truth=False,
        required_fields=(
            "blocker_id",
            "failure_class",
            "required_behavior",
            "blocks_surfaces",
            "source_owner",
        ),
        promotion_result="fail-closed",
    ),
)


@dataclass(frozen=True)
class HostPromotionReviewedSourceField:
    field_id: str
    required_record_id_field: str
    generated_report_path_suffix: str
    failure_class: str
    required_behavior: str

    def as_json(self) -> dict[str, Any]:
        return {
            "field_id": self.field_id,
            "required_record_id_field": self.required_record_id_field,
            "generated_report_path_suffix": self.generated_report_path_suffix,
            "reviewed_source_required": True,
            "generated_report_support_truth": False,
            "promotion_allowed_from_generated_evidence": False,
            "failure_class": self.failure_class,
            "required_behavior": self.required_behavior,
        }


HOST_PROMOTION_REVIEWED_SOURCE_FIELDS: tuple[
    HostPromotionReviewedSourceField, ...
] = (
    HostPromotionReviewedSourceField(
        field_id="object_identity",
        required_record_id_field="object_identity_record_id",
        generated_report_path_suffix="build/object-identity.json",
        failure_class="wrong-object-debug-format",
        required_behavior="fail-closed-before-package-publication",
    ),
    HostPromotionReviewedSourceField(
        field_id="debug_identity",
        required_record_id_field="debug_identity_record_id",
        generated_report_path_suffix="build/debug-identity.json",
        failure_class="wrong-object-debug-format",
        required_behavior="fail-closed-before-package-publication",
    ),
    HostPromotionReviewedSourceField(
        field_id="package_install_identity",
        required_record_id_field="package_install_identity_record_id",
        generated_report_path_suffix="install/install-receipt.json",
        failure_class="missing-install-receipt",
        required_behavior="fail-closed-before-native-execution-claim",
    ),
    HostPromotionReviewedSourceField(
        field_id="runtime_load_link_proof",
        required_record_id_field="runtime_load_link_proof_record_id",
        generated_report_path_suffix="execution/runtime-load-probe.json",
        failure_class="runtime-load-failure",
        required_behavior="fail-closed-before-native-execution-claim",
    ),
)


@dataclass(frozen=True)
class HostPromotionFailClosedBlocker:
    failure_class: str
    required_behavior: str
    blocks_surfaces: tuple[str, ...]
    source_owner: str

    def as_json(self) -> dict[str, Any]:
        return {
            "blocker_id": f"objc3c.host-promotion.reject.{self.failure_class}",
            "failure_class": self.failure_class,
            "required_behavior": self.required_behavior,
            "blocks_surfaces": list(self.blocks_surfaces),
            "source_owner": self.source_owner,
        }


HOST_PROMOTION_COMMON_FAIL_CLOSED_BLOCKERS: tuple[HostPromotionFailClosedBlocker, ...] = (
    HostPromotionFailClosedBlocker(
        failure_class="generated-only-evidence",
        required_behavior="fail-closed-before-support-promotion",
        blocks_surfaces=("support-row", "package", "install", "execution", "publication"),
        source_owner="platform-hardening-support-source",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="wrong-object-debug-format",
        required_behavior="fail-closed-before-package-publication",
        blocks_surfaces=("package", "execution", "publication"),
        source_owner="platform-hardening-build-package-validation",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="wrong-architecture",
        required_behavior="fail-closed-before-install",
        blocks_surfaces=("package", "install", "execution", "publication"),
        source_owner="platform-hardening-unsupported-host-fail-closed",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="missing-package-root",
        required_behavior="fail-closed-before-install",
        blocks_surfaces=("install", "execution", "publication"),
        source_owner="platform-hardening-build-package-validation",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="runtime-load-failure",
        required_behavior="fail-closed-before-native-execution-claim",
        blocks_surfaces=("execution", "publication"),
        source_owner="platform-hardening-build-package-validation",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="missing-install-receipt",
        required_behavior="fail-closed-before-native-execution-claim",
        blocks_surfaces=("install", "execution", "publication"),
        source_owner="platform-hardening-install-validation",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="unsupported-llvm-version",
        required_behavior="fail-closed-no-range-claim",
        blocks_surfaces=("toolchain", "package", "execution", "publication"),
        source_owner="platform-hardening-support-source",
    ),
    HostPromotionFailClosedBlocker(
        failure_class="sanitizer-variant-leakage",
        required_behavior="fail-closed-before-package-install",
        blocks_surfaces=("package", "install", "execution", "publication"),
        source_owner="platform-hardening-install-validation",
    ),
)


@dataclass(frozen=True)
class HostPromotionArtifactIdentity:
    object_format: str
    debug_format: str
    target_triple: str
    arch: str
    runtime_library_names: tuple[str, ...]
    loader_policy: str
    linker_flags: tuple[str, ...]

    def as_json(self) -> dict[str, Any]:
        return {
            "object_format": self.object_format,
            "debug_format": self.debug_format,
            "target_triple": self.target_triple,
            "arch": self.arch,
            "runtime_library_names": list(self.runtime_library_names),
            "loader_policy": self.loader_policy,
            "linker_flags": list(self.linker_flags),
        }


@dataclass(frozen=True)
class HostPromotionPlatformContract:
    platform_id: str
    issue_ref: int
    runner_label: str
    package_variant_row_id: str
    generated_evidence_record_id: str
    support_row_id: str
    package_id: str
    package_root_layout: tuple[str, ...]
    artifact_identity: HostPromotionArtifactIdentity

    @property
    def report_root(self) -> str:
        return generated_report_root_for_platform(self.platform_id)

    @property
    def required_record_ids(self) -> dict[str, str]:
        return host_evidence_review_record_ids(self.platform_id)

    def as_json(self) -> dict[str, Any]:
        identity = PLATFORM_IDENTITY_CONTRACTS[self.platform_id]
        report_root = self.report_root
        required_record_ids = self.required_record_ids
        reviewed_source_fields = list(HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS)
        return {
            "platform_id": self.platform_id,
            "issue_ref": self.issue_ref,
            "support_row_id": self.support_row_id,
            "support_state": "unsupported",
            "promotion_allowed": False,
            "claim_class": "fail-closed",
            "runner_label": self.runner_label,
            "host_identity": {
                "host_os": identity["host_os"],
                "host_arch": identity["host_arch"],
                "host_systems": list(identity["host_systems"]),
                "host_machines": list(identity["host_machines"]),
                "host_triples": list(identity["host_triples"]),
            },
            "package_variant_row_id": self.package_variant_row_id,
            "generated_evidence_record_id": self.generated_evidence_record_id,
            "support_claim_policy": {
                "prose_only_platform_support_claims_allowed": False,
                "source_truth_required": True,
                "generated_hosted_evidence_required": True,
                "required_output_root": report_root,
            },
            "generated_hosted_evidence": {
                "required": True,
                "runner_label": self.runner_label,
                "candidate_evidence_record_id": self.generated_evidence_record_id,
                "workflow_path": ".github/workflows/platform-host-evidence.yml",
                "dispatch_gateway_workflow_paths": [
                    ".github/workflows/conformance-minima.yml"
                ],
                "generated_report_contract_id": (
                    "objc3c.platform.hosted-runner.evidence-report.v1"
                ),
                "generated_report_root": report_root,
                "generated_report_paths": generated_report_paths_for_platform(self.platform_id),
                "review_candidate_source_truth_path": (
                    host_evidence_review_candidate_path_for_platform(self.platform_id)
                ),
                "ingestion_summary_path": f"{report_root}/ingestion-summary.json",
                "support_truth": False,
                "promotion_result": "refuse-source-truth-promotion",
            },
            "reviewed_source_truth_evidence": {
                "required": True,
                "source_review_record_id": (
                    f"objc3c.host-promotion.{self.platform_id}."
                    "reviewed-source-truth.required"
                ),
                "reviewed_source_paths": [],
                "support_row_id": self.support_row_id,
                "package_variant_row_id": self.package_variant_row_id,
                "required_record_ids": required_record_ids,
                "required_reviewed_source_fields": reviewed_source_fields,
                "reviewed_source_field_status": {
                    field_id: "missing-reviewed-source"
                    for field_id in reviewed_source_fields
                },
                "missing_record_classes": list(HOST_PROMOTION_REQUIRED_GATE_CLASSES),
                "reviewed_source_support_ready": False,
                "generated_evidence_support_truth": False,
                "support_truth": False,
            },
            "package_install_native_execution_evidence": {
                "package_variant_row_id": self.package_variant_row_id,
                "package_id": self.package_id,
                "package_root_layout": list(self.package_root_layout),
                "package_manifest_path": (
                    f"{report_root}/package/objc3c-runnable-toolchain-package.json"
                ),
                "required_package_root_present": False,
                "install_receipt_required": True,
                "install_receipt_present": False,
                "install_receipt_path": f"{report_root}/install/install-receipt.json",
                "native_execution_required": True,
                "native_execution_present": False,
                "execution_summary_path": (
                    f"{report_root}/execution/hosted-execution-smoke-summary.json"
                ),
                "native_smoke_summary_path": (
                    f"{report_root}/execution/native-execution-smoke-summary.json"
                ),
                "runtime_library_names": list(self.artifact_identity.runtime_library_names),
                "loader_policy": (
                    f"{self.artifact_identity.loader_policy} must be proven before support"
                ),
                "host_identity_record_id": required_record_ids["host_identity_record_id"],
                "support_truth": False,
            },
            "object_debug_identity": {
                "object_format": self.artifact_identity.object_format,
                "debug_format": self.artifact_identity.debug_format,
                "target_triple": self.artifact_identity.target_triple,
                "arch": self.artifact_identity.arch,
                "object_identity_report_path": f"{report_root}/build/object-identity.json",
                "debug_identity_report_path": f"{report_root}/build/debug-identity.json",
                "llvm_toolchain_record_id": (
                    required_record_ids["toolchain_probe_record_id"]
                ),
                "wrong_format_behavior": "fail-closed-before-package-publication",
                "wrong_arch_behavior": "fail-closed-before-install",
                "support_truth_without_package_install_execution": False,
            },
            "runtime_link_load_identity": {
                "runtime_library_names": list(self.artifact_identity.runtime_library_names),
                "runtime_library_manifest_path": (
                    f"{report_root}/package/runtime-library-manifest.json"
                ),
                "linker_flags": list(self.artifact_identity.linker_flags),
                "loader_policy": (
                    f"{self.artifact_identity.loader_policy} must be proven before support"
                ),
                "load_probe_path": f"{report_root}/execution/runtime-load-probe.json",
                "load_probe_exit_code": -1,
                "resolved_runtime_paths": [],
                "runtime_load_probe_required": True,
                "runtime_load_probe_present": False,
                "runtime_load_failure_behavior": (
                    "fail-closed-before-native-execution-claim"
                ),
                "support_truth": False,
            },
            "llvm_version_policy": {
                "supported_range_claim_present": False,
                "unsupported_version_behavior": "fail-closed-no-range-claim",
                "required_coherent_tools": [
                    "clang",
                    "clang++",
                    "llc",
                    "llvm-ar",
                    "headers-libs",
                ],
                "mixed_root_allowed": False,
            },
            "sanitizer_variant_boundary": {
                "release_runtime_allows_sanitizer_artifacts": False,
                "sanitizer_variant_leakage_behavior": (
                    "fail-closed-before-package-install"
                ),
                "sanitizer_reports_promote_platform_support": False,
            },
            "promotion_blockers": list(HOST_PROMOTION_FAIL_CLOSED_BLOCKER_CLASSES),
        }


HOST_PROMOTION_PLATFORM_CONTRACTS: tuple[HostPromotionPlatformContract, ...] = (
    HostPromotionPlatformContract(
        platform_id="linux-x64",
        issue_ref=8228,
        runner_label="ubuntu-24.04",
        package_variant_row_id="objc3c.package.runtime.linux-x64.release.fail-closed",
        generated_evidence_record_id=(
            "objc3c.evidence.hosted-ci.linux-x64.generated-host-run"
        ),
        support_row_id="objc3c.platform.linux-x64.unsupported",
        package_id="org.objc3c.runtime:objc3c-runtime-linux-x64-release",
        package_root_layout=(
            HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM["linux-x64"]
        ),
        artifact_identity=HostPromotionArtifactIdentity(
            object_format="ELF",
            debug_format="DWARF",
            target_triple="x86_64-unknown-linux-gnu",
            arch="x64",
            runtime_library_names=("libobjc3-runtime.so",),
            loader_policy="ELF rpath, RUNPATH, or package-root loader resolution",
            linker_flags=("-lobjc3-runtime",),
        ),
    ),
    HostPromotionPlatformContract(
        platform_id="darwin-arm64",
        issue_ref=8229,
        runner_label="macos-15",
        package_variant_row_id="objc3c.package.runtime.darwin-arm64.release.fail-closed",
        generated_evidence_record_id=(
            "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run"
        ),
        support_row_id="objc3c.platform.darwin-arm64.unsupported",
        package_id="org.objc3c.runtime:objc3c-runtime-darwin-arm64-release",
        package_root_layout=(
            HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM["darwin-arm64"]
        ),
        artifact_identity=HostPromotionArtifactIdentity(
            object_format="Mach-O",
            debug_format="DWARF/dSYM",
            target_triple="aarch64-apple-darwin",
            arch="arm64",
            runtime_library_names=("libobjc3-runtime.dylib",),
            loader_policy="@rpath, install_name, codesign, and package-root loader behavior",
            linker_flags=("-lobjc3-runtime",),
        ),
    ),
)


def host_promotion_platform_ids(
    contracts: Iterable[HostPromotionPlatformContract] = HOST_PROMOTION_PLATFORM_CONTRACTS,
) -> tuple[str, ...]:
    return tuple(contract.platform_id for contract in contracts)


def generated_report_root_for_platform(platform_id: str) -> str:
    return f"{HOST_PROMOTION_GENERATED_REPORT_ROOT}/{platform_id}"


def generated_report_paths_for_platform(platform_id: str) -> list[str]:
    return host_evidence_generated_report_paths_for_platform(platform_id)


def build_host_promotion_contract_payload() -> dict[str, Any]:
    platform_ids = host_promotion_platform_ids()
    unexpected = sorted(set(platform_ids) - set(UNSUPPORTED_PROMOTION_PLATFORM_IDS))
    return {
        "contract_id": HOST_PROMOTION_EVIDENCE_CONTRACT_ID,
        "schema_version": 1,
        "source_path": HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH,
        "upstream_source_paths": dict(HOST_PROMOTION_UPSTREAM_SOURCE_PATHS),
        "issue_refs": [8206, 8228, 8229],
        "promotion_policy": dict(HOST_PROMOTION_POLICY),
        "evidence_classes": [
            evidence_class.as_json()
            for evidence_class in HOST_PROMOTION_EVIDENCE_CLASSES
        ],
        "required_source_record_types": list(HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES),
        "reviewed_source_fields": [
            reviewed_source_field.as_json()
            for reviewed_source_field in HOST_PROMOTION_REVIEWED_SOURCE_FIELDS
        ],
        "required_gate_classes": list(HOST_PROMOTION_REQUIRED_GATE_CLASSES),
        "common_fail_closed_blockers": [
            blocker.as_json()
            for blocker in HOST_PROMOTION_COMMON_FAIL_CLOSED_BLOCKERS
        ],
        "unsupported_platform_ids": list(UNSUPPORTED_PROMOTION_PLATFORM_IDS),
        "unexpected_supported_platform_ids": unexpected,
        "platforms": [contract.as_json() for contract in HOST_PROMOTION_PLATFORM_CONTRACTS],
        "future_checker_contract": {
            "must_require_platform_rows": list(platform_ids),
            "must_reject_if_any_platform_promotion_allowed": True,
            "must_reject_generated_only_support_truth": True,
            "must_reject_empty_blocker_lists": True,
            "must_reject_missing_required_source_record_types": True,
            "must_reject_stale_reviewed_source_evidence": True,
            "must_reject_prose_only_reviewed_source_evidence": True,
            "must_reject_local_temp_promotion_claims": True,
            "must_require_hosted_toolchain_package_artifacts": True,
            "must_require_reviewed_source_fields": list(
                HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS
            ),
            "must_reject_sanitizer_artifacts_in_release_runtime": True,
            "must_reject_prose_only_platform_support_claims": True,
            "must_match_existing_platform_identity_contracts": True,
            "must_register_public_command": HOST_PROMOTION_EVIDENCE_ACTION,
            "must_be_child_of_public_action": (
                HOST_PROMOTION_VALIDATE_PLATFORM_HARDENING_ACTION
            ),
            "must_write_summary_under": HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH,
        },
    }


def build_host_promotion_reviewed_source_input_model_payload() -> dict[str, Any]:
    platform_ids = host_promotion_platform_ids()
    return {
        "contract_id": HOST_PROMOTION_REVIEWED_SOURCE_INPUT_CONTRACT_ID,
        "schema_version": 1,
        "source_path": HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH,
        "host_promotion_contract_path": HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH,
        "platform_ids": list(platform_ids),
        "generated_reports_are_source_truth": False,
        "source_review_required": True,
        "default_promotion_allowed": False,
        "stale_evidence_allowed": False,
        "prose_only_evidence_allowed": False,
        "promotion_allowed_rule": (
            "promotion_allowed is true only when every required reviewed-source "
            "record resolves to a promotion-ready checked-source record, generated "
            "reports remain non-truth inputs, hosted-runner/toolchain/package "
            "artifacts are referenced from platform-scoped generated reports, and "
            "blockers are empty"
        ),
        "required_durable_fixture_paths": list(
            HOST_PROMOTION_REVIEWED_SOURCE_DURABLE_FIXTURE_PATHS
        ),
        "required_hosted_promotion_artifact_suffixes": list(
            HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES
        ),
        "required_record_types_before_promotion_allowed": list(
            HOST_PROMOTION_PROMOTION_PREREQUISITE_RECORD_TYPES
        ),
        "record_sections": dict(HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE),
        "record_id_fields": dict(
            HOST_PROMOTION_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE
        ),
    }


__all__ = [
    "HOST_PROMOTION_COMMON_FAIL_CLOSED_BLOCKERS",
    "HOST_PROMOTION_EVIDENCE_CLASSES",
    "HOST_PROMOTION_EVIDENCE_ACTION",
    "HOST_PROMOTION_EVIDENCE_CONTRACT_ID",
    "HOST_PROMOTION_EVIDENCE_CONTRACT_RELATIVE_PATH",
    "HOST_PROMOTION_EVIDENCE_SUMMARY_CONTRACT_ID",
    "HOST_PROMOTION_EVIDENCE_SUMMARY_RELATIVE_PATH",
    "HOST_PROMOTION_FAIL_CLOSED_BLOCKER_CLASSES",
    "HOST_PROMOTION_GENERATED_REPORT_RELATIVE_PATHS",
    "HOST_PROMOTION_GENERATED_REPORT_ROOT",
    "HOST_PROMOTION_INSTALL_PREFIX_PACKAGE_ROOT_LAYOUT_PATHS",
    "HOST_PROMOTION_PACKAGE_CHANNEL_COMMON_PAYLOAD_PATHS",
    "HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM",
    "HOST_PROMOTION_PACKAGE_CHANNEL_MANIFEST_PATH",
    "HOST_PROMOTION_POLICY",
    "HOST_PROMOTION_PLATFORM_CONTRACTS",
    "HOST_PROMOTION_PROMOTION_PREREQUISITE_RECORD_TYPES",
    "HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES",
    "HOST_PROMOTION_REVIEWED_SOURCE_DURABLE_FIXTURE_PATHS",
    "HOST_PROMOTION_REVIEWED_SOURCE_INPUT_CONTRACT_ID",
    "HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH",
    "HOST_PROMOTION_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE",
    "HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE",
    "HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS",
    "HOST_PROMOTION_REQUIRED_GATE_CLASSES",
    "HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES",
    "HOST_PROMOTION_REVIEWED_SOURCE_FIELDS",
    "HOST_PROMOTION_UPSTREAM_SOURCE_PATHS",
    "HOST_PROMOTION_VALIDATE_PLATFORM_HARDENING_ACTION",
    "HOST_PROMOTION_WINDOWS_PACKAGE_CHANNEL_REQUIRED_PATHS",
    "HostPromotionArtifactIdentity",
    "HostPromotionEvidenceClass",
    "HostPromotionFailClosedBlocker",
    "HostPromotionPlatformContract",
    "HostPromotionReviewedSourceField",
    "build_host_promotion_contract_payload",
    "build_host_promotion_reviewed_source_input_model_payload",
    "generated_report_paths_for_platform",
    "generated_report_root_for_platform",
    "host_promotion_platform_ids",
]
