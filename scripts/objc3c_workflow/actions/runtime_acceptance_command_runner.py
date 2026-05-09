"""Runtime acceptance workflow command construction and execution."""

from __future__ import annotations

from pathlib import Path

from ..commands import run
from ..environment import ROOT
from .runtime_acceptance_route_catalog import runtime_acceptance_route

RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"


def runtime_acceptance_command(
    action: str,
    runtime_acceptance_script: Path = RUNTIME_ACCEPTANCE_PY,
) -> list[str]:
    return runtime_acceptance_route(action).command(runtime_acceptance_script)


def run_runtime_acceptance_action(action: str) -> int:
    return run(runtime_acceptance_command(action))
