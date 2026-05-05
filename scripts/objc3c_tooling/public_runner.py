"""Helpers for loading the canonical objc3c workflow runner."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT

WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_RUNNER_MODULE = "scripts.objc3c_workflow.runner"
DEFAULT_RUNNER_PATH = ROOT / "scripts" / "objc3c_workflow" / "runner.py"
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))


def public_workflow_command(*args: str) -> list[str]:
    return [sys.executable, "-m", WORKFLOW_MODULE, *args]


def load_public_workflow_runner(
    *,
    runner_path: Path = DEFAULT_RUNNER_PATH,
    module_name: str = WORKFLOW_RUNNER_MODULE,
) -> Any:
    if runner_path == DEFAULT_RUNNER_PATH and module_name == WORKFLOW_RUNNER_MODULE:
        from scripts.objc3c_workflow import runner

        return runner

    import importlib.util

    spec = importlib.util.spec_from_file_location(module_name, runner_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load public workflow runner from {runner_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module
