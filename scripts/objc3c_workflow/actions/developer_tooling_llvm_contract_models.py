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
    llc_found: bool
    llc_supports_filetype_obj: bool
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
            and self.llc_found
            and self.llc_supports_filetype_obj
        )

    @property
    def hosted_source_parity_supported(self) -> bool:
        return self.hosted_execution_supported

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
        if self.hosted_execution_supported:
            return "native_object_emission_supported"
        if not self.llc_found:
            return "native_object_emission_missing_llc"
        if not self.llc_supports_filetype_obj:
            return "native_object_emission_filetype_obj_unavailable"
        return "native_object_emission_unavailable"

    @property
    def hosted_runner_behavior(self) -> str:
        if self.hosted_execution_supported:
            return "native-object-emission-supported"
        return "fail-closed-no-native-object-success-claim"

    @property
    def conformance_minima_behavior(self) -> str:
        if self.hosted_execution_supported:
            return "native-object-emission-may-enter-conformance-minima"
        return "fail-closed-before-cross-lane-runtime-proof"

    def as_payload(self) -> dict[str, object]:
        return {
            "source_kind": self.source_kind,
            "mode": self.mode,
            "ok": self.ok,
            "clang_found": self.clang_found,
            "llc_found": self.llc_found,
            "llc_supports_filetype_obj": self.llc_supports_filetype_obj,
            "local_probe_diagnostic_only": self.local_probe_diagnostic_only,
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


def hosted_llvm_capability_truth_from_summary(
    summary: dict[str, object],
    *,
    source_kind: str,
) -> HostedLLVMCapabilityTruth:
    clang = _summary_section(summary, "clang")
    llc = _summary_section(summary, "llc")
    llc_features = _summary_section(summary, "llc_features")
    mode = summary.get("mode")
    ok = summary.get("ok") is True
    clang_found = bool(clang.get("found"))
    llc_found = bool(llc.get("found"))
    llc_supports_filetype_obj = bool(llc_features.get("supports_filetype_obj"))

    failure_reasons: list[str] = []
    if source_kind != HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE:
        failure_reasons.append("local LLVM probe summary is diagnostic-only")
    if mode != HOSTED_LLVM_CAPABILITY_MODE:
        failure_reasons.append("missing canonical LLVM capability summary mode")
    if not ok:
        failure_reasons.append("summary did not publish ok=true")
    if not clang_found:
        failure_reasons.append("clang availability missing")
    if not llc_found:
        failure_reasons.append("llc availability missing")
    elif not llc_supports_filetype_obj:
        failure_reasons.append("llc --filetype=obj support missing")

    return HostedLLVMCapabilityTruth(
        source_kind=source_kind,
        mode=mode,
        ok=ok,
        clang_found=clang_found,
        llc_found=llc_found,
        llc_supports_filetype_obj=llc_supports_filetype_obj,
        failure_reasons=tuple(failure_reasons),
    )


__all__ = [
    "CapabilityExplorerContract",
    "HostedLLVMCapabilityTruth",
    "ToolCapabilityOwnerContract",
    "hosted_llvm_capability_truth_from_summary",
]
