"""Public-command documentation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .docs_paths import (
    PUBLIC_COMMAND_BUDGET_PY,
    PUBLIC_COMMAND_CONTRACT_PY,
    PUBLIC_COMMAND_SURFACE_PY,
)


def action_build_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)])


def action_check_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"])


def action_build_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY)])


def action_check_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY), "--check"])


def action_check_public_command_budget(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_BUDGET_PY)])
