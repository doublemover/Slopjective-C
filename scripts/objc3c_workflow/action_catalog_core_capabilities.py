"""LLVM capability action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_CAPABILITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-ci-llvm": ActionSpec(
        "materialize-ci-llvm",
        "install the coherent LLVM prerequisites on the selected hosted CI platform",
        "runner-internal",
        validation_tier="ci",
        guarantee_owner=(
            "explicit host selection routes to its existing LLVM installer; "
            "installation does not promote language or platform support"
        ),
        pass_through_args=True,
    ),
    "check-llvm-capabilities": ActionSpec(
        "check-llvm-capabilities",
        "probe llvm capability availability and write the summary artifact",
        "python:scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        validation_tier="repo",
        guarantee_owner=(
            "llvm capability probe output stays tied to the live toolchain environment"
        ),
        pass_through_args=True,
    ),
    "check-hosted-llvm-capabilities": ActionSpec(
        "check-hosted-llvm-capabilities",
        "probe hosted-runner llvm capability availability and fail required gates without llc object emission",
        "python:scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json",
        validation_tier="ci",
        guarantee_owner=(
            "hosted CI publishes capability truth only from coherent llc "
            "--filetype=obj evidence; optional gates skip without success "
            "claims, and required conformance gates fail closed"
        ),
    ),
}
