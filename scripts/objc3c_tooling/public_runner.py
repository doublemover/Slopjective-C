"""Helpers for loading the canonical objc3c workflow public surface."""

from __future__ import annotations

import sys
from collections.abc import Iterable
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT

WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_DISPATCH_MODULE = "scripts.objc3c_workflow.action_dispatch"
DEFAULT_DISPATCH_PATH = ROOT / "scripts" / "objc3c_workflow" / "action_dispatch.py"
WORKFLOW_PACKAGE_MANAGER = "npm"
WORKFLOW_PACKAGE_SCRIPT = "objc3c"
WORKFLOW_NPM_ARGUMENT_SEPARATOR = "--"
WORKFLOW_PUBLIC_COMMAND_PREFIX = (
    WORKFLOW_PACKAGE_MANAGER,
    "run",
    WORKFLOW_PACKAGE_SCRIPT,
    WORKFLOW_NPM_ARGUMENT_SEPARATOR,
)
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    if str(import_root) not in sys.path:
        sys.path.insert(0, str(import_root))


def public_workflow_command(*args: str) -> list[str]:
    return [*WORKFLOW_PUBLIC_COMMAND_PREFIX, *args]


def public_workflow_command_tuple(*args: str) -> tuple[str, ...]:
    return tuple(public_workflow_command(*args))


def public_workflow_list_command() -> list[str]:
    return public_workflow_command("--list-json")


def public_workflow_describe_command(action: str) -> list[str]:
    return public_workflow_command("--describe", action)


def public_workflow_action_names() -> list[str]:
    from scripts.objc3c_workflow.registry_views import action_names

    return action_names()


def public_workflow_action_count() -> int:
    from scripts.objc3c_workflow.registry_views import action_count

    return action_count()


def public_workflow_action_identifiers() -> set[str]:
    identifiers: set[str] = set()
    for action in public_workflow_action_names():
        function_stem = action.replace("-", "_")
        identifiers.add(action)
        identifiers.add(function_stem)
        identifiers.add(f"action_{function_stem}")
    return identifiers


def public_workflow_has_actions(actions: Iterable[str]) -> bool:
    available = set(public_workflow_action_names())
    return all(action in available for action in actions)


def public_workflow_has_action_identifiers(identifiers: Iterable[str]) -> bool:
    available = public_workflow_action_identifiers()
    return all(identifier in available for identifier in identifiers)


def public_workflow_list_payload() -> dict[str, object]:
    from scripts.objc3c_workflow.action_payloads import list_actions_payload

    return list_actions_payload()


def public_workflow_action_payload(action: str) -> dict[str, object]:
    from scripts.objc3c_workflow.action_payloads import describe_action_payload

    return describe_action_payload(action)


def public_workflow_action_payloads() -> list[dict[str, object]]:
    return [public_workflow_action_payload(action) for action in public_workflow_action_names()]


def public_workflow_package_bridge_payload(script_name: str) -> dict[str, object]:
    from scripts.objc3c_workflow.npm_surface import describe_package_script_payload

    return describe_package_script_payload(script_name)


def load_public_workflow_runner(
    *,
    runner_path: Path = DEFAULT_DISPATCH_PATH,
    module_name: str = WORKFLOW_DISPATCH_MODULE,
) -> Any:
    if runner_path == DEFAULT_DISPATCH_PATH and module_name == WORKFLOW_DISPATCH_MODULE:
        from scripts.objc3c_workflow import action_dispatch

        return action_dispatch

    import importlib.util

    spec = importlib.util.spec_from_file_location(module_name, runner_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load public workflow runner from {runner_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module
