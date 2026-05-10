"""Public workflow dispatch module loading."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from .public_command_constants import DEFAULT_DISPATCH_PATH, WORKFLOW_DISPATCH_MODULE


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


__all__ = ["load_public_workflow_runner"]
