"""Public-command documentation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .docs_paths import (
    PUBLIC_COMMAND_BUDGET_SCRIPT,
    PUBLIC_COMMAND_BUDGET_PY,
    PUBLIC_COMMAND_CONTRACT_SCRIPT,
    PUBLIC_COMMAND_CONTRACT_PY,
    PUBLIC_COMMAND_SURFACE_SCRIPT,
    PUBLIC_COMMAND_SURFACE_PY,
)

BUILD_PUBLIC_COMMAND_SURFACE_ACTION = "build-public-command-surface"
CHECK_PUBLIC_COMMAND_SURFACE_ACTION = "check-public-command-surface"
BUILD_PUBLIC_COMMAND_CONTRACT_ACTION = "build-public-command-contract"
CHECK_PUBLIC_COMMAND_CONTRACT_ACTION = "check-public-command-contract"
CHECK_PUBLIC_COMMAND_BUDGET_ACTION = "check-public-command-budget"

BUILD_PUBLIC_COMMAND_SURFACE_SUMMARY = (
    "build the generated public command-surface appendix"
)
CHECK_PUBLIC_COMMAND_SURFACE_SUMMARY = (
    "check the generated public command-surface appendix for drift"
)
BUILD_PUBLIC_COMMAND_CONTRACT_SUMMARY = (
    "build the canonical public command contract artifact"
)
CHECK_PUBLIC_COMMAND_CONTRACT_SUMMARY = (
    "check the canonical public command contract artifact for drift"
)
CHECK_PUBLIC_COMMAND_BUDGET_SUMMARY = (
    "check the public command budget and appendix sync against the canonical "
    "command contract"
)

BUILD_PUBLIC_COMMAND_SURFACE_BACKEND = f"python:{PUBLIC_COMMAND_SURFACE_SCRIPT}"
CHECK_PUBLIC_COMMAND_SURFACE_BACKEND = (
    f"{BUILD_PUBLIC_COMMAND_SURFACE_BACKEND} --check"
)
BUILD_PUBLIC_COMMAND_CONTRACT_BACKEND = f"python:{PUBLIC_COMMAND_CONTRACT_SCRIPT}"
CHECK_PUBLIC_COMMAND_CONTRACT_BACKEND = (
    f"{BUILD_PUBLIC_COMMAND_CONTRACT_BACKEND} --check"
)
CHECK_PUBLIC_COMMAND_BUDGET_BACKEND = f"python:{PUBLIC_COMMAND_BUDGET_SCRIPT}"

PUBLIC_COMMAND_VALIDATION_TIER = "docs"
PUBLIC_COMMAND_SURFACE_GUARANTEE_OWNER = (
    "operator-facing machine appendix stays in sync with the live workflow "
    "runner and package scripts"
)
PUBLIC_COMMAND_BUDGET_GUARANTEE_OWNER = (
    "public command count, appendix sync, and package bridge coverage stay tied "
    "to the canonical command contract"
)

BUILD_PUBLIC_COMMAND_SURFACE_COMMAND = (sys.executable, PUBLIC_COMMAND_SURFACE_PY)
CHECK_PUBLIC_COMMAND_SURFACE_COMMAND = (
    sys.executable,
    PUBLIC_COMMAND_SURFACE_PY,
    "--check",
)
BUILD_PUBLIC_COMMAND_CONTRACT_COMMAND = (sys.executable, PUBLIC_COMMAND_CONTRACT_PY)
CHECK_PUBLIC_COMMAND_CONTRACT_COMMAND = (
    sys.executable,
    PUBLIC_COMMAND_CONTRACT_PY,
    "--check",
)
CHECK_PUBLIC_COMMAND_BUDGET_COMMAND = (sys.executable, PUBLIC_COMMAND_BUDGET_PY)


def action_build_public_command_surface(_: list[str]) -> int:
    return run([str(part) for part in BUILD_PUBLIC_COMMAND_SURFACE_COMMAND])


def action_check_public_command_surface(_: list[str]) -> int:
    return run([str(part) for part in CHECK_PUBLIC_COMMAND_SURFACE_COMMAND])


def action_build_public_command_contract(_: list[str]) -> int:
    return run([str(part) for part in BUILD_PUBLIC_COMMAND_CONTRACT_COMMAND])


def action_check_public_command_contract(_: list[str]) -> int:
    return run([str(part) for part in CHECK_PUBLIC_COMMAND_CONTRACT_COMMAND])


def action_check_public_command_budget(_: list[str]) -> int:
    return run([str(part) for part in CHECK_PUBLIC_COMMAND_BUDGET_COMMAND])
