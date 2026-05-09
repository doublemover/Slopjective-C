"""Hosted LLVM capability policy workflow action."""

from __future__ import annotations

import sys

from .developer_tooling_llvm_contracts import (
    HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    hosted_llvm_capability_truth_from_summary,
)
from .developer_tooling_llvm_probe import run_hosted_llvm_probe
from .hosted_llvm_summary import HOSTED_LLVM_CAPABILITY_MODE


def action_check_hosted_llvm_capabilities(_: list[str]) -> int:
    probe_exit, summary = run_hosted_llvm_probe()
    truth = hosted_llvm_capability_truth_from_summary(
        summary,
        source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    )
    if probe_exit == 0 and truth.hosted_execution_supported:
        print("Hosted runner exposes clang and llc object-emission capability.")
        return 0

    if truth.mode != HOSTED_LLVM_CAPABILITY_MODE:
        print(
            "Hosted runner capability probe failed without the canonical "
            "LLVM capability summary mode.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if not truth.ok:
        print(
            "Hosted runner capability probe did not publish an ok capability "
            "summary; hosted support claims remain unavailable.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if not truth.clang_found:
        print(
            "Hosted runner capability probe failed without clang availability.",
            file=sys.stderr,
        )
        return probe_exit or 1
    if not truth.llc_found:
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
