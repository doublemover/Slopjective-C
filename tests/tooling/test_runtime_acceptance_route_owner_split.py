from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions.runtime_acceptance_command_runner import (
    runtime_acceptance_command,
)
from scripts.objc3c_workflow.actions.runtime_acceptance_route_catalog import (
    RUNTIME_ACCEPTANCE_ROUTES,
)
from scripts.objc3c_workflow.actions.runtime_acceptance_route_model import (
    RuntimeAcceptanceRoute,
)
from scripts.objc3c_workflow.actions.runtime_acceptance_routes import (
    runtime_acceptance_command as public_runtime_acceptance_command,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "runtime_acceptance_route_model",
    "runtime_acceptance_route_catalog",
    "runtime_acceptance_command_runner",
)


def test_runtime_acceptance_routes_is_public_import_surface_only() -> None:
    facade_text = (ACTION_ROOT / "runtime_acceptance_routes.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.actions.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "dataclass" not in facade_text
    assert "from ..commands import run" not in facade_text
    assert "check_objc3c_runtime_acceptance.py" not in facade_text


def test_runtime_acceptance_route_catalog_preserves_public_suites() -> None:
    assert set(RUNTIME_ACCEPTANCE_ROUTES) == {
        "test-runtime-acceptance",
        "test-runtime-acceptance-fast",
        "test-runtime-acceptance-diagnostics",
        "test-runtime-acceptance-cross-module",
        "test-runtime-acceptance-block-arc",
        "test-runtime-acceptance-concurrency",
    }
    for action, route in RUNTIME_ACCEPTANCE_ROUTES.items():
        assert isinstance(route, RuntimeAcceptanceRoute)
        assert route.action == action
        assert route.target.endswith(f"--suite {route.suite}")


def test_runtime_acceptance_command_uses_explicit_script_path() -> None:
    script = Path("tmp/custom-runtime-acceptance.py")

    command = runtime_acceptance_command(
        "test-runtime-acceptance-fast",
        runtime_acceptance_script=script,
    )

    assert command[-3:] == [str(script), "--suite", "fast"]
    assert public_runtime_acceptance_command is runtime_acceptance_command
