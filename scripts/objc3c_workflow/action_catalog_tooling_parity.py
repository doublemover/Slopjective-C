"""Developer tooling parity action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_PARITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-capability-routed-source-parity": ActionSpec(
        "test-capability-routed-source-parity",
        "validate live library/CLI source parity when hosted llc object emission is available",
        "python:scripts/check_objc3c_library_cli_parity.py --route-cli-backend-from-capabilities",
        validation_tier="ci",
        guarantee_owner=(
            "hosted CI parity validation follows the llvm capability probe instead of "
            "duplicating helper commands in workflow action definitions"
        ),
    ),
}
