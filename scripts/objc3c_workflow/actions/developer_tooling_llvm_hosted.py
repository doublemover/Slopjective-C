"""Hosted LLVM capability policy workflow action."""

from __future__ import annotations

import sys

from .developer_tooling_llvm_probe import run_hosted_llvm_probe


def _tool_summary(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def action_check_hosted_llvm_capabilities(_: list[str]) -> int:
    probe_exit, summary = run_hosted_llvm_probe()
    if probe_exit == 0:
        print("Hosted runner exposes clang and llc object-emission capability.")
        return 0

    clang = _tool_summary(summary, "clang")
    llc = _tool_summary(summary, "llc")
    llc_features = _tool_summary(summary, "llc_features")
    if not bool(clang.get("found")):
        print(
            "Hosted runner capability probe failed without clang availability.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if bool(llc.get("found")) or bool(llc_features.get("supports_filetype_obj")):
        print(
            "Hosted runner capability probe reported an unexpected llc failure mode.",
            file=sys.stderr,
        )
        return probe_exit or 1
    print(
        "Hosted runner policy accepted clang availability without "
        "llc --filetype=obj; capability-routed source parity remains gated by "
        "the recorded capability summary."
    )
    return 0
