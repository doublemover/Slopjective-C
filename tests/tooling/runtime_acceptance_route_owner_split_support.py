from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "runtime_acceptance_route_model",
    "runtime_acceptance_route_catalog",
    "runtime_acceptance_command_runner",
)

PUBLIC_RUNTIME_ACCEPTANCE_ACTIONS = {
    "test-runtime-acceptance",
    "test-runtime-acceptance-fast",
    "test-runtime-acceptance-diagnostics",
    "test-runtime-acceptance-cross-module",
    "test-runtime-acceptance-block-arc",
    "test-runtime-acceptance-arc-cleanup-integration",
    "test-runtime-acceptance-concurrency",
}


def custom_runtime_acceptance_script() -> Path:
    return Path("tmp/custom-runtime-acceptance.py")
