"""Canonical public command helpers for objc3c workflow callers."""

from __future__ import annotations

from collections.abc import Iterable
from pathlib import Path
from typing import Any

from .path_bootstrap import install_workflow_import_roots
from .public_bridge_constants import WORKFLOW_BRIDGE_SCRIPT, WORKFLOW_MODULE

install_workflow_import_roots()

WORKFLOW_DISPATCH_MODULE = "scripts.objc3c_workflow.action_dispatch"
WORKFLOW_PACKAGE_MANAGER = "npm"
WORKFLOW_NPM_ARGUMENT_SEPARATOR = "--"
WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS = (
    WORKFLOW_PACKAGE_MANAGER,
    "run",
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_NPM_ARGUMENT_SEPARATOR,
)
DEFAULT_DISPATCH_PATH = (
    Path(__file__).resolve().parent / "action_dispatch.py"
)


def public_workflow_command(*args: str) -> list[str]:
    return [*WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS, *args]


def public_workflow_command_tuple(*args: str) -> tuple[str, ...]:
    return tuple(public_workflow_command(*args))


def public_workflow_list_command() -> list[str]:
    return public_workflow_command("--list-json")


def public_workflow_describe_command(action: str) -> list[str]:
    return public_workflow_command("--describe", action)


def public_workflow_action_names() -> list[str]:
    from .registry_views import action_names

    return action_names()


def public_workflow_action_count() -> int:
    from .registry_views import action_count

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
    from .action_payloads import list_actions_payload

    return list_actions_payload()


def public_workflow_action_payload(action: str) -> dict[str, object]:
    from .action_payloads import describe_action_payload

    return describe_action_payload(action)


def public_workflow_action_payloads() -> list[dict[str, object]]:
    return [
        public_workflow_action_payload(action)
        for action in public_workflow_action_names()
    ]


def public_workflow_package_bridge_payload(script_name: str) -> dict[str, object]:
    from .npm_surface import describe_package_script_payload

    return describe_package_script_payload(script_name)


def load_public_workflow_runner(
    *,
    runner_path: Path = DEFAULT_DISPATCH_PATH,
    module_name: str = WORKFLOW_DISPATCH_MODULE,
) -> Any:
    if runner_path == DEFAULT_DISPATCH_PATH and module_name == WORKFLOW_DISPATCH_MODULE:
        from . import action_dispatch

        return action_dispatch

    import importlib.util
    import sys

    spec = importlib.util.spec_from_file_location(module_name, runner_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load public workflow runner from {runner_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


__all__ = [
    "DEFAULT_DISPATCH_PATH",
    "WORKFLOW_DISPATCH_MODULE",
    "WORKFLOW_MODULE",
    "WORKFLOW_NPM_ARGUMENT_SEPARATOR",
    "WORKFLOW_PACKAGE_MANAGER",
    "WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS",
    "load_public_workflow_runner",
    "public_workflow_action_count",
    "public_workflow_action_identifiers",
    "public_workflow_action_names",
    "public_workflow_action_payload",
    "public_workflow_action_payloads",
    "public_workflow_command",
    "public_workflow_command_tuple",
    "public_workflow_describe_command",
    "public_workflow_has_action_identifiers",
    "public_workflow_has_actions",
    "public_workflow_list_command",
    "public_workflow_list_payload",
    "public_workflow_package_bridge_payload",
]
