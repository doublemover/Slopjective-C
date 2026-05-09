"""Registry-level payload ownership for workflow action listings."""

from __future__ import annotations

from collections.abc import Sequence

from .action_payload_fields import build_action_payload
from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .public_bridge import PACKAGE_BRIDGES


def build_registry_payload(specs: Sequence[ActionSpec]) -> dict[str, object]:
    action_count = len(specs)
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": action_count,
        "public_action_count": action_count,
        "internal_action_count": 0,
        "package_bridge_count": len(PACKAGE_BRIDGES),
        "package_bridges": list(PACKAGE_BRIDGES),
        "single_package_bridge_only": True,
        "actions": [build_action_payload(spec) for spec in specs],
    }
