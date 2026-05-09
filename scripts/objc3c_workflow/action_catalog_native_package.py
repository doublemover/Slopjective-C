"""Native package/proof action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

NATIVE_PACKAGE_ACTION_SPECS: dict[str, ActionSpec] = {
    "package-runnable-toolchain": ActionSpec("package-runnable-toolchain", "package the runnable native toolchain", "pwsh:scripts/package_objc3c_runnable_toolchain.ps1"),
    "proof-objc3c": ActionSpec("proof-objc3c", "run the native compile proof workflow", "pwsh:scripts/run_objc3c_native_compile_proof.ps1"),
}
