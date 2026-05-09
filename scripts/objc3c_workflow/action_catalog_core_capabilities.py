"""LLVM capability action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_CAPABILITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-llvm-capabilities": ActionSpec(
        "check-llvm-capabilities",
        "probe llvm capability availability and write the summary artifact",
        "python:scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        validation_tier="repo",
        guarantee_owner=(
            "llvm capability probe output stays tied to the live toolchain environment"
        ),
    ),
    "check-hosted-llvm-capabilities": ActionSpec(
        "check-hosted-llvm-capabilities",
        "probe hosted-runner llvm capability availability and fail closed without object emission",
        "python:scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json",
        validation_tier="ci",
        guarantee_owner=(
            "hosted Windows CI publishes capability truth only from clang plus llc "
            "object-emission probe evidence"
        ),
    ),
}
