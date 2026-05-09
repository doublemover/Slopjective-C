"""Hosted LLVM capability policy workflow action."""

from __future__ import annotations

import sys

from .developer_tooling_llvm_probe import run_hosted_llvm_probe
from .hosted_llvm_summary import HOSTED_LLVM_CAPABILITY_MODE


def _tool_summary(summary: dict[str, object], key: str) -> dict[str, object]:
    value = summary.get(key)
    return value if isinstance(value, dict) else {}


def action_check_hosted_llvm_capabilities(_: list[str]) -> int:
    probe_exit, summary = run_hosted_llvm_probe()
    clang = _tool_summary(summary, "clang")
    llc = _tool_summary(summary, "llc")
    llc_features = _tool_summary(summary, "llc_features")
    if (
        probe_exit == 0
        and summary.get("mode") == HOSTED_LLVM_CAPABILITY_MODE
        and summary.get("ok") is True
        and bool(clang.get("found"))
        and bool(llc.get("found"))
        and bool(llc_features.get("supports_filetype_obj"))
    ):
        print("Hosted runner exposes clang and llc object-emission capability.")
        return 0

    if summary.get("mode") != HOSTED_LLVM_CAPABILITY_MODE:
        print(
            "Hosted runner capability probe failed without the canonical "
            "LLVM capability summary mode.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if summary.get("ok") is not True:
        print(
            "Hosted runner capability probe did not publish an ok capability "
            "summary; hosted support claims remain unavailable.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if not bool(clang.get("found")):
        print(
            "Hosted runner capability probe failed without clang availability.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if not bool(llc.get("found")):
        print(
            "Hosted runner capability probe failed without llc availability; "
            "clang-only hosted execution is not a supported capability claim.",
            file=sys.stderr,
        )
        return probe_exit or 1
    print(
        "Hosted runner capability probe failed without llc --filetype=obj; "
        "hosted source parity and execution support claims are unavailable.",
        file=sys.stderr,
    )
    return probe_exit or 1
