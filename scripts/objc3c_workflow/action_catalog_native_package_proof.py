"""Native compile proof action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

NATIVE_PACKAGE_PROOF_ACTION_SPECS: dict[str, ActionSpec] = {
    "proof-objc3c": ActionSpec(
        "proof-objc3c",
        "run the native compile proof workflow",
        "pwsh:scripts/run_objc3c_native_compile_proof.ps1",
        validation_tier="repo",
        guarantee_owner=(
            "native compile proof stays exposed as a public workflow action with "
            "PowerShell script ownership pinned to the checked-in proof entrypoint"
        ),
    ),
}

__all__ = ["NATIVE_PACKAGE_PROOF_ACTION_SPECS"]
