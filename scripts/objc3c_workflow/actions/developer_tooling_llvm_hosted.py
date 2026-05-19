"""Hosted LLVM capability policy workflow action."""

from __future__ import annotations

import sys

from .developer_tooling_llvm_contracts import (
    HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    hosted_llvm_capability_truth_from_summary,
)
from .developer_tooling_llvm_probe import run_hosted_llvm_probe
from .hosted_llvm_summary import HOSTED_LLVM_CAPABILITY_MODE

_PARITY_NOT_READY_FAILURE = (
    "capability demo compatibility requires sema/type-system parity to stay ready"
)


def _capability_truth_failures(summary: dict[str, object]) -> list[str]:
    compatibility = summary.get("capability_demo_compatibility")
    if not isinstance(compatibility, dict):
        return ["missing capability demo compatibility surface"]

    failures = compatibility.get("failures")
    if not isinstance(failures, list):
        return ["malformed capability demo compatibility failures"]

    return [
        str(failure)
        for failure in failures
        if str(failure) != _PARITY_NOT_READY_FAILURE
    ]


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

    truth_failures = _capability_truth_failures(summary)
    if truth_failures:
        print(
            "Hosted runner capability probe found checked-in capability truth "
            "drift; hosted support claims remain unavailable.",
            file=sys.stderr,
        )
        for failure in truth_failures:
            print(f"Hosted capability truth failure: {failure}", file=sys.stderr)
        return 1
    if not truth.clang_found:
        print(
            "Hosted runner capability summary recorded no clang availability; "
            "hosted support claims remain unavailable."
        )
        return 0
    if not truth.llc_found:
        print(
            "Hosted runner capability summary recorded no llc availability; "
            "clang-only hosted execution is not a supported capability claim."
        )
        return 0
    print(
        "Hosted runner capability summary recorded no llc --filetype=obj support; "
        "hosted source parity and execution support claims are unavailable."
    )
    return 0
