"""Dataclass models and truth-policy helpers for hosted LLVM contracts."""

from __future__ import annotations

from dataclasses import dataclass

from .developer_tooling_llvm_contract_constants import (
    HOSTED_LLVM_CAPABILITY_MODE,
    HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
)


@dataclass(frozen=True)
class CapabilityExplorerContract:
    action: str
    summary: str
    backend: str
    guarantee_owner: str
    validation_tier: str = "repo"
    pass_through_args: bool = True


@dataclass(frozen=True)
class ToolCapabilityOwnerContract:
    action: str
    owner: str
    proof_source: str
    claim_scope: str
    unsupported_claims: tuple[str, ...]
    requires_hosted_probe_summary: bool
    fails_closed_without_live_capability: bool


@dataclass(frozen=True)
class HostedLLVMCapabilityTruth:
    source_kind: str
    mode: object
    ok: bool
    clang_found: bool
    clangxx_found: bool
    llc_found: bool
    llc_supports_filetype_obj: bool
    llc_supports_target_object_emission: bool
    llvm_ar_found: bool
    llvm_config_found: bool
    headers_libraries_discovered: bool
    toolchain_identity_claimable: bool
    summary_native_object_emission_status: str
    failure_reasons: tuple[str, ...]

    @property
    def is_hosted_source(self) -> bool:
        return self.source_kind == HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE

    @property
    def hosted_execution_supported(self) -> bool:
        return (
            self.is_hosted_source
            and self.mode == HOSTED_LLVM_CAPABILITY_MODE
            and self.ok
            and self.clang_found
            and self.clangxx_found
            and self.llc_found
            and self.llc_supports_filetype_obj
            and self.llc_supports_target_object_emission
            and self.llvm_ar_found
            and self.headers_libraries_discovered
            and self.toolchain_identity_claimable
            and self.summary_native_object_emission_status
            == "native_object_emission_supported"
        )

    @property
    def hosted_source_parity_supported(self) -> bool:
        return self.hosted_native_object_emission_supported

    @property
    def hosted_native_object_emission_supported(self) -> bool:
        return (
            self.is_hosted_source
            and self.mode == HOSTED_LLVM_CAPABILITY_MODE
            and self.clang_found
            and self.llc_found
            and self.llc_supports_filetype_obj
            and self.llc_supports_target_object_emission
            and self.toolchain_identity_claimable
            and self.summary_native_object_emission_status
            == "native_object_emission_supported"
        )

    @property
    def hosted_package_archive_supported(self) -> bool:
        return self.hosted_native_object_emission_supported and self.llvm_ar_found

    @property
    def hosted_headers_libraries_supported(self) -> bool:
        return self.hosted_native_object_emission_supported and self.headers_libraries_discovered

    @property
    def local_probe_diagnostic_only(self) -> bool:
        return not self.is_hosted_source

    @property
    def fail_closed_without_live_capability(self) -> bool:
        return not self.hosted_execution_supported

    @property
    def native_object_emission_status(self) -> str:
        if not self.is_hosted_source:
            return "native_object_emission_diagnostic_only"
        if not self.llc_found:
            return "native_object_emission_missing_llc"
        if not self.llc_supports_filetype_obj:
            return "native_object_emission_filetype_obj_unavailable"
        if not self.llc_supports_target_object_emission:
            return "native_object_emission_target_object_unavailable"
        if self.summary_native_object_emission_status:
            return self.summary_native_object_emission_status
        if self.hosted_native_object_emission_supported:
            return "native_object_emission_supported"
        return "native_object_emission_unavailable"

    @property
    def hosted_runner_behavior(self) -> str:
        if self.hosted_native_object_emission_supported:
            return "native-object-emission-supported"
        return "fail-closed-no-native-object-success-claim"

    @property
    def conformance_minima_behavior(self) -> str:
        if self.hosted_native_object_emission_supported:
            return "native-object-emission-may-enter-conformance-minima"
        return "fail-closed-before-cross-lane-runtime-proof"

    def as_payload(self) -> dict[str, object]:
        return {
            "source_kind": self.source_kind,
            "mode": self.mode,
            "ok": self.ok,
            "clang_found": self.clang_found,
            "clangxx_found": self.clangxx_found,
            "llc_found": self.llc_found,
            "llc_supports_filetype_obj": self.llc_supports_filetype_obj,
            "llc_supports_target_object_emission": (
                self.llc_supports_target_object_emission
            ),
            "llvm_ar_found": self.llvm_ar_found,
            "llvm_config_found": self.llvm_config_found,
            "headers_libraries_discovered": self.headers_libraries_discovered,
            "toolchain_identity_claimable": self.toolchain_identity_claimable,
            "local_probe_diagnostic_only": self.local_probe_diagnostic_only,
            "hosted_native_object_emission_supported": (
                self.hosted_native_object_emission_supported
            ),
            "hosted_package_archive_supported": self.hosted_package_archive_supported,
            "hosted_headers_libraries_supported": (
                self.hosted_headers_libraries_supported
            ),
            "hosted_execution_supported": self.hosted_execution_supported,
            "hosted_source_parity_supported": self.hosted_source_parity_supported,
            "fail_closed_without_live_capability": (
                self.fail_closed_without_live_capability
            ),
            "native_object_emission_status": self.native_object_emission_status,
            "hosted_runner_behavior": self.hosted_runner_behavior,
            "conformance_minima_behavior": self.conformance_minima_behavior,
            "failure_reasons": list(self.failure_reasons),
        }


