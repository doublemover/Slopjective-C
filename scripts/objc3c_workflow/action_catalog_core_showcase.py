"""Showcase, stdlib surface, and getting-started action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_SHOWCASE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-showcase-surface": ActionSpec("check-showcase-surface", "check the live showcase portfolio and compile its example sources through the public compiler path", "python:scripts/check_showcase_surface.py", validation_tier="repo", guarantee_owner="showcase examples stay compile-coupled, checked in, and tied to the public compiler path", pass_through_args=True),
    "check-stdlib-surface": ActionSpec("check-stdlib-surface", "check the checked-in stdlib boundary contracts, canonical module inventory, package-name mapping, and lowering/import artifact contract", "python:scripts/check_stdlib_surface.py", validation_tier="repo", guarantee_owner="stdlib roots, canonical module inventory, package-name mapping, and lowering/import artifact contract stay checked in and coherent"),
    "validate-showcase-runtime": ActionSpec("validate-showcase-runtime", "compile, link, and run the checked-in showcase examples through the live runtime launch contract", "pwsh:scripts/check_showcase_runtime.ps1", validation_tier="repo", guarantee_owner="showcase examples stay runnable through the real runtime archive and launch-contract wiring", pass_through_args=True),
    "validate-showcase": ActionSpec("validate-showcase", "run the integrated showcase compile and runtime validation flow", "python:scripts/check_showcase_integration.py", validation_tier="repo", guarantee_owner="showcase examples stay compiled, runnable, and wired into the normal repo validation path"),
    "validate-runnable-showcase": ActionSpec("validate-runnable-showcase", "run packaged showcase compile link and execution validation from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_showcase_end_to_end.py", validation_tier="full", guarantee_owner="showcase examples stay publishable and runnable from the staged runnable toolchain bundle"),
    "validate-getting-started": ActionSpec("validate-getting-started", "run the integrated getting-started tutorial and onboarding validation flow", "python:scripts/check_getting_started_integration.py", validation_tier="repo", guarantee_owner="getting-started tutorials stay compile-coupled, runnable, and wired into the normal repo validation path"),
}
