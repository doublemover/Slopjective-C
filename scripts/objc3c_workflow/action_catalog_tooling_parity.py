"""Developer tooling parity action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_PARITY_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-capability-routed-source-parity": ActionSpec(
        "test-capability-routed-source-parity",
        "validate live library/CLI source parity from hosted llc object-emission proof",
        "python:scripts/check_objc3c_library_cli_parity.py --route-cli-backend-from-capabilities",
        validation_tier="ci",
        guarantee_owner=(
            "hosted CI parity validation fails closed unless the llvm capability "
            "probe proves the native object route"
        ),
    ),
}
