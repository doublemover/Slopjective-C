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
    if summary.get("ok") is not True:
        return "native_object_emission_unavailable"
    return "native_object_emission_supported"


__all__ = [
    "HOSTED_LLVM_CAPABILITY_MODE",
    "hosted_llc_object_emission_available",
    "hosted_native_object_emission_status",
    "hosted_llvm_summary",
    "summary_section",
]
