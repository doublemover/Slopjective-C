"""Hosted LLVM capability summary helpers for workflow actions."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json

from .developer_tooling_paths import HOSTED_LLVM_CAPABILITIES_SUMMARY


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
    summary = hosted_llvm_summary()
    llc = summary_section(summary, "llc")
    llc_features = summary_section(summary, "llc_features")
    return bool(llc.get("found")) and bool(llc_features.get("supports_filetype_obj"))


__all__ = [
    "hosted_llc_object_emission_available",
    "hosted_llvm_summary",
    "summary_section",
]
