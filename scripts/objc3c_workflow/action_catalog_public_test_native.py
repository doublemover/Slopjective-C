"""Focused native public test action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PUBLIC_TEST_NATIVE_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-behavior-matrix": ActionSpec(
        "test-behavior-matrix",
        "behavior-first native fixture matrix",
        "python:scripts/check_objc3c_behavior_matrix.py",
        validation_tier="smoke",
        guarantee_owner=(
            "parser, sema, lowering, IR, runtime, and e2e behavior fixtures execute "
            "from tests/native"
        ),
    ),
    "test-recovery": ActionSpec(
        "test-recovery",
        "native recovery contract suite",
        "pwsh:scripts/check_objc3c_native_recovery_contract.ps1",
        validation_tier="recovery",
        guarantee_owner="recovery compile success and deterministic recovery diagnostics",
        pass_through_args=True,
    ),
    "test-compile-wrapper-self-audit": ActionSpec(
        "test-compile-wrapper-self-audit",
        "native compile wrapper self-audit",
        "python:scripts/check_objc3c_compile_wrapper_self_audit.py",
        validation_tier="fast",
        guarantee_owner=(
            "one wrapper compile proves invariant compile-output provenance, truthfulness, "
            "registration digest binding, and required artifact publication"
        ),
    ),
    "test-llvm-capability-routing": ActionSpec(
        "test-llvm-capability-routing",
        "targeted llvm capability routing and library/CLI parity unit tests",
        (
            "python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py "
            "tests/tooling/test_objc3c_library_cli_parity.py::<capability-routing-cases> "
            "tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py "
            "tests/tooling/test_objc3c_driver_cli_extraction.py -q"
        ),
        validation_tier="ci",
        guarantee_owner=(
            "capability routing tests stay represented as a named public workflow action"
        ),
    ),
    "test-execution-smoke": ActionSpec(
        "test-execution-smoke",
        "native execution smoke suite",
        "pwsh:scripts/check_objc3c_native_execution_smoke.ps1",
        validation_tier="smoke",
        guarantee_owner="compile/link/run execution behavior",
        pass_through_args=True,
    ),
    "test-hosted-execution-smoke": ActionSpec(
        "test-hosted-execution-smoke",
        "hosted-runner execution smoke suite gated by the live LLVM capability summary",
        "runner-internal + pwsh:scripts/check_objc3c_native_execution_smoke.ps1",
        validation_tier="ci",
        guarantee_owner=(
            "GitHub Actions execution smoke routing stays inside the public workflow bridge "
            "and skips only when hosted llc object emission is unavailable"
        ),
    ),
    "test-execution-replay": ActionSpec(
        "test-execution-replay",
        "native execution replay proof suite",
        "pwsh:scripts/check_objc3c_execution_replay_proof.ps1",
        validation_tier="full",
        guarantee_owner="replay and native-output truth",
        pass_through_args=True,
    ),
    "test-execution-replay-focused": ActionSpec(
        "test-execution-replay-focused",
        "focused native execution replay proof slice",
        "pwsh:scripts/check_objc3c_execution_replay_proof.ps1 -Limit 1",
        validation_tier="fast",
        guarantee_owner=(
            "one canonical replay case for ordinary developer validation while exhaustive "
            "replay remains available"
        ),
    ),
}
