"""Helpers for loading the public workflow runner without package imports."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT

DEFAULT_RUNNER_PATH = ROOT / "scripts" / "objc3c_public_workflow_runner.py"


def load_public_workflow_runner(
    *,
    runner_path: Path = DEFAULT_RUNNER_PATH,
    module_name: str = "objc3c_public_workflow_runner_dynamic",
) -> Any:
    spec = importlib.util.spec_from_file_location(module_name, runner_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load public workflow runner from {runner_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module

