"""Helpers for loading the canonical objc3c workflow public surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT

WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_DISPATCH_MODULE = "scripts.objc3c_workflow.action_dispatch"
DEFAULT_DISPATCH_PATH = ROOT / "scripts" / "objc3c_workflow" / "action_dispatch.py"
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    if str(import_root) not in sys.path:
        sys.path.insert(0, str(import_root))


def public_workflow_command(*args: str) -> list[str]:
    return [sys.executable, "-m", WORKFLOW_MODULE, *args]


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