def _summary_section(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def _native_object_emission_status_from_summary(summary: dict[str, object]) -> str:
    support_matrix = _summary_section(summary, "llvm_support_matrix")
    native_contract = _summary_section(support_matrix, "native_object_emission_contract")
    status = native_contract.get("status")
    return str(status) if isinstance(status, str) and status else ""


def hosted_llvm_capability_truth_from_summary(
    summary: dict[str, object],
    *,
    source_kind: str,
) -> HostedLLVMCapabilityTruth:
    clang = _summary_section(summary, "clang")
    clangxx = _summary_section(summary, "clangxx")
    llc = _summary_section(summary, "llc")
    llvm_ar = _summary_section(summary, "llvm_ar")
    llvm_config = _summary_section(summary, "llvm_config")
    llc_features = _summary_section(summary, "llc_features")
    llvm_config_features = _summary_section(summary, "llvm_config_features")
    toolchain_identity = _summary_section(summary, "toolchain_identity")
    mode = summary.get("mode")
    ok = summary.get("ok") is True
    clang_found = bool(clang.get("found"))
    clangxx_found = bool(clangxx.get("found"))
    llc_found = bool(llc.get("found"))
    llvm_ar_found = bool(llvm_ar.get("found"))
    llvm_config_found = bool(llvm_config.get("found"))
    llc_supports_filetype_obj = bool(llc_features.get("supports_filetype_obj"))
    llc_supports_target_object_emission = bool(
        llc_features.get("supports_target_object_emission")
    )
    headers_libraries_discovered = bool(
        llvm_config_features.get("headers_libraries_discovered")
    )
    toolchain_identity_claimable = bool(toolchain_identity.get("claimable", False))
    summary_native_object_emission_status = _native_object_emission_status_from_summary(
        summary
    )

    failure_reasons: list[str] = []
    if source_kind != HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE:
        failure_reasons.append("local LLVM probe summary is diagnostic-only")
    if mode != HOSTED_LLVM_CAPABILITY_MODE:
        failure_reasons.append("missing canonical LLVM capability summary mode")
    if not ok:
        failure_reasons.append("summary did not publish ok=true")
    if not clang_found:
        failure_reasons.append("clang availability missing")
    if not clangxx_found:
        failure_reasons.append("clang++ availability missing")
    if not llc_found:
        failure_reasons.append("llc availability missing")
    elif not llc_supports_filetype_obj:
        failure_reasons.append("llc --filetype=obj support missing")
    elif not llc_supports_target_object_emission:
        failure_reasons.append("llc target object emission missing")
    if not toolchain_identity_claimable:
        failure_reasons.append("coherent LLVM toolchain identity missing")
    if (
        summary_native_object_emission_status
        and summary_native_object_emission_status != "native_object_emission_supported"
    ):
        failure_reasons.append(
            f"native object emission status is {summary_native_object_emission_status}"
        )
    if not llvm_ar_found:
        failure_reasons.append("llvm-ar availability missing")
    if not headers_libraries_discovered:
        if llvm_config_found:
            failure_reasons.append("llvm-config headers/libs discovery missing")
        else:
            failure_reasons.append("LLVM headers/libs discovery missing")

    return HostedLLVMCapabilityTruth(
        source_kind=source_kind,
        mode=mode,
        ok=ok,
        clang_found=clang_found,
        clangxx_found=clangxx_found,
        llc_found=llc_found,
        llc_supports_filetype_obj=llc_supports_filetype_obj,
        llc_supports_target_object_emission=llc_supports_target_object_emission,
        llvm_ar_found=llvm_ar_found,
        llvm_config_found=llvm_config_found,
        headers_libraries_discovered=headers_libraries_discovered,
        toolchain_identity_claimable=toolchain_identity_claimable,
        summary_native_object_emission_status=summary_native_object_emission_status,
        failure_reasons=tuple(failure_reasons),
    )


__all__ = [
    "CapabilityExplorerContract",
    "HostedLLVMCapabilityTruth",
    "ToolCapabilityOwnerContract",
    "hosted_llvm_capability_truth_from_summary",
]
