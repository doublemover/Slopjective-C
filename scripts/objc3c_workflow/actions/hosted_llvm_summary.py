"""Hosted LLVM capability summary helpers for workflow actions."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json

from .developer_tooling_paths import HOSTED_LLVM_CAPABILITIES_SUMMARY

HOSTED_LLVM_CAPABILITY_MODE = "objc3c-llvm-capabilities-v2"


def hosted_llvm_summary() -> dict[str, object]:
    return (
        load_json(HOSTED_LLVM_CAPABILITIES_SUMMARY)
        if HOSTED_LLVM_CAPABILITIES_SUMMARY.is_file()
        else {}
    )


def summary_section(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def _native_object_emission_contract_status(summary: dict[str, object]) -> str:
    support_matrix = summary_section(summary, "llvm_support_matrix")
    native_contract = summary_section(support_matrix, "native_object_emission_contract")
    status = native_contract.get("status")
    return str(status) if isinstance(status, str) and status else ""


def _toolchain_identity_failure_status(toolchain_identity: dict[str, object]) -> str:
    if str(toolchain_identity.get("root_status")) == "mixed":
        return "native_object_emission_mixed_toolchain_root"
    version_status = str(toolchain_identity.get("version_status"))
    if version_status == "mismatched":
        return "native_object_emission_mismatched_tool_versions"
    if version_status == "unsupported":
        return "native_object_emission_unsupported_tool_version"
    return "native_object_emission_unresolved_tool_version"


def hosted_llc_object_emission_available() -> bool:
    return hosted_native_object_emission_status() == "native_object_emission_supported"


def hosted_native_object_emission_status() -> str:
    summary = hosted_llvm_summary()
    if summary.get("mode") != HOSTED_LLVM_CAPABILITY_MODE:
        return "native_object_emission_unavailable"
    clang = summary_section(summary, "clang")
    llc = summary_section(summary, "llc")
    llc_features = summary_section(summary, "llc_features")
    if not bool(clang.get("found")):
        return "native_object_emission_unavailable"
    if not bool(llc.get("found")):
        return "native_object_emission_missing_llc"
    if not bool(llc_features.get("supports_filetype_obj")):
        return "native_object_emission_filetype_obj_unavailable"
    if not bool(llc_features.get("supports_target_object_emission")):
        return "native_object_emission_target_object_unavailable"
    contract_status = _native_object_emission_contract_status(summary)
    toolchain_identity = summary_section(summary, "toolchain_identity")
    if contract_status and contract_status != "native_object_emission_supported":
        return contract_status
    if not bool(toolchain_identity.get("claimable", False)):
        return _toolchain_identity_failure_status(toolchain_identity)
    if contract_status:
        return contract_status
    return "native_object_emission_supported"


def hosted_full_toolchain_matrix_available() -> bool:
    summary = hosted_llvm_summary()
    if hosted_native_object_emission_status() != "native_object_emission_supported":
        return False
    clangxx = summary_section(summary, "clangxx")
    llvm_ar = summary_section(summary, "llvm_ar")
    llvm_config_features = summary_section(summary, "llvm_config_features")
    toolchain_identity = summary_section(summary, "toolchain_identity")
    return (
        summary.get("ok") is True
        and bool(clangxx.get("found"))
        and bool(llvm_ar.get("found"))
        and bool(llvm_config_features.get("headers_libraries_discovered"))
        and bool(toolchain_identity.get("claimable", False))
    )


__all__ = [
    "HOSTED_LLVM_CAPABILITY_MODE",
    "hosted_llc_object_emission_available",
    "hosted_full_toolchain_matrix_available",
    "hosted_native_object_emission_status",
    "hosted_llvm_summary",
    "summary_section",
]
